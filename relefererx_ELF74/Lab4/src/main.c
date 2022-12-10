#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

//constantes
#define GPIO_PA0_U0RX 0x00000001
#define GPIO_PA1_U0TX 0x00000401

uint8_t buffer[4];
uint8_t b_pos;


void UART_Interruption_Handler(void){
  buffer[b_pos%4] = (uint8_t)UARTCharGetNonBlocking(UART0_BASE);
  b_pos++;
  UARTIntClear(UART0_BASE,UARTIntStatus(UART0_BASE,true));
}

int main()
{
  int size = 4;
  b_pos = 0;
  uint8_t last_pos = 0;
  uint8_t *temp;
  int ascii_jump = 32, ascii_a = 65, ascii_z = 90;

  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0));

  GPIOPinConfigure(GPIO_PA0_U0RX);
  GPIOPinConfigure(GPIO_PA1_U0TX);
  
  GPIOPinTypeUART(GPIO_PORTA_BASE,(GPIO_PIN_0|GPIO_PIN_1));
  UARTConfigSetExpClk(UART0_BASE,(uint32_t)120000000,(uint32_t)115200,(UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE));
  
  UARTFIFODisable(UART0_BASE);
  UARTIntEnable(UART0_BASE,UART_INT_RX|UART_INT_RT);
  UARTIntRegister(UART0_BASE,UART_Interruption_Handler);
  
  while(1){

    if(last_pos != b_pos){
      temp = &buffer[last_pos%size];

      if(((*temp)<=(uint8_t)ascii_z) && ((*temp)>=(uint8_t)ascii_a))
        *temp += ascii_jump; 
      if(UARTCharPutNonBlocking(UART0_BASE,buffer[last_pos%size]))
        last_pos++;
  
    }         
  }
}
