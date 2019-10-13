
/* Includes ------------------------------------------------------------------*/
//#include "MDR32F9Qx_board.h"
#include "MDR32F9Qx_config.h"
#include "MDR32Fx.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_dac.h"              // Keil::Drivers:DAC
#include "MDR32F9Qx_rst_clk.h"

PORT_InitTypeDef PORT_InitStructure;

int res=0,i; 

int main(void)
{
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTA|
	RST_CLK_PCLK_PORTB | RST_CLK_PCLK_PORTC |
	RST_CLK_PCLK_PORTD | RST_CLK_PCLK_PORTE |
	RST_CLK_PCLK_DAC, ENABLE);
	
	// Set up pins for LEDs
	PORT_InitStructure.PORT_Pin=(PORT_Pin_0 | PORT_Pin_1); 
	PORT_InitStructure.PORT_OE = PORT_OE_OUT; 
	PORT_InitStructure.PORT_FUNC = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL; 
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW; 
	PORT_Init(MDR_PORTC, &PORT_InitStructure);
		
	//Nastroyka vyvoda dlya DAC
	PORT_InitStructure.PORT_Pin = PORT_Pin_0;
	PORT_InitStructure.PORT_OE = PORT_OE_OUT; 
	PORT_InitStructure.PORT_MODE = PORT_MODE_ANALOG; 
	PORT_Init(MDR_PORTE, &PORT_InitStructure); 
	
	DAC2_Init(DAC2_AVCC); //Vybor opornogo napryazheniya
	DAC2_Cmd(ENABLE);
	
	while(1)
	{
		for (i=0;i<4000;i++)  //inc
		{
			DAC2_SetData(i);   //Vyvesti peremennuyu v DAC
			PORT_SetBits(MDR_PORTC, PORT_Pin_0);
		}
		for (i=4000;i>0;i--)  //dec
		{
			DAC2_SetData(i);   //Vyvesti peremennuyu v DAC
			PORT_ResetBits(MDR_PORTC, PORT_Pin_0);
		}
	}
}
	