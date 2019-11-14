#include "lcd.h"
#include <stdio.h>
#include <stdlib.h>

static char message[64];

// Kalibrovochnyye dannyye datchika temperatury
#define ADC_TS_T1 25.0F     // Temperatura v 1-y tochke
#define ADC_TS_D1 0x6A0     // Znacheniye ATSP v 1-y tochke   
#define ADC_TS_T2 60.0F     // Temperatura vo 2-y tochke
#define ADC_TS_D2 0x7A0     // Znacheniye ATSP vo 2-y tochke   

void U_LCD_Task_Function(uint32_t dev)
{
		float temp;
	   	  
    temp = ((int32_t)dev - (int32_t) ADC_TS_D1) * (ADC_TS_T2 - ADC_TS_T1) / (ADC_TS_D2 - ADC_TS_D1) + ADC_TS_T1; 
	
		// Vyvesti rezul'tat izmereniya temperatury na ZHKI v gradusakh Tsel'siya
    sprintf(message , "\xD2\xE5\xEC\xEF\xE5\xF0.:%6.1f\xB0\x43", temp);   
		U_MLT_Put_String (message, 3);
		
		// Vyvesti rezul'tat analogo-tsifrovogo preobrazovaniya
    sprintf(message , "ADC: 0x%1x", dev);   
		U_MLT_Put_String (message, 4);
}
