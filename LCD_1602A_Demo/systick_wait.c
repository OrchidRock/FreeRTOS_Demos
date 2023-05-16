#include "systick_wait.h"
#include "driverlib.h"

static uint32_t clock_hz = 1000;

static void systick_wait1ms(void) {
    SysTickPeriodSet((clock_hz / 1000UL) - 1UL);
    SysTickValueClear();
    while (!SysTickCountIsSet()) {} 
}

static void systick_wait1us(void) {
    SysTickPeriodSet((clock_hz / 1000000UL) - 1UL);
    SysTickValueClear();
    while (!SysTickCountIsSet()) {} 
}

void systick_wait_init(void) {
    clock_hz = SysCtlClockGet();
    SysTickEnable();
}

void systick_wait_ms(uint32_t delay) {
    uint32_t i;
    for (i = 0; i < delay; ++i) {
        systick_wait1ms();
    }
}

void systick_wait_us(uint32_t delay) {
    uint32_t i;
    for (i = 0; i < delay; ++i) {
        systick_wait1us();
    }
}
