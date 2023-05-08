#include <stdbool.h>
#include <stdint.h>

#include "driverlib.h"
#include "uartstdio.h"
#define NUMBER_OF_INTS  1000

static volatile uint32_t g_ui32Counter = 0;

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
    
    //UARTConfigSetExpClk(UART0_BASE,16000000, 115200, UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE | UART_CONFIG_WLEN_8);
    //UARTEnable(UART0_BASE);
    UARTStdioConfig(0, 115200, 16000000);
}

void Timer0B_Handler(void) {
    TimerIntClear(TIMER0_BASE, TIMER_TIMB_TIMEOUT);
    g_ui32Counter ++;

    if (g_ui32Counter == NUMBER_OF_INTS) {
        IntDisable(INT_TIMER0B);
        TimerIntDisable(TIMER0_BASE, TIMER_TIMB_TIMEOUT);
        TimerIntClear(TIMER0_BASE, TIMER_TIMB_TIMEOUT);
    }
}

int main()
{
    uint32_t ui32PrevCount = 0;

    // 50M HZ
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    //ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    
    InitConsole();
     
    UARTprintf("16-Bit Timer Interrupt ->\r\n");
    UARTprintf("    Timer = Timer0B\r\n");
    UARTprintf("    Mode = Periodic\r\n");
    UARTprintf("    Number of interrupts = %d\r\n", NUMBER_OF_INTS);
    UARTprintf("    Rate = 1ms\r\n");

    TimerConfigure(TIMER0_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_B_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_B, SysCtlClockGet() / 1000UL);
    
    IntMasterEnable();
    TimerIntRegister(TIMER0_BASE, TIMER_B, Timer0B_Handler);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMB_TIMEOUT);
    IntEnable(INT_TIMER0B);

    g_ui32Counter = 0;

    TimerEnable(TIMER0_BASE, TIMER_B);
    
    

    for (;;) {
        if (ui32PrevCount != g_ui32Counter) {
            UARTprintf("Number of interrupts: %d\r\n", g_ui32Counter);
            ui32PrevCount = g_ui32Counter;
        }
        SysCtlSleep();
    }
}

