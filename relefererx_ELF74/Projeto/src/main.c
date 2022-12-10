
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "inc/hw_memmap.h"
#include "tx_api.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "system_TM4C1294.h"
#include "utils/uartstdio.h"
#include "driverlib/systick.h"
#include <inttypes.h>
#include "driverlib/interrupt.h"


#define BUFFER_SIZE 16
#define DEMO_BYTE_POOL_SIZE     9120
#define DEMO_STACK_SIZE         1024
#define DEMO_QUEUE_SIZE         100

#define GPIO_PA0_U0RX           0x00000001
#define GPIO_PA1_U0TX           0x00000401

TX_THREAD               thread_1;
TX_THREAD               thread_2;
TX_THREAD               thread_3;
TX_THREAD               thread_main;
TX_BYTE_POOL            byte_pool_0;
TX_QUEUE                queue_uart_esquerda;
TX_QUEUE                queue_uart_centro;
TX_QUEUE                queue_uart_direita;
TX_QUEUE                queue_interna_esquerda;
TX_QUEUE                queue_interna_centro;
TX_QUEUE                queue_interna_direita;
TX_MUTEX                mutex_uart;


UCHAR                   byte_pool_memory[DEMO_BYTE_POOL_SIZE];


void UARTInit(void){
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0));
  
  GPIOPinConfigure(GPIO_PA0_U0RX);
  GPIOPinConfigure(GPIO_PA1_U0TX);
  GPIOPinTypeUART(GPIO_PORTA_BASE,(GPIO_PIN_0|GPIO_PIN_1));
  UARTConfigSetExpClk(UART0_BASE,SystemCoreClock,(uint32_t)115200,(UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE));
  UARTFIFOEnable(UART0_BASE);
}

int main(){
  IntMasterEnable(); 
  SysTickPeriodSet(2500000);  
  SysTickIntEnable();
  SysTickEnable();
  UARTInit();
  
  tx_kernel_enter();
}



char num_andar_para_char(char primeiro, char segundo){
  int floor = (primeiro - '0') * 10 + (segundo - '0');
  
  switch (floor) {
      case 0:
          return 'a';
      case 1:
          return 'b';
      case 2:
          return 'c';
      case 3:
          return 'd';
      case 4:
          return 'e';
      case 5:
          return 'f';
      case 6:
          return 'g';
      case 7:
          return 'h';
      case 8:
          return 'i';
      case 9:
          return 'j';
      case 10:
          return 'k';
      case 11:
          return 'l';
      case 12:
          return 'm';
      case 13:
          return 'n';
      case 14:
          return 'o';
      case 15:
          return 'p';
      default:
          return ' ';
    }
          
}


