/* Includes ------------------------------------------------------------------*/
#include "MDR32Fx.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_adc.h"              // Keil::Drivers:ADC
#include "MDR32F9Qx_dma.h"              // Keil::Drivers:DMA


#include "common.h"
#include "stdio.h"

// Bufer dlya khraneniya rezul'tatov raboty ATSP i yego razmer
#define U_ADC_BUFFER_SIZE 256

// Struktura dlya initsializatsii DMA
DMA_CtrlDataInitTypeDef DMA_InitStructure;
// Struktura dlya initsializatsii kanala DMA
DMA_ChannelInitTypeDef DMA_Channel_InitStructure;    

// Sobytiye (Event) o zavershenii serii analogo-tsifrovykh preobrazovaniy
#define EVENT_ADC_EOC 0x0001

// Bufer dlya khraneniya rezul'tatov ATSP
static uint16_t ADC_Buffer [U_ADC_BUFFER_SIZE];

static void ADC_Config(void);
static void DMA_Config(void);
static int task = 0;

void U_ADC_Init (void)
{ 
  DMA_Config();
  ADC_Config();
}

static void DMA_Config(void)
{
  RST_CLK_PCLKcmd (RST_CLK_PCLK_DMA | RST_CLK_PCLK_SSP1 | RST_CLK_PCLK_SSP2, ENABLE); 
	
	// Zapretit' vse preryvaniya, v tom chisle ot SSP1 i SSP2.
  NVIC->ICPR[0] = 0xFFFFFFFF;
  NVIC->ICER[0] = 0xFFFFFFFF;
	
	// Sbrosit' vse nastroyki DMA
  DMA_DeInit();
  DMA_StructInit (&DMA_Channel_InitStructure);
	
	DMA_InitStructure.DMA_SourceBaseAddr = (uint32_t)(&(MDR_ADC->ADC1_RESULT));	// Bazovyy adres registra rezul'tata preobrazovaniya ATSP1, otkuda budem chitat' 
  DMA_InitStructure.DMA_DestBaseAddr = (uint32_t) &ADC_Buffer;			      // Bazovyy adres v pamyati, kuda budem pisat' rezul'taty raboty ATSP
  DMA_InitStructure.DMA_CycleSize = U_ADC_BUFFER_SIZE;							      // Razmer bufera
  DMA_InitStructure.DMA_SourceIncSize = DMA_SourceIncNo;				          // Zapretit' avtouvelicheniye adresa dlya periferii
  DMA_InitStructure.DMA_DestIncSize = DMA_DestIncHalfword;				        // Razreshit' avtouvelicheniye adresa dlya pamyati na 2
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;     // Peredayem dannyye po 2 bayta (polslova)  
  DMA_InitStructure.DMA_NumContinuous = DMA_Transfers_1;				          // Skol'ko raz povotoryayem sovokupnost' tsiklov - 1 raz
  DMA_InitStructure.DMA_SourceProtCtrl = DMA_SourcePrivileged;			      // Rezhim zashchity istochnika
  DMA_InitStructure.DMA_DestProtCtrl = DMA_DestPrivileged;			          // Rezhim zashchity priyemnika
  DMA_InitStructure.DMA_Mode = DMA_Mode_Basic;							              // Rezhim raboty DMA - bazovyy
  
  // Zadat' strukturu kanala
  DMA_Channel_InitStructure.DMA_PriCtrlData = &DMA_InitStructure;
  DMA_Channel_InitStructure.DMA_Priority = DMA_Priority_Default;
  DMA_Channel_InitStructure.DMA_UseBurst = DMA_BurstClear;
  DMA_Channel_InitStructure.DMA_SelectDataStructure = DMA_CTRL_DATA_PRIMARY;
  
	// Initsializirovat' kanal DMA 
  DMA_Init (DMA_Channel_ADC1, &DMA_Channel_InitStructure);
	
	// Snyat' zapret na zaprosy i odinochnyye zaprosy k DMA  
  MDR_DMA->CHNL_REQ_MASK_CLR = 1 << DMA_Channel_ADC1;
  MDR_DMA->CHNL_USEBURST_CLR = 1 << DMA_Channel_ADC1;

  // Razreshit' rabotu DMA
  DMA_Cmd (DMA_Channel_ADC1, ENABLE);

  // Zadat' prioritet apparatnogo preryvaniya ot DMA
  NVIC_SetPriority (DMA_IRQn, 1);
}

