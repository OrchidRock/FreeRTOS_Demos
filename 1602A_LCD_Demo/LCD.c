#include "LCD.h"
#include "systick_wait.h"
#include "driverlib.h"

#define LCD_RS_PIN GPIO_PIN_6
#define LCD_E_PIN GPIO_PIN_7
#define LCD_DATA_PIN 0xFF

#define LCD_RS  (LCD_RS_PIN)
#define LCD_E   (LCD_E_PIN)

void LCD_Init(void) {
    // GPIO Port A.
    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOA );
    while (!SysCtlPeripheralReady( SYSCTL_PERIPH_GPIOA )) {}
    GPIOPinTypeGPIOOutput( GPIO_PORTA_BASE, LCD_RS_PIN | LCD_E_PIN );
    GPIOPadConfigSet( GPIO_PORTA_BASE, LCD_RS_PIN | LCD_E_PIN, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);

    // GPIO Port B.
    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOB );
    while (!SysCtlPeripheralReady( SYSCTL_PERIPH_GPIOB )) {}
    GPIOPinTypeGPIOOutput( GPIO_PORTB_BASE, LCD_DATA_PIN);
    GPIOPadConfigSet( GPIO_PORTB_BASE, LCD_DATA_PIN, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);
    
    GPIOPinWrite( GPIO_PORTA_BASE, LCD_E_PIN | LCD_RS_PIN, 0);
    systick_wait_ms(15);

    LCD_OutCmd(LCD_CMD_FUNC_SET_8BIT); // 8bits, 2 rows, 5x8 font.
    LCD_OutCmd(LCD_CMD_CURSOR_ON);
    LCD_OutCmd(LCD_CMD_ENTRY_MODE);    

    LCD_Clear();
    systick_wait_ms(50);
}

/*
 * RS = 0; RW = 0; E = 1
 */
void LCD_OutCmd(uint8_t command) {
    
    
    // RS = 0; E = 0; RW = 0;
    GPIOPinWrite( GPIO_PORTA_BASE, LCD_RS_PIN | LCD_E_PIN, 0 );
    systick_wait_us(1);

    GPIOPinWrite( GPIO_PORTB_BASE, LCD_DATA_PIN, command );
    systick_wait_us(1);
    
    GPIOPinWrite( GPIO_PORTA_BASE, LCD_E_PIN, LCD_E ); // E = 1
    systick_wait_us(1);
    GPIOPinWrite( GPIO_PORTA_BASE, LCD_E_PIN, 0 ); // E = 0
    
    systick_wait_us(40);
}

void LCD_OutChar(char letter) {
    
    // RS = 1; E = 0; RW = 0;
    GPIOPinWrite( GPIO_PORTA_BASE, LCD_RS_PIN | LCD_E_PIN, LCD_RS );
    systick_wait_us(1);
    
    GPIOPinWrite( GPIO_PORTB_BASE, LCD_DATA_PIN, letter );
    systick_wait_us(1);

    GPIOPinWrite( GPIO_PORTA_BASE, LCD_E_PIN, LCD_E); // E = 1
    systick_wait_us(1);
    GPIOPinWrite( GPIO_PORTA_BASE, LCD_E_PIN, 0 ); // E = 0
    
    systick_wait_us(40);
}


void LCD_OutString(uint8_t addr, size_t len, char *pbuf) {
    size_t i;
    
    LCD_OutCmd(addr);
    for (i = 0; i < len; ++i) {
        LCD_OutChar(pbuf[i]);
    }
}

void LCD_Clear(void) {
    LCD_OutCmd(LCD_CMD_CLEAR_DISPLAY); // clear display
    systick_wait_ms(2);
    LCD_OutCmd(LCD_CMD_RET_HOME); //
    systick_wait_ms(2);
}
