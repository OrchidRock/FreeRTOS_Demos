#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "driverlib.h"

#define LED_RED GPIO_PIN_1
#define LED_BLUE GPIO_PIN_2
#define LED_GREEN GPIO_PIN_3

static void SysTickHandler(void);

void SysTick_Init(uint32_t period);
void ABCD_Port_Init(void);

static uint32_t clock_hz = 1000;
//static int LED_STATE = 0;

static uint8_t phasecw[4] = { 0x08, 0x04, 0x02, 0x01 };
static uint8_t phaseccw[4] = { 0x01, 0x02, 0x04, 0x08 };
static uint8_t step = 0;

int main()
{
    // 50M HZ
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_RED|LED_BLUE|LED_GREEN);
    
    clock_hz = ROM_SysCtlClockGet();
    
    SysTick_Init(clock_hz); // 1 s
    ABCD_Port_Init();

    for(;;) {
        SysCtlSleep();
    }
}


void SysTick_Init(uint32_t period) {
    SysTickDisable();
    SysTickValueClear();
    SysTickPeriodSet(period);
    
    SysTickIntRegister(SysTickHandler);
    IntPrioritySet(FAULT_SYSTICK, 2);

    SysTickEnable();
}

void SysTickHandler(void) {
    //GPIOPinWrite(GPIO_PORTF_BASE, LED_RED, LED_STATE);
    //LED_STATE ^= LED_RED;
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, phasecw[step]); 
    step = (step + 1) % 4;
}

/*
 * PD0: IN1
 * PD1: IN2
 * PD2: IN3
 * PD3: IN4 
 */
void ABCD_Port_Init() {
    ROM_SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOD );
    while (!ROM_SysCtlPeripheralReady( SYSCTL_PERIPH_GPIOD )) {}

    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
}