static void ADC_Config(void)
{
	// Struktury dlya initsializatsii ATSP
  ADC_InitTypeDef ADC_InitStructure;
	ADCx_InitTypeDef ADCx_InitStructure;
	
  // Razreshit' taktirovaniye ATSP
  RST_CLK_PCLKcmd (RST_CLK_PCLK_ADC, ENABLE);  

  ADC_DeInit();
  ADC_StructInit(&ADC_InitStructure);

  // Konfiguratsiya ATSP obshchaya
  ADC_InitStructure.ADC_SynchronousMode      = ADC_SyncMode_Independent;						// Nezavisimyy zapusk ATSP
  ADC_InitStructure.ADC_StartDelay           = 0;																		// Zadrezhka mezhdu zapuskami ATSP1 i ATSP2
  ADC_InitStructure.ADC_TempSensor           = ADC_TEMP_SENSOR_Enable;              // Razreshit' rabotu temperaturnogo datchika
  ADC_InitStructure.ADC_TempSensorAmplifier  = ADC_TEMP_SENSOR_AMPLIFIER_Enable;    // Razreshit' rabotu usilitelya dlya temperaturnogo datchika
  ADC_InitStructure.ADC_TempSensorConversion = ADC_TEMP_SENSOR_CONVERSION_Enable;   // Razreshit' preobrazovaniya dlya kanala temperaturnogo datchika  
  ADC_InitStructure.ADC_IntVRefConversion    = ADC_VREF_CONVERSION_Disable;         // Zapretit' preobrazovaniya dlya kanala VREF (vnutrenney opory)
  ADC_InitStructure.ADC_IntVRefTrimming      = 1;                                   // Podstroyka istochnika napryazheniya VREF
  ADC_Init (&ADC_InitStructure);

  // Konfiguratsiya ATSP1
  ADCx_StructInit (&ADCx_InitStructure);															
  ADCx_InitStructure.ADC_ClockSource      = ADC_CLOCK_SOURCE_CPU;						// Taktirovat' ATSP toy zhe chastotoy, chto i yadro MK 
  ADCx_InitStructure.ADC_SamplingMode     = ADC_SAMPLING_MODE_CICLIC_CONV;	// Rezhim tsiklicheskogo preobrazovaniya (neskol'ko raz podryad)
  ADCx_InitStructure.ADC_ChannelSwitching = ADC_CH_SWITCHING_Disable;				// Rezhim pereklyucheniya kanalov (zapretit')
  ADCx_InitStructure.ADC_ChannelNumber    = ADC_CH_TEMP_SENSOR;							// Vybrannyy kanal ATSP (temperaturnyy datchik)
  ADCx_InitStructure.ADC_Channels         = 0;															// Vybrannyye kanaly ATSP s posledovatel'nym oprosom (ne vybrany) 
  ADCx_InitStructure.ADC_LevelControl     = ADC_LEVEL_CONTROL_Disable;			// Kontrol' urovnya vkhodnrogo napryazheniya (otklyucheno)
  ADCx_InitStructure.ADC_LowLevel         = 0;															// Nizhniy uroven'
  ADCx_InitStructure.ADC_HighLevel        = 0;															// Verkhniy uroven'	
  ADCx_InitStructure.ADC_VRefSource       = ADC_VREF_SOURCE_INTERNAL;				// Vid istochnika opronogo napryazheniya (vnutrenniy)
  ADCx_InitStructure.ADC_IntVRefSource    = ADC_INT_VREF_SOURCE_INEXACT;	  // Vid vnutrennego istochnika opronogo napryazheniya (ne tochnyy)
  ADCx_InitStructure.ADC_Prescaler        = ADC_CLK_div_512;								// Preddelitel' chastoty taktirovaniya ATSP (512)
  ADCx_InitStructure.ADC_DelayGo          = 7;															// Zaderzhka mezhdu zapuskami ATSP (maksimal'naya)   	
  ADC1_Init (&ADCx_InitStructure);

  // Razreshit' rabotu ATSP1
  ADC1_Cmd (ENABLE);

  // Razreshit' preryvaniya ot DMA
  NVIC_EnableIRQ (DMA_IRQn); 
}

//OS_TID U_ADC_Task_ID;

int main (void)
{
	uint32_t D;    // Rezul'tat analogo-tsifrovogo preobrazovaniya
  float T;       // Izmerennaya temperatura 
	uint32_t i;
	 
	
	U_MLT_Init ();
	U_ADC_Init();

	while(1)
	{
    // Razreshit' rabotu DMA s ATSP. Tem samym zapuskayetsya tsikl analogo-tsifrovykh preobrazovaniy 
    DMA_Cmd (DMA_Channel_ADC1, ENABLE);
		
		// Dozhdat'sya okonchaniya preobrazovaniya
    //if (task == 1)
		{			

			// Usrednit' rezul'tat
			for (i = 0, D = 0; i < U_ADC_BUFFER_SIZE; i++)
				D += ADC_Buffer[i];
			D /= U_ADC_BUFFER_SIZE;
			
			U_LCD_Task_Function(D);
			
			// Preobrazovaniye pokazaniy ATSP v temperaturu v gradusakh Tsel'siya
			//T = ((int32_t)D - (int32_t) ADC_TS_D1) * (ADC_TS_T2 - ADC_TS_T1) / (ADC_TS_D2 - ADC_TS_D1) + ADC_TS_T1; 
			
			//U_LCD_Task_Function(T, D);
			
			//task = 0;
		}
		
		// Pauza v tikakh sistemnogo taymera. Zdes' 1 tik = 1 ms
    //os_dly_wait (250);  		
	}
	return 0;
}

void DMA_IRQHandler (void)
{
	// Podgotovit' k rabote novyy tsikl analogo-tsifrovykh preobrazovaniy
  DMA_InitStructure.DMA_CycleSize = U_ADC_BUFFER_SIZE;
  DMA_Init(DMA_Channel_ADC1, &DMA_Channel_InitStructure);

	// Zapretit' dal'neyshuyu rabotu kanala DMA c ADC
  DMA_Cmd (DMA_Channel_ADC1, DISABLE);
	
	// Ustanovit' sobytiye ob okonchanii tsikla analogo-tsifrovogo preobrazovaniya
  //task = 1;
}

	