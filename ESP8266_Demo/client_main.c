#include <stdbool.h>
#include <stdint.h>

#include "driverlib.h"
#include "uartstdio.h"
#include "buttons.h"

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

static void LED_BlueOn(void);
static void LED_BlueOff(void);
static void LED_GreenOn(void);
static void LED_GreenOff(void);
static void LED_RedToggle(void);

uint32_t clock_hz;

const char Fetch[] = "GET /data/2.5/weather?q=Austin&APPID=1bc54f645c5f1c75e681c102ed4bbca4 HTTP/1.1\r\nHost:api.openweathermap.org\r\n\r\n";
//char Fetch[] = "GET /data/2.5/weather?q=Austin%20Texas&APPID=1234567890abcdef1234567890abcdef HTTP/1.1\r\nHost:api.openweathermap.org\r\n\r\n";
// 1) go to http://openweathermap.org/appid#use 
// 2) Register on the Sign up page
// 3) get an API key (APPID) replace the 1234567890abcdef1234567890abcdef with your APPID

char Response[512];

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
    
    UARTStdioConfig(0, 115200, 16000000);
}

int main()
{
    // 50M HZ
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_RED|LED_BLUE|LED_GREEN);
    
    clock_hz = ROM_SysCtlClockGet();
    ROM_SysTickEnable();

    InitConsole(); //UART0 for debuging. 
    ButtonsInit();
        
    UARTprintf("ESP8266 Demo -> \n");
     
    if(!ESP8266_Init(true,false)) {  // initialize with rx echo
        UARTprintf("\r\n---No ESP detected\r\n");
        while(1) {}
    }
  
    UARTprintf("\r\n-----------System starting...\r\n");
    ESP8266_GetVersionNumber();
    
    UARTprintf("\r\n---------- Connect Wifi ---- \r\n") ;
    if(!ESP8266_Connect(true)) {  // connect to access point
        UARTprintf("\r\n---Failure connecting to access point\r\n");
        while(1) {}
    }
    
    for (;;) {
        ESP8266_GetStatus();
        if(ESP8266_MakeTCPConnection("api.openweathermap.org", 80, 0, false)){ // open socket to web server on port 80
            if(ESP8266_Send(Fetch)){  // send request 
                if(ESP8266_Receive(Response, 512)){  // receive response
                    if(strncmp(Response, "HTTP", 4) == 0) { // received HTTP response?
                        LED_BlueOff();
                        LED_GreenOn();
                    }
                }
            }
            ESP8266_CloseTCPConnection();  // close connection   
        }
        while((ButtonsPoll(0, 0) & LEFT_BUTTON) == 0){// wait for touch
        
        } 
        
        LED_GreenOff();
        LED_BlueOn();
        LED_RedToggle();
    }
    for(;;) {
    
    }
}

void LED_BlueOn() {
    GPIOPinWrite(GPIO_PORTF_BASE, LED_BLUE, LED_BLUE);
}
void LED_BlueOff() {
    GPIOPinWrite(GPIO_PORTF_BASE, LED_BLUE, 0);
}

void LED_GreenOn() {
    GPIOPinWrite(GPIO_PORTF_BASE, LED_GREEN, LED_GREEN);
}
void LED_GreenOff() {
    GPIOPinWrite(GPIO_PORTF_BASE, LED_GREEN, 0);
}

void LED_RedToggle() {
    uint32_t value = GPIOPinRead(GPIO_PORTF_BASE, LED_RED);
    GPIOPinWrite(GPIO_PORTF_BASE, LED_RED, LED_RED ^ value);
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
