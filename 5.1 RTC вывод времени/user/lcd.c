#include "lcd.h"
#include <stdio.h>
#include <stdlib.h>


// Bufer dlya formirovaniya soobshcheniy, vyvodimykh na ZHKI
static char message[128];

void U_LCD_Task_Function(char* current_date, char* current_time, time_t temp)
{
	// Vyvesti na ZHKI tekushchuyu datu
		U_MLT_Put_String(current_date, 4);

		// Vyvesti na ZHKI tekushcheye vremya
		U_MLT_Put_String(current_time, 5);
		
		// Vyvesti na ZHKI otmetku vremeni
    sprintf (message, "%u", temp);		
		U_MLT_Put_String(message, 6);
		
}
