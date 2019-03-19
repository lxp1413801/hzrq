//file name:m_event.h 
#include "./includes/includes.h"
#include "data-protocol-ex.h"




bool overFlowFlgforSendWarning=false;
bool rtcSync=false;
//<<--定义上一个时间
uint8_t aYearAgo=0;
uint8_t aMonthAgo=0;
uint8_t aDateAgo=0;
uint8_t aHourAgo=0;
uint8_t aMinuteAgo=0;
uint8_t aSecondAgo=0;

//-->>

uint32_t noEventTimeOut=NO_EVEN_MAX_TIME_OUT;


TaskHandle_t vTheadEvenID=(osThreadId)NULL;
TaskHandle_t vTheadFlowPulsID=(osThreadId)NULL;
//uint32_t userTickerSec=0x00L;
uint32_t sysTickerSec=0x00L;
uint32_t stampTickerSec=0x00L;

uint32_t globleTickerSec=0;

uint32_t payPromptTimeOut=0x00ul;
#if SEND_TIMES_TEST
uint32_t nbPopTimes=0x00ul;
#endif

#ifndef cfg_IRDA_EN
#define cfg_IRDA_EN 0
#endif
#if (config_NB_PSM_EN>0 && config_NB_PLAT !=COAP)
	#error "psm mode must set coap mode!!"
#endif
bool unlockFlag=true;

//冒泡记录
bool flgEverydayPoped=true;

uint16_t noNetworkTimer=0x00;

void event_set_globle_time_out_max(void)
{
	noEventTimeOut=NO_EVEN_MAX_TIME_OUT;
}

void event_disable_sleep_in_lwp(void)
{
	noEventTimeOut=NO_EVEN_MAX_TIME_OUT;
}


void even_send_msg_to_start_rf(__m_udpSendmsg_t *_x_msg)
{
	uint32_t msg;
	api_sysdata_save();	
	if((void*)nbSendMsgQ){
		#if SEND_TIMES_TEST
		nbPopTimes++;
		#endif
		msg=_x_msg->t32;
		osMessagePut(nbSendMsgQ,msg,10);
	}
}



void even_send_msg_to_rf_off(void)
{
	udpSendmsg.t32=0x00UL;
	udpSendmsg.stru.eventMsg=(uint8_t)flg_NB_PROCESS_MODULE_RF_CLOSE;
	if(( void*)nbSendMsgQ)even_send_msg_to_start_rf((__m_udpSendmsg_t*)&udpSendmsg);	
}

void even_send_msg_to_rf_send_pop(uint8_t popType,uint16_t eventCode,uint8_t eventFlg)
{
	uint16_t len;
	uint8_t buf[256];
	//<<
	len=hzrq_load_pop_frame(buf,sizeof(buf),popType,eventCode,eventFlg);
	if(popType==__hzrq_POP_TYPE_EVENT && rtcSync)record_save_event_log(eventCode);
	//>>
	osMutexWait(osMutexSysData,osWaitForever);
	if(len)rf_send_fifo_push(buf,len);
	osMutexRelease(osMutexSysData);
	if(!if_pwr_status_normal())return;
	udpSendmsg.t32=0x00UL;
	udpSendmsg.stru.popType=popType;
	udpSendmsg.stru.eventMsg=(uint8_t)flg_NB_PROCESS_SEND_OLD;	
	if(( void*)nbSendMsgQ)even_send_msg_to_start_rf((__m_udpSendmsg_t*)&udpSendmsg);		
}

void buzzer_beap_ms(uint16_t ms)
{
	m_buzzer_init();	
	m_buzzer_on();
	osDelay(ms);
	m_buzzer_off();	
}

void vavle_on_from_app_hook(void)
{
	//事件记录
	even_send_msg_to_rf_send_pop(__hzrq_POP_TYPE_EVENT,__hzrq_EVENT_VALVE_OPEN,1);
}

void vavle_off_from_app_hook(void)
{
	even_send_msg_to_rf_send_pop(__hzrq_POP_TYPE_EVENT,__hzrq_EVENT_VALVE_CLOSE,1);
}
void app_valve_on_msg_send(void)
{
	if((void*)vTheadMainID){
		osSignalSet( vTheadMainID, flg_MAIN_THREAD_VALVE_ON);
		osDelay(5);
	}		
}

void app_valve_off_msg_send(void)
{
	if((void*)vTheadMainID){
		osSignalSet( vTheadMainID, flg_MAIN_THREAD_VALVE_OFF);
		osDelay(5);
	}	
}


void vavle_on_from_app(void)
{
	if(vavleState!=VALVE_OFF)return;
	vavle_on_from_app_hook();
	app_valve_on_msg_send();

	flgValveErrSend=false;

	if(overFlowFlgforSendWarning){
		overFlowFlgforSendWarning=false;
	}
}

