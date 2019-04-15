#include "./includes/includes.h"


uint8_t nbAplReceivedBuffer[MAX_UART_RX_BUFFER_LEN];
uint16_t nbAplReceivedCount;

osMessageQDef(nbSendMsgQ, 8,sizeof(__m_udpSendmsg_t));
volatile osMessageQId nbSendMsgQ;
volatile __m_udpSendmsg_t udpSendmsg={0};
volatile bool  nbRun=false;

TaskHandle_t idTheadUdpId=(osThreadId)NULL;
nbPsmStateMachine_t nbPsmStateMachine=NB_PSM_SM_AT;

__hzrq_sendStaMichine_t sendStaMichine;

extern void nb_process_disable_sleep_in_lwp(void);
#if NB_MODULE_VENDOR==BC95
void vTheadCoapPsm(void * pvParameters)
{
	osEvent event;
	uint8_t buf[128];
	uint16_t redo,ret,sendNum=0;
	uint16_t len;//,t16=10;
	(void)pvParameters;	
	volatile __m_udpSendmsg_t __msg={0};
	
	while(1){
		nbRun=false;
		//osDelay(4000);
		
		
		api_calc_all();
		event=osMessageGet(nbSendMsgQ, osWaitForever );
		if(event.status != osEventMessage)continue;
		__msg.t32=event.value.v;
		api_sysdata_save();
		
		if(__msg.stru.eventMsg &  flg_NB_PROCESS_MODULE_RF_CLOSE){
			if(nbPsmStateMachine==NB_PSM_SM_NRZ)continue;
			baudRateOffset=0;
			m_lpusart1_deinit();
			osDelay(200);
			m_lpusart1_init(9600);		
			nb_module_cfun_off();
			nbPsmStateMachine=NB_PSM_SM_NRZ;
			continue;
		}
		nbRun=true;
		if(__msg.stru.eventMsg & (flg_NB_PROCESS_SEND_REAL | flg_NB_PROCESS_SEND_OLD)){	
			nb_process_disable_sleep_in_lwp();
			
			ret=rf_send_fifo_item_get_unread_num();
			if(ret<=0)continue;
			
			ret=nb_coap_psm_send_ready();
			if(ret<=0){
				menu=MENU_HOME;
				subMenu=subMENU_HOME_MAIN;
				continue;	
			}
			sendStaMichine=__hzrq_SEND_SM_REGISTER;
			len=0;
			do{
				if(sendStaMichine==__hzrq_SEND_SM_REGISTER){
					len=hzrq_load_register_frame(nbAplSendBuffer,sizeof(nbAplSendBuffer));
				}else if(sendStaMichine==__hzrq_SEND_SM_POP){
					
				}else if(sendStaMichine==__hzrq_SEND_SM_CMD){
					
				}else {
					break;
				}
				if(len<=0)break;
				ret=nb_udp_cereg_coap();
				if(ret<=0)break;
				nb_module_receive_buffer_clear();
				redo=0;
				do{
					redo++;
					ret=nb_send_process(nbAplSendBuffer,len);
					if(ret<=0)break;
					ret=nb_received_process();
					if(ret!=0)break;
				}while(redo<3);
				
				if(ret<=0){break;}
				
				if(nb_fota_process(nbAplReceivedBuffer,ret)){sendStaMichine=__hzrq_SEND_SM_FOTA;break;}
				ret=np_received_extract_process(nbAplReceivedBuffer,buf,ret);
				if(ret>sizeof(buf))ret=sizeof(buf);
				m_mem_cpy_len(nbAplReceivedBuffer,buf,ret);
			
				ret=hzrq_comm_received_process(nbAplReceivedBuffer,ret,nbAplSendBuffer,sizeof(nbAplSendBuffer),__msg.stru.popType);	
				if(sendStaMichine==__hzrq_SEND_SM_END){
					if(__hzrqUnSendNum>0){
						__hzrqUnSendNum=rf_send_fifo_delete_tail();
					}
					
					if(__hzrqUnSendNum>0){
						
						sendStaMichine=__hzrq_SEND_SM_REGISTER;
						continue;
					}
				}
				if(ret<=0)break;
				len=ret;
			}while(1);
			
			menu=MENU_HOME;
			subMenu=subMENU_HOME_NB_END;		
			ui_disp_menu();	
			
			if(sendStaMichine==__hzrq_SEND_SM_END || sendStaMichine==__hzrq_SEND_SM_FOTA){
				nbPsmStateMachine=NB_PSM_SM_PSM;
				nb_coap_psm_release_ra();
				nb_coap_enter_psm();	
				//nbPsmStateMachine=NB_PSM_SM_PSM
			}else{
				nb_module_cfun_off();
				nbPsmStateMachine=NB_PSM_SM_NRZ;
			}
			menu=MENU_HOME;
			subMenu=subMENU_HOME_MAIN;		
			ui_disp_menu();			
			
		}
	}	
}
#elif NB_MODULE_VENDOR==BC26


