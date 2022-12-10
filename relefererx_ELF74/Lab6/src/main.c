#include "tx_api.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#define A 0
#define B 1
#define C 2
#define D 3
#define E 4

#define DEMO_BYTE_POOL_SIZE     9120
#define DEMO_STACK_SIZE         1024

TX_THREAD               thread_1;
TX_THREAD               thread_2;
TX_THREAD               thread_3;
TX_BYTE_POOL            byte_pool_0;
TX_MUTEX                mutex_0;

UCHAR                   byte_pool_memory[DEMO_BYTE_POOL_SIZE];


void turnOn(uint8_t led){
  GPIOPinWrite(GPIO_PORTN_BASE,(GPIO_PIN_0 | GPIO_PIN_1), (GPIO_PIN_0 | GPIO_PIN_1) & (led>>2));
  GPIOPinWrite(GPIO_PORTF_BASE,(GPIO_PIN_0 | GPIO_PIN_4), (GPIO_PIN_0 & led) | (GPIO_PIN_4 & (led<<3)));
}

void turnOff(){
  GPIOPinWrite(GPIO_PORTN_BASE,(GPIO_PIN_0 | GPIO_PIN_1), (GPIO_PIN_0 | GPIO_PIN_1) & (0));
  GPIOPinWrite(GPIO_PORTF_BASE,(GPIO_PIN_0 | GPIO_PIN_4), (GPIO_PIN_0 & 0) | (GPIO_PIN_4 & (0)));
}


uint8_t schedulerMode(){
  // Escalonamento por time slice de 50ms. Todas as tarefas
  // com mesma prioridade
  //return A;
  
  // Escalonamento sem time slice e sem preempcao. Prioridades estabelecidas 
  // no passo 3. A preempcao pode ser evitada com o
  // preemption threshold do ThreadX
  //return B;

  // Escalonamento preemptivo por prioridade
  //return C;

  // Implemente um Mutex compartilhado entre T1 e T3. No inicio de cada job estas tarefas
  // devem solicitar este mutex e libera-lo no final. Use mutex sem heranca
  // de prioridade. Observe o efeito na temporizacao das tarefas
  //return D;

  // Idem acima, mas com herança de prioridade
  return E;
}

uint8_t loop_wait_led(uint8_t led, uint32_t numLoops){  
  for(uint32_t wait_counter = 0; wait_counter < numLoops; wait_counter++) {
     turnOn(led);
     turnOff();  
  }

  return 0;
}

void thread_entry(ULONG thread_entrada){

//tempo medido = 1,6us
//T1 300ms - 187500
//T2 500ms - 312500
//T3 800ms - 500000

  uint32_t t1 = 187500;
  uint32_t t2 = 312500;
  uint32_t t3 = 500000;
  
  if(thread_entrada==1){
    while(1){
      //D e E
      if(schedulerMode()== D | schedulerMode()== E){
        UINT status = tx_mutex_get(&mutex_0,TX_WAIT_FOREVER);
        if (status != TX_SUCCESS)
          break;
        loop_wait_led(8, t1);
        tx_thread_sleep(500);
      
        status = tx_mutex_put(&mutex_0);
        if (status != TX_SUCCESS)
          break;
      }
      //A, B e C
      else{
        loop_wait_led(8, t1);
        tx_thread_sleep(500);
      }

    }
  }

  else if(thread_entrada==2){
    while(1){
      loop_wait_led(4, t2);
      tx_thread_sleep(750);
    }
  }

  else if(thread_entrada==3){
    while(1){
      //D e E
      if(schedulerMode()== D | schedulerMode()== E){
        UINT status = tx_mutex_get(&mutex_0,TX_WAIT_FOREVER);
        if (status != TX_SUCCESS)
          break;
        
        loop_wait_led(2,t3);
        tx_thread_sleep(2000);

        status = tx_mutex_put(&mutex_0);
        if (status != TX_SUCCESS)
          break;

      //A, B e C
      }else{
        loop_wait_led(2, t3);
        tx_thread_sleep(2000);
      }

    }
  }

}

void initPeripherals(void) {
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION));

  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));

  GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1 );
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4 );
}


 int main(){
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION));

  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));

  GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1 );
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4 );

    
  //initPeripherals();    
  turnOff();
  
  tx_kernel_enter();
}


void    tx_application_define(void *first_unused_memory)
{

CHAR    *pointer = TX_NULL;


#ifdef TX_ENABLE_EVENT_TRACE
    tx_trace_enable(trace_buffer, sizeof(trace_buffer), 32);
#endif
    UINT time_slice[3], priority[3], preemption[3];

      //A
      if(schedulerMode() == A){
        for(int i=0;i<3;i++){
          priority[i]=0;
          preemption[i]=0;
          time_slice[i]=25;
        } 
      }

      //C, D e E
      else{
        for(int i=0;i<3;i++){
           priority[i]=i;
           preemption[i]=i;
           time_slice[i] = TX_NO_TIME_SLICE;
        }
      }

    //B
    if(schedulerMode() == B){
        preemption[0]=0;
        preemption[1]=0;
        preemption[2]=0;
    }

    /* Create a byte memory pool from which to allocate the thread stacks.  */
    tx_byte_pool_create(&byte_pool_0, "byte pool 0", byte_pool_memory, DEMO_BYTE_POOL_SIZE);

    /* Put system definition stuff in here, e.g. thread creates and other assorted
       create information.  */


    /* Allocate the stack for thread 1.  */
    tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    /* Create threads 1 and 2. These threads pass information through a ThreadX 
       message queue.  It is also interesting to note that these threads have a time
       slice.  */
    tx_thread_create(&thread_1, "thread 1", thread_entry, 1,  
            pointer, DEMO_STACK_SIZE, 
            priority[0], preemption[0], time_slice[0], TX_AUTO_START);

    /* Allocate the stack for thread 2.  */
    tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    tx_thread_create(&thread_2, "thread 2", thread_entry, 2,  
            pointer, DEMO_STACK_SIZE, 
            priority[1], preemption[1], time_slice[1], TX_AUTO_START);

    // /* Allocate the stack for thread 3.  */
    tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    // /* Create threads 3 and 4.  These threads compete for a ThreadX counting semaphore.  
    //    An interesting thing here is that both threads share the same instruction area.  */
    tx_thread_create(&thread_3, "thread 3", thread_entry, 3,  
            pointer, DEMO_STACK_SIZE, 
            priority[2], preemption[2], time_slice[2], TX_AUTO_START);

    //D
    if(schedulerMode()== D){
      tx_mutex_create(&mutex_0, "mutex 0", TX_NO_INHERIT);
    }
    else if(schedulerMode()== E){
      tx_mutex_create(&mutex_0, "mutex 0", TX_INHERIT);
    }
}