void vavle_off_from_app(uint8_t reson)
{
	uint8_t fiOff=0,fiSend=0;
	if(sysData.DLCS>DLC_STATUS_A){
		if(!(vavleState==VALVE_OFF || vavleState==VALVE_OPERATION_OFF)){
			fiOff=1;
		}
	}else {
		if(reson==OFF_REASON_SHELL_OPEN && vavleState==VALVE_OFF){
			
		}
		else if((vavleState==VALVE_OFF && VavleOffReason!=reson) || (vavleState == VALVE_ON)){
			fiOff=1;
		}			
	}
	if(fiOff){
		VavleOffReason=reson;
		vavle_off_from_app_hook();
		app_valve_off_msg_send();		
	}
}

uint8_t check_vavle_open_condition()
{
    uint8_t ret=1;
	if(vavleState!=VALVE_OFF)return 0;
	
	m_gpio_ste_port_rcc_enable();
	
	if(!(m_gpio_read(STE_PORT,STE_PIN)))return 0;
	m_gpio_alarm_port_rcc_enable();
	if(!(m_gpio_read(ALARM1_PORT,ALARM1_PIN)))return 0;
	//DeviceEvent.bits.bStrongMagnetic=0;
	
	if(sysData.devStatus.bits.bStrongMagnetic)return 0;
    if(sysData.devStatus.bits.bLeakage)return 0;
	
	if(!if_pwr_status_normal())return 0;

	if(sysData.lockReason.bits.bSeverOff )return 0;
	if(sysData.lockReason.bits.bBalance )return 0;
	if(sysData.lockReason.bits.bShellOpen)return 0;
	uint8_t tmtry=sysData.qcNoTryTimes;
	if(tmtry==1)tmtry=2;		
	if(sysData.lockReason.bits.bNoflow>=tmtry && tmtry!=0)return 0;
	
	//if(lockReason.bits.bSmallFlow)return 0;
	if(sysData.lockReason.bits.bStrongMagnetic >=MAX_STE_OFF_TIMES)return 0;
	
	rtVolumeRefreshFlag=true;
	//api_calc_all();  
	//if(ovmStatus==OV_OFF)ret=0;

	if(sysData.devStatus.bits.bNoNetWork)return 0;
/*
	lockReason.bits.bHifhtFlow=0;
	lockReason.bits.bPowerHight=0;
	lockReason.bits.bPowerDown=0;
	lockReason.bits.bLeakage=0;
	lockReason.bits.bNoNetwork=0;
	lockReason.bits.bCzsnWarnOff=0;
		*/
	return ret;
}

uint8_t valve_ulock_fi(void)
{
    uint16_t ret=0;
	/*
	unlockFlag=false;
	//if(vavleState==VALVE_OFF && lockReason.data){
	if(vavleState==VALVE_OFF){
        unlockFlag=true;
		ret=check_vavle_open_condition();

        if(ret){
			osSignalSet( vTheadEvenID, flg_EVENT_UNLOCK_ON);
		}
    }
    return ret;
	*/
	if(vavleState==VALVE_OFF){
		ret=check_vavle_open_condition();
        if(ret){
			osSignalSet( vTheadEvenID, flg_EVENT_UNLOCK_ON);
		}		
	}else{
		ret=0;
	}
	return ret;
}

void peocess_rtc_bateray_moniter(void)
{
	
	m_adc_start_conversion_first(ADC_CH_BATTERY_MONITOR);
	adcValue=m_adc_polling_read();
	m_adc_stop_conversion_first();
	__nop();
}

uint8_t event_process_unlock(void)
{
	//Event &= ~evenFlags_UNLOCK_ON;
	if(vavleState== VALVE_OFF){         
			vavle_on_from_app();
	}	
    return 0;
}


uint16_t even_key_down_super_pay(void)
{
	if(sysData.DLCS>DLC_STATUS_B)return 0;

	if(sysData.DLCS==DLC_STATUS_A && !fi_id_writed_in_dlcs_a())return 0;
	
	api_sysdata_save();

	if(menu!=MENU_TOTALE_VOL && menu!=MENU_RSSI)return 0;
	//if(sysData.superPayNums<SUPPER_PAY_TIMES_LIMITS && overageMoney<= 0x0l){
	if(overageMoney>0x0L)return 0;
	if(overageMoney<= 0x0L){
		if(sysData.DLCS==DLC_STATUS_A){
			sysData.OVerageVM=default_OVERAGE_VM_A;
		}else{
			sysData.OVerageVM=default_OVERAGE_VM_B;
		}
		//sysData.OVerageVM=25;
		api_sysdata_save();
		if(sysData.DLCS!=DLC_STATUS_A){
			sysData.superPayNums++;
		}
		//qc_data_device_even_lock_clear();
		vavle_on_from_app();
		
		payPromptTimeOut=PAY_PROMPT_TIME_LONG;
	}
	//valve_ulock_fi();
	menu=MENU_HOME;
	subMenu=subMENU_HOME_MAIN;
	return 1;		
}

