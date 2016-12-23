#ifndef __LCD_LIB_H__
#define __LCD_LIB_H__

#define LCD_ADDR 0x27 //i2c bus adddress

#define LCD_CMD 0 //command mode
#define LCD_WR  1//mode for writing text

#define PULSE_WIDTH 500//micro seconds
#define PULSE_DELAY 500//micro seconds 

#define LINE_ONE 0x80 //sets cursor to LHS of line one
#define LINE_TWO 0xc0

#define BL_ON 0x08//back light on 
#define BL_OFF 0x00
#define LCD_ENBL 0x04//enable pin

#define CLR_LINE_ONE 0x01
#define CLR_LINE_TWO 0x02

#include "i2c.h"
#include "stdint.h"

struct i2c_handle initLcd(void);

void lcdWrite(struct i2c_handle LCD, uint8_t Data, uint8_t Mode);

void lcdWrite4B(struct i2c_handle LCD, /*int*/ char Data4B);

int lcdDisplayLine(struct i2c_handle LCD, char * Line, uint8_t LineNum);

void clrLines(struct i2c_handle LCD, int Line1, int Line2);

#endif
