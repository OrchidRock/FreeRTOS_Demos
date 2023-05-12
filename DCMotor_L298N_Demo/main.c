#include <stdbool.h>
#include <stdint.h>

#include "driverlib.h"
#include "systick_wait.h"

#define LED_RED GPIO_PIN_1
#define LED_BLUE GPIO_PIN_2
#define LED_GREEN GPIO_PIN_3

void toggle_led(void);
void GPIOPortF_Handler(void);

int main()
{
    uint32_t clock_hz = 0;
    uint32_t pui32_pwm_period = 0;
    uint32_t duty = 0;
    // 80M HZ
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN | SYSCTL_RCC2_DIV400);
    //ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while (!ROM_SysCtlPeripheralReady( SYSCTL_PERIPH_GPIOF )) {}
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_RED|LED_BLUE|LED_GREEN);
    
    GPIOUnlockPin(GPIO_PORTF_BASE, GPIO_PIN_4); // Important !!!
    ROM_GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4); //SW2: PF4
     
    // Interrupt
    IntMasterEnable();
    GPIOIntTypeSet( GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_HIGH_LEVEL );
    GPIOIntRegister( GPIO_PORTF_BASE, GPIOPortF_Handler);
    GPIOIntClear( GPIO_PORTF_BASE, GPIO_INT_PIN_4 ); 
    //GPIOIntEnable( GPIO_PORTF_BASE, GPIO_INT_PIN_4 );
    IntPrioritySet( INT_GPIOF, 2 );

    clock_hz = ROM_SysCtlClockGet();
    
    // Set the PWM clock to the system clock.
    SysCtlPWMClockSet(SYSCTL_PWMDIV_1); 
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);

    // PB6: IN2
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    while (!ROM_SysCtlPeripheralReady( SYSCTL_PERIPH_GPIOB )) {}
    
    GPIOPinConfigure(GPIO_PB6_M0PWM0);
    GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_6);
    PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC); 
    PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, true); 
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, clock_hz / 1000UL); // 1k Hz
    pui32_pwm_period = PWMGenPeriodGet(PWM0_BASE, PWM_GEN_0);
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, pui32_pwm_period * (float)0.7); // 50% duty
    //PWMGenEnable(PWM0_BASE, PWM_GEN_0);
    
    // PB7: IN3 
    GPIOPinConfigure(GPIO_PB7_M0PWM1);
    GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_7);
    PWMGenConfigure(PWM0_BASE, PWM_GEN_1, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC); 
    PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, true); 
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, clock_hz / 1000UL); // 1k Hz
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, 0); // 0% duty
    PWMGenEnable(PWM0_BASE, PWM_GEN_1);
    
    // SysTick
    systick_wait_init();
    
    for (;;) {
        
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, pui32_pwm_period * (float)0.9);
        PWMGenEnable(PWM0_BASE, PWM_GEN_0);
        systick_wait_ms(5000); // 5s
        
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, pui32_pwm_period * (float)0.5);
        
        systick_wait_ms(5000); // 5s
        
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, pui32_pwm_period * (float)0.0);
        
        systick_wait_ms(2000); // 2s
        
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, pui32_pwm_period * (float)0.7);

        systick_wait_ms(5000); // 5s

        // Free
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, 0);
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, 0);
        
        systick_wait_ms(2000); // 2s

        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, pui32_pwm_period * (float)0.9);
        
        systick_wait_ms(5000); // 5s
        
        // STOP
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, pui32_pwm_period);
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, pui32_pwm_period);
        
        SysCtlSleep();
    }
}

void toggle_led(void) {
    uint32_t led_state = GPIOPinRead(GPIO_PORTF_BASE, LED_BLUE);
	GPIOPinWrite(GPIO_PORTF_BASE, LED_BLUE, LED_BLUE ^ led_state);
}

// Invert Motor
void GPIOPortF_Handler(void) {
    GPIOIntClear( GPIO_PORTF_BASE, GPIO_INT_PIN_4 ); 
    toggle_led();
    //uint32_t dir_state = GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_2 | GPIO_PIN_3);
    
    // STOP
    //GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2 | GPIO_PIN_3, GPIO_PIN_2 | GPIO_PIN_3); 
}