uint16_t even_key_down_valve_ctrl(void)
{
	//uint32_t t32;
	if(sysData.DLCS==DLC_STATUS_A){
		if(!fi_id_writed_in_dlcs_a())return 0;
		else if(menu!=MENU_HOME)return 0;
	}else if(sysData.DLCS==DLC_STATUS_B){
		//if(menu!=0)return 0;
		return 0;
	}else{
		return 0;
	}
	if(overrageVolume<=0L){
		//转阶段,流转到B
		api_sysdata_save();
		qc_data_clear_sysdata_to_dcls_b();	
		api_sysdata_save();
		rtVolumeRefreshFlag=true;

		m_flow_all_data_init();

		api_calc_all();	
		//
		vavle_on_from_app();

		menu=MENU_DLCS;
		subMenu=subMENU_DLCS;
		noEventTimeOut=120;
	}else{
		vavle_off_from_app(OFF_REASON_MANUAL);
	}
	return 1;	
}

void ir_received_ready(void)
{
}

uint16_t even_key_down_connect_ir(void)
{
	if(sysData.DLCS==DLC_STATUS_A){

		if(fi_id_writed_in_dlcs_a() && menu==MENU_ID){
			menu=MENU_IR;
			subMenu=subMENU_IR;
			ir_received_ready();
			return 1;				
		}else if(!fi_id_writed_in_dlcs_a()){
			menu=MENU_IR;
			subMenu=subMENU_IR;
			ir_received_ready();
			return 1;			
		}
	}
	else if(sysData.DLCS==DLC_STATUS_B && menu==MENU_ID){
		menu=MENU_IR;
		subMenu=subMENU_IR;
		ir_received_ready();
		return 1;
	}	
	return 0;
}

uint16_t even_key_down_connect_udp(void)
{
	if(sysData.DLCS==DLC_STATUS_A ){

		if(fi_id_writed_in_dlcs_a()){
			if(menu==MENU_ID || menu==MENU_HOME || menu==MENU_TOTALE_VOL || menu==MENU_RSSI)return 0;
		}

	}else if(sysData.DLCS==DLC_STATUS_B){
		if(menu==MENU_TOTALE_VOL || menu==MENU_ID || menu==MENU_RSSI)return 0;
	}	
	
	even_send_msg_to_rf_send_pop(__hzrq_POP_TYPE_MANUAL,__hzrq_ERR_NONE,0);
	return 1;
}

void event_key_down_process_menu_change(void)
{
	#if cfg_IRDA_EN
	uint8_t bkupMenu=menu;
	#endif
	uint8_t* pmenu;
	uint16_t len,i;
	if(sysData.DWM== DWM_COMMON_MODE){
		if(sysData.DLCS<DLC_STATUS_B){
			pmenu=(uint8_t*)menuTableVolMode;
			len=sizeof(menuTableVolMode);
		}else{
			pmenu=(uint8_t*)menuTableComReader;
			len=sizeof(menuTableComReader);
		}
	}else if(sysData.DWM==DWM_VOLUME_MODE){
		pmenu=(uint8_t*)menuTableVolMode;
		len=sizeof(menuTableVolMode);		
	}else{
		pmenu=(uint8_t*)menuTableMoneryMode;
		len=sizeof(menuTableMoneryMode);			
	}
	for(i=0;i<len;i++){
		if(menu==pmenu[i])break;
	}
	if(i<len-1){
		i++;
	}else{
		i=0;
		buzzer_beap_ms(200);	
	}
	menu=pmenu[i];
	subMenu=0;
	if(menu==MENU_IMEI || menu==MENU_CCID){
		noEventTimeOut=30;
	}
}
void event_key_down_process(void)
{
	uint32_t tm=0x00;
	m_gpio_config_key0_irq_disable();
	m_gpio_key0_port_rcc_enable();
	while(1){
		if(m_gpio_read(KEY0_PORT,KEY0_PIN))break;
		osDelay(20);
		tm+=20;
		if(tm>=2000)break;
	}
	
	event_disable_sleep_in_lwp();
	do{
		if(tm>=2000){
			event_set_globle_time_out_max();
			m_lcd_enable();
			buzzer_beap_ms(200);
			if(even_key_down_valve_ctrl())break;
			#if cfg_IRDA_EN
			if(even_key_down_connect_ir())break;
			#endif
			//if(even_key_down_clear_lock())break;
			if(even_key_down_connect_udp())break;
			if(even_key_down_super_pay())break;
		}
		buzzer_beap_ms(20);
		if(!(m_lcd_get_status())){
			event_set_globle_time_out_max();
			m_lcd_enable();
			break;
		}
		event_set_globle_time_out_max();
		if(valve_ulock_fi()){
			//m_gpio_config_key0_irq_re_enable();
			//return;
			break;
		}
		 #if cfg_IRDA_EN
		if(sysData.DLCS==DLC_STATUS_A && !fi_id_writed_in_dlcs_a()){
			//menu=0x00;subMenu=0x00;
			break;
		}		 
		 #else
		if(sysData.DLCS==DLC_STATUS_A && !fi_id_writed_in_dlcs_a()){
			//menu=0x00;subMenu=0x00;
			break;
		}
		#endif
		event_set_globle_time_out_max();
		event_key_down_process_menu_change();

	}while(0);
	ui_disp_menu();
	m_gpio_config_key0_irq_re_enable();
}


