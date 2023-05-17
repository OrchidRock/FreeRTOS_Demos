//UART1.h
//Dung Nguyen
//Wally Guzman

#include <stdint.h> 

#define UART1_FR_R  HWREG(UART1_BASE + UART_O_FR)

void UART1_Init(uint32_t baud);

//--------UART1_OutChar--------
// Prints a character to UART1
// Inputs: character to transmit
// Outputs: none
void UART1_OutChar(char data);
void UART1_OutString(char *pt);

int32_t UART1_InChar(void);

//--------UART1_EnableRXInterrupt--------
// Enables UART1 RX interrupt
// Inputs: none
// Outputs: none
void UART1_EnableRXInterrupt(void);

//--------UART1_DisableRXInterrupt--------
// Disables UART1 RX interrupt
// Inputs: none
// Outputs: none
void UART1_DisableRXInterrupt(void);

//------------UART1_FinishOutput------------
// Wait for all transmission to finish
// Input: none
// Output: none
void UART1_FinishOutput(void);

uint32_t UART1_IntStatus(void);
void UART1_IntClear(uint32_t flags); 
