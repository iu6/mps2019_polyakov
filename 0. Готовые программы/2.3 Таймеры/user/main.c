/* Includes ------------------------------------------------------------------*/
//#include "MDR32F9Qx_board.h"
#include "MDR32F9Qx_config.h"
#include "MDR32Fx.h"
#include "MDR32F9Qx_timer.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_port.h"
#include "lcd.h"
#include "mlt_lcd.h"

#define UP PORT_Pin_5//define pin 5  to UP button
#define DOWN PORT_Pin_1//define pin 1 to DOWN button
TIMER_CntInitTypeDef sTIM_CntInit;
TIMER_ChnInitTypeDef sTIM_ChnInit;
TIMER_ChnOutInitTypeDef sTIM_ChnOutInit;
TIMER_CntInitTypeDef TIM_CntInitStructure; 
PORT_InitTypeDef PORT_InitStructure;

uint16_t CCR1_Val = 0x3FF;
uint16_t CCR2_Val = 0x1FF;
uint16_t CCR3_Val = 0x7FF;
uint16_t value;
uint16_t presc, period;
int i,k=0;

void delayTick( uint32_t count)
{
    while (count--);
}

void timerINT(uint16_t presc, uint16_t period)
{
	 SCB->VTOR = 0x08000000; 
	 MDR_RST_CLK->TIM_CLOCK = 0x01000000; 
		
		 //******************************
	 TIM_CntInitStructure.TIMER_Prescaler = presc;
	 TIM_CntInitStructure.TIMER_Period = period; 
	 TIM_CntInitStructure.TIMER_CounterMode = TIMER_CntMode_ClkFixedDir; 
	 TIM_CntInitStructure.TIMER_CounterDirection = TIMER_CntDir_Up; 
	 TIMER_CntInit (MDR_TIMER1,&TIM_CntInitStructure);
	 //******************************
	 
	 NVIC_ClearPendingIRQ(Timer1_IRQn); //clear wait bit interrupt TIM1
	 __enable_irq(); // global interrupt. permisson
	 NVIC_EnableIRQ(Timer1_IRQn); //interrupt. permisson
	 TIMER_ITConfig(MDR_TIMER1, TIMER_STATUS_CNT_ARR, ENABLE); //Setting timer interrupts
	 TIMER_Cmd(MDR_TIMER1, ENABLE);
}

void lightning()
{
	if (PORT_ReadInputDataBit(MDR_PORTA, PORT_Pin_5) == 1)
		PORT_SetBits(MDR_PORTC, PORT_Pin_0);
	else
		PORT_ResetBits(MDR_PORTC, PORT_Pin_0);
			
	if (PORT_ReadInputDataBit(MDR_PORTA, PORT_Pin_3) == 1)
		PORT_SetBits(MDR_PORTC, PORT_Pin_1);
	else
		PORT_ResetBits(MDR_PORTC, PORT_Pin_1);
}

