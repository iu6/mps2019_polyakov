/* Includes ------------------------------------------------------------------*/
#include "MDR32F9Qx_config.h"
#include "MDR32Fx.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_wwdg.h"

#define LED1            PORT_Pin_0

PORT_InitTypeDef PORT_InitStructure;
uint32_t wwdg_flag = RESET;
uint32_t chto = 0;
void Delay(__IO uint32_t nCount);

void LEDOn(uint32_t LED_Num)
{
  PORT_SetBits(MDR_PORTC, LED_Num);
}

void LEDOff(uint32_t LED_Num)
{
  PORT_ResetBits(MDR_PORTC, LED_Num);
}

void BlinkLED1(uint32_t num, uint32_t del)
{
uint32_t cnt;

  for ( cnt = 0; cnt < num; cnt++)
  {
    LEDOn(LED1);
    Delay(del);
    LEDOff(LED1);
    Delay(del);
  }
}

void main(void)
{
  /* Enables the HSI clock for PORTC */
  RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);

  /* Configure all unused PORT pins to low power consumption */
  PORT_StructInit(&PORT_InitStructure);
  PORT_InitStructure.PORT_Pin = (PORT_Pin_All & ~(PORT_Pin_0 | PORT_Pin_1));
  PORT_Init(MDR_PORTC, &PORT_InitStructure);

  /* Configure PORTC pin 0 for output to switch LEDs on/off */
  PORT_InitStructure.PORT_Pin   = PORT_Pin_0 | PORT_Pin_1;
  PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
  PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
  PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
  PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
  PORT_Init(MDR_PORTC, &PORT_InitStructure);

  /* Enables the HSI clock for WWDG */
  RST_CLK_PCLKcmd(RST_CLK_PCLK_WWDG,ENABLE);

  NVIC_EnableIRQ(WWDG_IRQn);

  /* Set WWDG Prescaler value */
  WWDG_SetPrescaler	(WWDG_Prescaler_8);

  /* Enable WWDG and load start counter value */
  WWDG_Enable(0x2FF);
	chto = 0;

  /* Enables the WWDG Early Wakeup interrupt */
  WWDG_EnableIT();

  /* Infinite loop */
  while (1)
  {
    if (wwdg_flag == SET)
    {
      BlinkLED1(1,300000);
      wwdg_flag = RESET;
    }
		else
			chto += 1;
  }
}

void Delay(__IO uint32_t nCount)
{
  for (; nCount != 0; nCount--);
}


void WWDG_IRQHandler(void)
{
  /* Clears Early Wakeup interrupt flag */
  WWDG_ClearFlag();

  /* Load counter value */
	//BlinkLED1(1,30000);
  WWDG_SetCounter(0x1F);
	chto = 0;

  wwdg_flag = SET;
}