void event_clear_no_network(void)
{
	if(sysData.devStatus.bits.bNoNetWork){
		noNetworkTimer=0;
		sysData.devStatus.bits.bNoNetWork=0;
		//lockReason.bits.bNoNetwork=0;
		api_sysdata_save();
	}
}

void event_no_network_process(void)
{
	uint32_t t32;
	if(sysData.unNetTimeOut==0){
		noNetworkTimer=0x00L;
		return ;
	}
	if(sysData.devStatus.bits.bNoNetWork)return;
	t32=sysData.unNetTimeOut;
	//t32*=(86400L);
	t32*=(1440L);
	noNetworkTimer++;
	if(noNetworkTimer>=t32){
		sysData.devStatus.bits.bNoNetWork=1;
		//lockReason.bits.bNoNetwork=1;
		vavle_off_from_app(OFF_REASON_CZSN_NONET);
	}
}

uint32_t nextFreezeTm=0x00;


volatile uint32_t hzrqRoportNextTm=0x00UL;
void hzrq_report_next_tm_recalc(void)
{
	sysDataTime_t dt;
	uint32_t t32,x;
	if(sysData.hzrqPeriodType==0)return;
	
	m_mem_cpy_len((uint8_t*)&dt,(uint8_t*)&sysRtcDataTime,sizeof(sysDataTime_t));
	dt.DD=m_hex_2_bcd(sysData.hzrqPeriodValue);
	dt.hh=sysData.hzrqReportHour;
	dt.mm=sysData.hzrqReportMinute;
	dt.ss=0;	
	t32=system_dt_to_time_stamp(&dt);	
	
	x=my_scanf_int16(sysData.hzrqMeterId+sysData.hzrqMeterIdLen-3,'\0',NULL);
	x*=sysData.hzrqRefusesPeekInterval;
	x+=60;
	t32+=x;	
	hzrqRoportNextTm=t32;
}
void hzrq_fi_report(void)
{
	uint32_t t32,x;
	sysDataTime_t dt;
	
	if(sysData.DLCS<DLC_STATUS_B)return;
		
	t32=stampTickerSec;
	if(t32>=hzrqRoportNextTm){
		m_mem_cpy_len((uint8_t*)&dt,(uint8_t*)&sysRtcDataTime,sizeof(sysDataTime_t));
		
		osMutexWait(osMutexSysData,osWaitForever);
		//if(len)rf_send_fifo_push(buf,len);
		osMutexRelease(osMutexSysData);	
		
		if(sysData.hzrqPeriodType==0){
			//da
			dt.hh=sysData.hzrqReportHour;
			dt.mm=sysData.hzrqReportMinute;
			dt.ss=0;
			t32=system_dt_to_time_stamp(&dt);
			x=my_scanf_int16(sysData.hzrqMeterId+sysData.hzrqMeterIdLen-3,'\0',NULL);
			x*=sysData.hzrqRefusesPeekInterval;
			x+=60;
			t32+=x;
			
			x=sysData.hzrqPeriodValue;
			if(x==0)x=1;
			x*=86400UL;
			t32+=x;
			hzrqRoportNextTm=t32;
		}else{

			dt.DD=m_hex_2_bcd(sysData.hzrqPeriodValue);
			dt.hh=sysData.hzrqReportHour;
			dt.mm=sysData.hzrqReportMinute;
			dt.ss=0;	
			t32=system_dt_to_time_stamp(&dt);	

			t32+=259200L;
			hzrqRoportNextTm=t32;
		}
		//dt.DD=sysData.hzr;
		even_send_msg_to_rf_send_pop(__hzrq_POP_TYPE_TIMER,__hzrq_ERR_NONE,0);
	}
}

void event_rtc_change_second(void)
{

}

void event_rtc_change_minute(void)
{
	event_no_network_process();
	//szrq_fi_collect();
	//szrq_fi_report();
	//szrq_fi_freeze();
	hzrq_fi_report();
}

void event_rtc_change_hour(void)
{
	//szrq_fi_freeze();
	if(rtcSync)record_api_save_hour_log();
}

void event_rtc_change_day(void)
{
	if(rtcSync)record_api_save_day_log();
}

void event_rtc_change_month(void)
{
	hzrq_report_next_tm_recalc();
}

void event_rtc_date_time_x(void)
{
	if(aSecondAgo!=sysRtcDataTime.ss){
		aSecondAgo=sysRtcDataTime.ss;
		event_rtc_change_second();
	}
	
	if(aMinuteAgo!=sysRtcDataTime.mm){
		aMinuteAgo=sysRtcDataTime.mm;
		event_rtc_change_minute();
	}
	
	if(aHourAgo!=sysRtcDataTime.hh){
		aHourAgo=sysRtcDataTime.hh;
		event_rtc_change_hour();
	}
	
	if(aDateAgo!=sysRtcDataTime.DD){
		aDateAgo=sysRtcDataTime.DD;
		event_rtc_change_day();
	}
	if(aMonthAgo!=sysRtcDataTime.MM)
	{
		event_rtc_change_month();
	}
}

