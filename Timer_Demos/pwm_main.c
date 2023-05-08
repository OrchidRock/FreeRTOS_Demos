#include <stdbool.h>
#include <stdint.h>

#include "driverlib.h"
#include "uartstdio.h"

void
PrintRunningDots(void)
{
    UARTprintf(". ");
    SysCtlDelay(SysCtlClockGet() / 3);
    UARTprintf(". ");
    SysCtlDelay(SysCtlClockGet() / 3);
    UARTprintf(". ");
    SysCtlDelay(SysCtlClockGet() / 3);
    UARTprintf(". ");
    SysCtlDelay(SysCtlClockGet() / 3);
    UARTprintf(". ");
    SysCtlDelay(SysCtlClockGet() / 3);
    UARTprintf("\b\b\b\b\b\b\b\b\b\b");
    UARTprintf("          ");
    UARTprintf("\b\b\b\b\b\b\b\b\b\b");
    SysCtlDelay(SysCtlClockGet() / 3);
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

int main()
{
    uint32_t ui32PrevCount = 0;

    // 50M HZ
    //ROM_SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    
    //Timer 3A
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);
    
    // PB2
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinConfigure(GPIO_PB2_T3CCP0);

    InitConsole();
    
    GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_2);
    

    UARTprintf("16-Bit Timer PWM ->\r\n");
    UARTprintf("    Timer = Timer3A\r\n");
    UARTprintf("    Mode = PWM\r\n");
    UARTprintf("    Duty Cycle = 66%%\r\n");
    UARTprintf("    Rate = 1ms\r\n");
    UARTprintf("Generating PWM on CCP3 -> \r\n");

    TimerConfigure(TIMER3_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PWM);
    TimerLoadSet(TIMER3_BASE, TIMER_A, 60000);
    TimerMatchSet(TIMER3_BASE, TIMER_A, TimerLoadGet(TIMER3_BASE, TIMER_A) / 3);

    TimerEnable(TIMER3_BASE, TIMER_A);
    
    

    for (;;) {
        PrintRunningDots();
    }
}

