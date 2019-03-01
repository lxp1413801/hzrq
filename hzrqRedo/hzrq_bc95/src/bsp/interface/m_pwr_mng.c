//#include "m_interface.h"
#include "./includes/includes.h"

#ifndef configSYSTICK_CLOCK_HZ
	#define configSYSTICK_CLOCK_HZ configCPU_CLOCK_HZ
#endif

	

extern bool uartInited;
	
volatile uint8_t EnablePwrDown=1;

volatile uint32_t ioModeBackup[6];

//m_uart4_deinit
void m_pre_pre_sleep_processing(void)
{
	__HAL_RCC_PWR_CLK_ENABLE();
	HAL_PWREx_EnableUltraLowPower();
	HAL_PWREx_EnableFastWakeUp();
	__HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_STOP_WAKEUPCLOCK_MSI);
	
	//m_rtc_disable_wakeup();
	//m_rtc_enable_wakeup(cfg_RTC_INTERVAL);	
	m_lpusart1_deinit();
	m_gpio_into_lpw();
}

void PostSleepProcessing(uint32_t * ulExpectedIdleTime)
{
	
	__myReg_RCC_IOPENR=~0x0ul;//
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();	
	__HAL_RCC_GPIOC_CLK_ENABLE();	
	__HAL_RCC_GPIOD_CLK_ENABLE();	
	//__HAL_RCC_GPIOE_CLK_ENABLE();	
	//__HAL_RCC_GPIOH_CLK_ENABLE();	
	//__myReg_GPIOA_MODER=ioModeBackup[0];
	//__myReg_GPIOB_MODER=ioModeBackup[1];
	//__myReg_GPIOC_MODER=ioModeBackup[2];
	//__myReg_GPIOD_MODER=ioModeBackup[3];
	//__myReg_GPIOE_MODER=ioModeBackup[4];
	//__myReg_GPIOH_MODER=ioModeBackup[5];
	
	

}
void m_pre_pre_stop_processing(void)
{
	__HAL_RCC_PWR_CLK_ENABLE();
	HAL_PWREx_EnableUltraLowPower();
	HAL_PWREx_EnableFastWakeUp();
	__HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_STOP_WAKEUPCLOCK_MSI);
	
	//m_rtc_disable_wakeup();
	//m_rtc_enable_wakeup(cfg_RTC_INTERVAL);	
	m_lpusart1_deinit();
	m_gpio_into_lpw();
}

void PreSleepProcessing(uint32_t * ulExpectedIdleTime)
{

	//(void)ulExpectedIdleTime=(void)ulExpectedIdleTime;
}

void PostStopProcessing(uint32_t * ulExpectedIdleTime)
{
	(void) ulExpectedIdleTime;
	
	//!!!!!!!must modify,use hal libaray mybe use too long wake up time!!!!!!!	
	#if cfg_SYSTEM_CLOCK_HSI	
		m_clock_set_system_clock_hsi();
	#else
		m_clock_set_system_clock_msi();
	#endif
	//HAL_InitTick(TICK_INT_PRIORITY);
	prvSetupTimerInterrupt();
	/*
	__myReg_RCC_IOPENR=~0x0ul;//
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();		
	__myReg_GPIOA_MODER=ioModeBackup[0];
	__myReg_GPIOB_MODER=ioModeBackup[1];
	__myReg_GPIOC_MODER=ioModeBackup[2];
	__myReg_GPIOD_MODER=ioModeBackup[3];
	__myReg_GPIOE_MODER=ioModeBackup[4];
	__myReg_GPIOH_MODER=ioModeBackup[5];\
	*/

}
extern TaskHandle_t vTaskMainID;


