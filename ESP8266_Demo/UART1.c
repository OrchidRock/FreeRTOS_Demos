//UART1.c
//Dung Nguyen
//Wally Guzman

#include <stdint.h>
#include "UART1.h"
#include "driverlib.h"

extern void UART1_Handler(void);

/*
 * PB0:RX, PB1: TX 
 */
void UART1_Init(uint32_t baud){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB)) {} 
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_UART1)) {}
    
    // Set GPIO  B0 And B1 as peripheral function.
    GPIOPinTypeUART( GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 );
    GPIOPinConfigure( GPIO_PB0_U1RX );
    GPIOPinConfigure( GPIO_PB1_U1TX );
    
    UARTDisable(UART1_BASE);

    UARTFIFOEnable(UART1_BASE);
    UARTFIFOLevelSet(UART1_BASE, UART_FIFO_RX4_8, UART_FIFO_TX4_8);
    UARTTxIntModeSet(UART1_BASE, UART_TXINT_MODE_FIFO);
    
    // Configure the UART for 8-N-1 operation.
    UARTConfigSetExpClk( UART1_BASE, SysCtlClockGet(), baud, UART_CONFIG_WLEN_8 | UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE );
    
    UARTIntRegister(UART1_BASE, UART1_Handler); 
    UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT);  
    
    UARTEnable(UART1_BASE);
}

//--------UART1_OutChar--------
// Prints a character to UART1
// Inputs: character to transmit
// Outputs: none
void UART1_OutChar(char data){
    while (UARTBusy(UART1_BASE)) {}
    UARTCharPut(UART1_BASE, data);
}

void UART1_OutString(char *pt){
  while(*pt){
    UART1_OutChar(*pt);
    pt++;
  }
}

int32_t UART1_InChar(void) {
    UARTCharGet(UART1_BASE);
}

//--------UART1_EnableRXInterrupt--------
// - Enables UART1 RX interrupt
// Inputs: none
// Outputs: none
void UART1_EnableRXInterrupt(void){
    IntEnable(INT_UART1);
}

//--------UART1_DisableRXInterrupt--------
// - Disables UART1 RX interrupt
// Inputs: none
// Outputs: none
void UART1_DisableRXInterrupt(void){
    IntDisable(INT_UART1);
}

//------------UART1_FinishOutput------------
// Wait for all transmission to finish
// Input: none
// Output: none
void UART1_FinishOutput(void){
  // Wait for entire tx message to be sent
  // UART Transmit FIFO Empty =1, when Tx done
  while((HWREG(UART1_BASE + UART_O_FR) & UART_FR_TXFE) == 0);
  // wait until not busy
  while((UARTBusy(UART1_BASE)));
}


uint32_t UART1_IntStatus(void) {
    return UARTIntStatus(UART1_BASE, false);
}

void UART1_IntClear(uint32_t flags) {
    UARTIntClear(UART1_BASE, flags);
}
