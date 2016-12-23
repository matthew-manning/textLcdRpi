#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "lcdlib.h"
#include "i2c.h"

int main(void)
{
	struct i2c_handle LCD;
	char LineBuff[20];
	
	LCD = initLcd();
	
	
	lcdDisplayLine(LCD, "hello world", LINE_ONE);
	lcdDisplayLine(LCD, "this does not over flow asdsadsa", LINE_ONE);
	while (1)
	{
		printf("type line one\n>>>");
		fgets(LineBuff, 18, stdin);
		LineBuff[strlen(LineBuff)-1] = '\0';//cull new line
		//clrLines(LCD, 1, 0);
		lcdDisplayLine(LCD, LineBuff, LINE_ONE);
		sleep(1);
		
		
		printf("type line two\n>>>");
		fgets(LineBuff, 18, stdin);
		LineBuff[strlen(LineBuff)-1] = '\0';//cull new line
		//clrLines(LCD, 0, 1);
		lcdDisplayLine(LCD, LineBuff, LINE_TWO);
		sleep(1);	
	}

	return 0;
}
