#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "driverlib.h"
#include "systick_wait.h"
#include "LCD.h"

#define LED_RED GPIO_PIN_1
#define LED_BLUE GPIO_PIN_2
#define LED_GREEN GPIO_PIN_3

static void toggle_led(void);

char StrayBirds[] = "Stray birds of summer come to my window to sing and fly away."
                    " And yellow leaves of autumn, which have no songs, flutter and"
                    " fall there with a sign.";

int main()
{
    // 50M HZ
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_RED|LED_BLUE|LED_GREEN);
    
    systick_wait_init();
    
    LCD_Init(); 
    
    //size_t len = strlen(StrayBirds);
    //size_t index = 0;
    
    LCD_OutString(LCD_CMD_FIRSTROW_ADDR, strlen(StrayBirds), StrayBirds);
    
    for(;;) {
        LCD_OutCmd(LCD_CMD_SHIFT_DISPLAY_L);
        systick_wait_ms(300);
        toggle_led();
        //index = index % len;
        //LCD_OutString(LCD_CMD_FIRSTROW_ADDR, ((len-index)>=16)?16:(len-index), StrayBirds+index);
        //index = ((len-index) >= 16) ? (index+16) 
    }
}

void toggle_led(void) {
	ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_BLUE, LED_BLUE);
	systick_wait_ms(100);
    ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_BLUE, 0);
	systick_wait_ms(100);
}