#elif NB_MODULE_VENDOR==L620
/*
void vTheadNbL620(void * pvParameters)
{
	osEvent event;
	uint16_t len,sendRedo;
	int16_t ret;
	(void)pvParameters;	
	mtk_hal_pins_init();
	while(1){
		nbRun=false;
		mtk_hal_pins_init();
		//event=osSignalWait(flg_NB_MODULE_COMM_PROCESS_REQ | flg_NB_MODULE_RF_CLOSE,osWaitForever);
		event=osMessageGet(nbSendMsgQ, osWaitForever );
		nbRun=true;		
		if(event.value.v &  flg_NB_MODULE_RF_CLOSE){
			l620_hal_power_off();
			continue;
		}
		if(event.value.v & flg_NB_MODULE_COMM_PROCESS_REQ){	
			popPackageType=(popPackageType_t)(((event.value.v)>>24) & 0xffUL);
			ret=l620_send_ready();
			if(ret<=0){
				menu=0x00;
				subMenu=0;
				continue;	
			}			
			opRetCode=__15S_OP_NOTHING;
			_15sSendStaMichine=__15S_SEND_SM_POP;	
			do{
				mtk_disable_sleep();
				if(_15sSendStaMichine==__15S_SEND_SM_POP){
					len=qc_comm_send_pop_ready(nbAplSendBuffer,__15E_CB_CTRL_CODE_DATA);
					m_mem_cpy_len(nbAplSendBuffer+len,ICCID,20);
					len+=20;							
				}else if(_15sSendStaMichine==__15S_SEND_SM_CLEAR){
					len=qc_comm_send_pop_ready(nbAplSendBuffer,__15E_CB_CTRL_CODE_RET_CLEAR);
				}else{
					len=qc_comm_ins_reply_ready(nbAplSendBuffer,__15E_CB_CTRL_CODE_DATA);
				}	
				//backup
				
				
				sendRedo=0;
				do{
					ret=0;
					if(sendRedo>=2)break;
					ret=l620_send_process(nbAplSendBuffer,len);	
					sendRedo++;
					if(ret<0)break;
					__nop();
					if(_15sSendStaMichine==__15S_SEND_SM_CLEAR){
						break;
					}
					ret=l620_received(nbAplReceivedBuffer,sizeof(nbAplReceivedBuffer));
					if(ret)break;
				}while(1);
				//
				if(_15sSendStaMichine==__15S_SEND_SM_CLEAR){
					_15sSendStaMichine=__15S_SEND_SM_ACK;
					osDelay(2000);	
					continue;
				}else{
					_15sSendStaMichine=__15S_SEND_SM_CLEAR;
				}
				if(ret<0)break;
				
				ret=l620_received_extract(nbAplReceivedBuffer,ret);
				if(ret<=0)break;
				ret=qc_comm_received_process(nbAplReceivedBuffer,ret);
				__nop();
				if(ret==0 || opRetCode == __15S_OP_NOTHING || opRetCode==__15S_OP_RTC_SYNC)break;

				if(opRetCode ==__15S_OP_OK || opRetCode==__15S_OP_PAY_OK ){
					__nop();
				}else{
					//if(sendNum<19)sendNum=19;
				}	
				mtk_disable_sleep();
				osDelay(2000);				
			}while(1);
			menu=0x00;
			subMenu=subMENU_MAIN_HOME_NB_PSM;	
			if(ret>0 && mtkStateMachine==MTK_READY){
				event=osMessagePeek(nbSendMsgQ, 1);
				if(event.status==osEventMessage)continue;
				//l620_hal_power_off();
			}else{
				//l620_hal_power_off();
			}
			//nb_power_off_process();
			l620_cfun_off();
			l620_hal_power_off();
			if(!(sysData.DLCS==DLC_STATUS_A && !fi_id_writed_in_dlcs_a())){
				if(sysData.DWM != DWM_COMMON_MODE){
					menu=7;
				}else{
					menu=3;
				}
				subMenu=0;
			}
			data_api_day_change();			
		}
	}
}

*/
void vTheadL620(void * pvParameters)
{
	osEvent event;
	uint8_t buf[128];
	uint16_t redo,sendNum=0;
	int16_t ret;
	uint16_t len;//,t16=10;
	(void)pvParameters;	
	volatile __m_udpSendmsg_t __msg={0};
	
	while(1){
		nbRun=false;
		//osDelay(4000);
		
		
		api_calc_all();
		event=osMessageGet(nbSendMsgQ, osWaitForever );
		if(event.status != osEventMessage)continue;
		__msg.t32=event.value.v;
		api_sysdata_save();
		
		if(__msg.stru.eventMsg &  flg_NB_PROCESS_MODULE_RF_CLOSE){
			if(nbPsmStateMachine==NB_PSM_SM_NRZ)continue;
			//baudRateOffset=0;
			m_lpusart1_deinit();
			osDelay(200);
			m_lpusart1_init(9600);		
			//nb_module_cfun_off();
			l620_cfun_off();
			l620_hal_power_off();
			//nbPsmStateMachine=NB_PSM_SM_NRZ;
			continue;
		}
		nbRun=true;
		if(__msg.stru.eventMsg & (flg_NB_PROCESS_SEND_REAL | flg_NB_PROCESS_SEND_OLD)){	
			//nb_process_disable_sleep_in_lwp();
			mtk_disable_sleep();
			ret=rf_send_fifo_item_get_unread_num();
			if(ret<=0)continue;
			
			//ret=nb_coap_psm_send_ready();
			ret=l620_send_ready();
			if(ret<=0){
				menu=MENU_HOME;
				subMenu=subMENU_HOME_MAIN;
				continue;	
			}
			sendStaMichine=__hzrq_SEND_SM_REGISTER;
			len=0;
			do{
				if(sendStaMichine==__hzrq_SEND_SM_REGISTER){
					len=hzrq_load_register_frame(nbAplSendBuffer,sizeof(nbAplSendBuffer));
				}else if(sendStaMichine==__hzrq_SEND_SM_POP){
					
				}else if(sendStaMichine==__hzrq_SEND_SM_CMD){
					
				}else {
					break;
				}
				if(len<=0)break;
				//ret=nb_udp_cereg_coap();
				//if(ret<=0)break;
				//nb_module_receive_buffer_clear();
				redo=0;
				do{
					redo++;
					if(sendStaMichine==__hzrq_SEND_SM_REGISTER || sendStaMichine==__hzrq_SEND_SM_POP){
						len=_hzrq_load_frame_mod_send_index(nbAplSendBuffer,len);
					}
					ret=l620_send_process(nbAplSendBuffer,len);
					if(ret<=0)break;
					//ret=nb_received_process();
					ret=l620_received(nbAplReceivedBuffer,sizeof(nbAplReceivedBuffer));
					if(ret!=0)break;
				}while(redo<3);
				
				if(ret<=0){break;}
				
				//if(nb_fota_process(nbAplReceivedBuffer,ret)){sendStaMichine=__hzrq_SEND_SM_FOTA;break;}
				//ret=np_received_extract_process(nbAplReceivedBuffer,buf,ret);
				ret=l620_received_extract(nbAplReceivedBuffer,ret);
				if(ret>sizeof(buf))ret=sizeof(buf);
				//m_mem_cpy_len(nbAplReceivedBuffer,buf,ret);
			
				ret=hzrq_comm_received_process(nbAplReceivedBuffer,ret,nbAplSendBuffer,sizeof(nbAplSendBuffer),__msg.stru.popType);	
				if(sendStaMichine==__hzrq_SEND_SM_END){
					if(__hzrqUnSendNum>0){
						__hzrqUnSendNum=rf_send_fifo_delete_tail();
					}
					if(__hzrqUnSendNum>0){
						sendStaMichine=__hzrq_SEND_SM_REGISTER;
						continue;
					}
				}
				if(ret<=0)break;
				len=ret;
				noNetworkTimer=0;
			}while(1);
			if(ret<=0){	
				sysData.hzrqCommFailTimes++;
			}
			menu=MENU_HOME;
			subMenu=subMENU_HOME_NB_END;		
			ui_disp_menu();	
			
			if(ret>0 && mtkStateMachine==MTK_READY){
				event=osMessagePeek(nbSendMsgQ, 1);
				if(event.status==osEventMessage)continue;
				//l620_hal_power_off();
			}else{
				//l620_hal_power_off();
			}
			//nb_power_off_process();
			l620_cfun_off();
			l620_hal_power_off();
			menu=MENU_RSSI;
			subMenu=subMENU_RSSI;
			//data_api_day_change();		
		}
	}	
}
#else


#endif




void m_thread_create_nb_udp(void)
{
	nbSendMsgQ=osMessageCreate(osMessageQ(nbSendMsgQ),NULL);
#if NB_MODULE_VENDOR==BC95
	osThreadDef(TheadCoap, vTheadCoapPsm, osPriorityNormal, 0, configMINIMAL_STACK_SIZE*4);
#elif NB_MODULE_VENDOR==BC26
	//todo
#elif NB_MODULE_VENDOR==L620
	//todo  vTheadL620
	osThreadDef(TheadCoap, vTheadL620, osPriorityNormal, 0, configMINIMAL_STACK_SIZE*4);
#else
	#error "defined nb vendor error!!"
#endif	
	
	idTheadUdpId=osThreadCreate(osThread(TheadCoap), NULL);			

}
//file end
