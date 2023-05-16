#ifndef __SYSTICK_WAIT_H__
#define __SYSTICK_WAIT_H__

#include <stdint.h>

void systick_wait_init(void);
void systick_wait_ms(uint32_t);
void systick_wait_us(uint32_t);
void systick_wait_ns(uint32_t);

#endif // __SYSTICK_WAIT_H__