/*
void event_rtc_date_time_x(void)
{
	if(aSecondAgo.stru.minute != stRtcDataTime.mm){
		aSecondAgo.stru.minute=stRtcDataTime.mm;
		event_process_minute_change();
	}
	if(aDayAgo.stru.dayInMonth != stRtcDataTime.DD){
		aDayAgo.stru.dayInMonth=stRtcDataTime.DD;
		event_process_day_change();
		sendTimeTab.nextTimeLoc=0;
		sendTimeTab.tadayOver=0;
		sampleTimeTab.nextTimeLoc=0;
		sampleTimeTab.tadayOver=0;
	}
}
*/
/*
uint16_t get_minute_in_day(void)
{
	uint16_t t16;
	uint8_t t8;
	//m_rtc_get(&stRtcDataTime);
	t8=m_bcd_2_hex(sysRtcDataTime.hh);
	t16=t8;
	t16*=60;
	t8=m_bcd_2_hex(sysRtcDataTime.mm);
	t16+=t8;
	return t16;
}
*/


void event_rtc_process_sec(void)
{
	volatile uint32_t tm=0x00;
	globleTickerSec++;
	//userTickerSec++;
	if(noEventTimeOut)noEventTimeOut--;
	if(payPromptTimeOut)payPromptTimeOut--;
	m_rtc_get(&sysRtcDataTime);
	event_rtc_date_time_x();
	if(noEventTimeOut<=1){
		menu=MENU_HOME;
		subMenu=subMENU_HOME_MAIN;
	}
	tm=osKernelSysTick();
	#if 1
	if(payPromptTimeOut || noEventTimeOut ||  \
		vavleState==VALVE_OFF || sysData.devStatus.bits.bBalanceSta \
		|| pwrStatus!=POWER_STATUS_NORMAL){
		//m_lcd_enable();
		ui_disp_menu();

	}else{
		m_lcd_disable();
	}
	#else
		ui_disp_menu(sysTickerSec);
	#endif
	
	//搞锤子花样
	if(m_gpio_read(FLOW_PULSE_PORT,FLOW_PULSE_PIN)){
		//decanti_strong_pull_up();
	}else{
		//decanti_strong_pull_up_release();
	}
	tm=osKernelSysTick()-tm;
	__nop();
}

