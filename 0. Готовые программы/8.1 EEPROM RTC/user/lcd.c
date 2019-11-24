#include "lcd.h"
#include <stdio.h>
#include <stdlib.h>


// Bufer dlya formirovaniya soobshcheniy, vyvodimykh na ZHKI
static char message[128];

void U_LCD_Task_Function(char* current_date, char* current_time, time_t temp)
{
	// Vyvesti na ZHKI tekushchuyu datu
		U_MLT_Put_String(current_date, 2);

		// Vyvesti na ZHKI tekushcheye vremya
		U_MLT_Put_String(current_time, 3);
		
		// Vyvesti na ZHKI otmetku vremeni
    sprintf (message, "%u", temp);		
		U_MLT_Put_String(message, 4);
}

void U_LCD_Task_Function2(int x, int z)
{
	char str1[12];
	int y;
	y = abs(x - z) - 1;
  U_MLT_Put_String("\xC1\xFB\xEBo o\xF2\xEA\xEB\xFE\xF7\xE5\xED\o", 5);
	if (y<60)
		sprintf(str1, "%dc", y);
	else if (y<3600)
		sprintf(str1, "%d\xEC\xE8\xED %dc", y/60, y%60);
	else
		sprintf(str1, "%d\xF7 %d\xEC\xE8\xED %dc", y/3600, y/60%60, y%60);
	if (y>86400)
		sprintf(str1, "error", y/60, y%60);
	U_MLT_Put_String(str1, 6);
	

}
