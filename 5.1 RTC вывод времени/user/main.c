/* Includes ------------------------------------------------------------------*/
//#include "MDR32F9Qx_board.h"
#include "MDR32F9Qx_config.h"
#include "MDR32Fx.h"
#include "MDR32F9Qx_timer.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_bkp.h"
#include "MDR32F9Qx_port.h"
#include "lcd.h"
#include "mlt_lcd.h"

#include "time.h"
#include "common.h"

#define U_ALARM_WAIT_TIME 12
#define EVENT_ALARM 0x0001
// Preddelitel' chastoty dlya RTC
// Pri vneshnem kvartse 32768Gts diskretnost' RTC - 1c
#define RTC_PRESCALER 32768
#define RTC_CALIBRATION 0

int32_t i;
time_t TimeStamp;
static char current_time[16];
static char current_date[16];


//static char message[128];


int main (void)
{
	PORT_InitTypeDef PortInitStructure;	
	PORT_InitTypeDef GPIO_user_ini;
	
  RST_CLK_PCLKcmd (RST_CLK_PCLK_BKP | RST_CLK_PCLK_PORTE, ENABLE);
	
	U_MLT_Init();
	
  PORT_StructInit (&PortInitStructure);	
  PortInitStructure.PORT_Pin   = PORT_Pin_6 | PORT_Pin_7;
  PortInitStructure.PORT_MODE =  PORT_MODE_ANALOG;
  PORT_Init (MDR_PORTE, &PortInitStructure);	
	
  RST_CLK_LSEconfig (RST_CLK_LSE_ON); 
  while (RST_CLK_LSEstatus() != SUCCESS);
	 
  BKP_RTC_WaitForUpdate ();          
  BKP_RTCclkSource (BKP_RTC_LSEclk);
	
  BKP_RTC_WaitForUpdate ();       
  BKP_RTC_SetPrescaler (RTC_PRESCALER);
	
  BKP_RTC_WaitForUpdate ();          
  BKP_RTC_Calibration (RTC_CALIBRATION);
	
  BKP_RTC_WaitForUpdate ();         
  BKP_RTC_Enable (ENABLE);
	
	//ust nach datu
	U_RTC_Set_Start_DateTime ();
	
	while(1)
	{
		BKP_RTC_WaitForUpdate ();          
    TimeStamp = U_RTC_Get_DateTime_String(current_date, current_time);
		U_LCD_Task_Function(current_date, current_time, TimeStamp);
	}
}

void U_RTC_Set_Start_DateTime (void)
{
  struct tm timeinfo;
  time_t TimeStamp;	
	 
	// 30.11.2014 09:55:00
  timeinfo.tm_sec  = 0;               // (0..60)
  timeinfo.tm_min  = 46;              // (0..59)
  timeinfo.tm_hour = 19;               // (0..23)
  timeinfo.tm_mday = 25;              //  (1..31)
  timeinfo.tm_mon  = 9;          // Polnykh mesyatsev s nachala goda (0 - yanvar', 11 - dekabr')
  timeinfo.tm_year = 2019 - 1900;    //Polnykh let s 1900 goda

	TimeStamp = mktime (&timeinfo);         // Preobrazovat' strukturu daty i vremeni v otmetku vremeni
  
	BKP_RTC_WaitForUpdate ();               
	U_RTC_Set_DateTime_Stamp(TimeStamp);    

}

//poluch datu
time_t U_RTC_Get_DateTime_Stamp(void)
{
  BKP_RTC_WaitForUpdate ();          
  return BKP_RTC_GetCounter();
}

//ust datu
void U_RTC_Set_DateTime_Stamp(time_t TimeStamp)
{
  BKP_RTC_WaitForUpdate ();          
  BKP_RTC_SetCounter(TimeStamp);
}

// Poluchit' tekushchiye datu i vremya v vide strok formata: "dd.mm.yyyy"  i  "hh:uu:ss"
time_t U_RTC_Get_DateTime_String(char* dateString, char* timeString)
{
  struct tm *timeinfo;
  time_t TimeStamp;

  BKP_RTC_WaitForUpdate ();          
  TimeStamp = BKP_RTC_GetCounter ();
	
  timeinfo = localtime (&TimeStamp);
  strftime (timeString, 16, "%H:%M:%S ", timeinfo);
  strftime (dateString, 16, "%d.%m.%Y", timeinfo);
  //strftime (dateString, 16, "%d %b %Y", timeinfo);
  //strftime (dateString, 16, "%A", timeinfo);
	
	return TimeStamp;
}

void BACKUP_IRQHandler (void)
{
  if(BKP_RTC_GetFlagStatus (BKP_RTC_FLAG_ALRF) == SET)	
	{
   BKP_RTC_ITConfig (BKP_RTC_IT_ALRF, DISABLE);
	}
}