#include "lcd.h"
#include <stdio.h>
#include <stdlib.h>

void U_LCD_Task_Function(uint32_t Data, int i, int isend)
{
	char str1[32];
	char str2[32];
	char str3[32];
	char str4[32];
	sprintf(str1, "%u", i/4 + 1);
	sprintf(str2, "0x%X\n", Data);
	sprintf(str3, "%s%s", ")", str2);
	sprintf(str4, "%s%s", str1, str3);
	U_MLT_Scroll_String ("\xC7\xE0\xEF\xE8\xF1\xE0\xED\xEE:", 0, 14);
	U_MLT_Scroll_String (str4, isend + 1, 0);
}

void U_LCD_Task_FunctionRead(uint32_t Data, int i, int isend)
{
	char str1[32];
	char str2[32];
	char str3[32];
	char str4[32];
	sprintf(str1, "%u", i/4 + 1);
	sprintf(str2, "0x%X\n", Data);
	sprintf(str3, "%s%s", ")", str2);
	sprintf(str4, "%s%s", str1, str3);
	U_MLT_Scroll_String ("\xD1\xF7\xE8\xF2\xE0\xED\xEE:", 4, 14);
	U_MLT_Scroll_String (str4, isend + 5, 0);
}
