#include <stdbool.h>
#include <stdint.h>

#include "driverlib.h"
#include "tone_summer.h"
#include "systick_wait.h"

int main()
{
    uint32_t clock_hz = 0;

    // 80M HZ
    //ROM_SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN | SYSCTL_RCC2_DIV400);
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    
    //Timer 3A
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);
    
    // PB2
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinConfigure(GPIO_PB2_T3CCP0);
    GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_2);
    
    clock_hz = ROM_SysCtlClockGet();
     
    TimerConfigure(TIMER3_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PWM);
    
    // SysTick
    systick_wait_init();
    
    uint32_t tone_length = sizeof(tune) / sizeof(tune[0]); 
    uint32_t i;
    uint32_t delay_ms = 0;
    for (;;) {
        for(i = 0;  i < tone_length; i++) {
            
            TimerLoadSet(TIMER3_BASE, TIMER_A, clock_hz / tune[i]);
            TimerMatchSet(TIMER3_BASE, TIMER_A, TimerLoadGet(TIMER3_BASE, TIMER_A) / 2);
            TimerEnable(TIMER3_BASE, TIMER_A);
            
            //SysCtlDelay(delay_cycle >> 1);
            delay_ms = durt[i] * 1000UL;
            systick_wait_ms(delay_ms); 
            
            TimerDisable(TIMER3_BASE, TIMER_A);
        }
    }
}

