#include "stdint.h"
#include "stm32l0xx_hal.h"
#include "m_adc.h"
#include "cmsis_os.h"
#include "m_interface.h"
ADC_HandleTypeDef    AdcHandle;

/* ADC channel configuration structure declaration */
ADC_ChannelConfTypeDef        sConfig;

/* Variable used to get converted value */

void m_adc_error_handler(void)
{
	mcu_error_reset();
	while(1)
	{
		__nop();
	}
}



void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
	/* ADC1 Periph clock enable */
	__HAL_RCC_ADC1_CLK_ENABLE();
	m_gpio_config_adcin11();
}

/*
call this function to init. adc
*/

void m_adc_init(void)
{
	__HAL_RCC_ADC1_CLK_ENABLE();
	AdcHandle.Instance = ADC1;

	AdcHandle.Init.OversamplingMode      = DISABLE;

	AdcHandle.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV1;
	AdcHandle.Init.LowPowerAutoPowerOff  = DISABLE;
	AdcHandle.Init.LowPowerFrequencyMode = ENABLE;
	AdcHandle.Init.LowPowerAutoWait      = DISABLE;
	
	AdcHandle.Init.Resolution            = ADC_RESOLUTION_12B;
	AdcHandle.Init.SamplingTime          = ADC_SAMPLETIME_7CYCLES_5;
	AdcHandle.Init.ScanConvMode          = ADC_SCAN_DIRECTION_FORWARD;
	AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
	AdcHandle.Init.ContinuousConvMode    = ENABLE;
	AdcHandle.Init.DiscontinuousConvMode = DISABLE;
	AdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
	AdcHandle.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;
	AdcHandle.Init.DMAContinuousRequests = DISABLE;

	/* Initialize ADC peripheral according to the passed parameters */
	if (HAL_ADC_Init(&AdcHandle) != HAL_OK){
		m_adc_error_handler();
	}


	/* ### - 2 - Start calibration ############################################ */
	
	if (HAL_ADCEx_Calibration_Start(&AdcHandle, ADC_SINGLE_ENDED) != HAL_OK)
	{
		m_adc_error_handler();
	}

	/*
	sConfig.Channel = ADC_CHANNEL_4;    
	if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
	{
		m_adc_error_handler();
	}


	if (HAL_ADC_Start(&AdcHandle) != HAL_OK)
	{

		m_adc_error_handler();
	}
	*/
}

void m_adc_start_conversion_first(uint32_t ch)
{
	__HAL_RCC_ADC1_CLK_ENABLE();
	//sConfig.Channel = ch; 
	AdcHandle.Instance->CHSELR=ch;/*
	if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
	{
		m_adc_error_handler();
	}
	//osDelay(2);*/
	if (HAL_ADC_Start(&AdcHandle) != HAL_OK)
	{
	/* Start Conversation Error */
		m_adc_error_handler();
	}
}
void m_adc_stop_conversion_first(void)
{
	__HAL_RCC_ADC1_CLK_ENABLE();
	if (HAL_ADC_Stop(&AdcHandle) != HAL_OK)
	{
		/* Start Conversation Error */
		m_adc_error_handler();
	}
}

uint32_t m_adc_polling_read(void)
{
	
	uint32_t ret=0;
	HAL_ADC_PollForConversion(&AdcHandle, 2);
    if ((HAL_ADC_GetState(&AdcHandle) & HAL_ADC_STATE_REG_EOC) == HAL_ADC_STATE_REG_EOC)
    {
		/*##-6- Get the converted value of regular channel  ########################*/
		ret = HAL_ADC_GetValue(&AdcHandle);
    }
	return ret;
}



