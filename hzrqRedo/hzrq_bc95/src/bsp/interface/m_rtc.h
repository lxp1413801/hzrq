#ifndef __M_RTC_H__
#define __M_RTC_H__

#ifdef __cplusplus
extern "C"{
#endif

	//#include "../app/data.h"
	#include <stdint.h>
	#include "time-stamp.h"
	#define RTC_CLOCK_SOURCE_LSE
	/*#define RTC_CLOCK_SOURCE_LSI*/

	#ifdef RTC_CLOCK_SOURCE_LSI
	#define RTC_ASYNCH_PREDIV    0x7F
	#define RTC_SYNCH_PREDIV     0x0120
	#endif

	#ifdef RTC_CLOCK_SOURCE_LSE
	#define RTC_ASYNCH_PREDIV  0x7F
	#define RTC_SYNCH_PREDIV   0x00FF
	#endif
	/*
	typedef struct
	{
		uint8_t second;
		uint8_t minute;
		uint8_t hour;
		
		uint8_t dayInMonth;
		uint8_t month;
		uint8_t year;
		//uint8_t yh;
		
		uint8_t dayInWeek;
	}stRtcTypeDef_t;
	*/
	extern sysDataTime_t sysRtcDataTime;	
	
	extern void m_rtc_init(void);
	extern RTC_HandleTypeDef RtcHandle;
	extern void m_rtc_get(sysDataTime_t* dt);
	extern void api_rtc_set(sysDataTime_t* dt);
	
	//extern uint8_t rtcflag;
	extern RTC_DateTypeDef stDataTime;
	extern volatile uint16_t mRTCReadEn;
	//
	extern void m_rtc_enable_wakeup(uint8_t interval);
	extern void m_rtc_disable_wakeup(void);
	extern volatile uint16_t mSoftWdgTimes;
	extern void m_soft_wdg_clr(void);
	extern uint32_t get_sys_ticker_sec(void);
	
#ifdef __cplusplus
}
#endif
#endif
//file end
//@