bool shellOpened=false;
/*
bool fi_szrq_shell_open_close_vavle_close_en(void)
{
	bool ret=false;
	__szrq_valveCtrlByte_t szrqVctrl;
	szrqVctrl.t8=sysData.szrqValveCtrlByte ;
	if(szrqVctrl.bits.bShellOpenEn){
		ret=true;
	}
	return ret;
}
*/
void event_shell_open_process(void)
{
	m_shell_open_rcc_enable();
	//m_gpio_config_shell_open();
	if(m_gpio_read(SHELL_OPNE_DEC_PORT,SHELL_OPNE_DEC_PIN)){
		//osDelay(20)
		//shellOpened=true;
		sysData.devStatus.bits.bShellOpenB=1;
		sysData.lockReason.bits.bShellOpen=1;
		if(1){
			vavle_off_from_app(OFF_REASON_SHELL_OPEN);
			
		}else{
			sysData.lockReason.bits.bShellOpen=0;
		}
		//if(!szrqRtcSync)return 0;
		if(!shellOpened){
			//if(!szrqRtcSync)return;
			shellOpened=true;
			if(noEventTimeOut<NO_EVEN_MAX_TIME_OUT)noEventTimeOut=NO_EVEN_MAX_TIME_OUT;
			//osDelay(2000);
			//even_send_msg_to_rf_send_warnning(1,SZRQ_WARNVAL_SHELL_OPENB);	
			
		}
	}else{
		sysData.devStatus.bits.bShellOpenB=0;
		//if(sysData.DLCS<=DLC_STATUS_B){
		//if(!fi_szrq_shell_open_close_vavle_close_en()){
			sysData.lockReason.bits.bShellOpen=0;
		//}
		if(shellOpened){
			//if(!szrqRtcSync)return;
			shellOpened=false;	
			if(noEventTimeOut<NO_EVEN_MAX_TIME_OUT)noEventTimeOut=NO_EVEN_MAX_TIME_OUT;
			//even_send_msg_to_rf_send_warnning(0,SZRQ_WARNVAL_SHELL_OPENB);	
		}
		
	}
}
bool gasLeaked=false;
void event_alarm_process(void)
{
	//add dect
	m_gpio_alarm_dect_port_rcc_enable();
	if(!(m_gpio_read(ALARM1_DECT_PORT,ALARM1_DECT_PIN))){
#if config_USED_ALARM_LCD	
		if(DeviceEvent.bits.bAlarmConn==0){
			event_set_globle_time_out_max();
		}
#endif		
		sysData.devStatus.bits.bAlarmConn=1;
	}else{
		sysData.devStatus.bits.bAlarmConn=0;
		
	}

	m_gpio_config_alarm_irq_disable();
	m_gpio_alarm_port_rcc_enable();
	if(m_gpio_read(ALARM1_PORT,ALARM1_PIN)){
		sysData.devStatus.bits.bLeakage=0;
		m_gpio_config_alarm_irq_re_enable();
		
		if(gasLeaked){
			gasLeaked=false;	
			if(noEventTimeOut<NO_EVEN_MAX_TIME_OUT)noEventTimeOut=NO_EVEN_MAX_TIME_OUT;
			//even_send_msg_to_rf_send_warnning(0,SZRQ_WARNVAL_LEAKAGE);	
			//even_send_msg_to_rf_send_pop(__hzrq_POP_TYPE_EVENT,__hzrq_EVENT_STE,0);	
		}
		return;
	}
	if(sysData.devStatus.bits.bLeakage==0)osDelay(400);
	
	m_gpio_alarm_port_rcc_enable();
	if(!(m_gpio_read(ALARM1_PORT,ALARM1_PIN))){
		if(sysData.devStatus.bits.bLeakage==0){
			event_set_globle_time_out_max();
			m_lcd_enable();
		}
		sysData.devStatus.bits.bLeakage=1;
		//lockReason.bits.bLeakage=1;
		
		vavle_off_from_app(OFF_REASON_LEAKAGE);
		
		if(!gasLeaked){
			gasLeaked=true;	
			if(noEventTimeOut<NO_EVEN_MAX_TIME_OUT)noEventTimeOut=NO_EVEN_MAX_TIME_OUT;
			//even_send_msg_to_rf_send_warnning(1,SZRQ_WARNVAL_LEAKAGE);	
			even_send_msg_to_rf_send_pop(__hzrq_POP_TYPE_EVENT,__hzrq_EVENT_STE,1);			
		}
		
	}else{
	
	}
	__nop();
	m_gpio_config_alarm_irq_re_enable();
}
/*
bool fi_szrq_ste_close_vavle_close_en(void)
{
	bool ret=false;
	__szrq_valveCtrlByte_t szrqVctrl;
	szrqVctrl.t8=sysData.szrqValveCtrlByte ;
	if(szrqVctrl.bits.bSteEn){
		ret=true;
	}
	return ret;
}
*/
bool steStatusied=false;
void event_ste_process(void)
{
	m_gpio_config_ste_irq_disable();
	m_gpio_ste_port_rcc_enable();
	if(m_gpio_read(STE_PORT,STE_PIN)){
		sysData.devStatus.bits.bStrongMagnetic=0;
		m_gpio_config_ste_irq_re_enable();
		if(steStatusied){
			steStatusied=false;
			if(noEventTimeOut<NO_EVEN_MAX_TIME_OUT)noEventTimeOut=NO_EVEN_MAX_TIME_OUT;
			//even_send_msg_to_rf_send_warnning(0,SZRQ_WARNVAL_STE);
		}
		return;
	}
	osDelay(100);
	m_gpio_ste_port_rcc_enable();
	if(!(m_gpio_read(STE_PORT,STE_PIN))){
		sysData.devStatus.bits.bStrongMagnetic=1;
		if(sysData.DLCS >= DLC_STATUS_B){
			if(sysData.lockReason.bits.bStrongMagnetic<MAX_STE_OFF_TIMES && vavleState==VALVE_ON){
				if(1){
					sysData.lockReason.bits.bStrongMagnetic++;
				}else{
					sysData.lockReason.bits.bStrongMagnetic=0;;
				}
				osDelay(100);
			}
		}else{
			sysData.lockReason.bits.bStrongMagnetic=1;
		}
		if(1){
			vavle_off_from_app(OFF_REASON_STRONG_MAGTIC);
		}
		if(!steStatusied){
			steStatusied=true;		
			if(noEventTimeOut<NO_EVEN_MAX_TIME_OUT)noEventTimeOut=NO_EVEN_MAX_TIME_OUT;
			//even_send_msg_to_rf_send_warnning(1,SZRQ_WARNVAL_STE);
			
		}

	}else{
		//DeviceEvent.bits.bStrongMagnetic=0;
		
	}
	__nop();
	m_gpio_config_ste_irq_re_enable();
}



void flow_warning(void)
{
	if(sysData.devStatus.bits.bBalanceSta){
		
		if(sysData.devStatus.bits.bBalanceSta==CNM_BALANCE_WARNING){
			if(!(rtVolume & 0x03ul)){
				buzzer_beap_ms(10);
			}
		}
		else if(sysData.devStatus.bits.bBalanceSta>CNM_BALANCE_WARNING){
			buzzer_beap_ms(10);             
		}
		
	}
}

