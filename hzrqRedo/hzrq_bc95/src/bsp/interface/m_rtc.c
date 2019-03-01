/*#include "stdint.h"
#include "stm32l0xx_hal.h"
#include "cmsis_os.h"

#include "string.h"
#include "../protocol/m_string.h"
#include "m_gpio_config.h"
#include "m_clock_config.h"
#include "m_rtc.h"
#include "m_batter_monitor.h"
#include "main.h"*/
#include "../../includes/includes.h"
//#include "m_interface.h"
//#include "../protocol/m_protocol.h"
volatile uint16_t mRTCReadEn=0;
volatile uint16_t mSoftWdgTimes=0x00;
RTC_HandleTypeDef RtcHandle;
sysDataTime_t sysRtcDataTime={0x00,0x00,0x00,0x01,0x01,0x00,0x01};
#define m_read_reg_asm(x) (*(volatile uint32_t*)(x))
void m_rtc_enable_wakeup(uint8_t interval){
	HAL_RTCEx_SetWakeUpTimer_IT(&RtcHandle, (0x800)*(interval), RTC_WAKEUPCLOCK_RTCCLK_DIV16);
}
void m_rtc_disable_wakeup(void){
	HAL_RTCEx_DeactivateWakeUpTimer(&RtcHandle);
}
void m_rtc_error_handle(void)
{
	mcu_error_reset();
	while(1)
	{
		__nop();
	}
}


/* 1 <= m <= 12,  y > 1752 (in the U.K.) */
/*
uint8_t day_of_week(int16_t y, uint16_t m, uint16_t d)  
{
	static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
	y -= m < 3;
	return (uint8_t)((y + y/4 - y/100 + y/400 + t[m-1] + d) % 7);
}
*/




void m_rtc_set(sysDataTime_t* dt)
{
	uint8_t buf[sizeof(RTC_TimeTypeDef)]={0};
	RTC_DateTypeDef*  sdate ;
	RTC_TimeTypeDef*  stime ;	
	
	sdate=(RTC_DateTypeDef*)buf;
	
	sdate->Year=dt->YY;
	sdate->Month=dt->MM;
	sdate->Date=dt->DD;
	sdate->WeekDay=dt->day;
	//sdate->
	if(HAL_RTC_SetDate(&RtcHandle,sdate,RTC_FORMAT_BCD) != HAL_OK)
	{
	/* Initialization Error */
		m_rtc_error_handle(); 
	} 
	m_mem_set(buf,0,sizeof(buf));
	stime=(RTC_TimeTypeDef*)buf;
	
	//!!!!!!!!must by set zero
	stime->Hours=dt->hh;
	stime->Minutes=dt->mm;
	stime->Seconds=dt->ss;
	
	stime->TimeFormat = RTC_HOURFORMAT12_AM;
	stime->DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
	stime->StoreOperation = RTC_STOREOPERATION_RESET;	
	
	
	if(HAL_RTC_SetTime(&RtcHandle,stime,RTC_FORMAT_BCD) != HAL_OK)
	{
	/* Initialization Error */
		m_rtc_error_handle();
	}  
	
}

