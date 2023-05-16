#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "driverlib.h"
#include "systick_wait.h"
#include "ST7735.h"
#include "logo.h"

#define LED_RED GPIO_PIN_1
#define LED_BLUE GPIO_PIN_2
#define LED_GREEN GPIO_PIN_3

static void toggle_led(void);

const uint16_t Test2[] = {
  0x0000, 0x4208, 0x8410, 0xC618, 0xFFFF,
  0x001F, 0x07FF, 0x07E0, 0xF800, 0x0000,
  0x001F, 0x07FF, 0x07E0, 0xF800, 0xFFFF,
  0x001F, 0x07FF, 0x07E0, 0xF800, 0x0000,
  0x001F, 0x07FF, 0x07E0, 0xF800, 0xFFFF,
  0x001F, 0x07FF, 0x07E0, 0xF800, 0x0000
};

typedef enum {English, Spanish, Portuguese, French} Language_t;
Language_t myLanguage;
typedef enum {HELLO, GOODBYE, LANGUAGE} phrase_t;
const char Hello_English[] ="Hello";
const char Hello_Spanish[] ="\xADHola!";
const char Hello_Portuguese[] = "Ol\xA0";
const char Hello_French[] ="All\x83";
const char Goodbye_English[]="Goodbye";
const char Goodbye_Spanish[]="Adi\xA2s";
const char Goodbye_Portuguese[] = "Tchau";
const char Goodbye_French[] = "Au revoir";
const char Language_English[]="English";
const char Language_Spanish[]="Espa\xA4ol";
const char Language_Portuguese[]="Portugu\x88s";
const char Language_French[]="Fran\x87" "ais";

const char *Phrases[3][4]={
  {Hello_English,Hello_Spanish,Hello_Portuguese,Hello_French},
  {Goodbye_English,Goodbye_Spanish,Goodbye_Portuguese,Goodbye_French},
  {Language_English,Language_Spanish,Language_Portuguese,Language_French}
};

char StrayBirds[] = "Stray birds of summer come to my window to sing and fly away."
                    " And yellow leaves of autumn, which have no songs, flutter and"
                    " fall there with a sign.";

int main()
{
    uint32_t i, j, x, y, dx, dy;

    // 50M HZ
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!(SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))) {}
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_RED|LED_BLUE|LED_GREEN);
    
    systick_wait_init();
    
    Output_Init();
    
    ST7735_OutString(StrayBirds);
    systick_wait_ms(3000);
    ST7735_FillScreen(0xFFFF);            // set screen to white
    
    ST7735_DrawSmallCircle(10,20,ST7735_Color565(0, 255, 0));
    
    ST7735_DrawSmallCircle(10,20,ST7735_Color565(0, 255, 0));
    ST7735_DrawCircle(50,20,ST7735_Color565(255, 0, 0));
    ST7735_DrawCharS(0, 0, 'c', ST7735_Color565(255, 0, 0), 0, 1);
    ST7735_DrawCharS(6, 0, 'h', ST7735_Color565(255, 128, 0), 0, 1);
    ST7735_DrawCharS(12, 0, 'a', ST7735_Color565(255, 255, 0), 0, 1);
    ST7735_DrawCharS(18, 0, 'r', ST7735_Color565(128, 255, 0), 0, 1);
    ST7735_DrawCharS(30, 0, 'T', ST7735_Color565(0, 255, 0), 0, 1);
    ST7735_DrawCharS(36, 0, 'E', ST7735_Color565(0, 255, 255), 0, 1);
    ST7735_DrawCharS(42, 0, 'S', ST7735_Color565(0, 0, 255), 0, 1);
    ST7735_DrawCharS(48, 0, 'T', ST7735_Color565(128, 0, 255), 0, 1);
  
    ST7735_DrawCharS(0, 8, 'c', ST7735_Color565(255, 0, 0), 0, 2);
    ST7735_DrawCharS(12, 8, 'h', ST7735_Color565(255, 128, 0), 0, 2);
    ST7735_DrawCharS(24, 8, 'a', ST7735_Color565(255, 255, 0), 0, 2);
    ST7735_DrawCharS(36, 8, 'r', ST7735_Color565(128, 255, 0), 0, 2);
    ST7735_DrawCharS(60, 8, 'T', ST7735_Color565(0, 255, 0), 0, 2);
    ST7735_DrawCharS(72, 8, 'E', ST7735_Color565(0, 255, 255), 0, 2);
    ST7735_DrawCharS(84, 8, 'S', ST7735_Color565(0, 0, 255), 0, 2);
    ST7735_DrawCharS(96, 8, 'T', ST7735_Color565(128, 0, 255), 0, 2);
  
    ST7735_DrawCharS(0, 24, 'c', ST7735_Color565(255, 0, 0), 0, 3);
    ST7735_DrawCharS(18, 24, 'h', ST7735_Color565(255, 128, 0), 0, 3);
    ST7735_DrawCharS(36, 24, 'a', ST7735_Color565(255, 255, 0), 0, 3);
    ST7735_DrawCharS(54, 24, 'r', ST7735_Color565(128, 255, 0), 0, 3);
    ST7735_DrawCharS(90, 24, 'T', ST7735_Color565(0, 255, 0), 0, 3);
    ST7735_DrawCharS(108, 24, 'E', ST7735_Color565(0, 255, 255), 0, 3);
    ST7735_DrawCharS(126, 24, 'S', ST7735_Color565(0, 0, 255), 0, 3); // off the screen
    ST7735_DrawCharS(144, 24, 'T', ST7735_Color565(128, 0, 255), 0, 3); // totally off the screen
    
    systick_wait_ms(3000);
    ST7735_FillScreen(ST7735_BLACK);            // set screen to BLACK
    
    ST7735_FillRect(20,20,2,2,ST7735_YELLOW);
    for(i=0;i<80;i++){
        ST7735_FillRect(i,2*i,2,2,ST7735_MAGENTA);
    }
    
    systick_wait_ms(3000);
    ST7735_FillScreen(ST7735_BLACK);            // set screen to BLACK
    ST7735_PlotClear(0,4095);  // range from 0 to 4095
    for(j=0;j<128;j++){
        ST7735_PlotPoints(j*j/2+900-(j*j/256)*j,32*j); // cubic,linear
        ST7735_PlotNext();
    }   // called 128 times
    
    systick_wait_ms(3000);
    ST7735_FillScreen(ST7735_BLACK);            // set screen to BLACK
  
    x = 44;
    y = 159;
    dx = 1;
    dy = 1;
    
    for(;;) {
        systick_wait_ms(300);
        toggle_led();
        ST7735_DrawBitmap(x, y, Logo, 40, 160);
        x = x + dx;
        y = y + dy;
        if((x >= (ST7735_TFTWIDTH - 40 + 15)) || (x <= -15)){
            dx = -1*dx;
        }
        if((y >= (ST7735_TFTHEIGHT + 8)) || (y <= (ST7735_TFTHEIGHT - 8))){
        dy = -1*dy;
        }
    }
}

void toggle_led(void) {
    uint32_t led_state = GPIOPinRead(GPIO_PORTF_BASE, LED_BLUE);
	GPIOPinWrite(GPIO_PORTF_BASE, LED_BLUE, LED_BLUE ^ led_state);
}
