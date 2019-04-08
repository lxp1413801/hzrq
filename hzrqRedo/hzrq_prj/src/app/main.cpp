#include "./includes/includes.h"
#include "data-protocol-ex.h"

#ifndef cfg_IRDA_EN
	#define cfg_IRDA_EN 0
#endif

//uint32_t globleTimeOut=NO_EVEN_MAX_TIME_OUT;
TaskHandle_t vTheadMainID=(osThreadId)NULL;


void __sleep_in_osdelay_disable(void)
{
	if(noEventTimeOut<=NO_EVEN_MAX_TIME_OUT)noEventTimeOut=NO_EVEN_MAX_TIME_OUT;
	/*
	if((void*)vTheadMainID){
		osSignalSet( vTheadMainID, flg_MAIN_THREAD_RESUME);
	}
	*/
}

void __sleep_in_osdelay_enable(void)
{
	noEventTimeOut=0;
	/*
	if((void*)vTheadMainID){
		osSignalSet( vTheadMainID, flg_MAIN_THREAD_RESUME);
	}data_api_day_change
	*/
}	

void m_gpio_change(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	// Check the parameters 
	//assert_param(IS_GPIO_PIN_AVAILABLE(GPIOx,GPIO_Pin));
	//assert_param(IS_GPIO_PIN_ACTION(PinState));
	uint16_t t;
	t=GPIOx->ODR;
	t ^= GPIO_Pin;
	//t|=(x&GPIO_Pin);
	GPIOx->ODR=t;
}


void m_gpio_led_init(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitStruct.Pin=GPIO_PIN_12;
	GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);
}
#ifndef __my_DEBUG
	#define __my_DEBUG 1
#endif

void init_rtc_x_x(void)
{
	aYearAgo=sysRtcDataTime.YY;
	aMonthAgo=sysRtcDataTime.MM;
	aDateAgo=sysRtcDataTime.DD;
	aHourAgo=sysRtcDataTime.hh;
	aMinuteAgo=sysRtcDataTime.mm;
	aSecondAgo=sysRtcDataTime.ss;
}


void pre_star_thread_main_00(void)
{
	m_gpio_into_lpw();
	m_rtc_init();
	init_rtc_x_x();
	
	m_buzzer_init();
	m_lcd_init();
	m_lcd_all_on();
	m_lcd_enable();
	//m_gpio_config_vavle();
	
	
	osDelay(1000);
	
	m_clock_set_system_clock_msi();
	prvSetupTimerInterrupt();
	
	m_buzzer_on();
	osDelay(200);
	m_buzzer_off();
	m_lcd_all_off();

	osDelay(10);

	__nop();
	
	qc_data_mutex_create();
	m_thread_create_sysdata();
	//qc_data_sysdata_set_default();

	//szrq_part_format();
	qc_data_sysdata_init();
	ex_eeprom_verify();
	//if(sysData.QN != default_QN)sysData.QN = default_QN;
	//if(!m_str_match(sysData.commSN,(uint8_t*)"CDQC181100000001"))m_str_cpy(sysData.commSN,(uint8_t*)"CDQC181100000001");
//	if(!m_str_cmp_len(sysData.severRelease.ip,szrqDefaultSever.ip,4)){
//		m_mem_cpy_len(sysData.severRelease.ip,szrqDefaultSever.ip,4);
//	}
//	if(sysData.severRelease.port != szrqDefaultSever.port){
//		sysData.severRelease.port = szrqDefaultSever.port;
//	}
	rtVolumeRefreshFlag=true;
	api_calc_all();
}

void pre_star_thread_main_01(void)
{

	m_gpio_config_flow_pin();
	m_gpio_config_key0_irq();
	
	m_gpio_config_alarm_dect();
	m_gpio_config_alarm();
	m_gpio_config_ste();
	
	m_gpio_config_ain_leak_disable();
	
	m_gpio_exti2_3_enable();
	m_gpio_exti4_15_enable();	
}

void vTheadMain(void * pvParameters)
{
	(void)pvParameters;
	osEvent event;
	
	if(sizeof(stDeviceInf_t)>sizeof(commonBuf))while(1);
	if(sizeof(uartSendBuf)<sizeof(rfSendFifoBody_t))while(1);
	
	pre_star_thread_main_00();
	
	vavle_off_for_start();
	pre_star_thread_main_01();
	
	//vavle_on_from_app();	
	ui_disp_device_ver();
	m_lcd_refresh();

	m_rtc_disable_wakeup();
	m_rtc_enable_wakeup(cfg_RTC_INTERVAL);
	
	m_thread_create_nb_udp();
	m_thread_create_event();

	m_thread_create_flow_puls();
	//even_send_msg_to_start_rf(false);
	//for rest
	uint8_t bkMenu,bkSubMenu;
	bkMenu=menu;
	bkSubMenu=subMenu;
	

	while(1){
		event=osSignalWait(flg_MAIN_THREAD_ALL_BITS,osWaitForever);
		//osDelay(100);
		if(event.value.signals & flg_MAIN_THREAD_VALVE_ON){
			if(noEventTimeOut<NO_EVEN_MAX_TIME_OUT)noEventTimeOut=NO_EVEN_MAX_TIME_OUT;
			bkMenu=menu;
			bkSubMenu=subMenu;
			menu=MENU_HOME;
			subMenu=subMENU_HOME_VAVLE_ON;
			app_valve_on();
			subMenu=bkSubMenu;
			flgValveErrSend=false;			
		}
		if(event.value.signals & flg_MAIN_THREAD_VALVE_OFF){
			if(noEventTimeOut<NO_EVEN_MAX_TIME_OUT)noEventTimeOut=NO_EVEN_MAX_TIME_OUT;
			bkMenu=menu;
			bkSubMenu=subMenu;		
			menu=MENU_HOME;
			subMenu=subMENU_HOME_VAVLE_OFF;	
			ui_disp_menu();
			app_valve_off();
			osDelay(2000);
			//menu=bkMenu;
			//subMenu=bkSubMenu;	
			if(menu== MENU_HOME && subMenu==subMENU_HOME_VAVLE_OFF){
				menu=bkMenu;
				subMenu=bkSubMenu;				
			}			
		}
	}
}

void qc_ex_eeprom_mutex_create(void)
{
	osMutexDef(osMutexExEeprom);
	osMutexExEeprom = osMutexCreate(osMutex(osMutexExEeprom));	
}

void m_thread_create_main(void)
{
	qc_ex_eeprom_mutex_create();
	osThreadDef(TheadMain, vTheadMain, osPriorityHigh, 0, configMINIMAL_STACK_SIZE*2);
	vTheadMainID=osThreadCreate(osThread(TheadMain), NULL);	
}


int main(void)
{
	//HAL_Init();
	//while(1);
	#ifdef STM32L152xB
		m_clock_config_hsi();
	#else
		#if cfg_SYSTEM_CLOCK_HSI 
			m_clock_set_system_clock_hsi();
		#else
			m_clock_set_system_clock_msi();
		#endif
	#endif
	m_clock_set_system_clock_hsi();

	__nop();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();	
	__HAL_RCC_GPIOC_CLK_ENABLE();		 

	m_thread_create_main();
	vTaskStartScheduler();
	while(1){
		//some error.
		__nop();
		__nop();
	}		
}
void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
	while(1){
		__nop();
		__nop();
	}
}

void vApplicationMallocFailedHook(void)
{
	while(1){
		__nop();
		__nop();
	}
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
	while (1){
		__nop();
		__nop();
	}
}
#endif