uint8_t adcSampleIndex=0;
uint8_t event_process_check_batteray(void)
{
	//uint32_t t32;
	m_adc_init();
	if(adcSampleIndex==0){
		get_internal_vref_rtadc();
		adcSampleIndex++;
		return 0;		
	}
	if(adcSampleIndex)adcSampleIndex=0;
	get_battary_rtadc();
	pwrStatus=get_power_status();
    pwr_status_shift();
	

	
	
    if(pwrArr[2]!=pwrArr[3]){
        noEventTimeOut=NO_EVEN_MAX_TIME_OUT;
    }
	if(pwrArr[3]==POWER_STATUS_DOWN   && pwrArr[2]== POWER_STATUS_NORMAL)
	{		
		even_send_msg_to_rf_send_pop(__hzrq_POP_TYPE_EVENT,__hzrq_EVENT_POWER_DOWN,1);
	}
	if(pwrArr[3]==POWER_STATUS_LOW   && pwrArr[2]== POWER_STATUS_NORMAL)
	{		
		even_send_msg_to_rf_send_pop(__hzrq_POP_TYPE_EVENT,POWER_STATUS_LOW,1);
	}	
	
	if(pwrArr[0]==POWER_STATUS_LOW && pwrArr[3]== POWER_STATUS_NORMAL){

	}	
	if(pwrStatus==POWER_STATUS_LOW){
		sysData.devStatus.bits.bPwrStatus=POWER_STATUS_LOW;
//		if(nbRun==false){
//			
//		}
		
	}else 	if(pwrStatus==POWER_STATUS_DOWN  ){
		sysData.devStatus.bits.bPwrStatus=POWER_STATUS_DOWN;
		vavle_off_from_app(OFF_REASON_LO_VOLETAGE);  
		
	}
	else if(pwrStatus==POWER_STATUS_OVER){
		vavle_off_from_app(OFF_REASON_HI_VOLETAGE);
		sysData.devStatus.bits.bPwrStatus =POWER_STATUS_OVER;
	}else{
		
		if(sysData.devStatus.bits.bPwrStatus==POWER_STATUS_DOWN){
			//even_send_msg_to_rf_send_warnning(0,SZRQ_WARNVAL_PDBAT0);			
		}
		
		sysData.devStatus.bits.bPwrStatus =POWER_STATUS_NORMAL;
	}
	
	if(pwrStatus==POWER_STATUS_DOWN || pwrStatus==POWER_STATUS_LOW){
		nbPsmStateMachine=NB_PSM_SM_AT;
		//mtkStateMachine=MTK_POWER_OFF;
		
	}
    return 1;
}



void event_process_day_change(void)
{
	api_sysdata_save();
	/*
	if(sysData.DWM>DWM_COMMON_MODE){
		data_api_day_change();
	}
	*/

}
/*
void event_pop_every_day(void)
{
	uint32_t t32;
	
	if(sysData.DLCS==DLC_STATUS_A)return;

	t32=cal_second_in_day();
	if(t32<popTimesEveryday){
		flgEverydayPoped=false;
	}else{
		if(!flgEverydayPoped){
			flgEverydayPoped=true;
			even_send_msg_to_rf_every_day();
		}
	}
}

*/




void event_start_waite_power_up(void)
{
	uint8_t ret;
	while(1){
		ret=event_process_check_batteray();
		if(ret){
			#if(BATTARY_TYPE==BATTARY_TYPE_LI_SOCL)
			if(voltAvdd>=2400)break;
			#else
			if(voltAvdd>=2700)break;
			#endif
		}
		osDelay(1000);
	}
}
/*
void event_process_ir_received(void)
{
	uint16_t i,sta=0;
	uint8_t qn,sum;
	uint32_t tm,t32=0x00; 	
	irCmdDef_t* stp=(irCmdDef_t*)irRecevedBuf; 
	tm=osKernelSysTick();
	do{
		sta=0;
		if(osKernelSysTick()-tm>15000)break;
		event_disable_sleep_in_lwp();
		__nop();
		if(irRecevedByte < sizeof(irCmdDef_t)){
			ir_received_ready();
			//break;
			return;
		}
		if((stp->headCode[0] != IR_HEAD_CODE ) || (stp->headCode[1] != IR_HEAD_CODE))break;
		if(stp->endCode!= IR_END_CODE)break;
		if(stp->cmd!=IR_CMD_SET_ID)break;
		sum=0;
		for(i=2;i<9;i++){sum+=irRecevedBuf[i];}
		if(sum!=stp->sum)break;
		api_sysdata_save();
		m_mem_cpy_len((uint8_t*)(&t32),stp->id,sizeof(uint32_t));
		sysData.ID=t32;
		qn=stp->qn;
		//if(qn >= (sizeof(qnTable)/sizeof(qnTable[0])-1))break;
		if(qn>= 9-1)break;
		sysData.QN=qnTable[qn];
		sysData.QMAX=qnTable[qn+1];
		api_sysdata_save();
		sta=1;
		break;
	}while(1);
	if(sta){
		buzzer_beap_ms(500); 
		event_disable_sleep_in_lwp();
		menu=MENU_ID;
		subMenu=subMENU_ID_LO;
	}else{
		buzzer_beap_ms(100);
		osDelay(100);
		buzzer_beap_ms(100);
		osDelay(100);
		buzzer_beap_ms(100);	
		menu=MENU_HOME;
		subMenu=subMENU_HOME_MAIN;
	}
	
}
*/

