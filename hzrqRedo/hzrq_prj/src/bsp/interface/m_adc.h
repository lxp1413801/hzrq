#ifndef __m_adc_h__
#define __m_adc_h__
	#ifdef __cplusplus
		extern "C" {
	#endif
	#define ADC_CH_VREFINT ADC_CHSELR_CHSEL17
	#define ADC_CH_BATTERY_MONITOR ADC_CHSELR_CHSEL11
	#define ADC_CH_AVDD		ADC_CHSELR_CHSEL4
	extern void m_adc_init(void);
	extern void  m_adc_start_conversion_first(uint32_t ch);
	extern void  m_adc_stop_conversion_first(void);
	extern uint32_t m_adc_polling_read(void);
	
	
	extern ADC_HandleTypeDef    AdcHandle;

	#ifdef __cplusplus
		}
	#endif
#endif
//file end
//@