void timerInit(uint16_t Val1, uint16_t Presq, uint16_t Val2)
{
	 /* Configure TIMER1 pins: CH1, CH1N, CH2, CH2N, CH3 */
  /* Configure PORTA pins 1, 2, 3, 4, 5 */
  PORT_InitStructure.PORT_Pin   = (PORT_Pin_1 | PORT_Pin_2 | PORT_Pin_3 | PORT_Pin_4 | PORT_Pin_5);
  PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
  PORT_InitStructure.PORT_FUNC  = PORT_FUNC_ALTER;
  PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
  PORT_InitStructure.PORT_SPEED = PORT_SPEED_FAST;
  PORT_Init(MDR_PORTA, &PORT_InitStructure);

  /* Reset all TIMER1 settings */
  TIMER_DeInit(MDR_TIMER1);

  /* TIM1 Configuration ---------------------------------------------------
   Generates 5 PWM signals with 4 different duty cycles:
   TIM1CLK = 8 MHz, Prescaler = 0, TIM1 counter clock = 8 MHz
   TIM1 frequency = TIM1CLK/(TIM1_Period + 1) = 1.95 KHz
  - TIM1 Channel1 & Channel1N duty cycle = TIM1->CCR1 / (TIM1_Period + 1) = 50%
  - TIM1 Channel2 & Channel2N duty cycle = TIM1->CCR2 / (TIM1_Period + 1) = 25%
  - TIM1 Channel3 duty cycle = TIM1->CCR3 / (TIM1_Period + 1) = 12.5%
  ----------------------------------------------------------------------- */

  /* Initializes the TIMERx Counter ------------------------------------*/
  sTIM_CntInit.TIMER_Prescaler                = Presq; 
  sTIM_CntInit.TIMER_Period                   = 0xFFF;
  sTIM_CntInit.TIMER_CounterMode              = TIMER_CntMode_ClkFixedDir;
  sTIM_CntInit.TIMER_CounterDirection         = TIMER_CntDir_Up;
  sTIM_CntInit.TIMER_EventSource              = TIMER_EvSrc_None;
  sTIM_CntInit.TIMER_FilterSampling           = TIMER_FDTS_TIMER_CLK_div_1;
  sTIM_CntInit.TIMER_ARR_UpdateMode           = TIMER_ARR_Update_Immediately;
  sTIM_CntInit.TIMER_ETR_FilterConf           = TIMER_Filter_1FF_at_TIMER_CLK;
  sTIM_CntInit.TIMER_ETR_Prescaler            = TIMER_ETR_Prescaler_None;
  sTIM_CntInit.TIMER_ETR_Polarity             = TIMER_ETRPolarity_NonInverted;
  sTIM_CntInit.TIMER_BRK_Polarity             = TIMER_BRKPolarity_NonInverted;
  TIMER_CntInit (MDR_TIMER1,&sTIM_CntInit);

  /* Initializes the TIMER1 Channel 1,1N,2,2N,3 -------------------------------------*/
  TIMER_ChnStructInit(&sTIM_ChnInit);

  sTIM_ChnInit.TIMER_CH_Mode                = TIMER_CH_MODE_PWM;
  sTIM_ChnInit.TIMER_CH_REF_Format          = TIMER_CH_REF_Format6;
  sTIM_ChnInit.TIMER_CH_Number              = TIMER_CHANNEL1;
  TIMER_ChnInit(MDR_TIMER1, &sTIM_ChnInit);

  sTIM_ChnInit.TIMER_CH_Number              = TIMER_CHANNEL2;
  TIMER_ChnInit(MDR_TIMER1, &sTIM_ChnInit);

  sTIM_ChnInit.TIMER_CH_Number              = TIMER_CHANNEL3;
  TIMER_ChnInit(MDR_TIMER1, &sTIM_ChnInit);

  TIMER_SetChnCompare(MDR_TIMER1, TIMER_CHANNEL1, CCR1_Val);
  TIMER_SetChnCompare(MDR_TIMER1, TIMER_CHANNEL2, Val1);
  TIMER_SetChnCompare(MDR_TIMER1, TIMER_CHANNEL3, Val2);

  /* Initializes the TIMER1 Channel 1,1N,2,2N,3 Output -------------------------------*/

  TIMER_ChnOutStructInit(&sTIM_ChnOutInit);

  sTIM_ChnOutInit.TIMER_CH_DirOut_Polarity          = TIMER_CHOPolarity_NonInverted;
  sTIM_ChnOutInit.TIMER_CH_DirOut_Source            = TIMER_CH_OutSrc_REF;
  sTIM_ChnOutInit.TIMER_CH_DirOut_Mode              = TIMER_CH_OutMode_Output;
  sTIM_ChnOutInit.TIMER_CH_NegOut_Polarity          = TIMER_CHOPolarity_NonInverted;
  sTIM_ChnOutInit.TIMER_CH_NegOut_Source            = TIMER_CH_OutSrc_REF;
  sTIM_ChnOutInit.TIMER_CH_NegOut_Mode              = TIMER_CH_OutMode_Output;
  sTIM_ChnOutInit.TIMER_CH_Number                   = TIMER_CHANNEL1;
  TIMER_ChnOutInit(MDR_TIMER1, &sTIM_ChnOutInit);

  sTIM_ChnOutInit.TIMER_CH_Number                   = TIMER_CHANNEL2;
  TIMER_ChnOutInit(MDR_TIMER1, &sTIM_ChnOutInit);

  sTIM_ChnOutInit.TIMER_CH_Number                   = TIMER_CHANNEL3;
  TIMER_ChnOutInit(MDR_TIMER1, &sTIM_ChnOutInit);
	
		  /* Enable TIMER1 clock */
  TIMER_BRGInit(MDR_TIMER1,TIMER_HCLKdiv1);

  /* Enable TIMER1 */
  TIMER_Cmd(MDR_TIMER1,ENABLE);
}