void elevador(ULONG posicao_elevador){
  char comando_recebido[8];
  char andar_atual = 'a';
  char andar_destino ='a';
  char comando_atual[8];
  UINT status;
  int num_comandos_armazenados = 0;
  int esta_movendo = 0;
  int comando_executando = 0;
  
  //pega o mutex
  status =  tx_mutex_get(&mutex_uart, TX_WAIT_FOREVER);

  if (status != TX_SUCCESS)
      return;
  
  //inicializa o elevador
  UARTCharPut(UART0_BASE,posicao_elevador);
  UARTCharPut(UART0_BASE,'r');
  UARTCharPut(UART0_BASE,'\r');
  
  //fecha as portas
  UARTCharPut(UART0_BASE,posicao_elevador);
  UARTCharPut(UART0_BASE,'f');
  UARTCharPut(UART0_BASE,'\r');

  //libera o mutex
  status =  tx_mutex_put(&mutex_uart);

  if (status != TX_SUCCESS)
      return;

  
  while(1){
    //limpa e armazena o comando recebido da queue
    memset(comando_recebido, 0, sizeof comando_recebido);
    if(posicao_elevador=='e'){
      status = tx_queue_receive(&queue_uart_esquerda, comando_recebido, 2);  
      
    }
    else if(posicao_elevador=='c'){
      status = tx_queue_receive(&queue_uart_centro, comando_recebido, 2);  
      
    }
    else if(posicao_elevador=='d'){
      status = tx_queue_receive(&queue_uart_direita, comando_recebido, 2);  
      
    }
    
    //verifica se recebeu a posição atual do elevador
    if(comando_recebido[1]>=48 && comando_recebido[1]<=57)
    {
      if(comando_recebido[2]>=48 && comando_recebido[2]<=57){
        andar_atual = num_andar_para_char(comando_recebido[1], comando_recebido[2]);
      }else{
        andar_atual = num_andar_para_char('0', comando_recebido[1]);
      }
    }

    if(comando_recebido[1]=='E' || comando_recebido[1]=='I'){
      if(comando_recebido[1]=='I'){
        //pega mutex
        UINT status =  tx_mutex_get(&mutex_uart, TX_WAIT_FOREVER);
        if (status != TX_SUCCESS)
            break;
        
        //coloca comando para acender LED
        UARTCharPut(UART0_BASE,posicao_elevador);
        UARTCharPut(UART0_BASE,'L');
        UARTCharPut(UART0_BASE,comando_recebido[2]);
        UARTCharPut(UART0_BASE,'\r');

        //libera mutex
        status =  tx_mutex_put(&mutex_uart);
        if (status != TX_SUCCESS)
            break;
      }

      num_comandos_armazenados ++;

      //coloca o comando na queue interna
      if(posicao_elevador=='e'){
        status = tx_queue_send(&queue_interna_esquerda, comando_recebido, TX_WAIT_FOREVER);  
        if (status != TX_SUCCESS)
              break;
      }
      else if(posicao_elevador=='c'){
        status = tx_queue_send(&queue_interna_centro, comando_recebido, TX_WAIT_FOREVER);  
        if (status != TX_SUCCESS)
                break;
      }
      else if(posicao_elevador=='d'){
        status = tx_queue_send(&queue_interna_direita, comando_recebido, TX_WAIT_FOREVER);  
        if (status != TX_SUCCESS)
                break;
      }
    }

    //se tem algum comando armazenado e não tem nenhum comando sendo executado, pega o próximo
    if(num_comandos_armazenados>0 && !comando_executando){
      if(posicao_elevador=='e'){
      status = tx_queue_receive(&queue_interna_esquerda, comando_atual, TX_WAIT_FOREVER);  
      if (status != TX_SUCCESS)
              break;
      }
      else if(posicao_elevador=='c'){
        status = tx_queue_receive(&queue_interna_centro, comando_atual, TX_WAIT_FOREVER);  
        if (status != TX_SUCCESS)
                break;
      }
      else if(posicao_elevador=='d'){
        status = tx_queue_receive(&queue_interna_direita, comando_atual, TX_WAIT_FOREVER);  
        if (status != TX_SUCCESS)
                break;
      }
      comando_executando=1;
      num_comandos_armazenados--;
    }

    //dependendo do comando, salva o andar destino, se for E (dezena + decimal) -> char referente ao andar
    if(comando_atual[1]=='E'){
      andar_destino  = num_andar_para_char(comando_atual[2], comando_atual[3]);
    }
    //se for I pega o próprio andar recebido como char
    else if(comando_atual[1]=='I'){
      andar_destino = comando_atual[2];
    }

    if(andar_destino>andar_atual && !esta_movendo){
      //pega o mutex
      UINT status =  tx_mutex_get(&mutex_uart, TX_WAIT_FOREVER);

      /* Check status.  */
      if (status != TX_SUCCESS)
          break;
      
      //manda comando para subir o elevador
      UARTCharPut(UART0_BASE,posicao_elevador);
      UARTCharPut(UART0_BASE,'s');
      UARTCharPut(UART0_BASE,'\r');
      
      //libera o mutex
      status =  tx_mutex_put(&mutex_uart);

      if (status != TX_SUCCESS)
          break;

      esta_movendo=1;
    }
    else if(andar_destino<andar_atual && !esta_movendo){
      //pega o mutex
      UINT status =  tx_mutex_get(&mutex_uart, TX_WAIT_FOREVER);

      /* Check status.  */
      if (status != TX_SUCCESS)
          break;
      
      //manda o comando para descer o elevador
      UARTCharPut(UART0_BASE,posicao_elevador);
      UARTCharPut(UART0_BASE,'d');
      UARTCharPut(UART0_BASE,'\r');
      
      //libera o mutex
      status =  tx_mutex_put(&mutex_uart);

      if (status != TX_SUCCESS)
          break;
          
      esta_movendo=1;
    }

    if(andar_destino == andar_atual && comando_executando){
      //pega o mutex
      UINT status =  tx_mutex_get(&mutex_uart, TX_WAIT_FOREVER);

      /* Check status.  */
      if (status != TX_SUCCESS)
          break;
      
      //manda comando para subir o elevador
      UARTCharPut(UART0_BASE,posicao_elevador);
      UARTCharPut(UART0_BASE,'p');
      UARTCharPut(UART0_BASE,'\r');

      //abre as portas
      UARTCharPut(UART0_BASE,posicao_elevador);
      UARTCharPut(UART0_BASE,'a');
      UARTCharPut(UART0_BASE,'\r');

      
      //libera o mutex
      status =  tx_mutex_put(&mutex_uart);

      if (status != TX_SUCCESS)
          break;
   
      tx_thread_sleep(1000);

      //pega o mutex
      status =  tx_mutex_get(&mutex_uart, TX_WAIT_FOREVER);

      /* Check status.  */
      if (status != TX_SUCCESS)
          break;
      
      //fecha as portas
      UARTCharPut(UART0_BASE,posicao_elevador);
      UARTCharPut(UART0_BASE,'f');
      UARTCharPut(UART0_BASE,'\r');

      //apaga o led
      UARTCharPut(UART0_BASE,posicao_elevador);
      UARTCharPut(UART0_BASE,'D');
      UARTCharPut(UART0_BASE,andar_destino);
      UARTCharPut(UART0_BASE,'\r');

      //libera o mutex
      status =  tx_mutex_put(&mutex_uart);

      //limpa comando atual
      comando_executando = 0;
      memset(comando_atual, 0, sizeof comando_atual);
      esta_movendo=0;
    }
    tx_thread_sleep(1);
  }

}



