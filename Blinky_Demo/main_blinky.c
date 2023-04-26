/*
 * FreeRTOS V202112.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

/* Standard includes. */
#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* TI includes. */
#include "driverlib/driverlib.h"
#include "morsecode.h"

/* Priorities at which the tasks are created. */
#define mainQUEUE_RECEIVE_TASK_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define	mainQUEUE_SEND_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )
#define	mainBUTTON_TASK_PRIORITY		    ( tskIDLE_PRIORITY + 1 )

/* The rate at which data is sent to the queue.  The 200ms value is converted
to ticks using the portTICK_PERIOD_MS constant. */
#define mainQUEUE_SEND_FREQUENCY_MS			( pdMS_TO_TICKS( 1000UL ) )

/* The number of items the queue can hold.  This is 1 as the receive task
will remove items as they are added, meaning the send task should always find
the queue empty. */
#define mainQUEUE_LENGTH					( 1 )
#define mainDEBOUNCE_DELAY			        ( ( TickType_t ) 150 / portTICK_PERIOD_MS )
#define mainNO_DELAY				        ( ( TickType_t ) 0 )

/* Values passed to the two tasks just to check the task parameter
functionality. */
#define mainQUEUE_SEND_PARAMETER			( 0x1111UL )
#define mainQUEUE_RECEIVE_PARAMETER			( 0x22UL )

/* Demo board specifics. */
#define mainLED_RED         2
#define mainLED_GREEN       4
#define mainLED_BLUE        8
#define mainBAUD_RATE       ( 19200 )
#define mainFIFO_SET        ( 0x10 )
#define mainPUSH_BUTTON     0
static uint8_t mainToggle_LED = mainLED_RED;

/*-----------------------------------------------------------*/

/*
 * The tasks as described in the comments at the top of this file.
 */
static void prvQueueReceiveTask( void *pvParameters );
static void prvQueueSendTask( void *pvParameters );
static void prvButtonHandlerTask( void *pvParameters );

/*
 * Called by main() to create the simply blinky style application if
 * configCREATE_SIMPLE_TICKLESS_DEMO is set to 1.
 */
void main_blinky( void );

void vUART0_ISR( void );
void vGPIOF_ISR( void );

/* The queue used by both tasks. */
static QueueHandle_t xQueue = NULL;

/* The semaphore used to wake the button handler task from within the GPIO
interrupt handler. */
SemaphoreHandle_t xButtonSemaphore = NULL;

/* String that is transmitted on the UART. */
static char message_morse[3] = {'s', 'o', 's'};

static char *cMessage = "Task woken by button SW1 interrupt! --- ";
static volatile char *pcNextChar;

static const TickType_t xTickTockOn = pdMS_TO_TICKS( 100 );
static const TickType_t xTickTockOff = pdMS_TO_TICKS( 100 );
static const TickType_t xTickTockChar = pdMS_TO_TICKS( 100 * 3 );

static const TickType_t xShortBlock = pdMS_TO_TICKS( 5000);

/*-----------------------------------------------------------*/

void main_blinky( void )
{
	
    /* Create the semaphore used to wake the button handler task from the GPIO
	ISR. */
	xButtonSemaphore = xSemaphoreCreateBinary();
	xSemaphoreTake( xButtonSemaphore, 0 );

    /* Create the queue. */
	xQueue = xQueueCreate( sizeof(message_morse), sizeof(char));

	if( xQueue != NULL )
	{
		/* Start the two tasks as described in the comments at the top of this
		file. */
		xTaskCreate( prvQueueReceiveTask,					/* The function that implements the task. */
					"Rx", 									/* The text name assigned to the task - for debug only as it is not used by the kernel. */
					configMINIMAL_STACK_SIZE, 				/* The size of the stack to allocate to the task. */
					( void * ) mainQUEUE_RECEIVE_PARAMETER, /* The parameter passed to the task - just to check the functionality. */
					mainQUEUE_RECEIVE_TASK_PRIORITY, 		/* The priority assigned to the task. */
					NULL );									/* The task handle is not required, so NULL is passed. */

		xTaskCreate( prvQueueSendTask, "TX", configMINIMAL_STACK_SIZE, ( void * ) mainQUEUE_SEND_PARAMETER, mainQUEUE_SEND_TASK_PRIORITY, NULL );


		/* Start the tasks and timer running. */
		vTaskStartScheduler();
	}

	/* If all is well, the scheduler will now be running, and the following
	line will never be reached.  If the following line does execute, then
	there was insufficient FreeRTOS heap memory available for the idle and/or
	timer tasks	to be created.  See the memory management section on the
	FreeRTOS web site for more details. */
	for( ;; );
}
/*-----------------------------------------------------------*/

