/* Includes ------------------------------------------------------------------*/
#include "MDR32Fx.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_dac.h"              // Keil::Drivers:DAC
#include "MDR32F9Qx_comp.h"             // Keil::Drivers:COMP
#include "MDR32F9Qx_adc.h"              // Keil::Drivers:ADC


#include "common.h"
#include "stdio.h"

#define RANGE			COMP_CVREF_RANGE_Up;          // v verhnem diapazone / ne v nizhnem diapazone
#define SCALE			COMP_CVREF_SCALE_16_div_32;    //Vybirayet shkalu diapazona uvelicheniya CV CVRef

#define UP PORT_Pin_5//define pin 5  to UP button
#define DOWN PORT_Pin_1//define pin 1 to DOWN button


PORT_InitTypeDef PORT_InitStructure;
COMP_InitTypeDef COMP_InitStructure;
COMP_CVRefInitTypeDef COMP_CVRefInitStructure;
PORT_InitTypeDef PORT_InitStructure2;
ADC_InitTypeDef ADC_InitStructure;
ADCx_InitTypeDef ADCx_InitStructure;

int k,D;
#define DELAY(D) for (k = D; k > 0; k--)

int res=0,i; 
uint32_t CompRezAs = 0;
uint32_t CompRezSy = 1;
uint32_t CompRez = 0;	


int mainCOMP(void)
{
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTA|
	RST_CLK_PCLK_PORTB|RST_CLK_PCLK_PORTC|
	RST_CLK_PCLK_PORTD|RST_CLK_PCLK_ADC| RST_CLK_PCLK_DAC, ENABLE);  
		
	//Nastroyka vyvoda dlya DAC
	PORT_InitStructure.PORT_Pin = PORT_Pin_0;
	PORT_InitStructure.PORT_OE = PORT_OE_OUT; 
	PORT_InitStructure.PORT_MODE = PORT_MODE_ANALOG; 
	PORT_Init(MDR_PORTE, &PORT_InitStructure); 
	
	DAC2_Init(DAC2_AVCC); //Vybor opornogo napryazheniya
	DAC2_Cmd(ENABLE);
	
	/*************************************************/
		 /* Enables the HSI clock on PORTE */
  RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTE,ENABLE);
	
	/* Fill PortInit structure*/
  PORT_InitStructure.PORT_PULL_UP = PORT_PULL_UP_OFF;
  PORT_InitStructure.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
  PORT_InitStructure.PORT_MODE = PORT_MODE_ANALOG;
	
	/* Configure PORTE pins 2 (COMP_IN1) */
  PORT_InitStructure.PORT_OE = PORT_OE_OUT;
  PORT_InitStructure.PORT_Pin = PORT_Pin_2;
  PORT_Init(MDR_PORTE, &PORT_InitStructure);
	
	/* Enables the HSI clock for COMP control */
  RST_CLK_PCLKcmd(RST_CLK_PCLK_COMP,ENABLE);
	
	/* Fills each COMP_InitStructure member with its default value */
  COMP_StructInit(&COMP_InitStructure);
	
	/* Initialize COMP_InitStructure */
  COMP_InitStructure.COMP_PlusInputSource = COMP_PlusInput_CVREF;
  COMP_InitStructure.COMP_MinusInputSource = COMP_MinusInput_IN1;
#ifdef USE_INVERTION
  COMP_InitStructure.COMP_OutInversion = COMP_OUT_INV_Enable;
