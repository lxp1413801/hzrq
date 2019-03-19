//file name:m_event.h 
#ifndef __m_event_h__
#define __m_event_h__

#ifdef __cplusplus
extern "C"{
#endif

	extern TaskHandle_t vTheadEvenID;
	#define flg_EVENT_KEY_DOWN		(0x01ul<<0)
	#define flg_EVENT_STE_FALLING	(0x01ul<<1)
	#define flg_EVENT_FLW_FALLING	(0x01ul<<2)
	#define flg_EVENT_RTC_WAKE_UP	(0x01ul<<3)
	#define flg_EVENT_UNLOCK_ON		(0x01ul<<4)
	#define flg_EVENT_DAY_CHANGE	(0x01ul<<5)
	#define flg_EVENT_ALARM_FALLING (0x01ul<<6)
	#define flg_EVENT_IR_RECEIVED	(0x01ul<<7)
	#define flg_EVENT_HI_FLOW		(0x01ul<<8)
	
	#define flg_EVENT_ALL (flg_EVENT_KEY_DOWN | flg_EVENT_STE_FALLING \
	| flg_EVENT_FLW_FALLING | flg_EVENT_RTC_WAKE_UP | flg_EVENT_UNLOCK_ON \
	| flg_EVENT_DAY_CHANGE | flg_EVENT_ALARM_FALLING | flg_EVENT_IR_RECEIVED \
	| flg_EVENT_HI_FLOW)
	
	//extern uint32_t userTickerSec;
	
	extern uint32_t sysTickerSec;
	extern uint32_t stampTickerSec;
	extern uint32_t globleTickerSec;
	
	extern uint32_t noEventTimeOut;

	extern uint32_t payPromptTimeOut;
	
	extern TaskHandle_t vTheadEvenID;
	extern void m_thread_create_event(void);

	extern void even_send_msg_to_start_rf(void *_x_msg);
	extern void event_start_waite_power_up(void);
	
	
	extern TaskHandle_t vTheadFlowPulsID;
	#define flg_FLW_FALLING (0x01ul<<0)
	extern void m_thread_create_flow_puls(void);
	extern uint32_t nbPopTimes;
	
	//extern normalDate_t aDayAgo;
	//extern normalTime_t aSecondAgo;
	//api
	extern uint8_t aYearAgo;
	extern uint8_t aMonthAgo;
	extern uint8_t aDateAgo;
	extern uint8_t aHourAgo;
	extern uint8_t aMinuteAgo;
	extern uint8_t aSecondAgo;
	
	extern void event_clear_no_network(void);
	extern uint16_t get_minute_in_day(void);
	
	extern void event_process_day_change(void);
	extern void event_process_minute_change(void);
	
	extern bool overFlowFlgforSendWarning;
	
	extern bool rtcSync;
#ifdef __cplusplus
}
#endif

#endif
//file end
