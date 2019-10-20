/* Includes ------------------------------------------------------------------*/
//#include "MDR32F9Qx_board.h"
#include "MDR32F9Qx_config.h"
#include "MDR32Fx.h"
#include "MDR32F9Qx_timer.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_adc.h"              // Keil::Drivers:ADC
#include "MDR32F9Qx_port.h"
#include "lcd.h"
#include "mlt_lcd.h"

#define ADC_TS_T1 25.0F // ??????????? ? 1-? ?????
#define ADC_TS_D1 0x6A0 // ???????? ??? ? 1-? ?????
#define ADC_TS_T2 60.0F // ??????????? ?? 2-? ?????
#define ADC_TS_D2 0x7A0 // ???????? ??? ?? 2-? ?????

PORT_InitTypeDef PORT_InitStructure;
ADC_InitTypeDef ADC_InitStructure;
ADCx_InitTypeDef ADCx_InitStructure;

int res=0,temp=0,i; 

int main(void)
{
	//U_RST_Init();	
	U_MLT_Init(); //Инициализация дисплея
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTA|
	RST_CLK_PCLK_PORTB|RST_CLK_PCLK_PORTC|
	RST_CLK_PCLK_PORTD|RST_CLK_PCLK_ADC, ENABLE); 
	SCB->VTOR = 0x08000000; // Include a table of vectors

	// Enable ADC Clocking
	MDR_RST_CLK->ADC_MCO_CLOCK=0x2000;	 
	
	// Set up pins for LEDs
	/*PORT_InitStructure.PORT_Pin=(PORT_Pin_0 | PORT_Pin_1); 
	PORT_InitStructure.PORT_OE = PORT_OE_OUT; 
	PORT_InitStructure.PORT_FUNC = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL; 
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW; 
	PORT_Init(MDR_PORTC, &PORT_InitStructure);*/
	
	// Setting output for ADC
	PORT_InitStructure.PORT_Pin = PORT_Pin_7;
	PORT_InitStructure.PORT_OE = PORT_OE_IN;
	PORT_InitStructure.PORT_MODE = PORT_MODE_ANALOG; 
	PORT_Init(MDR_PORTD, &PORT_InitStructure);
	
	// ADC setup
	ADC_InitStructure.ADC_SynchronousMode = ADC_SyncMode_Independent; // Select Independent Mode
	ADC_InitStructure.ADC_StartDelay = 0;                             // Zero delay when starting the conversion
	ADC_InitStructure.ADC_TempSensor = ADC_TEMP_SENSOR_Enable;       // Turn on 
	ADC_InitStructure.ADC_TempSensorAmplifier = ADC_TEMP_SENSOR_AMPLIFIER_Enable; // Turn on the amplifier of the temperature sensor
 	ADC_InitStructure.ADC_TempSensorConversion = ADC_TEMP_SENSOR_CONVERSION_Enable;  // Turn on temperature sensor conversion
	ADC_InitStructure.ADC_IntVRefConversion = ADC_VREF_CONVERSION_Enable;  // Turn on preobrazovaniye opornogo napr
 	ADC_InitStructure.ADC_IntVRefTrimming = 1;                        // Coefficient for voltage reference (opornoe napryajenie)
	ADC_Init(&ADC_InitStructure);
	
	ADCx_InitStructure.ADC_ClockSource = ADC_CLOCK_SOURCE_CPU; //Select a clock source
	ADCx_InitStructure.ADC_SamplingMode = ADC_SAMPLING_MODE_CICLIC_CONV; //Continuous Conversion Mode
	ADCx_InitStructure.ADC_ChannelSwitching = ADC_CH_SWITCHING_Disable; //Prohibit channel switching
	ADCx_InitStructure.ADC_ChannelNumber = ADC_CH_TEMP_SENSOR; 
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
	
	while(1)
	{		
		
	}
}

void ADC_IRQHandler(void)
{
	ADC1_Cmd(DISABLE);              //Stop the ADC
	res=ADC1_GetResult()&0x0FFF;            //Read the conversion result
	
	// Перевод в Цельсии
	temp = ((int32_t)res - (int32_t)ADC_TS_D1)*(ADC_TS_T2-ADC_TS_T1)/(ADC_TS_D2 - ADC_TS_D1)+ ADC_TS_T1; 
	
	 //Вывод на дисплей
	U_LCD_Task_Function(temp);
	
	NVIC_ClearPendingIRQ(ADC_IRQn);  //Ochistka bita ozhidaniya vneshnego preryvaniya EXTI1
	ADC1_Cmd(ENABLE);               //Start ADC
	for (i=0;i<500000;i++) {};
}

