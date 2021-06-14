#include "cmsis_os.h"
#include "stm32f10x.h"

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

#define buffer_capacity 8
#define data_capacity 8

static int cbuffer[buffer_capacity];
static int data[data_capacity] = {1,2,3,4,5,6,7,8};
static int output [data_capacity];
static int cbufferHead = 0;
static int cbufferTail = 0;
static int i = 0;
static int j = 0;
static int value = 1;

int main (void)
{
	osKernelInitialize ();

	item = osSemaphoreCreate(osSemaphore(item), 0);	
	empty = osSemaphoreCreate(osSemaphore(empty), 1);	
	buffMutex = osMutexCreate(osMutex(buffMutex));
	T_uart1 = osThreadCreate(osThread(producer_thread), NULL);
	T_uart2 = osThreadCreate(osThread(consumer_thread), NULL);
	osKernelStart ();
	
}

void producer_thread (void const *argument)
{
	for (;;) {
		osSemaphoreWait(empty, osWaitForever);
		osMutexWait(buffMutex, osWaitForever);
		if((cbufferTail+1) % (buffer_capacity+1) != 0){
			cbuffer[cbufferTail] = value;
			i++; value++;
			cbufferTail = (cbufferTail+1) % (buffer_capacity);
			osMutexRelease(buffMutex);
			osSemaphoreRelease(item);
		}
		else {
			cbufferTail = 0;
			osMutexRelease(buffMutex);
			osSemaphoreRelease(item);
		}
	}
}

void consumer_thread(void const *argument)
{
	for (;;) {
		osSemaphoreWait(item, osWaitForever);
		osMutexWait(buffMutex, osWaitForever);
		if((j+1) % (buffer_capacity+1) != 0){
			output[j] = cbuffer[cbufferHead];
			cbuffer[cbufferHead]=NULL;
			j++;
			cbufferHead = (cbufferHead+1) % (buffer_capacity);
			osMutexRelease(buffMutex);
			osSemaphoreRelease(empty);
		}
		else{
			j =0;
			output[j]= cbuffer[cbufferHead];
			cbuffer[cbufferHead]=NULL;
			j++;
			cbufferHead = (cbufferHead+1) % (buffer_capacity);
			osMutexRelease(buffMutex);
			osSemaphoreRelease(empty);
		}
	}
}
