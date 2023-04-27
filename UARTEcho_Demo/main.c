#include <stdbool.h>
#include <stdint.h>

#include "driverlib.h"

#define LED_RED GPIO_PIN_1
#define LED_BLUE GPIO_PIN_2
#define LED_GREEN GPIO_PIN_3

#define CR  0x0D
#define LF  0x0A
#define BS  0x08
#define ESC 0x1B
#define SP  0x20
#define DEL 0x7F

static void systick_delay(uint32_t);
static void systick_delay1ms(void);
static void toggle_led(void);
void UART_OutString( const char* message );
void UART_InString(unsigned char* bufPt, uint16_t  max);

uint32_t clock_hz;
static uint32_t BEAT = 200; // ms
static unsigned char buffer[128] = {0};

int main()
{
    // 50M HZ
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_RED|LED_BLUE|LED_GREEN);
    
    clock_hz = ROM_SysCtlClockGet();
    
    ROM_SysTickEnable();
   

    // Enable The UART1  (PB0-PB1)
    SysCtlPeripheralEnable( SYSCTL_PERIPH_UART1 );  
    while ( !SysCtlPeripheralReady( SYSCTL_PERIPH_UART1 )) {}
    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOB );
    while ( !SysCtlPeripheralReady( SYSCTL_PERIPH_GPIOB ));

    // Set GPIO  B0 And B1 as peripheral function.
    GPIOPinTypeUART( GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 );
    GPIOPinConfigure( GPIO_PB0_U1RX );
    GPIOPinConfigure( GPIO_PB1_U1TX );

    // Configure the UART for 8-N-1 operation.
    UARTConfigSetExpClk( UART1_BASE, SysCtlClockGet(), 115200, UART_CONFIG_WLEN_8 | UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE );
    
    UART_OutString("UART Echo - Please Enter: \r\n");

    // Polling
    for (;;) {
        UART_InString(buffer, sizeof(buffer));
        UART_OutString((char*)buffer);
        UART_OutString("\r\n");
        toggle_led();
    }

}


void UART_OutString( const char* message ) {
    const char * pcNextChar = message;
    
	while( *pcNextChar != '\0')
    {
        UARTCharPut(UART1_BASE, *pcNextChar);
		pcNextChar++;
	}

}

void UART_InString(unsigned char* bufPt, uint16_t  max) {
    int length = 0;
    unsigned char character;
    
    character = UARTCharGet(UART1_BASE);
    while (character != CR) {
        if (character == BS) {
            if (length) {
                bufPt--;
                length--;
                //UARTCharPut(UART1_BASE, BS);
            }
        }else if (length < max) {
            *bufPt = character;
            bufPt ++;
            length ++;
            //UARTCharPut(UART1_BASE, character);
        }

        character = UARTCharGet(UART1_BASE);
    }
    *bufPt = 0;
}

void toggle_led(void) {
	ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, LED_BLUE);
	systick_delay(BEAT);
    ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, 0);
	systick_delay(BEAT);
}

void systick_delay1ms(void) {
    ROM_SysTickPeriodSet((clock_hz / 1000UL) - 1);
    SysTickValueClear();
    while(!SysTickCountIsSet()) {}
}

void systick_delay(uint32_t delay) {
    uint32_t i = 0;
    for(; i < delay ; i++) {
        systick_delay1ms();
    }
}
