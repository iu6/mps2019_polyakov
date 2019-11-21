#include "lcd.h"
#include <stdio.h>
#include <stdlib.h>

float Udac, Ures, Ui;
uint32_t DU; 
#define U_ADC_U  3.190F        
#define U_ADC_D  0xF90

void U_LCD_Task_Function (int dac, int i)
{
	char str1[12];
	char str2[12];
	char strR[12];
	char strD[12];
	
	//itoa(k, buffer, 10);
	sprintf(str1, "  %d", dac);
	sprintf(str2, "  %d", i);
	
	Udac = (U_ADC_U / U_ADC_D) * dac;
	Ui = (U_ADC_U / U_ADC_D) * i;
	
	sprintf(strD , "U = %5.3fV", Udac);
	sprintf(strR , "U = %5.3fV", Ui);
	
	U_MLT_Put_String ("DAC:", 1);
	U_MLT_Put_String (str1, 2);
	U_MLT_Put_String (strD, 3);
	U_MLT_Put_String ("Comp:", 4);
	U_MLT_Put_String (str2, 5);
	U_MLT_Put_String (strR, 6);
}

void U_LCD_Task_Function2 (int dac, int result)
{
	char str1[12];
	char str2[12];
	char strR[12];
	char strD[12];
	
	//itoa(k, buffer, 10);
	sprintf(str1, "  %d", dac);
	sprintf(str2, "  %d", result);
	
	Ures = (U_ADC_U / U_ADC_D) * result;
	Udac = (U_ADC_U / U_ADC_D) * dac;
	
	sprintf(strR , "U = %5.3fV", Ures);
	sprintf(strD , "U = %5.3fV", Udac);
	
	U_MLT_Put_String ("DAC:", 2);
	U_MLT_Put_String (str1, 3);
	U_MLT_Put_String (strD, 4);
	U_MLT_Put_String ("ADC:", 5);
	U_MLT_Put_String (str2, 6);
	U_MLT_Put_String (strR, 7);
}


void U_LCD_Task_Function3(int j)
{
	if (j == 2)
	{
		U_MLT_Disp_Off(0);
		U_MLT_Disp_Off(1);
	}
	else if (j == 0)
		U_MLT_Put_String ("1. DAC - ADC", 4);
	else U_MLT_Put_String ("2. DAC - COMP", 4);
}