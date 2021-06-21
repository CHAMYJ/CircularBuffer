#include "stm32f10x.h"
#include "cmsis_os.h"
#include "uart.h"

void x_Thread1 (void const *argument);
void x_Thread2 (void const *argument);
void x_Thread3 (void const *argument);
void x_Thread4 (void const *argument);
osThreadDef(x_Thread1, osPriorityNormal, 1, 0);
osThreadDef(x_Thread2, osPriorityNormal, 1, 0);
osThreadDef(x_Thread3, osPriorityNormal, 1, 0);
osThreadDef(x_Thread4, osPriorityNormal, 1, 0);

osThreadId T_x1;
osThreadId T_x2;
osThreadId T_x3;
osThreadId T_x4;

osMessageQId Q_LED;
osMessageQDef (Q_LED,0x16,unsigned char);
osEvent  result;

osMutexId x_mutex;
osMutexDef(x_mutex);
osSemaphoreId item_semaphore;                         // Semaphore ID
osSemaphoreDef(item_semaphore);                       // Semaphore definition
osSemaphoreId space_semaphore;                         // Semaphore ID
osSemaphoreDef(space_semaphore);                       // Semaphore definition

long int x=0;
long int i=0;
long int j=0;
long int k=0;
long int m=0;

const unsigned int N = 4;
unsigned char buffer[N];
unsigned int insertPtr = 0;
unsigned int removePtr = 0;
const unsigned char item1 = 0x30;
const unsigned char item2 = 0x31;
const unsigned char item3 = 0x32;
const unsigned char item4 = 0x33;
unsigned char input[N]={item1,item2,item3,item4};

void put(){
	osSemaphoreWait(space_semaphore, osWaitForever);
	osMutexWait(x_mutex, osWaitForever);
	buffer[insertPtr] = input[m];
	m++;
	if(m==4){
		m=0;
	}
	insertPtr = (insertPtr + 1) % N;
	osMutexRelease(x_mutex);
	osSemaphoreRelease(item_semaphore);

}

unsigned char get(){
	unsigned int rr = 0xff;
	osSemaphoreWait(item_semaphore, osWaitForever);
	osMutexWait(x_mutex, osWaitForever);
	rr = buffer[removePtr];
	SendChar(rr); SendChar('\n');
	buffer[removePtr]=NULL;
	removePtr = (removePtr + 1) % N;
	osMutexRelease(x_mutex);
	osSemaphoreRelease(space_semaphore);
	return rr;
}

int loopcount = 20;

void x_Thread1 (void const *argument) 
{
	//producer
	unsigned char item = 0x30;
	for(;;){
		put();
	}
}

void x_Thread2 (void const *argument) 
{
	//consumer (waiter #1)
	unsigned int data = 0x00;
	for(;;){
		data = get();
		osMessagePut(Q_LED,data,osWaitForever);             //Place a value in the message queue
	}
}

void x_Thread3 (void const *argument) 
{
	//consumer (waiter #2)
	unsigned int c2data = 0x00;
	for(;;){
		c2data = get();
		osMessagePut(Q_LED,c2data,osWaitForever);             //Place a value in the message queue
	}
}

void x_Thread4(void const *argument)
{
	//cashier
	for(;;){
		result = 	osMessageGet(Q_LED,osWaitForever);				//wait for a message to arrive
		SendChar(result.value.v);
	}
}

int main (void) 
{
	osKernelInitialize ();                    // initialize CMSIS-RTOS
	USART1_Init();
	item_semaphore = osSemaphoreCreate(osSemaphore(item_semaphore), 0);
	space_semaphore = osSemaphoreCreate(osSemaphore(space_semaphore), N);
	x_mutex = osMutexCreate(osMutex(x_mutex));	
	
	Q_LED = osMessageCreate(osMessageQ(Q_LED),NULL);					//create the message queue
	
	T_x1 = osThreadCreate(osThread(x_Thread1), NULL);//producer
	T_x2 = osThreadCreate(osThread(x_Thread2), NULL);//consumer
	T_x3 = osThreadCreate(osThread(x_Thread3), NULL);//another consumer
	T_x4 = osThreadCreate(osThread(x_Thread4), NULL);//casher
	
 
	osKernelStart ();                         // start thread execution 
}