#include <stdbool.h>
#include <stdint.h>

#include "driverlib.h"
#include "uartstdio.h"
#include "buttons.h"
#define NUMBER_OF_INTS  1000

volatile bool g_bFeedWatchdog = true;

void InitConsole(void);
void Watchdog0_Handler(void);
static uint32_t SW1ButtonPressed(void);

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
    FPULazyStackingEnable();
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    
    ButtonsInit();

    InitConsole();
     
    UARTprintf("WatchDog example.\r\n");
    
    UARTprintf("Feeding WatchDog... Press the SW1 button to stop.\n");

    SysCtlPeripheralEnable(SYSCTL_PERIPH_WDOG0);
    IntMasterEnable();

    // LED
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)) {}
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2); // RED
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
    
    WatchdogIntRegister(WATCHDOG0_BASE, Watchdog0_Handler);
    WatchdogReloadSet(WATCHDOG0_BASE, SysCtlClockGet()); // 1HZ
    WatchdogResetEnable(WATCHDOG0_BASE);
    WatchdogEnable(WATCHDOG0_BASE);

    for (;;) {
        // Polls for the SW1 button to be pressed.
        uint8_t ui8Buttons = ButtonsPoll(0, 0);
        
        if (ui8Buttons & LEFT_BUTTON) {
            SW1ButtonPressed();
            while (1) {
            
            }
        } 

        SysCtlSleep();
    }
}

void Watchdog0_Handler(void) {
    if (!g_bFeedWatchdog) {
        return;
    }

    WatchdogIntClear(WATCHDOG0_BASE);
    
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2) ^ GPIO_PIN_2));
}

static uint32_t SW1ButtonPressed(void) {
    g_bFeedWatchdog = false;

    return (0);
}
