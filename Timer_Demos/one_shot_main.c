#include <stdbool.h>
#include <stdint.h>

#include "driverlib.h"

static volatile bool g_bIntFlag = false;

void UART_OutString( const char* message ) {
    const char * pcNextChar = message;
    
	while( *pcNextChar != '\0')
    {
        UARTCharPut(UART0_BASE, *pcNextChar);
		pcNextChar++;
	}

}
/*
 * PA0: UART0_RX; PA1: UART0_TX
 */
void InitConsole(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA)) {} 
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0)) {}
    
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    
    UARTConfigSetExpClk(UART0_BASE,16000000, 115200, UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE | UART_CONFIG_WLEN_8);
    UARTEnable(UART0_BASE);
}

void Timer0B_Handler(void) {
    TimerIntClear(TIMER0_BASE, TIMER_TIMB_TIMEOUT);
    g_bIntFlag = true;    
}

int main()
{
    // 50M HZ
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    //ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    
    InitConsole();
     
    UART_OutString("16-Bit Timer Interrupt ->\r\n");
    UART_OutString("    Timer = Timer0B\r\n");
    UART_OutString("    Mode = One Shot\r\n");
    UART_OutString("    Rate = 1ms\r\n");

    TimerConfigure(TIMER0_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_B_ONE_SHOT);
    TimerLoadSet(TIMER0_BASE, TIMER_B, SysCtlClockGet() / 1000UL);
    
    IntMasterEnable();
    TimerIntRegister(TIMER0_BASE, TIMER_B, Timer0B_Handler);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMB_TIMEOUT);
    IntEnable(INT_TIMER0B);
    TimerEnable(TIMER0_BASE, TIMER_B);
    
    while (!g_bIntFlag) {
        SysCtlSleep();
    } 
    
    UART_OutString("\n\nOne Shot Timer interrupt received.\n");

    for (;;) {
        SysCtlSleep();
    }

}

