#include "./includes/includes.h"

#ifdef STM32L073xx
#define __myReg_ADC_CCR (*( ( volatile uint32_t *)	(0x40012400+0x0308)))	
#elif defined(STM32L152xB)
#define __myReg_ADC_CCR (*( ( volatile uint32_t *)	(0x40012400+0x0004)))
#endif	
//
#ifndef BATTARY_TYPE
#error "BATTARY_TYPE must be define!"
#endif
uint8_t pwrStatus=POWER_STATUS_NORMAL;
uint8_t pwrArr[4]={POWER_STATUS_NORMAL,POWER_STATUS_NORMAL,POWER_STATUS_NORMAL,POWER_STATUS_NORMAL};   

volatile uint32_t powerDownStatus=0x01;

uint32_t rtAdcValueBatteray;
uint32_t rtAdcValueAvdd;
uint32_t rtAdcValueRef;
extern bool nbRun;
uint32_t get_battary_rtadc(void)
{
	uint32_t i,x=0;	
	m_gpio_config_ain_leak_enable();
	//osDelay(2);
	m_adc_start_conversion_first(ADC_CH_BATTERY_MONITOR);
	
	m_adc_polling_read();
	for(i=0;i<4;i++){
		x+=m_adc_polling_read();
	}
	m_adc_stop_conversion_first();
	x=x/i;
	//m_gpio_config_ain_leak_disable();
	rtAdcValueBatteray=x;	
	powerDownStatus=1;

	__nop();
	m_gpio_config_ain_leak_disable();
	return x;
}

uint32_t get_avdd_rtadc(void)
{
	uint32_t i,x=0;	
	m_adc_start_conversion_first(ADC_CH_AVDD);
	m_adc_polling_read();
	for(i=0;i<4;i++){
		x+=m_adc_polling_read();
	}
	m_adc_stop_conversion_first();
	x=x/i;
	rtAdcValueAvdd=x;
	return x;
}

uint32_t get_internal_vref_rtadc(void)
{
	//
	uint32_t i,x=0;	
	
	x=__myReg_ADC_CCR;
	
	#ifdef STM32L073xx
	x |= (0x01ul<<22);
	#elif defined(STM32L152xB)
	x |= (0x01ul<<23);
	#endif	
	
	
	__myReg_ADC_CCR=x;
	osDelay(1);
	m_adc_start_conversion_first(ADC_CH_VREFINT);
	m_adc_polling_read();
	x=0;
	for(i=0;i<4;i++){
		x+=m_adc_polling_read();
	}
	m_adc_stop_conversion_first();
	x=x/i;
	rtAdcValueRef=x;

	x=__myReg_ADC_CCR;
	#ifdef STM32L073xx
	x &= ~(0x01ul<<22);
	#elif defined(STM32L152xB)
	x &= ~(0x01ul<<23);
	#endif	
	
	
	__myReg_ADC_CCR=x;
	
	return rtAdcValueRef;	
}


uint32_t voltBatArr[4]={0,0,0,0};
uint8_t get_power_status(void)
{
	//uint16_t t32;
	uint8_t sta=POWER_STATUS_NORMAL;	
	//voltBat=rtAdcValueBatteray*310/2048;
	//voltAvdd=rtAdcValueAvdd*310/2048;
	if(rtAdcValueRef==0)rtAdcValueRef=1;
	voltAvdd=4095*1224/rtAdcValueRef;
	//voltAvdd=3100;
	voltBat=rtAdcValueBatteray*voltAvdd/2048;

	__nop();
	
	#if(PWR_DOWN_IO_STATUS_EN ==0)
	if(voltBat<BAT_VOLT_DOWN){
	#else
	if(voltBat<BAT_VOLT_DOWN || powerDownStatus==0){
	#endif
		sta=POWER_STATUS_DOWN;

	}else if(voltBat<BAT_VOLT_LOW){
		sta=POWER_STATUS_LOW;

	}else if(voltBat<BAT_VOLT_HIGHT){
	
	}
	/*else if(voltBat<4500){
		sta=POWER_STATUS_OVER;
	}*/
	else{
		sta=POWER_STATUS_OVER;

	}
    return sta;	
}