#endif

  /* Configure COMP */
  COMP_Init(&COMP_InitStructure);

  /* Fills each COMP_CVRefInitStructure member with its default value */
  COMP_CVRefStructInit(&COMP_CVRefInitStructure);

	/* Initialize COMP_CVRefInitStructure */
  COMP_CVRefInitStructure.COMP_CVRefSource = COMP_CVREF_SOURCE_AVdd;
  COMP_CVRefInitStructure.COMP_CVRefRange = RANGE;
  COMP_CVRefInitStructure.COMP_CVRefScale = SCALE;
	
	/* Configure COMP CVRef */
  COMP_CVRefInit(&COMP_CVRefInitStructure);

  /* Enables COMP peripheral */
  COMP_Cmd(ENABLE);

  /* Check READY flag */
  while(COMP_GetCfgFlagStatus(COMP_CFG_FLAG_READY) != SET);
	
	/* Enables COMP CVRef */
  COMP_CVRefCmd(ENABLE);
	/*************************************************/
	
	
	//U_MLT_Init();
	// ciclichiy vivod sinusoidi
	U_MLT_Clear_Chip(1);
	U_MLT_Clear_Chip(0);
	int comp = 0;
	while(1)
	{
		for (i=50;i<4000;i++)  //inc
		{
			DAC2_SetData(i);   //Vyvesti peremennuyu v DAC
			DELAY(100);
			CompRezAs = COMP_GetFlagStatus(COMP_STATUS_FLAG_AS);
			if (CompRezAs != CompRezSy)
			{
				comp = i;
				U_LCD_Task_Function(i, comp);
			}
			if (i%100 == 0)
				U_LCD_Task_Function(i, comp);
			CompRezSy = COMP_GetFlagStatus(COMP_STATUS_FLAG_SY);
			CompRez = COMP_GetStatus;
			if (PORT_ReadInputDataBit(MDR_PORTE, DOWN) == 0)
			{
				U_MLT_Clear_Chip(0);
				U_MLT_Clear_Chip(1);
				U_MLT_Init();
				U_LCD_Task_Function3(1);
				return 0;
			}
		}
		for (i=4000;i>50;i--)  //dec
		{
			DAC2_SetData(i);   //Vyvesti peremennuyu v DAC
			DELAY(100);
			CompRezAs = COMP_GetFlagStatus(COMP_STATUS_FLAG_AS);
			if (CompRezAs != CompRezSy)
			{
				comp = i;
				U_LCD_Task_Function(i, comp);
			}
			if (i%100 == 0)
				U_LCD_Task_Function(i, comp);
			CompRezSy = COMP_GetFlagStatus(COMP_STATUS_FLAG_SY);
			CompRez = COMP_GetStatus;
			if (PORT_ReadInputDataBit(MDR_PORTE, DOWN) == 0)
			{
				U_MLT_Clear_Chip(0);
				U_MLT_Clear_Chip(1);
				U_MLT_Init();
				U_LCD_Task_Function3(1);
				return 0;
			}
		}
	}
}

