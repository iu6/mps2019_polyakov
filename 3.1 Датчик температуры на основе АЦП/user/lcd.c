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
#include "lcd.h"

// Задача по выводу бегущей строки на ЖКИ
void U_LCD_Task_Function (int temper)
{
	uint32_t k = 0;
	uint32_t i = 0;
	const char s38[] = "\xD2\xE5\xEC\xEF\xE5\xF0\xE0\xF2\xF3\xF0\xE0 \x33\x38 \xBA\x43";
	const char s37[] = "\xD2\xE5\xEC\xEF\xE5\xF0\xE0\xF2\xF3\xF0\xE0 \x33\x37 \xBA\x43";
	const char s36[] = "\xD2\xE5\xEC\xEF\xE5\xF0\xE0\xF2\xF3\xF0\xE0 \x33\x36 \xBA\x43";
	const char s35[] = "\xD2\xE5\xEC\xEF\xE5\xF0\xE0\xF2\xF3\xF0\xE0 \x33\x35 \xBA\x43";
	const char s34[] = "\xD2\xE5\xEC\xEF\xE5\xF0\xE0\xF2\xF3\xF0\xE0 \x33\x34 \xBA\x43";
	const char s33[] = "\xD2\xE5\xEC\xEF\xE5\xF0\xE0\xF2\xF3\xF0\xE0 \x33\x33 \xBA\x43";
	const char s32[] = "\xD2\xE5\xEC\xEF\xE5\xF0\xE0\xF2\xF3\xF0\xE0 \x33\x32 \xBA\x43";
	const char s31[] = "\xD2\xE5\xEC\xEF\xE5\xF0\xE0\xF2\xF3\xF0\xE0 \x33\x31 \xBA\x43";
	const char s30[] = "\xD2\xE5\xEC\xEF\xE5\xF0\xE0\xF2\xF3\xF0\xE0 \x33\x30 \xBA\x43";
	
	// ??????, ????????? ?? ????????? 
/*	if (temp==39) const char temp_str[] = "x33/x39";
	if (temp==38) const char temp_str[] = "x33/x38";
	if (temp==37) const char temp_str[] = "x33/x37";
	if (temp==36) const char temp_str[] = "x33/x36";
	if (temp==35) const char temp_str[] = "x33/x35";
	if (temp==34) const char temp_str[] = "x33/x34";
	if (temp==33) const char temp_str[] = "x33/x33";
	if (temp==32) const char temp_str[] = "x33/x32";
	if (temp==31) const char temp_str[] = "x33/x31";
	if (temp==30) const char temp_str[] = "x33/x30";
	if (temp==29) const char temp_str[] = "x32/x39";
	if (temp==28) const char temp_str[] = "x32/x39";
	if (temp==27) const char temp_str[] = "x32/x37";
	if (temp==26) const char temp_str[] = "x32/x36"; */
	//const char s[] = "\xD2\xE5\xEC\xEF\xE5\xF0\xE0\xD2\xF3\xF0\xE0"; // "??????? ??????"
	
	if (temper==35) U_MLT_Put_String (s35, 4);
	else if (temper==34) U_MLT_Put_String (s34, 4);
	else if (temper==33) U_MLT_Put_String (s33, 4);
	else if (temper==32) U_MLT_Put_String (s32, 4);
	else if (temper==31) U_MLT_Put_String (s31, 4);
	else if (temper==30) U_MLT_Put_String (s30, 4);
	else if (temper==36) U_MLT_Put_String (s36, 4);
	else if (temper==37) U_MLT_Put_String (s37, 4);
	else if (temper==38) U_MLT_Put_String (s38, 4);
	else U_MLT_Put_String ("0", 4);
		
}
