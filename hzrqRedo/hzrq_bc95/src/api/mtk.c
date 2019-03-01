//#include "./includes/includes.h"
#include "./includes/includes.h"

#define nbAplSendBuffer 	uartSendBuf

TaskHandle_t idTheadUdpId=(osThreadId)NULL;

uint8_t CIMI[16]={0};
uint8_t	CGSN[16]={0};
uint8_t	ICCID[21]={0};
uint8_t SPVER[8]={0};
uint8_t EARFCN[8]="cn:0000";

uint8_t nbAplReceivedBuffer[MAX_UART_RX_BUFFER_LEN];
uint16_t nbAplReceivedCount;

bool nbRun=false;
//extern const uint8_t HexTable[];
__mtkStateMachine_t mtkStateMachine=MTK_POWER_OFF;

//osMessageQDef(nbSendMsgQ, 8,sizeof(uint32_t));
__szrq_udpSendmsg_t udpSendmsg={0};
osMessageQDef(nbSendMsgQ, 8,sizeof(__szrq_udpSendmsg_t));
osMessageQId nbSendMsgQ;



uint32_t mtk_time_ms_2_s(uint32_t ms)
{
	uint32_t ret;
	ret=ms/1000;
	if(ms%1000)ret++;
	return ret;
}


void mtk_disable_sleep(void)
{
	noEventTimeOut=NO_EVEN_MAX_TIME_OUT;
}

void mtk_enable_sleep(void)
{
	noEventTimeOut=0;
}
//Expected response

