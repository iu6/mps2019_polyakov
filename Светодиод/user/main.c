#include "MDR32F9Qx_config.h"           // Keil::Device:Startup
#include "MDR32Fx.h"                    // Device header
#include "MDR32F9Qx_port.h"             // Keil::Drivers:PORT
#include "MDR32F9Qx_rst_clk.h"          // Keil::Drivers:RST_CLK

#define UP PORT_Pin_5//define pin 5  to UP button
#define DOWN PORT_Pin_1//define pin 1 to DOWN button

void delayTick( uint32_t count)
{
    while (count--);
}

int main(void)
{
	PORT_InitTypeDef GPIO_user_ini;
	uint32_t i = 1000000;
	//uint32_t k = 0;
	
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB, ENABLE);//switch on port b
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);//leds on port c
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTE, ENABLE);//switch on port e
	
	//init port B like input
	GPIO_user_ini.PORT_OE = PORT_OE_IN; // port mode -> input
	GPIO_user_ini.PORT_FUNC = PORT_FUNC_PORT; // port mode
	GPIO_user_ini.PORT_MODE = PORT_MODE_DIGITAL; // digital port mode
	GPIO_user_ini.PORT_SPEED = PORT_SPEED_SLOW;  // choose slow mode for front
	GPIO_user_ini.PORT_Pin = UP; // pin number 5 (PB5) which is connected to button
	PORT_Init(MDR_PORTB, &GPIO_user_ini); // init port
	
		//init port E like input
	GPIO_user_ini.PORT_OE = PORT_OE_IN; // port mode -> input
	GPIO_user_ini.PORT_FUNC = PORT_FUNC_PORT; // port mode
	GPIO_user_ini.PORT_MODE = PORT_MODE_DIGITAL; // digital port mode
	GPIO_user_ini.PORT_SPEED = PORT_SPEED_SLOW;  // choose slow mode for front
	GPIO_user_ini.PORT_Pin = DOWN; // pin number 1 (PE1) which is connected to button
	PORT_Init(MDR_PORTE, &GPIO_user_ini); // init port
	
	GPIO_user_ini.PORT_FUNC = PORT_FUNC_PORT;
	GPIO_user_ini.PORT_MODE = PORT_MODE_DIGITAL;
	GPIO_user_ini.PORT_OE = PORT_OE_OUT;
	GPIO_user_ini.PORT_Pin = PORT_Pin_0 | PORT_Pin_1;
	GPIO_user_ini.PORT_PULL_DOWN = PORT_PULL_DOWN_ON;
	GPIO_user_ini.PORT_PULL_UP = PORT_PULL_UP_OFF;
	PORT_Init(MDR_PORTC, &GPIO_user_ini);
	
  PORT_ResetBits(MDR_PORTC, PORT_Pin_0); 
	PORT_ResetBits(MDR_PORTC, PORT_Pin_1); 
	while(1)
	{
		if (PORT_ReadInputDataBit(MDR_PORTE, DOWN) == 0) // if button pressed
				{
					if (i<1750000)
						i += 250000; 
				}
		if (PORT_ReadInputDataBit(MDR_PORTB, UP) == 0)
				{
					if (i>250000)
						i -= 250000; 
				}
		PORT_SetBits(MDR_PORTC, PORT_Pin_0); // LED on
		PORT_ResetBits(MDR_PORTC, PORT_Pin_1);
		delayTick(i);
		PORT_SetBits(MDR_PORTC, PORT_Pin_1); // LED on
	  PORT_ResetBits(MDR_PORTC, PORT_Pin_0);
		delayTick(i);
		/* PORT_SetBits(MDR_PORTC, PORT_Pin_0);
		delayTick(i);
		PORT_ResetBits(MDR_PORTC, PORT_Pin_0);
		delayTick(i);
		if ((i < 1000000) && (k == 0))
			i = i + 200000;
		else
			k = 1;
		if ((i > 100000) && (k == 1))
			i = i - 200000;
		else
			k = 0; */
	}
}
