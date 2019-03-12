#include "./includes/includes.h"


uint8_t nbAplReceivedBuffer[MAX_UART_RX_BUFFER_LEN];
uint16_t nbAplReceivedCount;

osMessageQDef(nbSendMsgQ, 8,sizeof(uint32_t));
volatile osMessageQId nbSendMsgQ;
volatile __m_udpSendmsg_t udpSendmsg={0};
volatile bool  nbRun=false;

TaskHandle_t idTheadUdpId=(osThreadId)NULL;
nbPsmStateMachine_t nbPsmStateMachine=NB_PSM_SM_AT;

__hzrq_sendStaMichine_t sendStaMichine;

extern void nb_process_disable_sleep_in_lwp(void);
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
		
		/*
		nbRun=false;
		event=osSignalWait(flg_NB_MODULE_UDP_PROCESS_REQ | flg_NB_MODULE_RF_CLOSE,osWaitForever);
		nbRun=true;

		if((event.value.signals ) & (flg_NB_MODULE_UDP_PROCESS_REQ | flg_NB_MODULE_RF_CLOSE)){

		}else{
			continue;
		}

		if(event.value.signals &  flg_NB_MODULE_RF_CLOSE){
			if(nbPsmStateMachine==NB_PSM_SM_NRZ)continue;
			
			baudRateOffset=0;
			
			m_lpusart1_deinit();
			osDelay(200);
			m_lpusart1_init(9600);		
			nb_module_cfun_off();
			nbPsmStateMachine=NB_PSM_SM_NRZ;
			continue;
		}

		if(event.value.signals & flg_NB_MODULE_UDP_PROCESS_REQ){
			menu=0x00;
			subMenu=subMENU_HOME_NB_CONN;	
			ui_disp_menu();
			
			nb_baudrate_offset_scan();
			if(pwrStatus<POWER_STATUS_LOW){
				if(nbPsmStateMachine!=NB_PSM_SM_NRZ){
					m_lpusart1_deinit();
					osDelay(100);
					m_lpusart1_init(9600);	
					nb_module_cfun_off();	
					nbPsmStateMachine=NB_PSM_SM_NRZ;
				}				
			};
			sendNum=0;
			//rtcSyncTm=0;
			nb_process_disable_sleep_in_lwp();
			if(!(nb_coap_psm_send_ready()))continue;

			nb_create_socket();
			protocolCmd=0xff;
			opRetCode=__15S_OP_NOTHING;
			_15sSendStaMichine=__15S_SEND_SM_POP;
			sendNum=0;

			do{
				nb_process_disable_sleep_in_lwp();
				if(_15sSendStaMichine==__15S_SEND_SM_POP){
					len=qc_comm_send_pop_ready(nbAplSendBuffer,__15E_CB_CTRL_CODE_DATA);
					m_mem_cpy_len(nbAplSendBuffer+len,ICCID,20);
					len+=20;							
				}else if(_15sSendStaMichine==__15S_SEND_SM_CLEAR){
					len=qc_comm_send_pop_ready(nbAplSendBuffer,__15E_CB_CTRL_CODE_RET_CLEAR);
					//osDelay(2000);
				}else{
					len=qc_comm_ins_reply_ready(nbAplSendBuffer,__15E_CB_CTRL_CODE_DATA);
					//osDelay(2000);
				}				
				osDelay(1000);
				
				if(nb_get_cereg()==0)break;
				//osDelay(4000);
				//add 2018.05.13
				nb_module_receive_buffer_clear();
				//add end
				if(len>sizeof(buf))len=sizeof(buf);
				m_mem_cpy_len(buf,nbAplSendBuffer,len);
				ret=nb_send_process(nbAplSendBuffer,len);
				//ret=nb_udp_send_coap_ex(nbAplSendBuffer,len,false);	
				//if(!ret){nbPsmStateMachine=NB_PSM_SM_NRZ;break;}
				sendNum++;


				if(_15sSendStaMichine==__15S_SEND_SM_CLEAR){
					_15sSendStaMichine=__15S_SEND_SM_ACK;
					continue;
				}else{
					_15sSendStaMichine=__15S_SEND_SM_CLEAR;
				}
				redo=0x00;
				do{	
					if(redo>=1)break;	
					ret=nb_received_process();	
					//FFFE01134C9A0000
					if(!ret){
						nbPsmStateMachine=NB_PSM_SM_NO_RECEIVED;
						nb_process_disable_sleep_in_lwp();
						osDelay(4000);
						m_mem_cpy_len(nbAplSendBuffer,buf,len);
						nb_send_process(nbAplSendBuffer,len);
						redo++;
					}else{
						break;
					}
				}while(1);		

				//ret=nb_udp_send_coap_ex(nbAplSendBuffer,len,false);	
				if(!ret){nbPsmStateMachine=NB_PSM_SM_NO_RECEIVED;break;}
				//fota
				
				if(nb_fota_process(nbAplReceivedBuffer,ret)){nbPsmStateMachine=NB_PSM_SM_NO_RECEIVED;break;}
				
				ret=np_received_extract_process(nbAplReceivedBuffer,buf,ret);
				
				if(ret>sizeof(buf))ret=sizeof(buf);
				m_mem_cpy_len(nbAplReceivedBuffer,buf,ret);
			
				ret=qc_comm_received_process(nbAplReceivedBuffer,ret);
			
				if(ret==0 || opRetCode == __15S_OP_NOTHING || opRetCode==__15S_OP_RTC_SYNC)break;

				if(opRetCode ==__15S_OP_OK || opRetCode==__15S_OP_PAY_OK ){
					__nop();
				}else{
					if(sendNum<19)sendNum=19;
				}				
				
				if(sendNum>=21)break;
				if(pwrStatus==POWER_STATUS_DOWN)break;

			}while(1);
			
			opRetCode=__15S_OP_NOTHING;
			__nop();
			//处理过程中状态置NB_PSM_SM_NRZ并未关闭视频部分
			if(nbPsmStateMachine==NB_PSM_SM_NRZ){
				nb_module_cfun_off();
			}else{
				nb_coap_psm_release_ra();
				nb_coap_enter_psm();			
			}
			
			if(nbPsmStateMachine==NB_PSM_SM_NO_RECEIVED){
				nbPsmStateMachine=NB_PSM_SM_NRZ;
				nb_module_cfun_off();
			}
			if(nbPsmStateMachine!=NB_PSM_SM_NRZ)nbPsmStateMachine=NB_PSM_SM_PSM;
			
			//nb_power_off_process();
			if(!(sysData.DLCS==DLC_STATUS_A && !fi_id_writed_in_dlcs_a())){
				if(sysData.DWM!= DWM_COMMON_MODE){
					menu=7;
				}else{
					menu=3;
				}
				subMenu=0;
			}
			data_api_day_change();
		}
		*/
	}	
}



void m_thread_create_nb_udp(void)
{
	nbSendMsgQ=osMessageCreate(osMessageQ(nbSendMsgQ),NULL);
	osThreadDef(TheadCoap, vTheadCoapPsm, osPriorityNormal, 0, configMINIMAL_STACK_SIZE*4);
	idTheadUdpId=osThreadCreate(osThread(TheadCoap), NULL);			

}
//file end