void api_rtc_set(sysDataTime_t* dt)
{
	uint32_t t32;
	m_rtc_set(dt);
	t32=system_dt_to_time_stamp(dt);
	stampTickerSec=t32;
}
#define RTCHMS_REGESTER (*(volatile uint32_t*)(0x40002800))
#define RTCDMWWD_REGESTER (*(volatile uint32_t*)(0x40002804))	
#define m_read_reg_asm(x)  (*(volatile uint32_t*)(x))
void m_rtc_get(sysDataTime_t* dt)
{
	volatile uint32_t tmpreg = 0;
	__HAL_RCC_PWR_CLK_ENABLE();
//	RTC_DateTypeDef*  sdate;
//	RTC_TimeTypeDef*  stime;		
//	uint8_t buf[sizeof(RTC_TimeTypeDef)];
//	
//	sdate=(RTC_DateTypeDef*)buf;
//	if(HAL_RTC_GetDate(&RtcHandle,sdate,RTC_FORMAT_BCD) != HAL_OK)
//	{
//		m_rtc_error_handle();
//	}
//	dt->y=sdate->Year;
//	dt->m=sdate->Month;
//	dt->d=sdate->Date;
//	dt->dw=sdate->WeekDay;
//	  datetmpreg = (uint32_t)(hrtc->Instance->DR & RTC_DR_RESERVED_MASK);
		
	//tmpreg=RTCDMWWD_REGESTER;	
//	stime=(RTC_TimeTypeDef*)buf;
//	if(HAL_RTC_GetTime(&RtcHandle,stime,RTC_FORMAT_BCD) != HAL_OK)
//	{
//		m_rtc_error_handle();
//	}
//	dt->hh=stime->Hours;
//	dt->mm=stime->Minutes;
//	dt->ss=stime->Seconds;
	
	//tmpreg = RTCHMS_REGESTER;
	//extern uint32_t m_read_reg_asm(uint32_t addr);
	tmpreg=m_read_reg_asm(0x40002800);
	tmpreg=m_read_reg_asm(0x40002804);
  
	tmpreg=m_read_reg_asm(0x40002800);
	dt->hh=(uint8_t)((tmpreg & (RTC_TR_HT | RTC_TR_HU)) >> 16);
	dt->mm=(uint8_t)((tmpreg & (RTC_TR_MNT | RTC_TR_MNU)) >>8);
	dt->ss=(uint8_t)(tmpreg & (RTC_TR_ST | RTC_TR_SU));
	
	tmpreg=m_read_reg_asm(0x40002804);
	  /* Fill the structure fields with the read parameters */
	dt->YY = (uint8_t)((tmpreg & (RTC_DR_YT | RTC_DR_YU)) >> 16);
	dt->MM = (uint8_t)((tmpreg & (RTC_DR_MT | RTC_DR_MU)) >> 8);
	dt->DD = (uint8_t)(tmpreg & (RTC_DR_DT | RTC_DR_DU));
	dt->day = (uint8_t)((tmpreg & (RTC_DR_WDU)) >> 13); 	
	__nop();
}
void m_rtc_init(void)
{
	uint32_t t=0x1000;
	/*##-1- Configure the RTC peripheral #######################################*/
	RtcHandle.Instance = RTC;

	/* Configure RTC prescaler and RTC data registers */
	/* RTC configured as follows:
	  - Hour Format    = Format 24
	  - Asynch Prediv  = Value according to source clock
	  - Synch Prediv   = Value according to source clock
	  - OutPut         = Output Disable
	  - OutPutPolarity = High Polarity
	  - OutPutType     = Open Drain */
	RtcHandle.Init.HourFormat     = RTC_HOURFORMAT_24;
	RtcHandle.Init.AsynchPrediv   = RTC_ASYNCH_PREDIV;
	RtcHandle.Init.SynchPrediv    = RTC_SYNCH_PREDIV;
	RtcHandle.Init.OutPut         = RTC_OUTPUT_DISABLE;
	RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	RtcHandle.Init.OutPutType     = RTC_OUTPUT_TYPE_OPENDRAIN;	
	if (HAL_RTC_Init(&RtcHandle) != HAL_OK)
	{
		/* Initialization Error */
		m_rtc_error_handle();
	}
	//config rtc in hook function HAL_RTC_MspInit(),
	//m_clock_set_rtc_clock();
	while(t--);
	api_rtc_set(&sysRtcDataTime);
	
}
void m_rtc_aralm_set(RTC_DateTypeDef* dt)
{
	
}
/**
  * @brief RTC MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  * @param hrtc: RTC handle pointer
  * @note  Care must be taken when HAL_RCCEx_PeriphCLKConfig() is used to select 
  *        the RTC clock source; in this case the Backup domain will be reset in  
  *        order to modify the RTC Clock source, as consequence RTC registers (including 
  *        the backup registers) and RCC_CSR register are set to their reset values.  
  * @retval None
  */
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

	/*##-1- Enables the PWR Clock and Enables access to the backup domain ###################################*/
	/* To change the source clock of the RTC feature (LSE, LSI), You have to:
	 - Enable the power clock using __HAL_RCC_PWR_CLK_ENABLE()
	 - Enable write access using HAL_PWR_EnableBkUpAccess() function before to 
	   configure the RTC clock source (to be done once after reset).
	 - Reset the Back up Domain using __HAL_RCC_BACKUPRESET_FORCE() and 
	   __HAL_RCC_BACKUPRESET_RELEASE().
	 - Configure the needed RTC clock source */
	__HAL_RCC_PWR_CLK_ENABLE();
	HAL_PWR_EnableBkUpAccess();


	#ifdef RTC_CLOCK_SOURCE_LSE  
	RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
	if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{ 
		//Error_Handler();
		m_rtc_error_handle();
	}

	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
	PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
	if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
	{ 
		//Error_Handler();
		m_rtc_error_handle();
	}
	#elif defined (RTC_CLOCK_SOURCE_LSI)  
	RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
	if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{ 
		//Error_Handler();
		m_rtc_error_handle();
	}
	
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
	PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
	if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
	{ 
		//Error_Handler();
		m_rtc_error_handle();
	}
	#else
	#error no define rtc sorce close
	#endif /*RTC_CLOCK_SOURCE_LSE*/

	/*##-2- Enable the RTC & BKP peripheral Clock ##############################*/
	/* Enable RTC Clock */
	__HAL_RCC_RTC_ENABLE();
	HAL_NVIC_SetPriority(RTC_IRQn, 0x0, 0);
	HAL_NVIC_EnableIRQ(RTC_IRQn);	

}

/**
  * @brief RTC MSP De-Initialization 
  *        This function freeze the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  * @param hrtc: RTC handle pointer
  * @retval None
  */
void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc)
{
  /*##-1- Reset peripherals ##################################################*/
  __HAL_RCC_RTC_DISABLE();
}

extern TaskHandle_t vTheadEvenID;

uint32_t get_sys_ticker_sec(void)
{


	return sysTickerSec;

}

void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
	/* Clear Wake Up Flag */
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
	mSoftWdgTimes++;
	sysTickerSec++;
	stampTickerSec++;

	if(mSoftWdgTimes>=120){mcu_error_reset();}
	if((void*)vTheadEvenID){
		osSignalSet( vTheadEvenID, flg_EVENT_RTC_WAKE_UP);
	}
}
void m_soft_wdg_clr(void)
{
	mSoftWdgTimes=0;
}
//file end
//@
