#ifndef __LCD_H__
#define __LCD_H__

#include <stdint.h>
#include <stddef.h>

#define LCD_CMD_CLEAR_DISPLAY   0x01
#define LCD_CMD_RET_HOME        0x02
#define LCD_CMD_MOVE_CURSOR_R   0x06
#define LCD_CMD_MOVE_CURSOR_L   0x08
#define LCD_CMD_SHIFT_DISPLAY_R 0x1C
#define LCD_CMD_SHIFT_DISPLAY_L 0x18
#define LCD_CMD_CURSOR_BLINK    0x0F
#define LCD_CMD_CURSOR_OFF      0x0C
#define LCD_CMD_CURSOR_ON       0x0E
#define LCD_CMD_FUNC_SET_4BIT   0x28
#define LCD_CMD_FUNC_SET_8BIT   0x38
#define LCD_CMD_ENTRY_MODE      0x06
//#define Function_8_bit    0x32
//#define Set5x7FontSize    0x20
#define LCD_CMD_FIRSTROW_ADDR   0x80
#define LCD_CMD_SECONDROW_ADDR  0xc0

/*
 * RS: PA6, E: PA7, DB0-7: PB0-7
 */
void LCD_Init(void);
void LCD_OutCmd(uint8_t command);
void LCD_OutChar(char letetr);
void LCD_OutString(uint8_t addr, size_t len, char* pbuf);
void LCD_Clear(void);

#endif
