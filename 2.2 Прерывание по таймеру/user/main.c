/* Includes ------------------------------------------------------------------*/
//#include "MDR32F9Qx_board.h"
#include "MDR32F9Qx_config.h"
#include "MDR32Fx.h"
#include "MDR32F9Qx_timer.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_port.h"

PORT_InitTypeDef PORT_InitStructure; 
TIMER_CntInitTypeDef TIM_CntInitStructure; 

int main(void)
{
RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTA |
RST_CLK_PCLK_PORTB | RST_CLK_PCLK_PORTC |
RST_CLK_PCLK_PORTE | RST_CLK_PCLK_TIMER1, ENABLE);

 SCB->VTOR = 0x08000000; 
 MDR_RST_CLK->TIM_CLOCK = 0x01000000; 


 //******************************
 PORT_InitStructure.PORT_Pin = (PORT_Pin_0 | PORT_Pin_1); 
 PORT_InitStructure.PORT_OE = PORT_OE_OUT; 
 PORT_InitStructure.PORT_FUNC = PORT_FUNC_PORT; 
 PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
 PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
 PORT_Init(MDR_PORTC, &PORT_InitStructure); 
	//******************************
	
	 //******************************
 TIM_CntInitStructure.TIMER_Prescaler = 0x7FFF;
 TIM_CntInitStructure.TIMER_Period = 0x2F; 
 TIM_CntInitStructure.TIMER_CounterMode = TIMER_CntMode_ClkFixedDir; 
 TIM_CntInitStructure.TIMER_CounterDirection = TIMER_CntDir_Up; 
 TIMER_CntInit (MDR_TIMER1,&TIM_CntInitStructure);
 //******************************
 
 NVIC_ClearPendingIRQ(Timer1_IRQn); //clear wait bit interrupt TIM1
 __enable_irq(); // global interrupt. permisson
 NVIC_EnableIRQ(Timer1_IRQn); //interrupt. permisson
 TIMER_ITConfig(MDR_TIMER1, TIMER_STATUS_CNT_ARR, ENABLE); //Setting timer interrupts
 TIMER_Cmd(MDR_TIMER1, ENABLE);
 
 PORT_SetBits(MDR_PORTC, PORT_Pin_0);
 while(1)
 {
 }
}
 
 void Timer1_IRQHandler(void)
{
 NVIC_ClearPendingIRQ(Timer1_IRQn); //Clear the wait bit external interrupt TIMER1
 TIMER_ClearFlag(MDR_TIMER1, TIMER_STATUS_CNT_ARR);
	//Clear the flag when the counter reaches the period value
 MDR_PORTC->RXTX ^= 0x03; //reverse
 /*if (PORT_ReadInputDataBit(MDR_PORTC, PORT_Pin_0) == 0)
			PORT_SetBits(MDR_PORTC, PORT_Pin_0);
		else
			PORT_ResetBits(MDR_PORTC, PORT_Pin_0);*/
}
