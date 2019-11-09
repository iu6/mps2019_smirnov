#include "lcd.h"
#include <stdio.h>
#include <stdlib.h>

void U_LCD_Task_Function (time_t temp)
{
	//int k;
	//char str[128];
	char str1[32];
	char str2[16];
	char str3[48];
	time_t pr;
	int msc = 0;
	struct tm *timeinfo;
	
	pr = temp/10;
	timeinfo = localtime(&pr);
	msc = temp%10;
	
	strftime(str1, 16, "%H:%M:%S:", timeinfo);
	sprintf(str2, "%d", msc);
	sprintf(str3, "%s%s", str1, str2);
	
	U_MLT_Put_String (str3, 5);
		
}