void vTheadEvent(void * pvParameters)
{
	volatile uint32_t tm=0x00;
	(void)pvParameters;
	osEvent event;
	
	m_adc_init();
	m_gpio_config_shell_open();
	//m_flow_data_init();
	m_flow_all_data_init();
	//event_process_check_batteray();
	event_start_waite_power_up();

	
	even_send_msg_to_rf_off();
	//valve_ulock_fi();

	#if config_NB_PSM_EN
	
	if(sysData.DLCS>=DLC_STATUS_B){
		even_send_msg_to_rf_send_pop(__hzrq_POP_TYPE_EVENT,__hzrq_EVENT_RST,1);
	}
	
	#endif
	tm=osKernelSysTick();
	while(1){
		tm=osKernelSysTick()-tm;
		__nop();
		event=osSignalWait(flg_EVENT_ALL,osWaitForever);
		tm=osKernelSysTick();
		if(event.value.signals & flg_EVENT_KEY_DOWN){
			payPromptTimeOut=0;
			event_key_down_process();
		}
		if(event.value.signals & flg_EVENT_RTC_WAKE_UP){
			event_rtc_process_sec();
			//m_flow_test(); 
			m_flow_process_ex();
			event_ste_process();
			event_alarm_process();
			//event_shell_open_process();
			event_process_check_batteray();
			//event_pop_every_day();
			m_soft_wdg_clr();
			__nop();
		}
		
		if(event.value.signals & flg_EVENT_STE_FALLING){
			event_set_globle_time_out_max();
			event_ste_process();
		}
		
		if(event.value.signals & flg_EVENT_ALARM_FALLING){
			event_alarm_process();
		}
		
		if(event.value.signals & flg_EVENT_FLW_FALLING){
			//event_flow_process();
			api_calc_all();
			flow_warning();
			if(flgValveErrSend==false && sysData.devStatus.bits.bValveStatus==VALVE_FAULT){
				flgValveErrSend=true;
				if(sysData.DLCS>=DLC_STATUS_B){
					//even_send_msg_to_start_rf( );
					even_send_msg_to_rf_send_pop(__hzrq_POP_TYPE_EVENT,__hzrq_EVENT_METER_FAULT,1);	
				}
			}			
		}
		if(event.value.signals & flg_EVENT_UNLOCK_ON){
			event_process_unlock();
		}
		if(event.value.signals & flg_EVENT_DAY_CHANGE ){
			event_process_day_change();
		}
		/*
		if(event.value.signals & flg_EVENT_IR_RECEIVED){
		#if cfg_IRDA_EN
			//jevent_process_ir_received();
		#endif
		}
		*/
		if(event.value.signals & flg_EVENT_HI_FLOW){
			//even_send_msg_to_rf_send_warnning(0,SZRQ_WARNVAL_PDBAT0);
			if(overFlowFlgforSendWarning){
				//even_send_msg_to_rf_send_warnning(1,SZRQ_WARNVAL_HF);
				even_send_msg_to_rf_send_pop(__hzrq_POP_TYPE_EVENT,__hzrq_EVENT_FLOW_ABNORMAL,1);	
			}else{
				//even_send_msg_to_rf_send_warnning(0,SZRQ_WARNVAL_HF);
			}
		}
	}
}

void m_thread_create_event(void)
{
	osThreadDef(TheadEvent, vTheadEvent, osPriorityNormal, 0, configMINIMAL_STACK_SIZE*4);
	vTheadEvenID=osThreadCreate(osThread(TheadEvent), NULL);	
}

void flow_process(void)
{
	m_gpio_config_flow_irq_disable();
	do{
		if(m_gpio_read(FLOW_PULSE_PORT,FLOW_PULSE_PIN))break;
		//osDelay(5);
		//if(m_gpio_read(FLOW_PULSE_PORT,FLOW_PULSE_PIN))break;
		__disable_irq();
		__rtVolume++;
		__enable_irq();
		rtVolumeRefreshFlag=true;
		
		m_flow_pulse_timer_table_push(sysTickerSec);
		__nop();
		if(vavleState==VALVE_OFF){
			sysData.devStatus.bits.bValveStatus=1;
		}else{
			sysData.devStatus.bits.bValveStatus=0;
		}
	}while(0);
	//osSignalWait(flg_FLW_FALLING,1);
	m_gpio_config_flow_irq_re_enable();
}

void vTheadFlowPuls(void * pvParameters)
{
	volatile uint32_t tm=0x00;
	(void)pvParameters;
	osEvent event;
	while(1){
		event=osSignalWait(flg_FLW_FALLING,osWaitForever);
		if(event.value.signals & flg_FLW_FALLING)
		{
			flow_process();
			if((void*)vTheadEvenID){
				osSignalSet( vTheadEvenID, flg_EVENT_FLW_FALLING);
			}
		}
	}
}

void m_thread_create_flow_puls(void)
{
	osThreadDef(TheadFlowPuls, vTheadFlowPuls, osPriorityHigh, 0, configMINIMAL_STACK_SIZE*1);
	vTheadFlowPulsID=osThreadCreate(osThread(TheadFlowPuls), NULL);		
}
//file end