static void prvQueueSendTask( void *pvParameters )
{
TickType_t xNextWakeTime;
//const unsigned long ulValueToSend = 100UL;

	/* Check the task parameter is as expected. */
	configASSERT( ( ( unsigned long ) pvParameters ) == mainQUEUE_SEND_PARAMETER );

	/* Initialise xNextWakeTime - this only needs to be done once. */
	//xNextWakeTime = xTaskGetTickCount();
    

	for( ;; )
	{
		/* Place this task in the blocked state until it is time to run again.
		The block time is specified in ticks, the constant used converts ticks
		to ms.  While in the Blocked state this task will not consume any CPU
		time. */
		//vTaskDelayUntil( &xNextWakeTime, mainQUEUE_SEND_FREQUENCY_MS );
	    vTaskDelay( xShortBlock );

		/* Send to the queue - causing the queue receive task to unblock and
		toggle the LED.  0 is used as the block time so the sending operation
		will not block - it shouldn't need to block as the queue should always
		be empty at this point in the code. */
		xQueueSend( xQueue, message_morse, 0U );
	}
}
/*-----------------------------------------------------------*/

static void prvQueueReceiveTask( void *pvParameters )
{
    //unsigned long ulReceivedValue;
    char msg[3] = {'\0'};
    int i, j, k, cnt;
	/* Check the task parameter is as expected. */
	configASSERT( ( ( unsigned long ) pvParameters ) == mainQUEUE_RECEIVE_PARAMETER );

	for( ;; )
	{
		/* Wait until something arrives in the queue - this task will block
		indefinitely provided INCLUDE_vTaskSuspend is set to 1 in
		FreeRTOSConfig.h. */
		xQueueReceive( xQueue, msg, portMAX_DELAY );

		/*  To get here something must have been received from the queue, but
		is it the expected value?  If it is, toggle the LED. */
		for(i = 0; i < 3 &&  msg[i] != '\0'; i++)
		{
            for(j = 0; j< 5 && MorseCode[msg[i]-'A'][j] != '\0'; j++){
                if (MorseCode[msg[i]-'A'][j] == '.'){
                    cnt = 1;   
                }else{ // '_'
                    cnt = 3; 
                }
                for (k = 0; k < cnt ; k++){
                    GPIOPinWrite( GPIO_PORTF_BASE, mainToggle_LED,  mainToggle_LED);
			        vTaskDelay( xTickTockOn );
                    GPIOPinWrite( GPIO_PORTF_BASE, mainToggle_LED, 0 ); 
                    vTaskDelay( xTickTockOn );
                }
                vTaskDelay(xTickTockOff);
            }
            
            vTaskDelay(xTickTockChar);
		}
	}
}

/*-----------------------------------------------------------*/
void vGPIOF_ISR( void )
{
portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    
    uint32_t s = GPIOIntStatus(GPIO_PORTF_BASE, true);
	/* Clear the interrupt. */
	GPIOIntClear(GPIO_PORTF_BASE, s);
    
	/* Wake the button handler task. */
	//xSemaphoreGiveFromISR( xButtonSemaphore, &xHigherPriorityTaskWoken );
    
    if (mainToggle_LED == mainLED_BLUE)
        mainToggle_LED = mainLED_RED;
    else
        mainToggle_LED = mainToggle_LED + 1;
     
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
    
}

void vUART0_ISR( void )
{
unsigned long ulStatus;

	/* What caused the interrupt. */
	ulStatus = UARTIntStatus( UART0_BASE, pdTRUE );

	/* Clear the interrupt. */
	UARTIntClear( UART0_BASE, ulStatus );

	/* Was a Tx interrupt pending? */
	if( ulStatus & UART_INT_TX )
	{
		/* Send the next character in the string.  We are not using the FIFO. */
		if( *pcNextChar != 0 )
		{
			if( !( HWREG( UART0_BASE + UART_O_FR ) & UART_FR_TXFF ) )
			{
				HWREG( UART0_BASE + UART_O_DR ) = *pcNextChar;
			}
			pcNextChar++;
		}
	}
}

void vPreSleepProcessing( uint32_t ulExpectedIdleTime )
{
}
/*-----------------------------------------------------------*/

#if( configCREATE_SIMPLE_TICKLESS_DEMO == 1 )

	void vApplicationTickHook( void )
	{
		/* This function will be called by each tick interrupt if
		configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
		added here, but the tick hook is called from an interrupt context, so
		code must not attempt to block, and only the interrupt safe FreeRTOS API
		functions can be used (those that end in FromISR()). */

		/* Only the full demo uses the tick hook so there is no code is
		executed here. */
	}

#endif