#if configUSE_TICKLESS_IDLE == 1
void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime )
{
	eSleepModeStatus eReturn;
	uint32_t ulReloadValue, ulCompleteTickPeriods, ulCompletedSysTickDecrements, ulSysTickCTRL;
	TickType_t xModifiableIdleTime;
	//return ;
	if(noEventTimeOut)return;
	
	if( xExpectedIdleTime > xMaximumPossibleSuppressedTicks ){
		xExpectedIdleTime = xMaximumPossibleSuppressedTicks;
	}
	//add by lxp
	if((xExpectedIdleTime*1000/configTICK_RATE_HZ) < 20)return;
	portNVIC_SYSTICK_CTRL &= ~portNVIC_SYSTICK_ENABLE;

	ulReloadValue = portNVIC_SYSTICK_CURRENT_VALUE + ( ulTimerCountsForOneTick * ( xExpectedIdleTime - 1UL ) );
	if( ulReloadValue > ulStoppedTimerCompensation ){
		ulReloadValue -= ulStoppedTimerCompensation;
	}
	__disable_irq();

	eReturn=eTaskConfirmSleepModeStatus();
	if( eReturn == eAbortSleep ){
		portNVIC_SYSTICK_LOAD = portNVIC_SYSTICK_CURRENT_VALUE;
		portNVIC_SYSTICK_CTRL |= portNVIC_SYSTICK_ENABLE;
		portNVIC_SYSTICK_LOAD = ulTimerCountsForOneTick - 1UL;
		__enable_irq();
	}else if(eReturn==eStandardSleep){
		portNVIC_SYSTICK_LOAD = ulReloadValue;
		portNVIC_SYSTICK_CURRENT_VALUE = 0UL;
		portNVIC_SYSTICK_CTRL |= portNVIC_SYSTICK_ENABLE;
		xModifiableIdleTime = xExpectedIdleTime;
		
		m_pre_pre_sleep_processing();
		__HAL_RCC_PWR_CLK_ENABLE();
		uartInited=false;
		HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

		if( xModifiableIdleTime > 0 ){
			__dsb( portSY_FULL_READ_WRITE );
			__wfi();
			__isb( portSY_FULL_READ_WRITE );
		}
		configPOST_SLEEP_PROCESSING( &xExpectedIdleTime );

		ulSysTickCTRL = portNVIC_SYSTICK_CTRL;
		portNVIC_SYSTICK_CTRL = ( ulSysTickCTRL & ~portNVIC_SYSTICK_ENABLE );
		__enable_irq();

		if( ( ulSysTickCTRL & portNVIC_SYSTICK_COUNT_FLAG ) != 0 ){
			uint32_t ulCalculatedLoadValue;
			ulCalculatedLoadValue = ( ulTimerCountsForOneTick - 1UL ) - ( ulReloadValue - portNVIC_SYSTICK_CURRENT_VALUE );
			if( ( ulCalculatedLoadValue < ulStoppedTimerCompensation ) || ( ulCalculatedLoadValue > ulTimerCountsForOneTick ) )	{
				ulCalculatedLoadValue = ( ulTimerCountsForOneTick - 1UL );
			}
			portNVIC_SYSTICK_LOAD = ulCalculatedLoadValue;
			ulCompleteTickPeriods = xExpectedIdleTime - 1UL;
		}else{
			ulCompletedSysTickDecrements = ( xExpectedIdleTime * ulTimerCountsForOneTick ) - portNVIC_SYSTICK_CURRENT_VALUE;
			ulCompleteTickPeriods = ulCompletedSysTickDecrements / ulTimerCountsForOneTick;
			portNVIC_SYSTICK_LOAD = ( ( ulCompleteTickPeriods + 1 ) * ulTimerCountsForOneTick ) - ulCompletedSysTickDecrements;
		}
		portNVIC_SYSTICK_CURRENT_VALUE = 0UL;
		portENTER_CRITICAL();{
			portNVIC_SYSTICK_CTRL |= portNVIC_SYSTICK_ENABLE;
			vTaskStepTick( ulCompleteTickPeriods );
			portNVIC_SYSTICK_LOAD = ulTimerCountsForOneTick - 1UL;
		}
		portEXIT_CRITICAL();
	}else{
	
		m_pre_pre_stop_processing();
		__HAL_RCC_PWR_CLK_ENABLE();
		__disable_irq();
		uartInited=false;
		
		HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);	
		if( xModifiableIdleTime > 0 ){
			__dsb( portSY_FULL_READ_WRITE );
			__wfi();
			__isb( portSY_FULL_READ_WRITE );
		}
		
		configPOST_SLEEP_PROCESSING( &xExpectedIdleTime );
		//m_pwr_exit_stop_mode();
		portNVIC_SYSTICK_CURRENT_VALUE = 0UL;
		portENTER_CRITICAL();{
			portNVIC_SYSTICK_CTRL |= portNVIC_SYSTICK_ENABLE;
			vTaskStepTick( ulCompleteTickPeriods );
			portNVIC_SYSTICK_LOAD = ulTimerCountsForOneTick - 1UL;
		}
		portEXIT_CRITICAL();	
		__enable_irq();		
	}
}
#endif 

	
void prvSetupTimerInterrupt( void )
{
			/* Calculate the constants required to configure the tick interrupt. */
		#if configUSE_TICKLESS_IDLE == 1
		{
			ulTimerCountsForOneTick = ( configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ );
			xMaximumPossibleSuppressedTicks = portMAX_24_BIT_NUMBER / ulTimerCountsForOneTick;
			ulStoppedTimerCompensation = portMISSED_COUNTS_FACTOR / ( configCPU_CLOCK_HZ / configSYSTICK_CLOCK_HZ );
		}
		#endif /* configUSE_TICKLESS_IDLE */

	/* Configure SysTick to interrupt at the requested rate. */
	portNVIC_SYSTICK_LOAD = ( configCPU_CLOCK_HZ / configTICK_RATE_HZ ) - 1UL;
	portNVIC_SYSTICK_CTRL = portNVIC_SYSTICK_CLK | portNVIC_SYSTICK_INT | portNVIC_SYSTICK_ENABLE;
}

void mcu_error_reset(void)
{
	NVIC_SystemReset();
	while(1);
}
void mcu_wdg_reset(void)
{

}
//file end
//
