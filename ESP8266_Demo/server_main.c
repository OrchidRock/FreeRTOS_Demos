#include <stdbool.h>
#include <string.h> 
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

void LED_BlueOn(void);
void LED_BlueOff(void);
static void LED_GreenOn(void);
static void LED_GreenOff(void);
static void LED_RedToggle(void);

uint32_t clock_hz;

const char formBody[] = 
  "<!DOCTYPE html><html><body><center> \
<h1>Enter a message to send to your microcontroller:</h1> \
  <form> \
  <input type=\"text\" name=\"message\" value=\"Hello ESP8266!\"> \
  <br><input type=\"submit\" value=\"Go!\"> \
  </form></center></body></html>";

const char statusBody[] = 
  "<!DOCTYPE html><html><body><center> \
  <h1>Message sent successfully!</h1> \
  </body></html>";

char HTTP_Request[64];

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

int HTTP_ServePage(const char* body){
    char header[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\nContent-Length: ";
    
    char contentLength[16];
    sprintf(contentLength, "%d\r\n\r\n", strlen(body));

    if(!ESP8266_SendBuffered(header)) return 0;
    if(!ESP8266_SendBuffered(contentLength)) return 0;
    if(!ESP8266_SendBuffered(body)) return 0;    
  
    if(!ESP8266_SendBufferedStatus()) return 0;
  
    return 1;
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
    
    if (!ESP8266_StartServer(8089, 600)) {
        UARTprintf("\r\n---Failure starting server\r\n");
        while(1) {}
    } 
    
    LED_BlueOn();

    for (;;) {
        // Wait for connection
        ESP8266_WaitForConnection();
    
        // Receive request
        if(!ESP8266_Receive(HTTP_Request, 64)){
            ESP8266_CloseTCPConnection();
            continue;
        }
        
        // check for HTTP GET
        if(strncmp(HTTP_Request, "GET", 3) == 0) {
            char* messagePtr = strstr(HTTP_Request, "?message=");
            if(messagePtr) {
                // Process form reply
                if(HTTP_ServePage(statusBody)) {
                    LED_GreenOff();
                    LED_BlueOn();
                }
                // Terminate message at first separating space
                char* messageEnd = strchr(messagePtr, ' ');
                if(messageEnd) *messageEnd = 0;  // terminate with null character
                // Print message on terminal
                UARTprintf("\r\n---Message from the Internet: %s\r\n", messagePtr + 9);
            } else {
                // Serve web page
                if(HTTP_ServePage(formBody)) {
                    LED_BlueOff();
                    LED_GreenOn();
                }
            }
        } else {
            // handle data that may be sent via means other than HTTP GET
        }
        LED_RedToggle();
        ESP8266_CloseTCPConnection();
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
