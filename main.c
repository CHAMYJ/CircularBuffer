#include "stm32f10x.h"
#include "cmsis_os.h"
#include "uart.h"

void producer_thread (void const *argument);
void consumer_thread(void const *argument);

// Define threads
osThreadDef(producer_thread, osPriorityNormal,1,0);
osThreadDef(consumer_thread, osPriorityNormal,1,0);

// Define the semaphores
osSemaphoreId item;
osSemaphoreDef(item);
osSemaphoreId empty;
osSemaphoreDef(empty);

// Define the mutex
osMutexId buffMutex;
osMutexDef(buffMutex);

osThreadId T_uart1;
osThreadId T_uart2;

unsigned int t;
unsigned int t1=0;
unsigned int t2=0;
const unsigned int N = 8;
unsigned char buffer[N];
unsigned int cbufferHead = 0;
unsigned int cbufferTail = 0;
unsigned char itemRemove = 0x00;


void insert(unsigned char item1){
	osSemaphoreWait(empty, osWaitForever);
	osMutexWait(buffMutex, osWaitForever);
	buffer[cbufferTail] = item1;
	cbufferTail = (cbufferTail + 1) % N;
	osMutexRelease(buffMutex);
	osSemaphoreRelease(item);
}

void producer_thread (void const *argument)
{
	unsigned char item = 0x01;
	for(;;){
	long int i=0;
	for(i=0; i<10; i++){
		insert(item++);
		t1++;
		}
	}
}


unsigned char remove(unsigned char data1){
	unsigned char output = 0x00;
	osSemaphoreWait(item, osWaitForever);
	osMutexWait(buffMutex, osWaitForever);
	output = buffer[cbufferHead];
	buffer[cbufferHead] = data1;
	cbufferHead = (cbufferHead + 1) % N;
	osMutexRelease(buffMutex);
	osSemaphoreRelease(empty);
	return output;
}

void consumer_thread (void const *argument) 
{ 
	for(;;){
	unsigned char data = 0x00;
	long int j=0;
	for(j=0; j<10; j++){
		itemRemove=remove(data);
		SendChar(itemRemove);
		t2++;
		}
  }
}

int main (void)
{
	osKernelInitialize ();   // initialize CMSIS-RTOS

	USART1_Init ();
	item = osSemaphoreCreate(osSemaphore(item), 0);	
	empty = osSemaphoreCreate(osSemaphore(empty), 8);	
	buffMutex = osMutexCreate(osMutex(buffMutex));
	T_uart1 = osThreadCreate(osThread(producer_thread), NULL);  //producer
	T_uart2 = osThreadCreate(osThread(consumer_thread), NULL);  //consumer
	
	osKernelStart ();	  // start thread execution 
}
