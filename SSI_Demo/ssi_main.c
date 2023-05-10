#include <stdbool.h>
#include <stdint.h>

#include "driverlib.h"
#include "uartstdio.h"
#define NUMBER_SSI_DATA  3

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
    uint32_t pui32DataTx[NUMBER_SSI_DATA] = {0};
    uint32_t pui32DataRx[NUMBER_SSI_DATA] = {0};
    uint32_t ui32Index;

    // 50M HZ
    //ROM_SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    InitConsole();
     
    UARTprintf("SSI ->\n");
    UARTprintf("    Mode = SPI\n");
    UARTprintf("    Data = 8-bit\r\n");
    
    // SSI0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    GPIOPinConfigure(GPIO_PA2_SSI0CLK);
    GPIOPinConfigure(GPIO_PA3_SSI0FSS);
    GPIOPinConfigure(GPIO_PA4_SSI0RX);
    GPIOPinConfigure(GPIO_PA5_SSI0TX);

    GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);
    SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 1000000, 8);
    //SSILoopbackEnable(SSI0_BASE);
    SSIEnable(SSI0_BASE);

    // SSI2
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinConfigure(GPIO_PB4_SSI2CLK);
    GPIOPinConfigure(GPIO_PB5_SSI2FSS);
    GPIOPinConfigure(GPIO_PB6_SSI2RX);
    GPIOPinConfigure(GPIO_PB7_SSI2TX);

    GPIOPinTypeSSI(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    SSIConfigSetExpClk(SSI2_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, SSI_MODE_SLAVE, 1000000, 8);
    SSIEnable(SSI2_BASE);
    
    while (SSIDataGetNonBlocking(SSI0_BASE, &pui32DataRx[0])) {} 
    
    pui32DataTx[0] = 'G';
    pui32DataTx[1] = 'Y';
    pui32DataTx[2] = 'L';

    UARTprintf("\nSSI0 Send:  ");
    for (ui32Index = 0;  ui32Index < NUMBER_SSI_DATA; ui32Index++) {
        UARTprintf("'%c' ", pui32DataTx[ui32Index]);
        SSIDataPut(SSI0_BASE, pui32DataTx[ui32Index]);
    }
    
    while(SSIBusy(SSI0_BASE)) {}
    
    UARTprintf("\nSSI0 Receive:  ");
    for (ui32Index = 0; ui32Index < NUMBER_SSI_DATA; ui32Index++) {
        SSIDataGet(SSI0_BASE, &pui32DataRx[ui32Index]);
        pui32DataRx[ui32Index] &= 0x00FF;
        UARTprintf("'%c' ", pui32DataRx[ui32Index]);
    } 
    
    pui32DataRx[0] = 0;
    pui32DataRx[1] = 0;
    pui32DataRx[2] = 0;

    UARTprintf("\nSSI2 Receive:  ");
    for (ui32Index = 0; ui32Index < NUMBER_SSI_DATA; ui32Index++) {
        SSIDataGet(SSI2_BASE, &pui32DataRx[ui32Index]);
        pui32DataRx[ui32Index] &= 0x00FF;
        UARTprintf("'%c' ", pui32DataRx[ui32Index]);
    } 
     
    UARTprintf("\n\nDone.\n");

    for (;;) {
        SysCtlSleep();
    }
}

