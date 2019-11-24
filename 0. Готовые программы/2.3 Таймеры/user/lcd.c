/************************************************************************************
  Проект: Lab1_2
  Программа: Работа с жидкокристаллическим индикатором
  Микроконтроллер: K1986ВЕ92QI
  Устройство: Evaluation Board For MCU MDR32F2Q
  Файл: lсd.c
  Назначение: Вывод информации на жидкокристаллический индикатор
  Компилятор:  Armcc 5.06.0 из комплекта Keil uVision 5.20.0
************************************************************************************/

#include "lcd.h"

// Задача по выводу бегущей строки на ЖКИ
void U_LCD_Task_Function (int temp)
{
	if (temp == 3)
	{
		U_MLT_Disp_Off(0);
		U_MLT_Disp_Off(1);
	}
	else if (temp == 0)
		U_MLT_Put_String ("1. PWM", 4);
	else if (temp == 1) 
		U_MLT_Put_String ("2. Timer Interrupt", 4);
	else 
		U_MLT_Put_String ("3. PWM Period", 4);
}

void U_LCD_Task_Function2 (int temper, int per)
{
	char str1[12];
	char str2[12];
	char str3[16];
	char str4[12];
	char str5[12];
	char str6[16];
	char str7[12];
	int ms;
	sprintf(str1, "%d", temper);
	sprintf(str2, "0x%x", temper);
	sprintf(str3, "P: %s %s", str1, str2);
	sprintf(str4, "%d", per);
	sprintf(str5, "0x%x", per);
	sprintf(str6, "T: %s %s", str4, str5);
	ms = temper * per / 80000;
	sprintf(str7, "t: %d.%d c", ms/100, ms%100);
	//sprintf(str7, "%d", ms);
	U_MLT_Put_String (str3, 4);
	U_MLT_Put_String (str6, 5);
	U_MLT_Put_String (str7, 6);
}

void U_LCD_Task_Function3 (int presq, int per, int ch2, int ch3)
{
	char str1[12];
	char str2[12];
	char str3[16];
	char str4[12];
	char str5[12];
	char str6[12];
	float ms;
	sprintf(str1, "P: 0x%x", presq);
	sprintf(str2, "T: 0x%x", per);
	ms = presq * per / 80000;
	sprintf(str4, "  %4.2f c", ms/100);
	sprintf(str5, "  %4.2f c", (ms/100)*ch2/per);
	sprintf(str6, "  %4.2f c", (ms/100)*ch3/per);
	//sprintf(str5, "%d.%dc %d.%dc", (ms*per/ch2/100), (ms*per/ch2)%100, (ms*per/ch3/100), (ms*per/ch3)%100);
	//sprintf(str7, "%d", ms);
	U_MLT_Put_String (str1, 1);
	U_MLT_Put_String (str2, 2);
	U_MLT_Put_String (str3, 3);
	U_MLT_Put_String (str4, 4);
	U_MLT_Put_String (str5, 5);
	U_MLT_Put_String (str6, 6);
}
