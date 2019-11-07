/* Includes ------------------------------------------------------------------*/
#include "MDR32Fx.h"
#include "MDR32F9Qx_eeprom.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"

#define EEPROM_PAGE_SIZE             (4*1024)
#define MAIN_EEPAGE                  5
#define UP PORT_Pin_5//define pin 5  to UP butto

/* Private variables ---------------------------------------------------------*/
static PORT_InitTypeDef PORT_InitStructure;

uint32_t Pseudo_Rand(uint32_t addr)
{
  uint32_t hash = 0;
  uint32_t i = 0;
  uint8_t* key = (uint8_t *)&addr;

  for (i = 0; i < 4; i++)
  {
    hash += key[i];
    hash += (hash << 10);
    hash ^= (hash >> 6);
  }

  for (i = 0; i < 256; i++)
  {
    hash += (hash << 10);
    hash ^= (hash >> 6);
  }

  hash += (hash << 3);
  hash ^= (hash >> 11);
  hash += (hash << 15);
  return hash;
}

void Delay(void)
{
  volatile uint32_t i = 0;
  for (i = 0; i < 40000; i++)
  {
  }
}

int32_t main (void)
{
  uint32_t Address = 0;
  uint32_t BankSelector = 0;
  uint32_t Data = 0;
  uint32_t i = 0;
  uint32_t ibegin = 0;
  uint32_t isend = 0;
  uint32_t iend = 9;
	PORT_InitTypeDef GPIO_user_ini;

  /* Enables the clock on PORTC */
  RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC | RST_CLK_PCLK_PORTB, ENABLE);
  /* Enables the clock on EEPROM */
  RST_CLK_PCLKcmd(RST_CLK_PCLK_EEPROM, ENABLE);
	
	U_MLT_Init();
	U_MLT_Clear_Chip(0);
	U_MLT_Clear_Chip(1);

  PORT_Init(MDR_PORTC, &PORT_InitStructure);
	
  //init port B like input	
	GPIO_user_ini.PORT_OE = PORT_OE_IN; // port mode -> input
	GPIO_user_ini.PORT_FUNC = PORT_FUNC_PORT; // port mode
	GPIO_user_ini.PORT_MODE = PORT_MODE_DIGITAL; // digital port mode
	GPIO_user_ini.PORT_SPEED = PORT_SPEED_SLOW;  // choose slow mode for front
	GPIO_user_ini.PORT_Pin = UP; // pin number 5 (PB5) which is connected to button
	
	PORT_Init(MDR_PORTB, &GPIO_user_ini); // init port

  // Fill main memory page MAIN_EEPAGE */
  Address = 0x08000000 + MAIN_EEPAGE * EEPROM_PAGE_SIZE;
  BankSelector = EEPROM_Main_Bank_Select;
		loop:	
  for (i = ibegin, isend = 0; i < iend; i += 4, isend += 1)
  {
    Data = Pseudo_Rand (Address + i);
    EEPROM_ProgramWord (Address + i, BankSelector, Data);
		U_LCD_Task_Function(Data, i, isend);
		Delay();
  }
  isend = 0;
	
  /* Check main memory page MAIN_EEPAGE */
  Address = 0x08000000 + MAIN_EEPAGE * EEPROM_PAGE_SIZE;
  BankSelector = EEPROM_Main_Bank_Select;
  for (i = ibegin, isend = 0; i < iend; i += 4, isend += 1)
  {
    Data = EEPROM_ReadWord(Address + i, BankSelector);
		U_LCD_Task_FunctionRead(Data, i, isend);
		Delay();
  }
	
	//U_MLT_Clear_Chip(1);	
	
	while (1)
  {
		if (PORT_ReadInputDataBit(MDR_PORTB, UP) == 0)
		{
			ibegin += 12;
			iend += 12;
			goto loop;
		}
  }
}
