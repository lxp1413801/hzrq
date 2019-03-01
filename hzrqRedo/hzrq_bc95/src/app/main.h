

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
//#include "cmsis_os.h"
#ifndef __myDEBUG
	#define __myDEBUG 0
#endif

#define NO_EVEN_MAX_TIME_OUT (15)

extern TaskHandle_t vTheadMainID;
#define flg_MAIN_THREAD_SUSPEND		(1ul<<0)
#define flg_MAIN_THREAD_RESUME		(1ul<<1)
#define flg_MAIN_THREAD_VALVE_OFF	(1ul<<2)
#define flg_MAIN_THREAD_VALVE_ON	(1ul<<3)

//#define flg_PORTA_PIN10_EIXT	(1ul<<2)
#define flg_MAIN_THREAD_ALL_BITS	(flg_MAIN_THREAD_SUSPEND | flg_MAIN_THREAD_RESUME  | flg_MAIN_THREAD_VALVE_OFF | flg_MAIN_THREAD_VALVE_ON)
/*	

#define E_EVEN_FLG_FLOW_FALLING	(1<<0)
#define E_EVEN_FLG_FLOW_RISING	(1<<1)
#define E_EVEN_FLG_STE_FALLING	(1<<2)
#define E_EVEN_FLG_STE_RISING		(1<<3)
#define E_EVEN_FLG_KEY0_FALLING	(1<<4)
#define E_EVEN_FLG_KEY0_RISING	(1<<5)
#define E_EVEN_FLG_KEY1_FALLING	(1<<6)
#define E_EVEN_FLG_KEY1_RISING	(1<<7)	

#define E_EVEN_FLG_ALARM0_FALLING	(1<<8)
#define E_EVEN_FLG_ALARM0_RISING	(1<<9)

#define E_EVEN_FLG_ALARM1_FALLING	(1<<10)
#define E_EVEN_FLG_ALARM1_RISING	(1<<11)




#define E_EVEN_FLG_PDM_FALLING	(1<<12)
#define E_EVEN_FLG_PDM_RISING	(1<<13)
//#define M_EVEN_FLG_RTC_WKUP	(1<<16)



#define E_EVEN_FLG_ALL_BITS (0x03ffful)
//#define EVEN_FLG_ALL_BITS_BUT_ADC (0x0fffful)

//#define EVEN_FLG_PDM_RISING	(1<<15)


#define M_EVEN_FLG_ADC_OVER 		(1<<0)
#define M_EVEN_FLG_RTC_WKUP 		(1<<1)
#define M_EVEN_FLG_RESUME 			(1<<2)
#define M_EVEN_FLG_EVENS_WKUP 		(1<<3)
#define M_EVEN_FLG_EVENS_UNLOCK 	(1<<4)
#define M_EVEN_FLG_ALL_BITS 		(0x0f)	

extern osMutexId osMutexLcd;
extern osMutexId osMutexAdc;
extern osMutexId osMutexKey;
//extern osMutexId osMutexLCD;
*/
//extern uint32_t globleTimeOut;

extern volatile uint8_t keyLockTimer;

extern void __sleep_in_osdelay_enable(void);
extern void __sleep_in_osdelay_disable(void);

extern void m_gpio_change(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);


#ifdef __cplusplus
}
#endif
#endif /* __MAIN_H */
//file  end
