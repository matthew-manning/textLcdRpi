#include <stdio.h>
#include <string.h>
#include "lcdlib.h"
#include "i2c.h"

int main(void)
{
	struct i2c_handle LCD;
	char LineBuff[20];
	
	LCD = initLcd();
	
	
	lcdDisplayLine(LCD, "hello world", LINE_ONE);
	while (1)
	{
		printf("type line one\n>>>");
		fgets(LineBuff, 17, stdin);
		LineBuff[strlen(LineBuff)-1] = '\0';//cull new line
		//clrLines(LCD, 1, 0);
		lcdDisplayLine(LCD, LineBuff, LINE_ONE);
		
		printf("type line two\n>>>");
		fgets(LineBuff, 17, stdin);
		LineBuff[strlen(LineBuff)-1] = '\0';//cull new line
		//clrLines(LCD, 0, 1);
		lcdDisplayLine(LCD, LineBuff, LINE_TWO);
			
	}

	return 0;
}