int mainPMW(void)
{
  //RST_CLK_DeInit();
  RST_CLK_CPU_PLLconfig (RST_CLK_CPU_PLLsrcHSIdiv2,0);
  /* Enable peripheral clocks --------------------------------------------------*/
  RST_CLK_PCLKcmd((RST_CLK_PCLK_RST_CLK | RST_CLK_PCLK_TIMER1),ENABLE);
  RST_CLK_PCLKcmd((RST_CLK_PCLK_PORTA), ENABLE);

  /* Init NVIC */
  SCB->AIRCR = 0x05FA0000 | ((uint32_t)0x500);
  SCB->VTOR = 0x08000000;
  /* Disable all interrupt */
  NVIC->ICPR[0] = 0xFFFFFFFF;
  NVIC->ICER[0] = 0xFFFFFFFF;
  /* Reset PORTB settings */
  PORT_DeInit(MDR_PORTA);

	U_MLT_Clear_Chip(0);
	U_MLT_Clear_Chip(1);

	value = 0x0FF;
  timerInit(value, 0x000F, 0x10FF - value);
	
  while(1)
  {
		for (k=0;k<15;k++)
		{
			for (i=0;i<1000;i++)  
			{
				lightning();
				if (PORT_ReadInputDataBit(MDR_PORTE, DOWN) == 0)
					{
						U_MLT_Clear_Chip(0);
						U_MLT_Clear_Chip(1);
						U_MLT_Init();
						U_LCD_Task_Function(0);
						return 0;
					}
			}
			TIMER_DeInit(MDR_TIMER1);
			value += 0x100;
			timerInit(value, 0x000F, 0x10FF - value);
		}
		for (k=15;k>0;k--)
		{
			for (i=0;i<1000;i++) 
			{
				lightning();
				if (PORT_ReadInputDataBit(MDR_PORTE, DOWN) == 0)
					{
						U_MLT_Clear_Chip(0);
						U_MLT_Clear_Chip(1);
						U_MLT_Init();
						U_LCD_Task_Function(0);
						return 0;
					}
			}
			TIMER_DeInit(MDR_TIMER1);
			value -= 0x100;
			timerInit(value, 0x000F, 0x10FF - value);
		}
  }
}

int mainINT(void)
{
	TIMER_DeInit(MDR_TIMER1);
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTA |
	RST_CLK_PCLK_PORTB | RST_CLK_PCLK_PORTC |
	RST_CLK_PCLK_PORTE | RST_CLK_PCLK_TIMER1, ENABLE);
	presc = 0x7FFF/11;
	period = 0x7C2*5;
  U_LCD_Task_Function2(presc, period);
	timerINT(presc, period);
	
	while(1)
	{
		if (PORT_ReadInputDataBit(MDR_PORTE, DOWN) == 0)
			{
				U_MLT_Clear_Chip(0);
				U_MLT_Clear_Chip(1);
				U_MLT_Init();
				U_LCD_Task_Function(1);
				NVIC_ClearPendingIRQ(Timer1_IRQn); //Clear the wait bit external interrupt TIMER1
				TIMER_ClearFlag(MDR_TIMER1, TIMER_STATUS_CNT_ARR);
				TIMER_DeInit(MDR_TIMER1);
				return 0;
			}
	}
}

