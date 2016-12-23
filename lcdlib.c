#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "i2c.h"
#include "lcdlib.h"


struct i2c_handle initLcd(void)
{
	struct i2c_handle LCD;
	LCD = *i2c_init(1, LCD_ADDR, 0);//this assignment okay?
	lcdWrite(LCD, 0x33, LCD_CMD);
	lcdWrite(LCD, 0x32, LCD_CMD);
	lcdWrite(LCD, 0x06, LCD_CMD);
	lcdWrite(LCD, 0x0c, LCD_CMD);
	lcdWrite(LCD, 0x28, LCD_CMD);
	lcdWrite(LCD, 0x01, LCD_CMD);
	usleep(PULSE_WIDTH);
	
	return LCD;
}

void lcdWrite(struct i2c_handle LCD, uint8_t Data, uint8_t Mode)
{
	//writes a 8 bit value out to the lcd
	uint8_t L4B; //left 4 bits
	uint8_t R4B; //right 4 bits
	
	L4B = Mode | (Data & 0xf0) | BL_ON;
	R4B = Mode | ( (Data << 4) & 0xF0 ) | BL_ON;
	
	
	lcdWrite4B(LCD, L4B);
	lcdWrite4B(LCD, R4B);
	
}

void lcdWrite4B(struct i2c_handle LCD, char/*int*/ Data4B)
{
	//writes out an 8 bit block with 4 bits of data and 4 bits of flags
	i2c_write_to_reg(&LCD, 0, &Data4B, 1);//guessing register address
	usleep(PULSE_WIDTH);
	Data4B |= LCD_ENBL;
	i2c_write_to_reg(&LCD, 0, &Data4B, 1);
	usleep(PULSE_WIDTH);
	Data4B &= ~LCD_ENBL;
	i2c_write_to_reg(&LCD, 0, &Data4B, 1);
	Data4B |= LCD_ENBL;
}

int lcdDisplayLine(struct i2c_handle LCD, char * Line, uint8_t LineNum)
{
	int Length;
	int i;
	char WriteBuf[17] = "                ";//16 spaces
	
	
	
	Length = strlen(Line);
	if(Length > 16)
	{
		Length = 16; //culls to 16 characters
	}
	
	strncpy(WriteBuf, Line, Length);
	printf("%s\n", WriteBuf);
	
	
	for(i = 0; i < Length; i++ )
	{
		lcdWrite(LCD, Line[i], LCD_WR);
	}
	
	return Length;
}

void clrLines(struct i2c_handle LCD, int Line1, int Line2)
{
	//Line1 and Line2 are flags to say if that line should be cleared
	if(Line1)
	{
		lcdWrite(LCD, CLR_LINE_ONE, LCD_CMD);
	}
	
	if(Line2)
	{
		lcdWrite(LCD, CLR_LINE_TWO, LCD_CMD);
	}
}
