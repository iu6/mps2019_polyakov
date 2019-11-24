/************************************************************************************
  Проект: Lab1_2
  Программа: Работа с жидкокристаллическим индикатором
  Микроконтроллер: K1986ВЕ92QI
  Устройство: Evaluation Board For MCU MDR32F2Q
  Файл: lсd.h
  Назначение: Вывод информации на жидкокристаллический индикатор
  Компилятор:  Armcc 5.06.0 из комплекта Keil uVision 5.20.0
************************************************************************************/

#ifndef __U_LCD
 #define __U_LCD

#include "common.h"
#include "time.h"
#include "mlt_lcd.h"

// Задача по выводу информации на ЖКИ
void U_LCD_Task_Function (int dac, int i);
void U_LCD_Task_Function2 (int dac, int i);
void U_LCD_Task_Function3(int j);

#endif 