void uart_receive(ULONG aux){
  
  int posicao_buffer_interno=0;
  char comando_recebido[8];
  char receiving_char;
  int comando_encerrado = 0;
  UINT status;

  while(1){
      
     while(UARTCharsAvail(UART0_BASE)){
      //guarda o char recebido 
      receiving_char = UARTCharGet(UART0_BASE);

      //se esta recebendo um comando
      if(receiving_char!='\n' && receiving_char!='\r'){
        // guarda o char no comando recebido para compor ele
       comando_recebido[posicao_buffer_interno] = receiving_char;
       posicao_buffer_interno++;
        //se for F ou A ignora e limpa o vetor
        if(receiving_char=='F' || receiving_char=='A'){
          memset(comando_recebido, 0, sizeof comando_recebido);
          posicao_buffer_interno=0;
        }
      }
      //se o comando acabou
      else{
        posicao_buffer_interno=0;
        comando_encerrado=1;
        }
        //da um sleep na thread para permitir dar tempo de receber o próximo char
      tx_thread_sleep(2);
     }
      
      //se acabou o comando envia para a fila respectiva
      if(comando_encerrado){
        if(comando_recebido[0]=='e'){
          status = tx_queue_send(&queue_uart_esquerda, comando_recebido, TX_WAIT_FOREVER);
          if (status != TX_SUCCESS)
              break;
        }
        if(comando_recebido[0]=='c'){
          status = tx_queue_send(&queue_uart_centro, comando_recebido, TX_WAIT_FOREVER);
          if (status != TX_SUCCESS)
              break;
        }
        if(comando_recebido[0]=='d'){
          status = tx_queue_send(&queue_uart_direita, comando_recebido, TX_WAIT_FOREVER);
          if (status != TX_SUCCESS)
              break;
        }
        //após o envio, reseta o comando recebido
        memset(comando_recebido, 0, sizeof comando_recebido);
        comando_encerrado=0;
      }
     
      tx_thread_sleep(1);
    }
  }
//}


void    tx_application_define(void *first_unused_memory)
{

    CHAR    *pointer;


#ifdef TX_ENABLE_EVENT_TRACE
    tx_trace_enable(trace_buffer, sizeof(trace_buffer), 32);
#endif
    
    tx_byte_pool_create(&byte_pool_0, "byte pool 0", byte_pool_memory, DEMO_BYTE_POOL_SIZE);

    //Aloca as threads dos elevadores
    tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    tx_thread_create(&thread_1, "elevador esquerda", elevador, 'e',  
            pointer, DEMO_STACK_SIZE, 
            0, 0, 5, TX_AUTO_START);

    tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    tx_thread_create(&thread_2, "elevador centro", elevador, 'c',  
            pointer, DEMO_STACK_SIZE, 
            0, 0, 5, TX_AUTO_START);

    tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    tx_thread_create(&thread_3, "elevador direita", elevador, 'd',  
            pointer, DEMO_STACK_SIZE, 
            0, 0, 5, TX_AUTO_START);

    //Aloca a thread do uart
    tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    tx_thread_create(&thread_main, "thread uart", uart_receive, 1,  
            pointer, DEMO_STACK_SIZE, 
            0, 0, 5, TX_AUTO_START);


    //queues de recebimento dos comandos do uart
    tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_QUEUE_SIZE*sizeof(ULONG), TX_NO_WAIT);
    tx_queue_create(&queue_uart_esquerda, "queue uart esquerda", TX_1_ULONG, pointer, DEMO_QUEUE_SIZE*sizeof(ULONG));

    tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_QUEUE_SIZE*sizeof(ULONG), TX_NO_WAIT);
    tx_queue_create(&queue_uart_centro, "queue uart centro", TX_1_ULONG, pointer, DEMO_QUEUE_SIZE*sizeof(ULONG));

    tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_QUEUE_SIZE*sizeof(ULONG), TX_NO_WAIT);
    tx_queue_create(&queue_uart_direita, "queue uart direita", TX_1_ULONG, pointer, DEMO_QUEUE_SIZE*sizeof(ULONG));
    
    //queues internas para armazenamento de comandos de cada elevador
    tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_QUEUE_SIZE*sizeof(ULONG), TX_NO_WAIT);
    tx_queue_create(&queue_interna_esquerda, "queue interna esquerda", TX_1_ULONG, pointer, DEMO_QUEUE_SIZE*sizeof(ULONG));

    tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_QUEUE_SIZE*sizeof(ULONG), TX_NO_WAIT);
    tx_queue_create(&queue_interna_centro, "queue interna centro", TX_1_ULONG, pointer, DEMO_QUEUE_SIZE*sizeof(ULONG));

    tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_QUEUE_SIZE*sizeof(ULONG), TX_NO_WAIT);
    tx_queue_create(&queue_interna_direita, "queue interna direita", TX_1_ULONG, pointer, DEMO_QUEUE_SIZE*sizeof(ULONG));

    //mutex para acesso à escrita do uart
    tx_mutex_create(&mutex_uart, "mutex uart write", TX_NO_INHERIT);

    tx_block_release(pointer);
}
