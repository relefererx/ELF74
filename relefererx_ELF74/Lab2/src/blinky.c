//*****************************************************************************
//
// blinky.c - Simple example to blink the on-board LED.
//
// Copyright (c) 2013-2020 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.2.0.295 of the EK-TM4C1294XL Firmware Package.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include <iostream>
#include "driverlib/interrupt.h"

using std::cout;


//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>Blinky (blinky)</h1>
//!
//! A very simple example that blinks the on-board LED using direct register
//! access.
//

#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
    while(1);
}
#endif

#define AUX_CLOCK 10000000
#define TRES_SEG 36
volatile uint32_t count = 0;

void incrementCounter(void) {
  count++;
}

void delay(int time) {
  volatile int milli_seconds = 220 * time;
  
  clock_t start_time = clock();
  
  while (clock() < start_time + milli_seconds) {
  }
}

void prepare(void) {
  //port N = led
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION))
    {
    }
    
    //port J = chave
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ))
    {
    }

    //seta LED como saída e Chave 1 como entrada
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0);
    GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
}

 int
main(void)
{    
    //algumas inicializacoes
    prepare();
    
    uint32_t timeAux = 0;
    uint32_t sw_1 = 0;
    
    IntMasterEnable();
    SysTickIntRegister(incrementCounter);
    SysTickPeriodSet(AUX_CLOCK);
    
    //enables do SysTick
    SysTickIntEnable();
    SysTickEnable();

    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0x0);
    
    //espera aprox. 1 segundo para acender o LED
    delay(1);
    
    //acende o lED
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0);
    count = 0;
     
    sw_1 = GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0);
    timeAux = SysTickValueGet();
   
    //enquanto a chave 1 nao for pressionada ou nao passarem 3 seg
    while(sw_1 == 1 && count < TRES_SEG) {
      sw_1 = GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0);
      //se passaram 3 segundos
      if (count >= TRES_SEG) {
        printf("Voce nao conseguiu apertar antes de 3seg! \n");
      }
    }
    
    uint32_t reaction = SysTickValueGet() + AUX_CLOCK*count - timeAux;
    float reactionInSeconds = reaction*0.000000008333;
    
    //printa o tempo de reacao
    printf("Tempo: %.3f segundos", reactionInSeconds);
}
