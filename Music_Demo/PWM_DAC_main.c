#include <stdbool.h>
#include <stdint.h>

#include "driverlib.h"
#include "tone_summer.h"
#include "systick_wait.h"

#define LED_RED GPIO_PIN_1
#define LED_BLUE GPIO_PIN_2
#define LED_GREEN GPIO_PIN_3

#define SINEWAVE_POINT_MAX 32

static void toggle_led(void);
void Timer0B_Handler(void);

static uint32_t clock_hz = 0;
static uint32_t g_ui32pwm0_period = 0;

static volatile uint32_t g_ui32SineWavePointIndex = 0;
static float g_fdutyTable[SINEWAVE_POINT_MAX] = {
    0.5, 0.572, 0.636, 0.7, 0.756, 0.8, 0.832, 0.852, 
    0.86, 0.852, 0.832, 0.8, 0.756, 0.7, 0.636, 0.572,
    0.5, 0.428, 0.364, 0.3, 0.244, 0.2, 0.168, 0.148, 
    0.14, 0.148, 0.168, 0.2, 0.244, 0.3, 0.364, 0.428
};

/*
 * PWM DAC: we need a external RC filter(LPF) to do DAC tansform.
 */
int main()
{
    // 50M HZ
    //ROM_SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_RED|LED_BLUE|LED_GREEN);
    
    clock_hz = ROM_SysCtlClockGet();
    
    ROM_SysTickEnable();
    systick_wait_init(); 

    // PWM0 Init
    SysCtlPWMClockSet(SYSCTL_PWMDIV_1); 
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    
    GPIOPinConfigure(GPIO_PB6_M0PWM0);
    GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_6);
    
    PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC); 
    PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, true); 
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, clock_hz / 100000); // 100 kHZ
    g_ui32pwm0_period = PWMGenPeriodGet(PWM0_BASE, PWM_GEN_0); 
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, g_ui32pwm0_period >> 1); // 50% duty
    PWMGenEnable(PWM0_BASE, PWM_GEN_0);

    // Timer 0B
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_B_PERIODIC);
    // Prescale 

    IntMasterEnable();
    TimerIntRegister(TIMER0_BASE, TIMER_B, Timer0B_Handler);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMB_TIMEOUT);
    
    uint32_t tone_length = sizeof(tune) / sizeof(tune[0]); 
    uint32_t i;
    uint32_t delay_ms = 0;
        
    for (;;) {
        for (i = 0; i < tone_length; ++i) {
            TimerLoadSet(TIMER0_BASE, TIMER_B, clock_hz / (tune[i] * SINEWAVE_POINT_MAX));
            TimerEnable(TIMER0_BASE, TIMER_B); 
            
            toggle_led(); 
            
            delay_ms = durt[i] * 1000UL;
            systick_wait_ms(delay_ms); 

            TimerDisable(TIMER0_BASE, TIMER_B); 
        }
    }

}

void toggle_led(void) {
    uint32_t led_state = GPIOPinRead(GPIO_PORTF_BASE, LED_BLUE);
	GPIOPinWrite(GPIO_PORTF_BASE, LED_BLUE, LED_BLUE ^ led_state);
}

void Timer0B_Handler(void) {
    TimerIntClear(TIMER0_BASE, TIMER_TIMB_TIMEOUT);
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, g_ui32pwm0_period * g_fdutyTable[g_ui32SineWavePointIndex]);
    g_ui32SineWavePointIndex = (g_ui32SineWavePointIndex + 1) % SINEWAVE_POINT_MAX;
}