int mainPMW2(void)
{
  //U_LCD_Task_Function2(presc, period);
	
	//RST_CLK_DeInit();
  RST_CLK_CPU_PLLconfig (RST_CLK_CPU_PLLsrcHSIdiv2,0);
  /* Enable peripheral clocks --------------------------------------------------*/
  RST_CLK_PCLKcmd((RST_CLK_PCLK_RST_CLK | RST_CLK_PCLK_TIMER1),ENABLE);
  RST_CLK_PCLKcmd((RST_CLK_PCLK_PORTA), ENABLE);

  /* Init NVIC */
  SCB->AIRCR = 0x05FA0000 | ((uint32_t)0x500);
  SCB->VTOR = 0x08000000;
  /* Disable all interrupt */
  NVIC->ICPR[0] = 0xFFFFFFFF;
  NVIC->ICER[0] = 0xFFFFFFFF;
  /* Reset PORTB settings */
  //PORT_DeInit(MDR_PORTA);
				U_MLT_Clear_Chip(0);
				U_MLT_Clear_Chip(1);
  U_LCD_Task_Function3(0x2FFF, 0xFFF, 0x0FF, 0x7FF);

  timerInit(0x0FF, 0x2FFF, 0x7FF);
	while (1)
	{
		if (PORT_ReadInputDataBit(MDR_PORTA, PORT_Pin_5) == 1)
			PORT_SetBits(MDR_PORTC, PORT_Pin_0);
		else
			PORT_ResetBits(MDR_PORTC, PORT_Pin_0);
		
		if (PORT_ReadInputDataBit(MDR_PORTA, PORT_Pin_3) == 1)
			PORT_SetBits(MDR_PORTC, PORT_Pin_1);
		else
			PORT_ResetBits(MDR_PORTC, PORT_Pin_1);
		if (PORT_ReadInputDataBit(MDR_PORTE, DOWN) == 0)
			{
				U_MLT_Clear_Chip(0);
				U_MLT_Clear_Chip(1);
				U_MLT_Init();
				U_LCD_Task_Function(2);
				return 0;
			}
	}
}

void main(void)
{
	int j=0;
	
	PORT_InitTypeDef GPIO_user_ini;
	
	SCB->VTOR = 0x08000000; // Include a table of vectors
	
	U_MLT_Init();
	U_LCD_Task_Function(0);
	
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB, ENABLE);//switch on port b
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);//leds on port c
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTE, ENABLE);//switch on port e
	
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);//leds on port c
	GPIO_user_ini.PORT_FUNC = PORT_FUNC_PORT;
	GPIO_user_ini.PORT_MODE = PORT_MODE_DIGITAL;
	GPIO_user_ini.PORT_OE = PORT_OE_OUT;
	GPIO_user_ini.PORT_Pin = PORT_Pin_0 | PORT_Pin_1;
	GPIO_user_ini.PORT_PULL_DOWN = PORT_PULL_DOWN_ON;
	GPIO_user_ini.PORT_PULL_UP = PORT_PULL_UP_OFF;
	PORT_Init(MDR_PORTC, &GPIO_user_ini);
	
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
	
	while(1)
	{
		if (PORT_ReadInputDataBit(MDR_PORTE, DOWN) == 0) // if button pressed
		{
			if (j==0) j=1;
			else if (j==1) j=2;
			else j=0;
			U_LCD_Task_Function(j);
			delayTick(400000);
		}
		
		if ((PORT_ReadInputDataBit(MDR_PORTB, UP) == 0) && (j==0))
			mainPMW();
		
		if ((PORT_ReadInputDataBit(MDR_PORTB, UP) == 0) && (j==1))
			mainINT();
		
		if ((PORT_ReadInputDataBit(MDR_PORTB, UP) == 0) && (j==2))
			mainPMW2();
	}
}

void Timer1_IRQHandler(void)
{
 NVIC_ClearPendingIRQ(Timer1_IRQn); //Clear the wait bit external interrupt TIMER1
 TIMER_ClearFlag(MDR_TIMER1, TIMER_STATUS_CNT_ARR);
	//Clear the flag when the counter reaches the period value
 U_LCD_Task_Function2(presc, period);
 //MDR_PORTC->RXTX ^= 0x03; 
}