int mainADC(void)
{
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTA|
	RST_CLK_PCLK_PORTB|RST_CLK_PCLK_PORTC|
	RST_CLK_PCLK_PORTD|RST_CLK_PCLK_ADC| RST_CLK_PCLK_DAC, ENABLE);  
		
	//Nastroyka vyvoda dlya DAC
	PORT_InitStructure.PORT_Pin = PORT_Pin_0;
	PORT_InitStructure.PORT_OE = PORT_OE_OUT; 
	PORT_InitStructure.PORT_MODE = PORT_MODE_ANALOG; 
	PORT_Init(MDR_PORTE, &PORT_InitStructure); 
	
	DAC2_Init(DAC2_AVCC); //Vybor opornogo napryazheniya
	DAC2_Cmd(ENABLE);
	
	// Setting output for ADC
	PORT_InitStructure.PORT_Pin = PORT_Pin_7;
	PORT_InitStructure.PORT_OE = PORT_OE_IN;
	PORT_InitStructure.PORT_MODE = PORT_MODE_ANALOG; 
	PORT_Init(MDR_PORTD, &PORT_InitStructure);
	
	// ADC setup
	ADC_InitStructure.ADC_SynchronousMode = ADC_SyncMode_Independent; // Select Independent Mode
	ADC_InitStructure.ADC_StartDelay = 0;                             // Zero delay when starting the conversion
	ADC_InitStructure.ADC_TempSensor = ADC_TEMP_SENSOR_Disable;       // Turn off 
	ADC_InitStructure.ADC_TempSensorAmplifier = ADC_TEMP_SENSOR_AMPLIFIER_Disable; // Turn off the amplifier of the temperature sensor
  ADC_InitStructure.ADC_TempSensorConversion = ADC_TEMP_SENSOR_CONVERSION_Disable;  // Turn off temperature sensor conversion
	ADC_InitStructure.ADC_IntVRefConversion = ADC_VREF_CONVERSION_Disable;  // Turn off conversion opornogo napr
  ADC_InitStructure.ADC_IntVRefTrimming = 0;                        // Coefficient for voltage reference (opornoe napryajenie)
	ADC_Init(&ADC_InitStructure);
	
	ADCx_InitStructure.ADC_ClockSource = ADC_CLOCK_SOURCE_CPU; //Select a clock source
	ADCx_InitStructure.ADC_SamplingMode = ADC_SAMPLING_MODE_CICLIC_CONV; //Continuous Conversion Mode
	ADCx_InitStructure.ADC_ChannelSwitching = ADC_CH_SWITCHING_Disable; //Prohibit channel switching
	ADCx_InitStructure.ADC_ChannelNumber = ADC_CH_ADC7; 
	ADCx_InitStructure.ADC_Channels = 1;                           //Number of channels
	ADCx_InitStructure.ADC_LevelControl = ADC_LEVEL_CONTROL_Disable; //Disable level of input signal control
	ADCx_InitStructure.ADC_VRefSource = ADC_VREF_SOURCE_INTERNAL; //vnutr.(internal not external) istovhnic opornogo napr
	ADCx_InitStructure.ADC_Prescaler = ADC_CLK_div_32768; 
	ADCx_InitStructure.ADC_DelayGo = 250;                        // Delay between conversions
	ADC1_Init(&ADCx_InitStructure);
	
	// Set up interrupts
	ADC1_ITConfig(ADCx_IT_END_OF_CONVERSION, ENABLE); //Permission of interrupts at the end of conversion
	__enable_irq(); 
	NVIC_EnableIRQ(ADC_IRQn); //ADC External Interrupt Permissions
	ADC1_Cmd (ENABLE);  // Start ADC
	
	//U_MLT_Init();
	U_MLT_Clear_Chip(0);
	U_MLT_Clear_Chip(1);
	// ciclichiy vivod sinusoidi
	//while(1)
	//{
		for (i=1345;i<1654;i++)  //inc
		{
			DAC2_SetData(i);   //Vyvesti peremennuyu v DAC
			DELAY(100);
			//if ((i%5) == 0)
			U_LCD_Task_Function2 (i,res);
		}
		for (i=1654;i>1000;i--)  //dec
		{
			DAC2_SetData(i);   //Vyvesti peremennuyu v DAC
			DELAY(100);
			//if ((i%10) == 0)
			U_LCD_Task_Function2 (i,res);
		}
	//}
}

void main(void)
{
	int j=0;
	
	PORT_InitTypeDef GPIO_user_ini;
	
	SCB->VTOR = 0x08000000; // Include a table of vectors
	
	U_MLT_Init();
	U_LCD_Task_Function3(0);
	
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
			else j=0;
			U_LCD_Task_Function3(j);
			DELAY(100000);
		}
		
		if ((PORT_ReadInputDataBit(MDR_PORTB, UP) == 0) && (j==0))
			mainADC();
		
		if ((PORT_ReadInputDataBit(MDR_PORTB, UP) == 0) && (j==1))
			mainCOMP();
	}
}


void ADC_IRQHandler(void)
{
	ADC1_Cmd(DISABLE);              //Stop the ADC
	res=ADC1_GetResult()&0xFFFF;            //Read the conversion result
	NVIC_ClearPendingIRQ(ADC_IRQn);  //Ochistka bita ozhidaniya vneshnego preryvaniya EXTI1
	ADC1_Cmd(ENABLE);               //Start ADC
}
	