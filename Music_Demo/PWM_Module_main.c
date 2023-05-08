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
    
    // Set the PWM clock to the system clock.
    SysCtlPWMClockSet(SYSCTL_PWMDIV_1); 
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);

    // PB6
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    
    GPIOPinConfigure(GPIO_PB6_M0PWM0);
    GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_6);
    
    PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC); 
    PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, true); 

    clock_hz = ROM_SysCtlClockGet();
    
    // SysTick
    systick_wait_init();
    
    uint32_t tone_length = sizeof(tune) / sizeof(tune[0]); 
    uint32_t i;
    uint32_t delay_ms = 0;
    for (;;) {
        for(i = 0;  i < tone_length; i++) {
            PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, clock_hz / tune[i]);
            PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, PWMGenPeriodGet(PWM0_BASE, PWM_GEN_0) / 2);
            PWMGenEnable(PWM0_BASE, PWM_GEN_0);

            //SysCtlDelay(delay_cycle >> 1);
            delay_ms = durt[i] * 1000UL;
            systick_wait_ms(delay_ms); 
            
            PWMGenDisable(PWM0_BASE, PWM_GEN_0);
        }
    }
}

