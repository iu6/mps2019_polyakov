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
#include "mlt_lcd.h"

// Задача по выводу информации на ЖКИ
void U_LCD_Task_Function (int temp);
void U_LCD_Task_Function2 (int temper, int per);
void U_LCD_Task_Function3 (int presq, int per, int ch2, int ch3);


#endif 
