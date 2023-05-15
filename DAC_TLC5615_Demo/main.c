#include <stdbool.h>
#include <stdint.h>

#include "driverlib.h"
#include "tone_summer.h"
//#include "systick_wait.h"

#define LED_RED GPIO_PIN_1
#define LED_BLUE GPIO_PIN_2
#define LED_GREEN GPIO_PIN_3

#define TLC5615_SCLK_PIN    GPIO_PIN_2 // PA2
#define TLC5615_CS_PIN      GPIO_PIN_3 // PA3
#define TLC5615_DIN_PIN     GPIO_PIN_4 // PA4

#define SINEWAVE_POINT_MAX 32

static void toggle_led(void);
static void DAC_Output(uint16_t value);
static void Timer0B_Handler(void);
static void SysTick_Handler(void);

static uint32_t clock_hz = 0;
static uint32_t g_ui32pwm0_period = 0;
static bool g_bsystickInt = false;

static volatile uint32_t g_ui32SineWavePointIndex = 0;
/*
static float g_fdutyTable[SINEWAVE_POINT_MAX] = {
    0.5, 0.572, 0.636, 0.7, 0.756, 0.8, 0.832, 0.852, 
    0.86, 0.852, 0.832, 0.8, 0.756, 0.7, 0.636, 0.572,
    0.5, 0.428, 0.364, 0.3, 0.244, 0.2, 0.168, 0.148, 
    0.14, 0.148, 0.168, 0.2, 0.244, 0.3, 0.364, 0.428
};*/

static uint16_t g_ui16DigitTable[SINEWAVE_POINT_MAX] = {
    512, 586, 651, 717, 774, 819, 852, 872, 
    881, 872, 852, 819, 774, 717, 651, 586,
    512, 438, 373, 307, 250, 205, 172, 152, 
    143, 152, 172, 205, 250, 307, 373, 438
};

/*
 * 10bit - DAC TLC-5615.
 * PA2: SCLK
 * PA3: CS
 * PA4: DIN
 */
int main()
{
    // 50M HZ
    //ROM_SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_RED|LED_BLUE|LED_GREEN);
    
    clock_hz = ROM_SysCtlClockGet();
    
    // SysTick
    SysTickIntRegister(SysTick_Handler);
    IntPrioritySet(FAULT_SYSTICK, 2);
    SysTickEnable();
    SysTickIntDisable();

    //ROM_SysTickEnable();
    //systick_wait_init(); 
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); 
    while (!(SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA))) {}
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, TLC5615_DIN_PIN | TLC5615_SCLK_PIN| TLC5615_CS_PIN);

    // Init 
    GPIOPinWrite(GPIO_PORTA_BASE, TLC5615_CS_PIN, TLC5615_CS_PIN);
    GPIOPinWrite(GPIO_PORTA_BASE, TLC5615_DIN_PIN | TLC5615_SCLK_PIN, 0);

    // Timer 0B
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_B_PERIODIC);
    
    IntMasterEnable();
    TimerIntRegister(TIMER0_BASE, TIMER_B, Timer0B_Handler);
    IntPrioritySet(INT_TIMER0B, 3);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMB_TIMEOUT);
    

    uint32_t tone_length = sizeof(tune) / sizeof(tune[0]); 
    uint32_t i;
    uint32_t delay_ms = 0;
    
    TimerLoadSet(TIMER0_BASE, TIMER_B, clock_hz / (262 * SINEWAVE_POINT_MAX));
    TimerEnable(TIMER0_BASE, TIMER_B); 
     
    for (;;) {
        for (i = 0; i < tone_length; ++i) {
            TimerLoadSet(TIMER0_BASE, TIMER_B, clock_hz / (tune[i] * SINEWAVE_POINT_MAX));
            TimerEnable(TIMER0_BASE, TIMER_B); 
            
            toggle_led(); 
            
            g_bsystickInt = false;
            SysTickPeriodSet(clock_hz * durt[i]);
            SysTickIntEnable();
            
            while (!g_bsystickInt) {
                DAC_Output(g_ui16DigitTable[g_ui32SineWavePointIndex]); 
            }
             
            TimerDisable(TIMER0_BASE, TIMER_B);
        }

    }

}

void toggle_led(void) {
    uint32_t led_state = GPIOPinRead(GPIO_PORTF_BASE, LED_BLUE);
	GPIOPinWrite(GPIO_PORTF_BASE, LED_BLUE, LED_BLUE ^ led_state);
}

void DAC_Output(uint16_t value) {
    uint8_t i;
    uint16_t temp = value << 6;
    
    // CS : 0, SCLK: 0
    GPIOPinWrite(GPIO_PORTA_BASE, TLC5615_CS_PIN, 0);
    GPIOPinWrite(GPIO_PORTA_BASE, TLC5615_SCLK_PIN, 0);

    for (i = 0; i < 12; ++i) {
        if (temp & 0x8000) {
            GPIOPinWrite(GPIO_PORTA_BASE, TLC5615_DIN_PIN, TLC5615_DIN_PIN);
        }else{
            GPIOPinWrite(GPIO_PORTA_BASE, TLC5615_DIN_PIN, 0);
        }
        // SCLK: 1
        GPIOPinWrite(GPIO_PORTA_BASE, TLC5615_SCLK_PIN, TLC5615_SCLK_PIN);
        
        temp <<= 1;

        // SCLK: 0
        GPIOPinWrite(GPIO_PORTA_BASE, TLC5615_SCLK_PIN, 0);
    }
    
    // CS: 1
    GPIOPinWrite(GPIO_PORTA_BASE, TLC5615_CS_PIN, TLC5615_CS_PIN);
    
    // SCLK: 0    
    GPIOPinWrite(GPIO_PORTA_BASE, TLC5615_SCLK_PIN, 0);
}

void Timer0B_Handler(void) {
    TimerIntClear(TIMER0_BASE, TIMER_TIMB_TIMEOUT);
    g_ui32SineWavePointIndex = (g_ui32SineWavePointIndex + 1) % SINEWAVE_POINT_MAX;

}

void SysTick_Handler(void) {
    g_bsystickInt = true; 
}