int16_t mtk_at_cmd(uint8_t* cmd,uint8_t* expResp,uint8_t* rbuf,uint16_t ssize,uint32_t tm)
{
	int16_t ret=0;
	uint16_t i,t16;
	uint32_t tmSec;
	
	osEvent event;
	if(!rbuf)return 0;
	mtk_disable_sleep();
	m_mem_set(rbuf,'\0',ssize);
	if(cmd)m_lpusart1_send_str(cmd);
	tmSec=mtk_time_ms_2_s(tm);
	for(i=0;i<tmSec;i++){
		if(pwrStatus==POWER_STATUS_DOWN){ret=-1;break;};
		mtk_disable_sleep();
		ret=0;
		event=osSignalWait(flg_NB_MODULE_ALL_BITS,1000);
		if(event.status!=osEventSignal)continue;
		if(event.value.signals & flg_NB_MODULE_UART_RECEIVED_LF){
			if(uartRceivedBufCountUser){
				t16=uartRceivedBufCountUser;
				if(t16>ssize){
					t16=ssize;
				}
				if((void*)(rbuf) != (void*)(uartReceivedBufUser)){
					m_mem_cpy_len(rbuf,uartReceivedBufUser,t16);
				}
				//return t16;
				if(expResp){
					ret=m_str_match(rbuf,expResp);
					if(ret){
						ret=t16;
						break;
					}
				}
				if(m_str_match(rbuf,(uint8_t*)"ERROR")){ret=0;break;}
			}
			//m_mem_set(rbuf,'\0',ssize);
		}
		if(event.value.signals & flg_NB_MODULE_UART_RECEIVED_ERROR){
			ret=0;
			m_lpusart1_deinit();
			osDelay(200);
			m_lpusart1_init(9600);
			__nop();			
		}
	}
	return ret;
}
/*
int16_t mtk_at_cmd_const(const l620AtCmd_t* cmd,uint8_t* rbuf,uint16_t ssize,uint32_t tm)
{
	int16_t ret;
	ret=mtk_at_cmd((uint8_t*)cmd->cmd,(uint8_t*)cmd->resp,rbuf,ssize,tm);
	return ret;
}
*/
int16_t mtk_hal_pins_init(void)
{
	m_gpio_set_mode(NB_MTK_PWR_PORT,NB_MTK_PWR_PIN,GPIO_MODE_INPUT);
	m_gpio_set_mode(NB_MTK_PSM_PORT,NB_MTK_PSM_PIN,GPIO_MODE_INPUT);
	m_gpio_set_mode(NB_MTK_RST_PORT,NB_MTK_RST_PIN,GPIO_MODE_INPUT);
	return 1;
	
}
#if NB_MODULE_VENDOR== L620
int16_t l620_hal_power_on(void)
{
	int16_t ret;
	uint16_t i=0;
	
	mtk_disable_sleep();
	m_uart4_deinit();
	osDelay(200);
	m_uart4_init(9600);		
	//send at
	
	
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	m_gpio_set_mode(NB_MTK_PWR_PORT,NB_MTK_PWR_PIN,GPIO_MODE_OUTPUT_PP);
	m_gpio_write(NB_MTK_PWR_PORT,NB_MTK_PWR_PIN,NB_MTK_PWR_PIN);
	osDelay(400);
	//m_gpio_write(NB_MTK_PWR_PORT,NB_MTK_PWR_PIN,0);
	m_gpio_set_mode(NB_MTK_PWR_PORT,NB_MTK_PWR_PIN,GPIO_MODE_INPUT);
	for(i=0;i<3;i++){
			
		
		ret=mtk_at_cmd((uint8_t*)"AT\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
		if(ret)break;
		if(ret<0)return ret;
	}
	__nop();
	ret=mtk_at_cmd(NULL,(uint8_t*)"+EGACT:1,1,1,1",recbuf,reclen,20*configTICK_RATE_HZ);
	return ret;	
}

int16_t l620_hal_power_off(void)
{
	uint16_t i;
	mtk_disable_sleep();
	m_uart4_deinit();
	osDelay(200);
	m_uart4_init(9600);	
	
	m_gpio_set_mode(NB_MTK_PWR_PORT,NB_MTK_PWR_PIN,GPIO_MODE_OUTPUT_PP);
//	m_gpio_write(NB_MTK_PWR_PORT,NB_MTK_PWR_PIN,NB_MTK_PWR_PIN);
//	osDelay(800);
//	m_gpio_write(NB_MTK_PWR_PORT,NB_MTK_PWR_PIN,0);
//	osDelay(500);
	m_gpio_write(NB_MTK_PWR_PORT,NB_MTK_PWR_PIN,NB_MTK_PWR_PIN);
	for(i=0;i<11;i++){
		mtk_disable_sleep();
		osDelay(1000);
	}		
	//m_gpio_write(NB_MTK_PWR_PORT,NB_MTK_PWR_PIN,0);
	m_gpio_set_mode(NB_MTK_PWR_PORT,NB_MTK_PWR_PIN,GPIO_MODE_INPUT);
	__nop();
	mtkStateMachine=MTK_POWER_OFF;
	return 1;
}

int16_t l620_get_ver(void)
{
	int16_t ret;
	uint16_t t16;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	//ret=l620_at_cmd_const(&L620_ATCMD_MENGINFO,recbuf,reclen,3000);	
	ret=mtk_at_cmd((uint8_t*)"ATI\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
	if(ret<=0)return ret;
	/*
	ret=m_str_match_ex(recbuf,(uint8_t*)"L620_V",&t16);
	if(!ret)return ret;
	t16=t16+sizeof("L620_V")-1;
	m_mem_cpy_len(SPVER,(uint8_t*)"l620-u4",7);
	m_mem_cpy_len(SPVER+6,recbuf+t16,1);
	SPVER[7]='\0';
	return ret;		
	*/
	ret=m_str_match_ex(recbuf,(uint8_t*)"L620_v",&t16);
	if(!ret)return ret;
	t16=t16+sizeof("L620_v")-1;	
	m_mem_cpy_len(SPVER,recbuf+t16,2);
	m_mem_cpy_len(SPVER+2,recbuf+t16+3,5);
	SPVER[7]='\0';
	return ret;		
}

int16_t l620_get_menginfo(void)
{
	int16_t ret;
	uint16_t t16;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	//ret=mtk_at_cmd_const(&L620_ATCMD_MENGINFO,recbuf,reclen,3000);
	ret=mtk_at_cmd((uint8_t*)"AT*MENGINFO=0\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);	
	if(ret<=0)return ret;
	ret=m_str_match_ex(recbuf,(uint8_t*)"*MENGINFOSC: ",&t16);
	if(!ret)return ret;
	t16=t16+sizeof("*MENGINFOSC: ")-1;
	m_mem_cpy_len(EARFCN,(uint8_t*)"cn:0000",7);
	m_mem_cpy_len(EARFCN+3,recbuf+t16,4);
	EARFCN[7]='\0';
	return ret;		
}

//at+cesq
int16_t l620_get_csq(void)
{
	int16_t ret;
	uint16_t t16,csq;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	//ret=mtk_at_cmd_const(&L620_ATCMD_CSQ,recbuf,reclen,3000);	
	ret=mtk_at_cmd((uint8_t*)"AT+CESQ\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
	if(ret<=0)return ret;
	
	ret=m_str_match_ex(recbuf,(uint8_t*)"+CESQ: ",&t16);
	if(!ret)return ret;
	t16=t16+sizeof("+CESQ: ")-1;
	csq=my_scanf_int16(recbuf+t16,',',&t16);
	//m_mem_cpy_len(ICCID,recbuf+t16,20);
	//ICCID[20]='\0';
	ret=-110;
	ret=ret+(csq-1);
	return ret;		
}

int16_t l620_get_ccid(void)
{
	int16_t ret;
	uint16_t t16;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	//ret=mtk_at_cmd_const(&L620_ATCMD_CCID,recbuf,reclen,3000);	
	ret=mtk_at_cmd((uint8_t*)"AT*MICCID\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);	
	if(ret<=0)return ret;
	ret=m_str_match_ex(recbuf,(uint8_t*)"*MICCID: ",&t16);
	if(!ret)return ret;
	t16=t16+sizeof("*MICCID: ")-1;
	m_mem_cpy_len(ICCID,recbuf+t16,20);
	ICCID[20]='\0';
	return ret;	
}

int16_t l620_get_cgsn(void)
{
	int16_t ret;
	uint16_t t16;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	//ret=mtk_at_cmd_const(&L620_ATCMD_CGSN,recbuf,reclen,3000);
	ret=mtk_at_cmd((uint8_t*)"AT+CGSN\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);	
	if(ret<=0)return ret;
	ret=m_str_match_ex(recbuf,(uint8_t*)"+CGSN: ",&t16);
	if(!ret)return ret;
	t16=t16+sizeof("+CGSN: ")-1;
	m_mem_cpy_len(CGSN,recbuf+t16,15);
	CGSN[15]='\0';
	return ret;
}


int16_t l620_register_net(void)
{
	int16_t ret;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	mtk_at_cmd((uint8_t*)"AT\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
	mtk_at_cmd((uint8_t*)"AT\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);

	mtk_at_cmd((uint8_t*)"ATE0\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
	//mtk_at_cmd_const(&L620_ATCMD_SM_LOCK,recbuf,reclen,1000);
	mtk_at_cmd((uint8_t*)"AT+SM=LOCK\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
	//
	l620_get_cgsn();
	l620_get_ccid();
	ret=l620_get_csq();
	rssi=(int8_t)(ret);
	l620_get_menginfo();
	l620_get_ver();
	return 1;
}

int16_t l620_register_iot(void)
{
	uint8_t sbuf[64];
	uint16_t t16;
	int16_t ret;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	uint8_t* p=sbuf;
	//at+m2mclinew=117.60.157.137,5683,"869858030007463",90
	t16=m_str_cpy(p,(uint8_t*)"at+m2mclinew=");
	p=p+t16-1;
	t16=m_str_cpy(p,(uint8_t*)IOT_IP);
	p=p+t16-1;
	t16=m_str_cpy(p,(uint8_t*)",5683,\"");
	p=p+t16-1;
	m_mem_cpy_len(p,CGSN,15);
	p=p+15;
	t16=m_str_cpy(p,(uint8_t*)"\",90\r\n");
	p=p+t16;
	*p='\0';
	t16=0;
	do{
		t16++;
		ret=mtk_at_cmd(sbuf,(uint8_t*)"+M2MCLI:observe success",recbuf,reclen,20*configTICK_RATE_HZ);
		if(ret!=0)break;
		ret=mtk_at_cmd((uint8_t*)"at+m2mclidel\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
	}while(t16<3);
	__nop();
	return ret;
}

int16_t l620_hal_wakeup(void)
{
	//return 0;
	uint16_t i;
	int16_t ret;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	m_gpio_set_mode(NB_MTK_PSM_PORT,NB_MTK_PSM_PIN,GPIO_MODE_OUTPUT_PP);
	m_gpio_write(NB_MTK_PSM_PORT,NB_MTK_PSM_PIN,NB_MTK_PSM_PIN);
	osDelay(200);
	//m_gpio_write(NB_MTK_PSM_PORT,NB_MTK_PSM_PIN,0);	
	m_gpio_set_mode(NB_MTK_PSM_PORT,NB_MTK_PSM_PIN,GPIO_MODE_INPUT);
	for(i=0;i<3;i++){
		ret=mtk_at_cmd((uint8_t*)"AT\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
		if(ret)break;
		if(ret<0)return ret;
	}

	mtk_at_cmd((uint8_t*)"AT+SM=LOCK\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
	return ret;
}

int16_t l620_enter_psm(void)
{
	//return 0;
	//int16_t ret;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	mtk_at_cmd((uint8_t*)"at+sm=unlock\r\n",(uint8_t*)"OK",recbuf,reclen,1*configTICK_RATE_HZ);
	
	return 0;
}

int16_t l620_send_process(uint8_t* sbuf,uint16_t slen)
{
	//at+m2mclisend=112233aabbccdd
	uint8_t t8,chr;
	uint16_t i;
	int16_t ret;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);	
	menu=MENU_HOME;
	subMenu=subMENU_MAIN_HOME_NB_SEND;	
	ui_disp_menu(0);	
	
	m_uart_send_str((uint8_t*)"at+m2mclisend=00");
	for(i=0;i<slen;i++)
	{
		mtk_disable_sleep();
		t8=sbuf[i];
		chr=HexTable[(t8&0xf0)>>4];
		m_uart4_send_byte_poll(chr);
		chr=HexTable[(t8&0x0f)];
		m_uart4_send_byte_poll(chr);
	}
	m_uart_send_str((uint8_t*)"\r\n");
	ret=mtk_at_cmd(NULL,(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
	return ret;
}

int16_t l620_received(uint8_t* rbuf,uint16_t rsize)
{
	int16_t ret;
	menu=MENU_HOME;
	subMenu=subMENU_MAIN_HOME_NB_RECEIVED;	
	ui_disp_menu(0);	
	
	ret=mtk_at_cmd(NULL,(uint8_t*)"M2MCLIRECV:",rbuf,rsize,20*configTICK_RATE_HZ);
	return ret;
}

int16_t l620_received_extract(uint8_t* rbuf,uint16_t len)
{
	int16_t ret;
	uint16_t t16,i;
	if(len>=sizeof(nbAplReceivedBuffer))len=sizeof(nbAplReceivedBuffer)-1;
	rbuf[len]='\0';
	ret=m_str_match_ex(rbuf,(uint8_t*)"M2MCLIRECV:",&t16);
	//uint
	if(!ret)return ret;
	t16=t16+sizeof("M2MCLIRECV:")-1;
	for(i=0;t16<len;t16++,i++){
		rbuf[i]=rbuf[t16];
	}
	return i;
}

int16_t l620_send_ready(void)
{
	int16_t ret;
//	uint8_t* recbuf=nbAplReceivedBuffer;
//	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	uint32_t tm;
	//init usart
	menu=MENU_HOME;
	subMenu=subMENU_MAIN_HOME_NB_ATTACH;	
	ui_disp_menu(0);		
	
	m_uart4_deinit();
	osDelay(200);
	m_uart4_init(9600);	
	//
	tm=osKernelSysTick();
	while(1){
		ret=0;
		mtk_disable_sleep();
		switch(mtkStateMachine){ 
			case MTK_POWER_OFF:
				ret=l620_hal_power_on();
				if(ret>0){
					mtkStateMachine=MTK_REG_NET;
				}else if(ret==0){
					l620_hal_power_off();
					mtkStateMachine=MTK_POWER_OFF;
				}
				break;
			case MTK_REG_NET:
				ret=l620_register_net();
				if(ret>0){
					mtkStateMachine=MTK_REG_IOT;
				}else if(ret==0){
					l620_hal_power_off();
					mtkStateMachine=MTK_POWER_OFF;					
				}
				break;		
			case MTK_REG_IOT:
				ret=l620_register_iot();
				//ret=1;
				if(ret>0){
					mtkStateMachine=MTK_READY;
				}else if(ret==0){
					l620_hal_power_off();
					mtkStateMachine=MTK_POWER_OFF;						
				}
				break;
			case MTK_READY:
				break;
			
			case MTK_PSM:
				ret=l620_hal_wakeup();
				if(ret>0){
					//l620StateMachine=MTK_REG_IOT;
					mtkStateMachine=MTK_READY;
				}else if(ret==0){
					l620_hal_power_off();
					mtkStateMachine=MTK_POWER_OFF;
				}
				break;
			default:
				l620_hal_power_off();
				mtkStateMachine=MTK_POWER_OFF;	
				break;
		}
		//exit 0
		if(ret<0){
			menu=MENU_HOME;
			subMenu=subMENU_MAIN_HOME_NB_PSM;				
			l620_hal_power_off();
			mtkStateMachine=MTK_POWER_OFF;	
			break;
		}
		//exit 1
		if(mtkStateMachine==MTK_READY){
			ret=1;break;
		}
		//exit 0
		if((osKernelSysTick()-tm)>120*configTICK_RATE_HZ){
			menu=MENU_HOME;
			subMenu=subMENU_MAIN_HOME_NB_PSM;			
			l620_hal_power_off();
			mtkStateMachine=MTK_POWER_OFF;
			ret=0;
			break;
		}		
	}
	return ret;
}

void vTheadNbL620(void * pvParameters)
{
	osEvent event;
	
	uint16_t len;
	int16_t ret;
	(void)pvParameters;	
	mtk_hal_pins_init();
	while(1){
		nbRun=false;
		mtk_hal_pins_init();
		event=osMessageGet(nbSendMsgQ, osWaitForever );
		nbRun=true;		
		if( event.status != osEventMessage)continue;
		

		
		if(event.value.v &  flg_NB_MODULE_RF_CLOSE){
			l620_hal_power_off();
			continue;
		}
		if(event.value.v & flg_NB_MODULE_COMM_PROCESS_REQ){	
			popPackageType=(popPackageType_t)(((event.value.v)>>24) & 0xffUL);
			ret=l620_send_ready();
			if(ret<=0){
				menu=MENU_HOME;
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
				ret=l620_send_process(nbAplSendBuffer,len);	
				if(ret<0)break;
				__nop();
				if(_15sSendStaMichine==__15S_SEND_SM_CLEAR){
					_15sSendStaMichine=__15S_SEND_SM_ACK;
					mtk_disable_sleep();
					osDelay(2000);
					continue;
				}else{
					_15sSendStaMichine=__15S_SEND_SM_CLEAR;
				}	
				ret=l620_received(nbAplReceivedBuffer,sizeof(nbAplReceivedBuffer));
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
			menu=MENU_HOME;
			subMenu=subMENU_MAIN_HOME_NB_PSM;	
			if(ret>0 && mtkStateMachine==MTK_READY){
				//l620_enter_psm();
				//l620StateMachine=MTK_PSM;
				event=osMessagePeek(nbSendMsgQ, 1);
				if(event.status==osEventMessage)continue;
				l620_hal_power_off();
			}else{
				//l620_enter_psm();
				//l620StateMachine=MTK_POWER_OFF;
				l620_hal_power_off();
			}
			//nb_power_off_process();
			if(!(sysData.DLCS==DLC_STATUS_A && !fi_id_writed_in_dlcs_a())){
				menu=MENU_RSSI;
				subMenu=subMENU_RSSI;
			}
			data_api_day_change();			
			
		}
	}
}

void m_thread_create_nb_mtk(void)
{
	//osMessageQDef(nbSendMsgQ, 5,sizeof(uint32_t));
	//osMessageQId nbSendMsgQ;
	nbSendMsgQ=osMessageCreate(osMessageQ(nbSendMsgQ),NULL);
	osThreadDef(TheadNb, vTheadNbL620, osPriorityNormal, 0, configMINIMAL_STACK_SIZE*4);
	idTheadUdpId=osThreadCreate(osThread(TheadNb), NULL);		
}
#elif NB_MODULE_VENDOR==BC26
int16_t bc26_hal_power_on(void)
{
	int16_t ret;
	uint16_t i=0;
	//uint16_t t16;
	mtk_disable_sleep();
	//send at

	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	m_gpio_set_mode(NB_MTK_PWR_PORT,NB_MTK_PWR_PIN,GPIO_MODE_OUTPUT_PP);
	m_gpio_write(NB_MTK_PWR_PORT,NB_MTK_PWR_PIN,NB_MTK_PWR_PIN);
	osDelay(500);
	m_gpio_set_mode(NB_MTK_PWR_PORT,NB_MTK_PWR_PIN,GPIO_MODE_INPUT);
	osDelay(200);
	m_lpusart1_deinit();
	osDelay(200);
	m_lpusart1_init(9600);		
	for(i=0;i<3;i++){
		ret=mtk_at_cmd((uint8_t*)"AT\r\n",(uint8_t*)"OK",recbuf,reclen,1*configTICK_RATE_HZ);
		if(ret)break;
		if(ret<0)return ret;
		osDelay(1*configTICK_RATE_HZ);
	}
	/*
	if(ret==0){
		m_lpusart1_deinit();
		osDelay(200);
		m_lpusart1_init(115200);	
		for(i=0;i<3;i++){
			ret=mtk_at_cmd((uint8_t*)"AT+IPR=9600\r\n",(uint8_t*)"OK",recbuf,reclen,1*configTICK_RATE_HZ);
			if(ret)break;
		}
		mtk_at_cmd((uint8_t*)"AT\r\n",(uint8_t*)"OK",recbuf,reclen,1*configTICK_RATE_HZ);
		mtk_at_cmd((uint8_t*)"AT\r\n",(uint8_t*)"OK",recbuf,reclen,1*configTICK_RATE_HZ);
		//无通讯
		if(ret==0){
			m_gpio_set_mode(NB_MTK_RST_PORT,NB_MTK_RST_PIN,GPIO_MODE_OUTPUT_PP);
			m_gpio_write(NB_MTK_RST_PORT,NB_MTK_RST_PIN,NB_MTK_RST_PIN);
			osDelay(500);
			m_gpio_set_mode(NB_MTK_RST_PORT,NB_MTK_RST_PIN,GPIO_MODE_INPUT);	
			return ret;
		}else{
		mtk_at_cmd((uint8_t*)"AT+QRST=1\r\n",(uint8_t*)"OK",recbuf,reclen,1*configTICK_RATE_HZ);
		}
		//AT+QRST=1
		osDelay(200);
		m_lpusart1_deinit();
		osDelay(200);
		m_lpusart1_init(9600);		
		return 0;
	}
	*/
	//mtk_at_cmd((uint8_t*)"AT+CFUN=0\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
	mtk_at_cmd((uint8_t*)"AT+QBAND=1,8\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
	//mtk_at_cmd((uint8_t*)"AT+CFUN=1\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
	mtk_at_cmd((uint8_t*)"AT+SM=LOCK\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);	
	ret=mtk_at_cmd((uint8_t*)"ATE0\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
	i=0;
	
	/*
	do{
		t16=0;
		ret=mtk_at_cmd((uint8_t*)"AT+CGPADDR=1\r\n",(uint8_t*)"+CGPADDR: 1,",recbuf,reclen,2*configTICK_RATE_HZ);
		if(ret<0)return ret;		
		ret=m_str_match_ex(recbuf,(uint8_t*)"+CGPADDR: 1,",&t16);	
		if(ret<0)return ret;
		t16=t16+sizeof("+CGPADDR: 1,")-1;
		t16=my_scanf_int16(recbuf+t16,'.',&t16);
		if(t16>0)break;
		osDelay(1*configTICK_RATE_HZ);
		i++;
	}while(i<30);
	if(i>=30){
		mtk_at_cmd((uint8_t*)"AT+QRST=1\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
	}
	ret=ret<0?ret:t16;
	*/
	return ret;
}

int16_t bc26_hal_power_off(void)
{
	uint16_t i;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	int16_t ret;
	mtk_disable_sleep();
	
	m_lpusart1_deinit();
	osDelay(200);
	m_lpusart1_init(9600);	
	for(i=0;i<3;i++){
		ret=mtk_at_cmd((uint8_t*)"AT\r\n",(uint8_t*)"OK",recbuf,reclen,1*configTICK_RATE_HZ);
		if(ret)break;
	}	
	/*
	if(i>=3){
		m_lpusart1_deinit();
		osDelay(200);
		m_lpusart1_init(115200);	
		for(i=0;i<2;i++){
			ret=mtk_at_cmd((uint8_t*)"AT+IPR=9600\r\n",(uint8_t*)"OK",recbuf,reclen,1*configTICK_RATE_HZ);
			if(ret)break;
		}
		m_lpusart1_deinit();
		osDelay(200);
		m_lpusart1_init(9600);			
	}
	*/
	mtk_at_cmd((uint8_t*)"AT+CFUN=0\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
	mtk_at_cmd((uint8_t*)"AT+QICLOSE=0\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
	for(i=0;i<3;i++){
		ret=mtk_at_cmd((uint8_t*)"AT+QPOWD=0\r\n",(uint8_t*)"OK",recbuf,reclen,1*configTICK_RATE_HZ);
		if(ret!=0)break;
	}	

	mtkStateMachine=MTK_POWER_OFF;
	return 1;
}

int16_t bc26_get_ver(void)
{
	int16_t ret;
	uint16_t t16;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);

	ret=mtk_at_cmd((uint8_t*)"ATI\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
	if(ret<=0)return ret;

	ret=m_str_match_ex(recbuf,(uint8_t*)"BC26NB",&t16);
	if(!ret)return ret;
	t16=t16+sizeof("BC26NB")-1;	
	m_mem_cpy_len(SPVER,recbuf+t16,6);
	//m_mem_cpy_len(SPVER+2,recbuf+t16+3,5);
	SPVER[6]='\0';
	
	return ret;		
}
/*
int16_t bc26_get_csq(void)
{
	int16_t ret;
	uint16_t t16,csq;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	
	//ret=mtk_at_cmd_const(&L620_ATCMD_CSQ,recbuf,reclen,3000);	
	ret=mtk_at_cmd((uint8_t*)"AT+CESQ\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
	if(ret<=0)return ret;
	
	ret=m_str_match_ex(recbuf,(uint8_t*)"+CESQ: ",&t16);
	if(!ret)return ret;
	t16=t16+sizeof("+CESQ: ")-1;
	csq=my_scanf_int16(recbuf+t16,',',&t16);
	if(csq==99 || csq==0){
		ret=0;
	}else{
		ret=-110;
		ret=ret+(csq-1);
		if(ret==-1)ret=-2;
		szrqRssi=csq/2;
	}
	return ret;		
}

*/
int16_t bc26_get_csq(void)
{
	int16_t ret;
	uint16_t t16,csq;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	
	//ret=mtk_at_cmd_const(&L620_ATCMD_CSQ,recbuf,reclen,3000);	
	ret=mtk_at_cmd((uint8_t*)"AT+CSQ\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
	if(ret<=0)return ret;
	
	ret=m_str_match_ex(recbuf,(uint8_t*)"+CSQ: ",&t16);
	if(!ret)return ret;
	t16=t16+sizeof("+CSQ: ")-1;
	csq=my_scanf_int16(recbuf+t16,',',&t16);
	if(csq==99 || csq==0){
		ret=0;
	}else{
		
		ret=-113;
		szrqRssi=csq;
		//ret=ret+(csq-1);
		//if(ret==-1)ret=-2;
		ret=ret+(csq*2);
		//szrqRssi=csq/2;
	}
	return ret;		
}


int16_t bc26_get_ccid(void)
{
	int16_t ret;
	uint16_t t16;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	//ret=mtk_at_cmd_const(&L620_ATCMD_CCID,recbuf,reclen,3000);	
	ret=mtk_at_cmd((uint8_t*)"AT+QCCID\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);	
	if(ret<=0)return ret;
	ret=m_str_match_ex(recbuf,(uint8_t*)"+QCCID: ",&t16);
	if(!ret)return ret;
	t16=t16+sizeof("+QCCID: ")-1;
	m_mem_cpy_len(ICCID,recbuf+t16,20);
	ICCID[20]='\0';
	return ret;	
}

int16_t bc26_get_cgsn(void)
{
	int16_t ret;
	uint16_t t16;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	//ret=mtk_at_cmd_const(&L620_ATCMD_CGSN,recbuf,reclen,3000);
	
	ret=mtk_at_cmd((uint8_t*)"AT+CGSN=1\r\n",(uint8_t*)"OK",recbuf,reclen,1*configTICK_RATE_HZ);
	if(ret<=0)return ret;
	ret=m_str_match_ex(recbuf,(uint8_t*)"+CGSN: ",&t16);
	
	if(!ret)return ret;
	t16=t16+sizeof("+CGSN: ")-1;
	m_mem_cpy_len(CGSN,recbuf+t16,15);
	CGSN[15]='\0';
	return ret;
}

int16_t bc26_get_menginfo(void)
{
	int16_t ret;
	uint16_t t16;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	//ret=mtk_at_cmd_const(&L620_ATCMD_MENGINFO,recbuf,reclen,3000);
	ret=mtk_at_cmd((uint8_t*)"AT+QENG=0\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
	if(ret<=0)return ret;
	ret=m_str_match_ex(recbuf,(uint8_t*)"+QENG: 0,",&t16);
	if(!ret)return ret;
	t16=t16+sizeof("+QENG: 0,")-1;
	m_mem_cpy_len(EARFCN,(uint8_t*)"cn:0000",7);
	m_mem_cpy_len(EARFCN+3,recbuf+t16,4);
	EARFCN[7]='\0';
	return ret;		
}

int16_t bc26_get_cimi(void)
{
	int16_t ret;
	uint16_t t16;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	
	ret=mtk_at_cmd((uint8_t*)"AT+CIMI\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
	if(ret<=0)return ret;
	if(ret<18)return 0;
	//ret=m_str_match_ex(recbuf,(uint8_t*)"+QENG: 0,",&t16);
	//if(!ret)return ret;
	//t16=t16+sizeof("+QENG: 0,")-1;
	//m_mem_cpy_len(EARFCN,(uint8_t*)"cn:0000",7);
	//m_mem_cpy_len(EARFCN+3,recbuf+t16,4);
	//EARFCN[7]='\0';
	m_mem_cpy_len(CIMI,nbAplReceivedBuffer+2,sizeof(CIMI));
	return ret;		
	
}

int16_t bc26_register_net(void)
{
	int16_t ret,tyetm;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);

	osDelay(2*configTICK_RATE_HZ);
	bc26_get_cgsn();
	bc26_get_ccid();
	
	/*
	ret=mtk_at_cmd((uint8_t*)"AT+CGATT?\r\n",(uint8_t*)"+CGATT: 1",recbuf,reclen,2*configTICK_RATE_HZ);
	if(ret==0){
		mtk_at_cmd((uint8_t*)"AT+CGATT=1\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
	}
	tyetm=0;
	do{
		tyetm++;
		ret=mtk_at_cmd((uint8_t*)"AT+CGATT?\r\n",(uint8_t*)"+CGATT: 1",recbuf,reclen,2*configTICK_RATE_HZ);
		if(ret==0){
			osDelay(2*configTICK_RATE_HZ);
			if(tyetm<=20)continue;
		}
		break;
	}while(1);
	
	if(ret<=0) return ret;
	
	ret=bc26_get_csq();
	rssi=(int8_t)(ret);
	*/
	bc26_get_menginfo();
	bc26_get_ver();
	bc26_get_cimi();
	return 1;
}
uint16_t port=1234;

uint16_t ipv4_bin_to_str(uint8_t* str,uint8_t* ip)
{
	uint8_t* strhead;
	uint16_t i,len=0;
	uint8_t t8;
	
	strhead=str;
	for(i=0;i<4;i++){
		t8=ip[i];
		if(t8>=100){
			*str++=(t8/100)+'0';
			t8%=100;
			*str++=(t8/10)+'0';
			t8%=10;
			*str++=(t8)+'0';
			*str++='.';
			len+=4;
		}else if(t8>=10){
			*str++=(t8/10)+'0';
			t8%=10;
			*str++=(t8)+'0';
			*str++='.';
			len+=3;			
		}else{
			*str++=(t8)+'0';
			*str++='.';
			len+=2;			
		}
	}
	len--;
	strhead[len]='\0';
	__nop();
	return len;
}
int16_t bc26_register_iot(void)
{
	uint16_t tryTm=0;
	//uint8_t sbuf[64];
	uint8_t tmpbuf[8];
	uint16_t t16;
	int16_t ret;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	uint8_t* p;
	//mtk_at_cmd((uint8_t*)"at+cmee=2\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
	mtk_at_cmd((uint8_t*)"AT+QICLOSE=0\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
	
	do{
		/*
		if(port<1000 || port>3000)port=1001;
		
		//m_uart_send_str((uint8_t*)"AT+QIOPEN=1,0,UDP,171.221.221.50,20017,");
		m_uart_send_str((uint8_t*)"AT+QIOPEN=1,0,UDP,218.17.114.5,7701,");
		
		ret=m_int_2_str_ex(tmpbuf,port,7);
		

		m_uart_send_str(tmpbuf);	
		//,1\r\n"
		m_uart_send_str((uint8_t*)",0\r\n");
		ret=mtk_at_cmd(NULL,(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
		port++;
		if(ret<0)break;
		if(ret==0){
			mtk_at_cmd((uint8_t*)"AT+QICLOSE=0\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
			tryTm++;
			continue;
		}
		break;
		*/
		if(port<1000 || port>3000)port=1001;
		if(sysData.severRelease.ip[0]!=0){
			m_lpusart1_send_str((uint8_t*)"AT+QIOPEN=1,0,UDP,");
			ipv4_bin_to_str(tmpbuf,sysData.severRelease.ip);
			m_lpusart1_send_str(tmpbuf);
			m_lpusart1_send_byte_poll(',');
			ret=m_int_2_str_ex(tmpbuf,sysData.severRelease.port,7);
			m_lpusart1_send_str(tmpbuf);
			m_lpusart1_send_byte_poll(',');
			ret=m_int_2_str_ex(tmpbuf,port,7);
			m_lpusart1_send_str(tmpbuf);
			m_lpusart1_send_str((uint8_t*)",0\r\n");
			//m_uart_send_str("uint")
		}else{
			m_lpusart1_send_str((uint8_t*)"AT+QIOPEN=1,0,UDP,218.17.114.5,7701,");	
			ret=m_int_2_str_ex(tmpbuf,port,7);	
			m_lpusart1_send_str(tmpbuf);	
			//,1\r\n"
			m_lpusart1_send_str((uint8_t*)",0\r\n");
		}
		ret=mtk_at_cmd(NULL,(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
		port++;
		if(ret<0)break;
		if(ret==0){
			mtk_at_cmd((uint8_t*)"AT+QICLOSE=0\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
			tryTm++;
			continue;
		}
		break;		
		
		//
	}while(tryTm<20);
	return ret;
}	
/*
int16_t bc26_register_iot(void)
{
	uint8_t sbuf[64];
	uint16_t t16;
	int16_t ret;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	uint8_t* p;
	//AT+QLWSERV="180.101.147.115",5683
	do{
		///????AT+QLWDEL
		m_str_cpy(sbuf,(uint8_t*)"AT+QLWDEL\r\n");
		ret=mtk_at_cmd(sbuf,(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
		//if(ret<=0)break;		
		p=sbuf;
		t16=m_str_cpy(p,(uint8_t*)"AT+QLWSERV=\"");
		p=p+t16-1;
		//t16=m_str_cpy(p,(uint8_t*)IOT_IP);
		t16=m_str_cpy(p,(uint8_t*)sysData.severUser.ip);
		p=p+t16-1;
		t16=m_str_cpy(p,(uint8_t*)"\",5683\r\n");
		//p=p+t16-1;
		ret=mtk_at_cmd(sbuf,(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
		if(ret<=0)break;
		//AT+QLWCONF="866971030576800"
		p=sbuf;
		t16=m_str_cpy(p,(uint8_t*)"AT+QLWCONF=\"");
		p=p+t16-1;
		m_mem_cpy_len(p,CGSN,15);
		p+=15;
		t16=m_str_cpy(p,(uint8_t*)"\"\r\n");
		ret=mtk_at_cmd(sbuf,(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
		if(ret<=0)break;
		//AT+QLWADDOBJ=19,0,1,"0"
		m_str_cpy(sbuf,(uint8_t*)"AT+QLWADDOBJ=19,0,1,\"0\"\r\n");
		ret=mtk_at_cmd(sbuf,(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
		if(ret<=0)break;
		//AT+QLWADDOBJ=19,1,1,"0"
		m_str_cpy(sbuf,(uint8_t*)"AT+QLWADDOBJ=19,1,1,\"0\"\r\n");
		ret=mtk_at_cmd(sbuf,(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
		if(ret<=0)break;
		//AT+QLWOPEN=0
		m_str_cpy(sbuf,(uint8_t*)"AT+QLWOPEN=0\r\n");
		ret=mtk_at_cmd(sbuf,(uint8_t*)"CONNECT OK",recbuf,reclen,20*configTICK_RATE_HZ);
		if(ret<=0)break;
		osDelay(2000);
		//AT+QLWUPDATE
		
		m_str_cpy(sbuf,(uint8_t*)"AT+QLWUPDATE\r\n");
		//ret=mtk_at_cmd(sbuf,(uint8_t*)"UPDATE OK",recbuf,reclen,20*configTICK_RATE_HZ);
		ret=mtk_at_cmd(sbuf,(uint8_t*)"OK",recbuf,reclen,20*configTICK_RATE_HZ);
		if(ret<=0)break;
		
		//AT+QLWCFG="dataformat",1,1
		m_str_cpy(sbuf,(uint8_t*)"AT+QLWCFG=\"dataformat\",1,1\r\n");
		ret=mtk_at_cmd(sbuf,(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
		if(ret<=0)break;
		ret=bc26_get_menginfo();
		
	}while(0);
	return ret;
}
*/
int16_t bc26_hal_wakeup(void)
{
	//return 0;
	uint16_t i;
	int16_t ret;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	m_gpio_set_mode(NB_MTK_PSM_PORT,NB_MTK_PSM_PIN,GPIO_MODE_OUTPUT_PP);
	m_gpio_write(NB_MTK_PSM_PORT,NB_MTK_PSM_PIN,NB_MTK_PSM_PIN);
	osDelay(200);
	//m_gpio_write(NB_MTK_PSM_PORT,NB_MTK_PSM_PIN,0);	
	m_gpio_set_mode(NB_MTK_PSM_PORT,NB_MTK_PSM_PIN,GPIO_MODE_INPUT);
	for(i=0;i<3;i++){

		ret=mtk_at_cmd((uint8_t*)"AT\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
		if(ret)break;
	}
	//mtk_at_cmd_const(&L620_ATCMD_SM_LOCK,recbuf,reclen,1000);
	ret=mtk_at_cmd((uint8_t*)"AT+SM=LOCK\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
	return ret;
}

int16_t bc26_enter_psm(void)
{
	//return 0;
	//int16_t ret;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	mtk_at_cmd((uint8_t*)"at+sm=unlock\r\n",(uint8_t*)"OK",recbuf,reclen,1*configTICK_RATE_HZ);
	
	return 0;
}
/*
int16_t bc26_send_process(uint8_t* sbuf,uint16_t slen)
{
	//AT+QLWDATASEND=19,0,0,04,00001122,0x0000
	uint8_t t8,chr;
	uint8_t tmpbuf[6];
	uint16_t i;
	int16_t ret;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);	
	menu=0x00;
	subMenu=subMENU_MAIN_HOME_NB_SEND;	
	ui_disp_menu(0);	

	m_uart_send_str((uint8_t*)"AT+QLWDATASEND=19,0,0,");

	if(slen>99)slen=99;
	m_int_2_str(tmpbuf+3,slen+1,4);
	m_str_cpy(tmpbuf+4,(uint8_t*)",00");
	m_uart_send_str(tmpbuf);
	for(i=0;i<slen;i++)
	{
		mtk_disable_sleep();
		t8=sbuf[i];
		chr=HexTable[(t8&0xf0)>>4];
		m_uart4_send_byte_poll(chr);
		chr=HexTable[(t8&0x0f)];
		m_uart4_send_byte_poll(chr);
	}
	m_uart_send_str((uint8_t*)",0x0000\r\n");
	ret=mtk_at_cmd(NULL,(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
	return ret;
}
*/

int16_t bc26_send_process(uint8_t* sbuf,uint16_t slen)
{
	uint8_t t8,chr;
	uint8_t tmpbuf[8];
	uint16_t i;
	int16_t ret;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);	
	menu=MENU_HOME;
	subMenu=subMENU_HOME_NB_SEND;	
	ui_disp_menu();
	
	//ret=bc26_register_iot();
	//if(ret<=0)return ret;
	
	m_lpusart1_send_str((uint8_t*)"AT+QISENDEX=0,");
	if(slen>500)slen=500;
	ret=m_int_2_str_ex(tmpbuf,slen,7);
	tmpbuf[ret-1]=',';
	tmpbuf[ret]='\0';
	m_lpusart1_send_str(tmpbuf);
	for(i=0;i<slen;i++)
	{
		mtk_disable_sleep();
		t8=sbuf[i];
		chr=HexTable[(t8&0xf0)>>4];
		m_lpusart1_send_byte_poll(chr);
		chr=HexTable[(t8&0x0f)];
		m_lpusart1_send_byte_poll(chr);
	}
	m_lpusart1_send_str((uint8_t*)"\r\n");
	ret=mtk_at_cmd(NULL,(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
	return ret;	
}
int16_t bc26_send_ready(void)
{
	int16_t ret;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	uint32_t tm;
	//init usart
	menu=MENU_HOME;
	subMenu=subMENU_HOME_NB_CONN;	
	ui_disp_menu();		
	
	m_lpusart1_deinit();
	osDelay(200);
	m_lpusart1_init(9600);	
	//
	tm=osKernelSysTick();
	while(1){
		ret=0;
		mtk_disable_sleep();
		switch(mtkStateMachine){ 
			case MTK_POWER_OFF:
				ret=bc26_hal_power_on();
				if(ret>0){
					mtkStateMachine=MTK_REG_CSQ;
				}else if(ret==0){
					bc26_hal_power_off();
					mtkStateMachine=MTK_POWER_OFF;
				}
				break;
			case MTK_REG_CSQ:
				osDelay(1000);
				ret=bc26_get_csq();
				if(ret==0){
					
				}else if(ret==-1){
					bc26_hal_power_off();
					mtkStateMachine=MTK_POWER_OFF;				
				}else{
					rssi=(int8_t)(ret);
					ret=1;
					mtkStateMachine=MTK_REG_CGATT;
				}
				break;
			case MTK_REG_CGATT:
				ret=mtk_at_cmd((uint8_t*)"AT+CGATT?\r\n",(uint8_t*)"+CGATT: 1",recbuf,reclen,2*configTICK_RATE_HZ);
				if(ret<0){
					bc26_hal_power_off();
					mtkStateMachine=MTK_POWER_OFF;						
				}else if(ret==0){
				}else{
					mtkStateMachine=MTK_REG_CEREG;
				}
				break;
			case MTK_REG_CEREG:
				ret=mtk_at_cmd((uint8_t*)"AT+CEREG?\r\n",(uint8_t*)"+CEREG: 0,1",recbuf,reclen,2*configTICK_RATE_HZ);
				if(ret==0){
					ret=m_str_match(recbuf,(uint8_t*)"+CEREG:0,5");
				}
				
				if(ret<0){
					bc26_hal_power_off();
					mtkStateMachine=MTK_POWER_OFF;						
				}else if(ret==0){
				}else{
					mtkStateMachine=MTK_REG_NET;
				}
				break;				
				//AT+CEREG?
			case MTK_REG_NET:
				//AT+CPSMS=0;
				mtk_at_cmd((uint8_t*)"AT+CPSMS=0\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
				ret=bc26_register_net();
				if(ret>0){
					mtkStateMachine=MTK_REG_IOT;
				}else if(ret==0){
					bc26_hal_power_off();
					mtkStateMachine=MTK_POWER_OFF;					
				}
				break;		
			case MTK_REG_IOT:
			
				ret=bc26_register_iot();
				//ret=1;
				if(ret>0){
					mtkStateMachine=MTK_READY;
				}else if(ret==0){
					bc26_hal_power_off();
					mtkStateMachine=MTK_POWER_OFF;						
				}
				
				//mtkStateMachine=MTK_READY;
				break;
			case MTK_READY:
				break;
			
			case MTK_PSM:
				ret=bc26_hal_wakeup();
				if(ret>0){
					mtkStateMachine=MTK_READY;
				}else if(ret==0){
					bc26_hal_power_off();
					mtkStateMachine=MTK_POWER_OFF;
				}
				break;
			default:
				bc26_hal_power_off();
				mtkStateMachine=MTK_POWER_OFF;	
				break;
		}
		//exit 0
		if(ret<0){
			menu=MENU_HOME;
			subMenu=subMENU_HOME_NB_END;			
			bc26_hal_power_off();
			mtkStateMachine=MTK_POWER_OFF;	
			break;
		}
		//exit 1
		if(mtkStateMachine==MTK_READY){
			ret=1;break;
		}
		//exit 0
		if((osKernelSysTick()-tm)>120*configTICK_RATE_HZ){
			menu=MENU_HOME;
			subMenu=subMENU_HOME_NB_END;			
			bc26_hal_power_off();
			mtkStateMachine=MTK_POWER_OFF;
			ret=0;
			break;
		}	
		
	}
	return ret;
}

int16_t bc26_received(uint8_t* rbuf,uint16_t rsize)
{
	int16_t ret=0x00;
	menu=MENU_HOME;
	subMenu=subMENU_HOME_NB_REC;	
	ui_disp_menu();	
	
	ret=mtk_at_cmd(NULL,(uint8_t*)"+QIURC: \"recv\",0,",rbuf,rsize,20*configTICK_RATE_HZ);
	return ret;
}



int16_t bc26_received_pool(uint8_t* rbuf,uint16_t rsize,uint32_t tmOut)
{
	int16_t ret=0x00;
	uint16_t t16,i,rlen;
	uint8_t* p;
	uint32_t tm,
	menu=MENU_HOME;
	menu=menu;
	subMenu=subMENU_HOME_NB_REC;	
	ui_disp_menu();	
	tm=osKernelSysTick();
	if(tmOut<configTICK_RATE_HZ)tmOut=configTICK_RATE_HZ;
	do{
		rlen=0;
		ret=0;
		if(osKernelSysTick()-tm>tmOut)break;

		ret=mtk_at_cmd((uint8_t*)"\r\n\r\n\r\nAT+QIRD=0,512\r\n",(uint8_t*)"+QIRD: ",rbuf,rsize,1*configTICK_RATE_HZ);
		if(ret==0){
			osDelay(1000);
			continue;
		}
		if(ret<0)break;
		ret=m_str_match_ex(rbuf,(uint8_t*)"+QIRD: ",&t16);
		if(!ret)continue;
		t16=t16+sizeof("+QIRD: ")-1;
		
		rlen=my_scanf_int16(rbuf+t16,'\r',&i);
		if(rlen==0)continue;
		if(rlen>=sizeof(nbAplReceivedBuffer))rlen=sizeof(nbAplReceivedBuffer);
		p=rbuf+t16+i+2;	
		m_mem_cpy_len(rbuf,p,rlen);
		return rlen;		
	}while(1);
	return 	ret>0?rlen:ret;
}

//+QIURC: "recv",0,
int16_t bc26_received_extract(uint8_t* rbuf,uint16_t len)
{
	//uint8_t buf[256];
	int16_t ret;
	uint16_t t16,i,rlen=0;
	uint8_t* p;
	if(len>=sizeof(nbAplReceivedBuffer))len=sizeof(nbAplReceivedBuffer)-1;
	rbuf[len]='\0';
	ret=m_str_match_ex(rbuf,(uint8_t*)"+QIURC: \"recv\",0,",&t16);
	//uint
	if(!ret)return ret;
	t16=t16+sizeof("+QIURC: \"recv\",0,")-1;
	rlen=my_scanf_int16(rbuf+t16,'\r',&i);
	if(rlen<=0 || rlen>=sizeof(nbAplReceivedBuffer))return 0;
	p=rbuf+t16+i+2;
	//rlen=m_str_h2b(buf,p,rlen);
	
	//+QLWDATARECV: 19,1,0,18,1125171A000012091D10333208060000AFA8
	m_mem_cpy_len(rbuf,p,rlen);
	return rlen;
}
/*
int16_t bc26_received_extract(uint8_t* rbuf,uint16_t len)
{
	uint8_t buf[256];
	int16_t ret;
	uint16_t t16,i,rlen=0;
	uint8_t* p;
	if(len>=sizeof(nbAplReceivedBuffer))len=sizeof(nbAplReceivedBuffer)-1;
	rbuf[len]='\0';
	ret=m_str_match_ex(rbuf,(uint8_t*)"+QLWDATARECV: 19,1,0,",&t16);
	//uint
	if(!ret)return ret;
	t16=t16+sizeof("+QLWDATARECV: 19,1,0,")-1;
	rlen=my_scanf_int16(rbuf+t16,',',&i);
	if(rlen<=0 || rlen>=100)return 0;
	p=rbuf+t16+i+1;
	rlen=m_str_h2b(buf,p,rlen*2);
	
	//+QLWDATARECV: 19,1,0,18,1125171A000012091D10333208060000AFA8
	m_mem_cpy_len(rbuf,buf,rlen);
	return rlen;
}
*/

szrqSendStaMichine_t szrqSendStaMichine;

//打包上报不加入缓存，单包上报和报警上报加入缓存

void vTheadNbBc26(void * pvParameters)
{
	osEvent event;
	__szrq_udpSendmsg_t __msg;
	uint16_t len;
	int16_t ret;
	uint16_t sendRedo=0;
	(void)pvParameters;	
	mtk_hal_pins_init();
	while(1){
		nbRun=false;
		//osDelay(4000);
		api_calc_all();
		event=osMessageGet(nbSendMsgQ, osWaitForever );
		if(event.status != osEventMessage)continue;
		__msg.t32=event.value.v;
		api_sysdata_save();
		
		if(__msg.str.eventMsg & flg_NB_PROCESS_SEND_OLD){
			osMutexWait(osMutexSysData,osWaitForever);
			len=rf_send_fifo_get_tail(nbAplSendBuffer,(uint16_t*)&ret);
			osMutexRelease(osMutexSysData);	
			if(len<sizeof(__szrq_framHeader_t))continue;
		}
		
		nbRun=true;	
		mtk_hal_pins_init();

		if(__msg.str.eventMsg &  flg_NB_PROCESS_MODULE_RF_CLOSE){
			bc26_hal_power_off();
			continue;
		}

		if(__msg.str.eventMsg & (flg_NB_PROCESS_SEND_REAL | flg_NB_PROCESS_SEND_OLD)){	
			ret=bc26_send_ready();
			if(ret<=0){
				menu=MENU_HOME;
				subMenu=subMENU_HOME_MAIN;
				continue;	
			}	
			len=0;
			szrqSendStaMichine=__15S_SEND_SM_POP;

			do{
				mtk_disable_sleep();
				if(szrqSendStaMichine==__15S_SEND_SM_POP){
					len=0;
					if(__msg.str.eventMsg & flg_NB_PROCESS_SEND_REAL){
						//send mult package
						if(__msg.str.popType!=POP_TYPE_MULT)break;
						szrqMulHasMore=0;
						szrqMultSendNum=0x00;
						len=__szrq_load_frame_pop_m(nbAplSendBuffer,sizeof(nbAplSendBuffer));						
					}else{
						
						osMutexWait(osMutexSysData,osWaitForever);
						len=rf_send_fifo_get_tail(nbAplSendBuffer,(uint16_t*)&ret);
						osMutexRelease(osMutexSysData);	
						
						if(ret>1 && len>sizeof(__szrq_framHeader_t))__szrq_modify_hase_more(nbAplSendBuffer,len,1);
						if(len<sizeof(__szrq_framHeader_t))len=0;
					}
				}else{

				}
				if(!len)break;
				sendRedo=0;
				do{
					sendRedo++;
					ret=0;
					if(sendRedo>3)break;
					szrqRecHasMore=0;
					ret=bc26_send_process(nbAplSendBuffer,len);	
					__nop();
					if(szrqSendStaMichine==__15S_SEND_SM_END_AFTER_SEND){
						ret=0;
						szrqSendStaMichine=__15S_SEND_SM_END;
						break;
					}
					
					ret=bc26_received_pool(nbAplReceivedBuffer,sizeof(nbAplReceivedBuffer),12*configTICK_RATE_HZ);
					if(ret==0){
						continue;
					}else if(ret<0){
						break;
					}
					//ret >0;
					//收到包
					ret=__szrq_received_process(nbAplReceivedBuffer,ret,nbAplSendBuffer,sizeof(nbAplSendBuffer));
					//多包发送
					if(ret && szrqSendStaMichine==__15S_SEND_SM_POP){
						ret=1;
						break;
					}
					
					if(ret==0 && szrqRecHasMore){
					//等待第二包
						ret=bc26_received_pool(nbAplReceivedBuffer,sizeof(nbAplReceivedBuffer),20*configTICK_RATE_HZ);
						if(ret==0)continue;
						if(ret<0)break;
						ret=__szrq_received_process(nbAplReceivedBuffer,ret,nbAplSendBuffer,sizeof(nbAplSendBuffer));
					}				
					//接收到指令
					if(ret>0){
						len=ret;
						break;
					}
					
					
				}while(sendRedo<=2 );
				if(ret<=0 || szrqSendStaMichine==__15S_SEND_SM_END)break;
				//if(ret<0 || szrqSendStaMichine==__15S_SEND_SM_END)break;
				len=ret;
				
			}while(1);		
			if(mtkStateMachine==MTK_READY){
				//event=osMessagePeek(nbSendMsgQ, 1);
				//if(event.status==osEventMessage)continue;
			}
			menu=MENU_HOME;
			subMenu=subMENU_HOME_NB_END;
			
			bc26_hal_power_off();
			if(!(sysData.DLCS==DLC_STATUS_A && !fi_id_writed_in_dlcs_a())){
				menu=MENU_RSSI;
				subMenu=subMENU_RSSI;
			}	
		}
	}
}

void m_thread_create_nb_mtk(void)
{
	nbSendMsgQ=osMessageCreate(osMessageQ(nbSendMsgQ),NULL);
	osThreadDef(TheadNb, vTheadNbBc26, osPriorityNormal, 0, configMINIMAL_STACK_SIZE*4);
	idTheadUdpId=osThreadCreate(osThread(TheadNb), NULL);		
}
#else
	#error "defined nb vendor error!!"
#endif

//file end
