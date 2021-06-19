#include "cmsis_os.h"
#include "stm32f10x.h"
#include <stdio.h>
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

	USART1_Init ();
	item = osSemaphoreCreate(osSemaphore(item), 0);	
	empty = osSemaphoreCreate(osSemaphore(empty), 1);	
	buffMutex = osMutexCreate(osMutex(buffMutex));
	T_uart1 = osThreadCreate(osThread(producer_thread), NULL);
	T_uart2 = osThreadCreate(osThread(consumer_thread), NULL);
	
	osKernelStart ();	
}

void test()
{
	uint64_t x;
		if(cbufferHead==cbufferTail) {
		  //printf("Queue is Empty");
			SendChar('E'); SendChar('\n'); SendChar('\r');
	    }
		else {
		//printf ("Producer insert!\r\n
		//int m=0; int n=0; n=cbufferTail-1; m=cbuffer[n];
    //printf("Circular buffer is"); printf("%d\t",m);
		SendChar('T');  //cbufferTail
		for( x=0;x<1;x++){	
      if(cbufferTail==1) {
			SendChar('Q'); SendChar('['); SendChar('0'); SendChar(']'); SendChar('\n'); SendChar('\r');
			}
		  else if(cbufferTail==2) {
			SendChar('Q'); SendChar('['); SendChar('1'); SendChar(']'); SendChar('\n'); SendChar('\r');
			}
			else if(cbufferTail==3) {
			SendChar('Q'); SendChar('['); SendChar('2'); SendChar(']'); SendChar('\n'); SendChar('\r');
			}
			else if(cbufferTail==4) {
			SendChar('Q'); SendChar('['); SendChar('3'); SendChar(']'); SendChar('\n'); SendChar('\r');
			}
			else if(cbufferTail==5) {
			SendChar('Q'); SendChar('['); SendChar('4'); SendChar(']'); SendChar('\n'); SendChar('\r');
			}
			else if(cbufferTail==6) {
			SendChar('Q'); SendChar('['); SendChar('5'); SendChar(']'); SendChar('\n'); SendChar('\r');
			}
			else if(cbufferTail==7) {
			SendChar('Q'); SendChar('['); SendChar('6'); SendChar(']'); SendChar('\n'); SendChar('\r');
			}
			else {
			SendChar('Q'); SendChar('['); SendChar('7'); SendChar(']'); SendChar('\n'); SendChar('\r');
			}
		}
	}
}


void test2()
{
	uint64_t x;
		if(cbufferTail-cbufferHead==8) {
		  //printf("Queue is Full");
			SendChar('F'); SendChar('\n'); SendChar('\r');
	    }
		else {
		//printf ("Consumer remove!\r\n");
		//int m=0; int n=0; n=cbufferTail-1; m=cbuffer[n];
    //printf("Circular buffer is"); printf("%d\t",m);
		SendChar('H');  //cbufferHead
		for( x=0;x<1;x++){	
      if(cbufferHead==1) {
			SendChar('Q'); SendChar('['); SendChar('0'); SendChar(']'); SendChar('\n'); SendChar('\r');
			}
		  else if(cbufferHead==2) {
			SendChar('Q'); SendChar('['); SendChar('1'); SendChar(']'); SendChar('\n'); SendChar('\r');
			}
			else if(cbufferHead==3) {
			SendChar('Q'); SendChar('['); SendChar('2'); SendChar(']'); SendChar('\n'); SendChar('\r');
			}
			else if(cbufferHead==4) {
			SendChar('Q'); SendChar('['); SendChar('3'); SendChar(']'); SendChar('\n'); SendChar('\r');
			}
			else if(cbufferHead==5) {
			SendChar('Q'); SendChar('['); SendChar('4'); SendChar(']'); SendChar('\n'); SendChar('\r');
			}
			else if(cbufferHead==6) {
			SendChar('Q'); SendChar('['); SendChar('5'); SendChar(']'); SendChar('\n'); SendChar('\r');
			}
			else if(cbufferHead==7) {
			SendChar('Q'); SendChar('['); SendChar('6'); SendChar(']'); SendChar('\n'); SendChar('\r');
			}
			else {
			SendChar('Q'); SendChar('['); SendChar('7'); SendChar(']'); SendChar('\n'); SendChar('\r');
			}
		}
	}
}

void producer_thread (void const *argument)
{
	for (;;) {
		osSemaphoreWait(empty, osWaitForever);
		osMutexWait(buffMutex, osWaitForever);
		test();
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
	test();
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
		test2();  
	}
}
