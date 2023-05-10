#include <stdbool.h>
#include <stdint.h>

#include "driverlib.h"
#include "uartstdio.h"

#define NUMBER_I2C_DATA  3
#define SLAVE_ADDRESS 0x3C

void I2C1Slave_Handler(void);

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

uint32_t pui32DataTx[NUMBER_I2C_DATA] = {0};
uint32_t pui32DataRx[NUMBER_I2C_DATA] = {0};
uint32_t ui32Index;



/*
 * This demo should connect a external pull-up resistance (4.7k) on I2C Bus.
 */
int main()
{

    //ROM_SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    InitConsole();
     
    UARTprintf("I2C Interrupt Example ->\n");
    UARTprintf("    Module = I2C0 & I2C1\n");
    UARTprintf("    Mode = Signal Send/Receive\n");
    UARTprintf("    Rate = 100kbps\n");
    
    // I2C0 Master
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);
    
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);

    // I2C1 Slave
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    
    GPIOPinConfigure(GPIO_PA6_I2C1SCL);
    GPIOPinConfigure(GPIO_PA7_I2C1SDA);
    GPIOPinTypeI2CSCL(GPIO_PORTA_BASE, GPIO_PIN_6);
    GPIOPinTypeI2C(GPIO_PORTA_BASE, GPIO_PIN_7);
    I2CSlaveInit(I2C1_BASE, SLAVE_ADDRESS);
    I2CSlaveEnable(I2C1_BASE);
    
    // Interrupt
    I2CSlaveIntEnableEx(I2C1_BASE, I2C_SLAVE_INT_DATA);
    I2CIntRegister(I2C1_BASE, I2C1Slave_Handler);
    IntMasterEnable(); 

    pui32DataTx[0] = 'I';
    pui32DataTx[1] = '2';
    pui32DataTx[2] = 'C';

    UARTprintf("\nTransferring from: Master -> Slave\n");
    
    // Master Transmission mode.
    for (ui32Index = 0;  ui32Index < NUMBER_I2C_DATA; ui32Index++) {
        UARTprintf("Sending: '%c' ... ", pui32DataTx[ui32Index]);
        I2CMasterSlaveAddrSet(I2C0_BASE, SLAVE_ADDRESS, false);
        I2CMasterDataPut(I2C0_BASE, pui32DataTx[ui32Index]);
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);
        
        while (I2CMasterBusy(I2C0_BASE)) {}

        UARTprintf("Received: '%c'\n", pui32DataRx[ui32Index]);
    }
    
    pui32DataRx[0] = 0;
    pui32DataRx[1] = 0;
    pui32DataRx[2] = 0;

    UARTprintf("\nTransferring from: Slave -> Master\n");
    I2CMasterSlaveAddrSet(I2C0_BASE, SLAVE_ADDRESS, true);
    
    for (ui32Index = 0;  ui32Index < NUMBER_I2C_DATA; ui32Index++) {
        UARTprintf("Sending: '%c' ... ", pui32DataTx[ui32Index]);
        I2CSlaveDataPut(I2C1_BASE, pui32DataTx[ui32Index]);
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
        
        while(!(I2CSlaveStatus(I2C1_BASE) & I2C_SLAVE_ACT_TREQ)) {}
        
        pui32DataRx[ui32Index] = I2CMasterDataGet(I2C0_BASE);
        
        UARTprintf("Received: '%c'\n", pui32DataRx[ui32Index]);
        
    } 
    
    UARTprintf("\nDone.\n\n");
    for (;;) {
        SysCtlSleep();
    }
}

void I2C1Slave_Handler(void) {
    const uint32_t status = I2CSlaveStatus(I2C1_BASE);
    pui32DataRx[ui32Index] = I2CSlaveDataGet(I2C1_BASE);
    I2CSlaveIntClearEx(I2C1_BASE, I2C_SLAVE_INT_DATA);
}
