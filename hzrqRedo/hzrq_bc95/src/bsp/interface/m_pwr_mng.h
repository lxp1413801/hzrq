#ifndef __M_PWR_MNG_H__
#define __M_PWR_MNG_H__


	#ifdef __cplusplus
		extern "C" {
	#endif

	#if configUSE_TICKLESS_IDLE == 1
		extern volatile unsigned long ulTimerCountsForOneTick;
		extern volatile unsigned long xMaximumPossibleSuppressedTicks;
		extern volatile unsigned long ulStoppedTimerCompensation;
		extern volatile uint8_t EnablePwrDown;		
			
		extern volatile eSleepModeStatus systemtState;
	#endif /* configUSE_TICKLESS_IDLE */
	
	#ifndef portMAX_24_BIT_NUMBER
	#define portMAX_24_BIT_NUMBER				( 0xffffffUL )		
	#endif
	#ifndef portMISSED_COUNTS_FACTOR
	#define portMISSED_COUNTS_FACTOR			( 45UL )			
	#endif

	extern void m_pre_pre_stop_processing(void);
	extern void mcu_error_reset(void);
	extern void mcu_wdg_reset(void);
	#ifdef __cplusplus
		}
	#endif


#endif
//file end
//@
