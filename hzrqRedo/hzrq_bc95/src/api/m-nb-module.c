#include "./includes/includes.h"

#ifndef	NBIOT_ROUTINES_DISP_EN
	#define NBIOT_ROUTINES_DISP_EN 0
#endif


#define nbAplSendBuffer 	uartSendBuf
uint8_t nbAplReceivedBuffer[MAX_UART_RX_BUFFER_LEN];
uint16_t nbAplReceivedCount;

//nbiotDescInf_t nbiotDescInf={0};

TaskHandle_t idTheadUdpId=(osThreadId)NULL;


uint8_t CIMI[16]={0};
uint8_t	CGSN[16]={0};
uint8_t	ICCID[21]={0};
uint8_t SPVER[8]={0};
uint8_t EARFCN[8]="cn:0000";
uint8_t SNR[8]="snr:---";
volatile bool nbFirstCattch=true;	

nbPsmStateMachine_t nbPsmStateMachine=NB_PSM_SM_AT;

bc95Ver_t bc95SubVer=SP3;
em_bc95MasVer_t bc95MainVer=BC95B5HB;


int16_t nb_at_cmd(uint8_t* cmd,uint8_t* expResp,uint8_t* rbuf,uint16_t ssize,uint32_t tm);

void nb_process_disable_sleep_in_lwp(void)
{
	noEventTimeOut=NO_EVEN_MAX_TIME_OUT;
}

void nb_process_enable_sleep_in_lwp(void)
{
	noEventTimeOut=0;
}

void nb_module_pwr_pins_init(void)
{
	m_gpio_set_mode(NB_PWR_PORT,NB_PWR_PINS,GPIO_MODE_OUTPUT_PP);
}

void nb_module_pwr_pins_deinit(void)
{
	m_gpio_set_mode(NB_PWR_PORT,NB_PWR_PINS,GPIO_MODE_ANALOG);
}

void nb_module_pwr_on(void)
{

	m_gpio_write(NB_PWR_PORT,NB_PWR_PINS,NB_PWR_PINS);

}

void nb_module_pwr_off(void)
{
	m_gpio_write(NB_PWR_PORT,NB_PWR_PINS,0);

}
// rst
void nb_module_rst_pins_init(void)
{
	m_gpio_set_mode(NB_RST_PORT,NB_RST_PINS,GPIO_MODE_OUTPUT_PP);
}

void nb_module_rst_pins_deinit(void)
{
	m_gpio_set_mode(NB_RST_PORT,NB_RST_PINS,GPIO_MODE_ANALOG);
}

void nb_module_reset_enable(void)
{
	//nb_module_rst_pins_init();
	m_gpio_write(NB_RST_PORT,NB_RST_PINS,NB_RST_PINS);
	//nbiotDescInf.nbStatus=NB_POWER_ON;
}

void nb_module_reset_disable(void)
{
	//nb_module_pwr_pins_deinit();
	m_gpio_write(NB_RST_PORT,NB_RST_PINS,0);
}

void nb_module_hal_init(void)
{
	nb_module_pwr_pins_init();
	nb_module_pwr_on();

}

void nb_module_hal_deinit(void)
{

}
//apl
void nb_routines_information_disp(uint8_t* str)
{
	m_lcd_disp_str_refresh(str);
}
/*
uint16_t nb_at_cmd_waite_resp(uint8_t* rec,uint32_t tm,uint16_t receiveLen)
{
	uint16_t t16;
	osEvent event;
	//m_mem_set(rec,'\0',receiveLen);
	nb_process_disable_sleep_in_lwp();
	event=osSignalWait(flg_NB_MODULE_ALL_BITS,tm);
	
	if(event.status==osEventSignal){
		if(event.value.signals & flg_NB_MODULE_UART_RECEIVED_LF){
			//return uartRceivedBufCountUser;
			if(uartRceivedBufCountUser && (void*)rec){
				t16=uartRceivedBufCountUser;
				if(t16>receiveLen){
					t16=receiveLen;
				}
				if((void*)(rec)!=NULL && (void*)(rec) != (void*)(uartReceivedBufUser)){
					m_mem_cpy_len(rec,uartReceivedBufUser,t16);
				}
				return t16;
			}
		}
		if(event.value.signals & flg_NB_MODULE_UART_RECEIVED_ERROR){
			t16=0x00;
			m_uart4_deinit();
			osDelay(200);
			m_uart4_init(9600);
			__nop();
		}
		
	}
	return 0;
}


uint16_t nb_at_cmd_ex(const st_ATcmdDef* cmd,uint8_t* recbuf,uint16_t reclen)
{
	uint16_t sta=0;
	uint16_t len=0;
	uint16_t tryTm=0x00;
	if(!cmd)return 0;
	if(!recbuf)return 0;
	nb_process_disable_sleep_in_lwp();
	#ifdef STM32L152xB
	osDelay(1000);
	#else
	osDelay(100);
	#endif
	m_mem_set(recbuf,'\0',reclen);
	m_uart_send_str((uint8_t*)(cmd->cmdStr));
	#if NBIOT_ROUTINES_DISP_EN
	nb_routines_information_disp((uint8_t*)(cmd->promptStr));
	#endif
	for(tryTm=0;tryTm<6;tryTm++){
		len=0;
		if(pwrStatus==POWER_STATUS_DOWN)return 0;
		len=nb_at_cmd_waite_resp(recbuf,1000,reclen);
		if(len<=2)continue;
		else
			break;
	}
	if(!(cmd->retAssert)){
		return len;
	}
	
	recbuf[len]='\0';
	if(!(cmd->retAssert)){
		sta=len;
	}else if(cmd->respStr){
		sta=m_str_match(recbuf,(uint8_t*)(cmd->respStr));	
	}else if(cmd->okMatchStr){
		sta=m_str_match(recbuf,(uint8_t*)(cmd->okMatchStr));
	}
	tryTm=cmd->respExTimes;
	if(tryTm>3)tryTm=3;
	while(tryTm){

		if(pwrStatus==POWER_STATUS_DOWN)return 0;	
		nb_at_cmd_waite_resp(recbuf,3000,reclen);
		tryTm--;
	}
	__nop();
	if(!sta){

		return 0;
	}else{
	
		return len;
	}
}
*/
//apl 


uint16_t nb_get_mver(void)
{
	uint8_t* p;
	int16_t t16;
	int16_t ret=0;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);	
	//ret=nb_at_cmd_ex(&AT_CMD_ATI,recbuf,reclen);
	nb_at_cmd((uint8_t*)"AT\r\n",(uint8_t*)"OK",recbuf,reclen,1*configTICK_RATE_HZ);
	ret=nb_at_cmd((uint8_t*)"ATI\r\n",(uint8_t*)"OK",recbuf,reclen,1*configTICK_RATE_HZ);
	do{
		__nop();
		if(ret<=0)break;
		__nop();
		ret=m_str_match_ex(recbuf,(uint8_t*)"BC95B5",(uint16_t*)&t16);
		if(!ret)break;
		p=recbuf+t16+sizeof("BC95B5")-1;
		t16=0;
		if(p[0]=='H'){
			if(p[1]=='B'){bc95MainVer=BC95B5HB;}
			else if(p[1]=='C'){bc95MainVer=BC95B5HC;}
			else{
				bc95MainVer=BC95B5UNDEF;
			}
			
		}else if(p[0]=='J' && p[1]=='B'){
			bc95MainVer=BC95B5JB;
		}else{
			bc95MainVer=BC95B5UNDEF;
		}
		ret=1;
	}while(0);
	if(ret){
		return bc95MainVer;
	}else{
		return 0;
	}
}

int16_t nb_get_snr(void)
{
	uint8_t* p8;
	
	int16_t t16;
	int8_t ret;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);	
	//ret=nb_at_cmd_ex(&AT_CMD_CSQ,recbuf,reclen);
	nbAplReceivedBuffer[reclen-1]='\0';
	do{
		//if(!ret)break;
		ret=m_str_match_ex(recbuf,(uint8_t*)"SNR:",(uint16_t*)&t16);
		if(!ret)break;
		p8=recbuf+t16+sizeof("SNR:")-1;
		t16=4;
		while(!(*p8=='\r' ||  *p8=='\n')){
			SNR[t16]=*p8;
			t16++;p8++;
			if(t16>=7)break;
		}
		SNR[t16]='\0';
	}while(0);
	return 1;
}

int8_t nb_get_csq(void)
{
	uint8_t* p8;
	int16_t t16;
	int16_t ret;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);	
	//ret=nb_at_cmd_ex(&AT_CMD_CSQ,recbuf,reclen);
	ret=nb_at_cmd((uint8_t*)"AT+CSQ\r\n",(uint8_t*)"OK",recbuf,reclen,1*configTICK_RATE_HZ);
	do{
		if(!ret)break;
		ret=m_str_match_ex(recbuf,(uint8_t*)"+CSQ:",(uint16_t*)&t16);
		if(!ret)break;
		p8=recbuf+t16+sizeof("+CSQ:")-1;
		t16=0;
		while(*p8!=','){
			t16*=10;
			t16+=(*p8-'0');
			p8++;
			if(t16>100)break;
		}
		ret=(int8_t)t16;
	}while(0);
	if(ret>=0 && ret<=31){
		ret=-113+(ret*2);
	}else{
		ret=-127;
	}
	return ret;
}

uint16_t nb_extract_cimi(uint8_t* recbuf,uint16_t reclen,uint8_t* cimi)
{
	__nop();
	if(reclen<19)return 0;
	m_mem_cpy_len(cimi,recbuf+2,15);
	cimi[15]='\0';
	return 1;
}

uint16_t nb_extract_cgsn(uint8_t* recbuf,uint16_t reclen,uint8_t* cgsn)
{
	__nop();
	if(reclen<30)return 0;
	m_mem_cpy_len(cgsn,recbuf+8,15);
	cgsn[15]='\0';
	return 1;
}

uint16_t nb_extract_nccid(uint8_t* recbuf,uint16_t reclen,uint8_t* nccid)
{
	__nop();
	if(reclen<36)return 0;
	m_mem_cpy_len(nccid,recbuf+9,20);
	nccid[20]='\0';
	return 1;
}

int16_t nb_module_cfun_off(void)
{
	uint16_t redo=0,ret=0;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	while(redo<4){
		redo++;
		//ret=nb_at_cmd_ex(&AT_CMD_CFUN_0,recbuf,reclen);
		ret=nb_at_cmd((uint8_t*)"AT+CFUN=0\r\n",(uint8_t*)"OK\r\n",recbuf,reclen,2*configTICK_RATE_HZ);
		if(ret!=0)break;
		osDelay(1000);
	}
	return ret;
}


#if config_NB_PLAT==UDP
uint16_t nb_attach_network(void)
{
	uint32_t tm;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	uint16_t tryTm,ret;

	if(!(nb_at_cmd_ex(&AT_CMD_AT,recbuf,reclen)))return 0;

	
	nb_at_cmd_ex(&AT_CMD_CFUN,recbuf,reclen);
	nb_at_cmd_ex(&AT_CMD_CFUN_R,recbuf,reclen);
	
	nb_at_cmd_ex(&AT_CMD_CMI,recbuf,reclen);
	nb_extract_cimi(recbuf,reclen,CIMI);
	
	nb_at_cmd_ex(&AT_CMD_NCCID,recbuf,reclen);
	nb_extract_nccid(recbuf,reclen,ICCID);
	
	nb_at_cmd_ex(&AT_CMD_CGSN,recbuf,reclen);
	nb_extract_cgsn(recbuf,reclen,CGSN);
	
	
	nb_at_cmd_ex(&AT_CMD_CSQ,recbuf,reclen);
	
	tryTm=0;
	while(tryTm<5){
		tryTm++;
		ret=nb_at_cmd_ex(&AT_CMD_CGDCONT,recbuf,reclen);
		if(ret)break;
		osDelay(1000);
	}
	if(tryTm>=5)return 0;
	//nb_at_cmd_ex(&AT_CMD_CGDCONT,recbuf,reclen);
	//osDelay(4000);
	 tryTm=0;
	while(tryTm<10){	
		tryTm++;
		ret=nb_at_cmd_ex(&AT_CMD_CGATT,recbuf,reclen);
		if(ret)break;
	};
	if(tryTm>=10)return 0;
	
	tryTm=0;
	tm=osKernelSysTick();
	while(tryTm<100){
		tryTm++;
		osDelay(1000);
		ret=nb_at_cmd_ex(&AT_CMD_CGATT_R,recbuf,reclen);
		if(osKernelSysTick()-tm>100*1000)break;
		if(ret)break;
	};
	if(tryTm>=100){
		nbiotDescInf.nbStatus=NB_ERROR;
		return 0;
	}
	nbiotDescInf.nbStatus=NB_CGATTED;
	rssi=nb_get_csq();
	return 1;

}

uint16_t nb_udp_cereg(void)
{
	uint16_t ret;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	uint16_t tyrtm=10;	
	while(tyrtm){
		ret=nb_at_cmd_ex(&AT_CMD_CEREG_R,recbuf,reclen);
		if(ret)break;
		nb_at_cmd_ex(&AT_CMD_CEREG,recbuf,reclen);
		__nop();
		osDelay(1000);
		tyrtm--;

	}
	return tyrtm;

}
#else
uint16_t nb_udp_cereg_coap(void)
{
	uint16_t ret;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	uint16_t tyrtm=10;	
	while(tyrtm){
		//ret=nb_at_cmd_ex(&AT_CMD_CGPADDR,recbuf,reclen);
		ret=nb_at_cmd((uint8_t*)"AT+CGPADDR\r\n",(uint8_t*)"OK",recbuf,reclen,1*configTICK_RATE_HZ);
		if(ret)break;

		osDelay(1000);
		tyrtm--;

	}
	return tyrtm;
}	
#endif

uint16_t nb_get_cereg(void)
{
	uint16_t ret;
	#if config_NB_PLAT==UDP
	ret=nb_udp_cereg();
	#else
	ret=nb_udp_cereg_coap();
	#endif
	return ret;
}
#if config_NB_PLAT==UDP
uint16_t nb_udp_create_socket(void)
{
	uint16_t ret;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	uint16_t tyrtm=4;
	//if(nbiotDescInf.nbStatus>=NB_UPD_CONNECTED)return 1;
	while(tyrtm){
		tyrtm--;
		//nb_at_cmd_ex(&AT_CMD_NSOCL,recbuf,reclen);
		__nop();

		ret=nb_at_cmd_ex(&AT_CMD_NSOCR,recbuf,reclen);
		if(ret)break;
		nb_at_cmd_ex(&AT_CMD_NSOCL,recbuf,reclen);

	}
	if(tyrtm){
		nbiotDescInf.nbStatus=NB_CONNECTED;
	}else{
		nbiotDescInf.nbStatus=NB_ERROR;
	}
	return tyrtm;
}
#else
//const uint8_t testNcdp[]="AT+NCDP=117.60.157.137,5684\r\n";
/*
uint16_t nb_coap_ncbp(void)
{
	st_ATcmdDef atcmdTemp={0};
	
	uint16_t ret;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	uint16_t tyrtm=4;
	m_mem_cpy_len((uint8_t*)&atcmdTemp,(uint8_t*)&AT_CMD_NCDP,sizeof(st_ATcmdDef));
	atcmdTemp.cmdStr=sysData.venderUdpSetting;
	//atcmdTemp.cmdStr=testNcdp;
	while(tyrtm){
		tyrtm--;
		__nop();
		//ret=nb_at_cmd_ex(&AT_CMD_NCDP,recbuf,reclen);
		ret=nb_at_cmd_ex(&atcmdTemp,recbuf,reclen);
		if(ret)break;
		//nb_at_cmd_ex(&AT_CMD_NSOCL,recbuf,reclen);
	}
	if(!tyrtm)return tyrtm;
	
	tyrtm=4;
	while(tyrtm){
		tyrtm--;
		__nop();
		ret=nb_at_cmd_ex(&AT_CMD_NCDP_R,recbuf,reclen);
		if(ret)break;
	}	

	return tyrtm;
}	
*/
#endif
uint16_t nb_create_socket(void)
{
	#if config_NB_PLAT==UDP
	return  nb_udp_create_socket();
	#else
	//return nb_coap_ncbp();
	return 1;
	#endif
}

void nb_udp_close_socket(void)
{
	//uint16_t ret;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);	
	//nb_at_cmd_ex(&AT_CMD_NSOCL,recbuf,reclen);
	nb_at_cmd((uint8_t*)"AT+NSOCL=0\r\n",(uint8_t*)"OK",recbuf,reclen,1*configTICK_RATE_HZ);
	//nbiotDescInf.nbStatus=NB_CGATTED;
}



#if config_NB_PLAT==UDP
const uint8_t testUdpSetting[]="211.149.175.111,2200";
uint8_t* nb_udp_send_load(uint8_t* buf,uint16_t len,uint16_t ipidex)
{
	uint16_t t16,ret;
	uint8_t tmpBuf[256];
	uint8_t* d=tmpBuf;
	uint8_t* ipsettins=NULL;	
	
	ret=m_str_cpy(d,(uint8_t*)"AT+NSOST=0,");
	
	t16=ret-1;
	d+=(ret-1);
	if(sysData.DLCS<DLC_STATUS_B){
		ipsettins=(uint8_t*)testUdpSetting;
	}else{
		if(ipidex==0){
			ipsettins=sysData.venderUdpSetting;
		}else{
			ipsettins=sysData.userUdpSetting;
			//ipsettins=(uint8_t*)testUdpSetting;
		}
	}
	ret=m_str_cpy(d,ipsettins);
	d[ret-1]=',';

	t16+=(ret);
	d+=(ret);
	ret=int32_2_d(d,len);
	
	t16+=ret;
	d+=ret;
	*d++=',';
	t16+=1;
	
	ret=m_str_b2h(d,buf,len);
	d+=ret;
	t16+=ret;
	
	*d++='\r';
	*d++='\n';
	*d++='\0';
	
	t16+=3;
	
	m_mem_cpy_len(nbAplSendBuffer,tmpBuf,t16);
	return nbAplSendBuffer;

}
#else
//const uint8_t testUdpSetting[]="211.149.175.111,2200";
uint8_t* nb_udp_send_load_coap(uint8_t* buf,uint16_t len,uint16_t ipidex)
{
	uint16_t t16,ret;
	uint8_t tmpBuf[256];
	//tmpBuf[0]='0';
	//tmpBuf[1]='0';
	uint8_t* d=tmpBuf;
	//uint8_t* ipsettins=NULL;	
	
	ret=m_str_cpy(d,(uint8_t*)"AT+NMGS=");
	
	t16=ret-1;
	d+=(ret-1);

	ret=int32_2_d(d,len+1);
	t16+=ret;
	d+=ret;
	*d++=',';
	t16+=1;
	
	*d++='0';
	*d++='0';
	t16+=2;
	
	ret=m_str_b2h(d,buf,len);
	d+=ret;
	t16+=ret;
	
	*d++='\r';
	*d++='\n';
	*d++='\0';
	
	t16+=3;
	
	m_mem_cpy_len(nbAplSendBuffer,tmpBuf,t16);
	return nbAplSendBuffer;

}

uint8_t* nb_udp_send_load_coap_con(uint8_t* buf,uint16_t len)
{
	uint16_t t16,ret;
	uint8_t tmpBuf[256];
	uint8_t* d=tmpBuf;
	//if(len>100)len=100;
	
	ret=m_str_cpy(d,(uint8_t*)"AT+QLWULDATAEX=");
	
	t16=ret-1;
	d+=(ret-1);

	ret=int32_2_d(d,len+1);
	t16+=ret;
	d+=ret;
	*d++=',';
	t16+=1;
	
	*d++='0';
	*d++='0';
	t16+=2;
	
	ret=m_str_b2h(d,buf,len);
	d+=ret;
	t16+=ret;
	
	*d++=',';
	t16++;
	
	ret=m_str_cpy(d,(uint8_t*)"0x0001");
	t16+=(ret-1);
	d+=(ret-1);
	
	*d++='\r';
	*d++='\n';
	*d++='\0';
	
	t16+=3;
	
	m_mem_cpy_len(nbAplSendBuffer,tmpBuf,t16);
	return nbAplSendBuffer;
}

#endif
#if config_NB_PLAT==UDP
uint16_t nb_udp_send(uint8_t* buf,uint16_t len)
{
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	
	nb_process_disable_sleep_in_lwp();
	//nb_udp_create_socket();
	
	m_mem_set(nbAplReceivedBuffer,'\0',sizeof(nbAplReceivedBuffer));
	uint8_t tmpbuf[20]={0};
	
	uint16_t tryTm,ret;
	uint8_t* str;
	m_mem_cpy(tmpbuf,(uint8_t*)"\r\n0,");
	int32_2_d(tmpbuf+4,len);
	str=nb_udp_send_load(buf,len,0);
	m_uart_send_str(str);
	#if NBIOT_ROUTINES_DISP_EN
	nb_routines_information_disp((uint8_t*)("send-"));
	#endif	
	
	for(tryTm=0;tryTm<5;tryTm++){
		len=0;
		if(pwrStatus==POWER_STATUS_DOWN)break;
		
		len=nb_at_cmd_waite_resp(recbuf,1000,reclen);
		__nop();
		if(len<=2)continue;
		else{
			//break;
			__nop();
			ret=m_str_cmp(recbuf,tmpbuf);
			if(ret)break;
		}
	}
	__nop();
	nb_process_disable_sleep_in_lwp();
	for(tryTm=0;tryTm<5;tryTm++){
		len=0;ret=0;
		if(pwrStatus==POWER_STATUS_DOWN)break;	
		len=nb_at_cmd_waite_resp(recbuf,1000,reclen);
		__nop();
		if(len<=2)continue;
		else{
			ret=m_str_cmp(recbuf,(uint8_t*)"+NSONMI:0,");
			if(ret)break;
			break;
		}
	}	
	//nb_udp_close_socket();
	return ret;
}
#else
uint16_t nb_udp_send_coap(uint8_t* buf,uint16_t len)
{
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	
	nb_process_disable_sleep_in_lwp();
	//nb_udp_create_socket();
	
	m_mem_set(nbAplReceivedBuffer,'\0',sizeof(nbAplReceivedBuffer));
	//uint8_t tmpbuf[20]={0};
	
	uint16_t tryTm,ret,sendTime;
	uint8_t* str;
	//m_mem_cpy(tmpbuf,(uint8_t*)"\r\n0,");
	//int32_2_d(tmpbuf+4,len);
	str=nb_udp_send_load_coap(buf,len,0);
	sendTime=0x00;
	do{
		ret=0;
		if(pwrStatus==POWER_STATUS_DOWN)break;	
		m_lpusart1_send_str(str);
		#if NBIOT_ROUTINES_DISP_EN
		nb_routines_information_disp((uint8_t*)("send-"));
		#endif	
		
		for(tryTm=0;tryTm<5;tryTm++){
			len=0;ret=0;
			if(pwrStatus==POWER_STATUS_DOWN)break;		
			//len=nb_at_cmd_waite_resp(recbuf,1000,reclen);
			len=nb_at_cmd(NULL,(uint8_t*)"OK\r\n",recbuf,reclen,1000);
			__nop();
			if(len<=2)continue;
			else{
				//break;
				__nop();
				ret=m_str_match(recbuf,(uint8_t*)"OK\r\n");
				if(ret)break;
				if(m_str_match(recbuf,(uint8_t*)"ERROR:513\r\n")){
					ret=0;
					__nop();
					break;
				}
			}
		}
		__nop();
		if(ret>0)break;
		ret=0;
		if(pwrStatus==POWER_STATUS_DOWN)break;	
		nb_process_disable_sleep_in_lwp();
		osDelay(10000);
		
	}while(sendTime++<3);
	if(ret==0 && sendTime>3){
		nbFirstCattch=true;
	}
	nb_process_disable_sleep_in_lwp();
	//nb_udp_close_socket();
	return ret;
}
uint16_t nb_udp_send_coap_ex(uint8_t* buf,uint16_t len,bool releasRa)
{
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	
	nb_process_disable_sleep_in_lwp();
	//nb_udp_create_socket();
	
	m_mem_set(nbAplReceivedBuffer,'\0',sizeof(nbAplReceivedBuffer));
	//uint8_t tmpbuf[20]={0};
	
	uint16_t tryTm,ret,sendTime;
	uint8_t* str;
	if(releasRa){
		str=nb_udp_send_load_coap_con(buf,len);
	}else{
		str=nb_udp_send_load_coap(buf,len,0);
	}
	sendTime=0x00;
	do{
		ret=0;
		if(pwrStatus==POWER_STATUS_DOWN)break;	
		m_lpusart1_send_str(str);
		#if NBIOT_ROUTINES_DISP_EN
		nb_routines_information_disp((uint8_t*)("send-"));
		#endif	
		
		for(tryTm=0;tryTm<5;tryTm++){
			len=0;ret=0;
			if(pwrStatus==POWER_STATUS_DOWN)break;		
			//len=nb_at_cmd_waite_resp(recbuf,1000,reclen);
			len=nb_at_cmd(NULL,(uint8_t*)"OK\r\n",recbuf,reclen,1000);
			__nop();
			if(len<=2)continue;
			else{
				//break;
				__nop();
				ret=m_str_match(recbuf,(uint8_t*)"OK\r\n");
				if(ret)break;
				if(m_str_match(recbuf,(uint8_t*)"ERROR:513\r\n")){
					ret=0;
					__nop();
					break;
				}
			}
		}
		__nop();
		if(ret>0)break;
		ret=0;
		if(pwrStatus==POWER_STATUS_DOWN)break;	
		nb_process_disable_sleep_in_lwp();
		osDelay(10000);
		
	}while(sendTime++<3);
	if(ret==0 && sendTime>3){
		nbFirstCattch=true;
	}
	nb_process_disable_sleep_in_lwp();
	//nb_udp_close_socket();
	return ret;
}	
#endif

#if config_NB_PLAT==UDP
uint16_t np_udp_received_extract(uint8_t* str,uint8_t* bin,uint16_t len)
{
	uint8_t t8=0;
	uint16_t i;
	uint16_t t16,ret;
	t16=0;
	//if()
	for(i=0;i<len;i++){
		if(str[i]==',')t8++;
		if(t8==3 && str[i]!=','){
			t16*=10;
			t16+=(str[i]-'0');
		}
		if(t8==4)break;
	}
	if(t8!=4)return 0;
	if(t16>len)return 0;
	t16<<=1;
	str=str+i+1;
	ret=m_str_h2b(bin,str,t16);
	return ret;
}

uint16_t nb_udp_received_test(void)
{
	uartRceivedBufCount=0;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);	
	uint16_t ret;
	ret=nb_at_cmd_ex(&AT_CMD_NSORF,recbuf,reclen);
	return ret;
}

#else
/*
uint16_t my_scanf_int16(uint8_t* str,uint8_t eof,uint16_t* loc)
{
	uint16_t t16,i=0;
	uint16_t ret=0x00;
	uint8_t chr;
	do{
		chr=*str;
		if(chr==eof)break;
		ret*=10;
		
		t16=0;
		_chr2hex(t16,chr);
		ret += t16;
		str++;
		i++;
		if(i>=4)break;
		//;
	}while(1);
	if(loc){
		*loc=i;
	}
	return ret;
}
*/
uint16_t np_udp_received_extract_coap(uint8_t* str,uint8_t* bin,uint16_t len)
{
	//uint8_t t8=0;
	uint16_t i;
	uint16_t t16,ret,loc;
/*
	if(bc95MainVer < BC95B5JB){
		t16=0;
		m_str_match_ex(str,(uint8_t*)("\r\n"),&loc);
		t16=my_scanf_int16(str+2+loc,',',&i);
		t16<<=1;
		str=str+i+3;
		ret=m_str_h2b(bin,str,t16);
	}else{
		t16=0;
		m_str_match_ex(str,(uint8_t*)("+NNMI:"),&loc);
		str=str+sizeof("+NNMI:")-1+loc;
		t16=my_scanf_int16(str,',',&i);
		t16<<=1;
		str=str+i+1;
		ret=m_str_h2b(bin,str,t16);		
	}
*/
	t16=0;
	m_str_match_ex(str,(uint8_t*)("+NNMI:"),&loc);
	str=str+sizeof("+NNMI:")-1+loc;
	t16=my_scanf_int16(str,',',&i);
	t16<<=1;
	str=str+i+1;
	ret=m_str_h2b(bin,str,t16);		
	return ret;
}

uint16_t nb_module_receive_buffer_clear(void)
{
	uint32_t tm;
	uint16_t t16;

	uint16_t ret;
	uartRceivedBufCount=0;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);	
	tm=osKernelSysTick();
	do{
		ret=0;
		if((osKernelSysTick()-tm)>10*1000)break;
		osDelay(500);
		//ret=nb_at_cmd_ex(&AT_CMD_NQMGR,recbuf,reclen);
		ret=nb_at_cmd((uint8_t*)"AT+NQMGR\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
		if(!ret)continue;
		m_str_match_ex(recbuf,(uint8_t*)("BUFFERED="),&t16);
		ret=my_scanf_int16(recbuf+9+t16,',',NULL);
		if(ret==0)break;		
		osDelay(500);
		m_mem_set(nbAplReceivedBuffer,0,sizeof(nbAplReceivedBuffer));
		//ret=nb_at_cmd_ex(&AT_CMD_NMGR,recbuf,reclen);
		ret=nb_at_cmd((uint8_t*)"AT+NMGR\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
	}while(1);
	return 1;
}

uint16_t nb_udp_received_coap_nnmi1(void)
{
	uint32_t tm;
	uint16_t t16;
	//uint16_t loc;
	//uint16_t tryTm=0x00;
	//uint16_t ret;
	uartRceivedBufCount=0;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);	
	tm=osKernelSysTick();
	do{
		//t16=nb_at_cmd_waite_resp(recbuf,2000,reclen);
		t16=nb_at_cmd(NULL,(uint8_t*)"+NNMI",recbuf,reclen,1000);
		if(t16)break;
		if((osKernelSysTick()-tm)>22*configTICK_RATE_HZ)break;

	}while(1);
	return t16;
}

uint16_t nb_udp_received_coap(void)
{
	uint32_t tm;
	uint16_t t16;
	//uint16_t loc;
	uint16_t tryTm=0x00;
	uint16_t ret;
	uartRceivedBufCount=0;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);	
	tm=osKernelSysTick();
	do{
		ret=0;
		if((osKernelSysTick()-tm)>40*1000)break;
		
		osDelay(1000);
		tryTm++;
		//if(tryTm>30)break;
		//ret=nb_at_cmd_ex(&AT_CMD_NQMGR,recbuf,reclen);
		ret=nb_at_cmd((uint8_t*)"AT+NQMGR\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
		if(!ret)continue;
		m_str_match_ex(recbuf,(uint8_t*)("BUFFERED="),&t16);
		//if(t16)
		ret=my_scanf_int16(recbuf+9+t16,',',NULL);
		if(ret==0)continue;
		osDelay(2000);
		m_mem_set(nbAplReceivedBuffer,0,sizeof(nbAplReceivedBuffer));
		//ret=nb_at_cmd_ex(&AT_CMD_NMGR,recbuf,reclen);
		ret=nb_at_cmd((uint8_t*)"AT+NMGR\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
		//np_udp_received_extract_coap();
		__nop();
		
		if(!ret)continue;
		__nop();
		break;
	}while(1);
	return ret;
}
#endif

uint16_t np_received_extract_process(uint8_t* str,uint8_t* bin,uint16_t len)
{
	uint16_t ret;
	#if config_NB_PLAT==UDP
		ret=np_udp_received_extract(str,bin,len);
	#else
		ret=np_udp_received_extract_coap(str,bin,len);
	#endif
	return ret;
}
/*
const  uint8_t TestBuff[]={
0x11,0x03,0xFD,0xEB,0x00,0x00,0x21,0x05,0x04,0x00,0xB8,0x22,0x43,0x00,0x00,0x07,0x35,0x2D,0x33};
*/
/*
uint16_t nb_power_on_process_waite_ok(void)
{

	uint32_t tmSta=0,tm=0;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	
	m_mem_set(nbAplReceivedBuffer,0,sizeof(nbAplReceivedBuffer));
	
	tmSta=osKernelSysTick();
	do{
		nb_process_enable_sleep_in_lwp();

		if(pwrStatus==POWER_STATUS_DOWN)return 0;		
		nb_at_cmd_waite_resp(recbuf,1000,reclen);
		tm=osKernelSysTick()-tmSta;
		if(m_str_match(recbuf,(uint8_t*)"\r\nOK\r\n"))break;
		//tryTm++;
		
		if(tm>10000)break;
	}while(1);
	if(tm<10000){
		return 1;
	}else{
		nbiotDescInf.nbStatus=NB_ERROR;
		return 0;
	}
		
}
*/
int16_t nb_power_on_process(void)
{
	int16_t ret=0;
	uint16_t tryTm=0;
	//if(nbiotDescInf.nbStatus>=NB_UPD_CONNECTED)return 1;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);	
	
	menu=0x00;
	subMenu=subMENU_HOME_NB_CONN;	
	ui_disp_menu();
	m_lpusart1_deinit();
	nb_module_rst_pins_init();
	nb_module_reset_disable();
	nb_module_pwr_pins_init();
	m_lpusart1_init(9600);
	do{
		nb_process_enable_sleep_in_lwp();
		nb_module_pwr_on();

		ret=nb_at_cmd(NULL,(uint8_t*)"OK\r\n",recbuf,reclen,10*configTICK_RATE_HZ);
		if(ret>0)break;
		tryTm++;
		nb_module_pwr_off();
		osDelay(1000);
	}while(tryTm<3);	
	if(tryTm>=3)return 0;
	#if config_NB_PLAT==UDP
	ret=nb_attach_network();
	#else

	#endif
	return ret;
}



uint16_t nb_send_process(uint8_t* buf,uint16_t len)
{
	if(pwrStatus==POWER_STATUS_DOWN)return 0;
	menu=0x00;
	subMenu=subMENU_HOME_NB_SEND;	
	ui_disp_menu();
	#if config_NB_PLAT==UDP
	return nb_udp_send(buf,len);
	#else
	return nb_udp_send_coap_ex(buf,len,false);	
	#endif
	//subMenu=0;
	//return 1;
}

uint16_t nb_received_process(void)
{
	uint16_t ret;
	menu=0x00;
	subMenu=subMENU_HOME_NB_REC;	
	ui_disp_menu();
	#if config_NB_PLAT==UDP
	ret=nb_udp_received_test();
	#else
	//ret=nb_udp_received_coap();	

	ret=nb_udp_received_coap_nnmi1();	
	//>>

	#endif
	return ret;
}

uint16_t nb_power_off_process(void)
{
	nb_process_disable_sleep_in_lwp();
	//menu=0x00;
	//subMenu=0;	
	ui_disp_menu();	
	m_lpusart1_deinit();	
	nb_module_pwr_pins_init();
	nb_module_pwr_off();
	osDelay(1000);
	return 1;
}

typedef enum{
	NB_SEND_POP,
	NB_SEND_ACK,
}nbSendType_t;

#if config_NB_PSM_EN ==0 
void vTheadUdp(void * pvParameters)
{
	osEvent event;
	uint8_t buf[128];
	uint16_t ret,sendNum=0,subSendNum=0x00;
	uint16_t len;//,t16=10;
	(void)pvParameters;	

	while(1){
		event=osSignalWait(flg_NB_MODULE_UDP_PROCESS_REQ,osWaitForever);
		if(event.value.signals & flg_NB_MODULE_UDP_PROCESS_REQ){
			if(pwrStatus<POWER_STATUS_LOW)continue;
			sendNum=0;

			do{
				nb_process_disable_sleep_in_lwp();
				ret=nb_power_on_process();
				if(!ret)break;
				nb_create_socket();
				protocolCmd=0xff;
				opRetCode=__15S_OP_NOTHING;
				_15sSendStaMichine=__15S_SEND_SM_POP;
				sendNum=0;
				subSendNum=0;
				
				do{
					nb_process_disable_sleep_in_lwp();
					if(_15sSendStaMichine==__15S_SEND_SM_POP){
						len=qc_comm_send_pop_ready(nbAplSendBuffer,__15E_CB_CTRL_CODE_DATA);
					}else if(_15sSendStaMichine==__15S_SEND_SM_CLEAR){
						len=qc_comm_send_pop_ready(nbAplSendBuffer,__15E_CB_CTRL_CODE_RET_CLEAR);
					}else{
						len=qc_comm_ins_reply_ready(nbAplSendBuffer,__15E_CB_CTRL_CODE_DATA);
					}
					
					if(subSendNum>=4){delay_long_lwp_dis(50);subSendNum=0x00;}
					if(nb_get_cereg()==0)break;

					nb_send_process(nbAplSendBuffer,len);
					sendNum++;
					subSendNum++;	
					
					if(_15sSendStaMichine==__15S_SEND_SM_CLEAR){
						_15sSendStaMichine=__15S_SEND_SM_ACK;
						continue;
					}else{
						_15sSendStaMichine=__15S_SEND_SM_CLEAR;
					}
					if(opRetCode ==__15S_OP_OK || opRetCode==__15S_OP_PAY_OK ){
					}else{
						if(sendNum<19)sendNum=19;
					}
					
					ret=nb_received_process();
					if(!ret){break;}
					ret=np_received_extract_process(nbAplReceivedBuffer,buf,ret);
					
					if(ret>sizeof(buf))ret=sizeof(buf);
					m_mem_cpy_len(nbAplReceivedBuffer,buf,ret);
				
					ret=qc_comm_received_process(nbAplReceivedBuffer,ret);
				
					if(ret==0 || opRetCode == __15S_OP_NOTHING || opRetCode==__15S_OP_RTC_SYNC)break;
					if(sendNum>=21)break;
					if(pwrStatus==POWER_STATUS_DOWN)break;

				}while(1);
				//附加一包
				opRetCode=__15S_OP_NOTHING;
				break;
			}while(1);
			__nop();
			//nb_udp_close_socket();
			nb_power_off_process();
			//nb_udp_close_socket();
			if(!(sysData.DLCS==DLC_STATUS_A && !fi_id_writed_in_dlcs_a())){
				menu=7;
				subMenu=0;
			}
		}
		data_api_day_change();
		//test 
		#if SEND_TIMES_TEST
		if(sysData.DLCS==DLC_STATUS_C){
			nb_process_disable_sleep_in_lwp();
			menu=0;
			subMenu=subMENU_MAIN_SEND_TIMES;
			osDelay(5000);
			even_send_msg_to_start_rf(false);
		}
		#endif
	}	
}
#else



uint16_t nb_coap_clear_freq_point(void)
{
	uint16_t ret;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	if(bc95SubVer<SP5 && bc95MainVer < BC95B5JB)return 1;
	//ret=nb_at_cmd_ex(&AT_CMD_CFUN_0,recbuf,reclen);
	ret=nb_at_cmd((uint8_t*)"AT+CFUN=0\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
	__nop();	
	//ret=nb_at_cmd_ex(&AT_CMD_NCSEARFCN,recbuf,reclen);
	ret=nb_at_cmd((uint8_t*)"AT+NCSEARFCN\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
	__nop();	
	return ret;
}


int16_t nb_coap_psm_close_erda(void)
{
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);	
	int16_t ret;
	if(bc95SubVer<SP5 && bc95MainVer < BC95B5JB)return 1;
	do{
		//ret=nb_at_cmd_ex(&AT_CMD_CEDRXS_0,recbuf,reclen);
		ret=nb_at_cmd((uint8_t*)"AT+CEDRXS=0,5,\"0101\"\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
		//ret=nb_at_cmd_ex(&AT_CMD_CEDRXRDP,recbuf,reclen);
		ret=nb_at_cmd((uint8_t*)"AT+CEDRXRDP\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
	}while(0);
	return ret;
}

int16_t nb_coap_psm_enable(void)
{
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);	
	int16_t ret=0,tryTm=0;
	if(bc95SubVer<SP5 && bc95MainVer < BC95B5JB)return 1;
	do{
		tryTm++;
		//ret=nb_at_cmd_ex(&AT_CMD_CPSMS,recbuf,reclen);
		ret=nb_at_cmd((uint8_t*)"AT+CPSMS=1,,,01101010,00000001\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
		if(ret!=0)break;
		//ret=nb_at_cmd_ex(&AT_CMD_CPSMS_R,recbuf,reclen);
		ret=nb_at_cmd((uint8_t*)"AT+CPSMS?\r\n",(uint8_t*)"+CPSMS:1,,,01101010,00000001\r\n",recbuf,reclen,2*configTICK_RATE_HZ);
		if(ret>0)break;
		continue;
	}while(tryTm<3);
	//return tryTm<3?ret:0;	
	return ret;
}

int16_t nb_coap_psm_reselect_enable(void)
{
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);	
	int16_t ret;
	if(bc95SubVer<SP5 && bc95MainVer < BC95B5JB)return 1;
	//ret=nb_at_cmd_ex(&AT_CMD_NCONFIG,recbuf,reclen);
	ret=nb_at_cmd((uint8_t*)"AT+NCONFIG=CELL_RESELECTION,TRUE\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
	return ret;	
}

uint16_t nb_coap_psm_release_ra(void)
{
	uint16_t ret,len=0;
	uint8_t t8;
	if(bc95SubVer<SP5 && bc95MainVer < BC95B5JB)return 1;


	nbAplSendBuffer[0]=0xff;
	nbAplSendBuffer[1]=(uint8_t)(NB20E11_SUB_VER);
	nbAplSendBuffer[2]=(uint8_t)(sw_VER_NUM);
	//nbAplSendBuffer[3]=(uint8_t)(int8_t)baudRateOffset;	
	nbAplSendBuffer[3]=0;
	extern volatile uint16_t buadOfsetNum;
	nbAplSendBuffer[4]=(uint8_t)buadOfsetNum;
	
	t8=(uint8_t)(voltBat/100UL);
	t8=m_hex_2_bcd(t8);
	nbAplSendBuffer[5]=t8;
	len=6;

	ret=nb_udp_send_coap_ex(nbAplSendBuffer,len,true);

	return ret;
}
/*
uint16_t nb_set_pwr(uint8_t pwr)
{
	uint16_t ret;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	//if(bc95SubVer<SP5 && bc95MainVer < BC95B5JB)return 1;
	ret=nb_at_cmd_ex(&AT_CMD_NRDTEST0,recbuf,reclen);
	__nop();	
	ret=nb_at_cmd_ex(&AT_CMD_NRDTEST1,recbuf,reclen);
	__nop();	
	ret=nb_at_cmd_ex(&AT_CMD_NRDTEST240,recbuf,reclen);
	__nop();	
	return ret;	
}
*/

uint32_t nb_time_ms_2_s(uint32_t ms)
{
	uint32_t ret;
	ret=ms/1000;
	if(ms%1000)ret++;
	return ret;
}

int16_t nb_at_cmd(uint8_t* cmd,uint8_t* expResp,uint8_t* rbuf,uint16_t ssize,uint32_t tm)
{
	int16_t ret=0;
	uint16_t i,t16;
	uint32_t tmSec;
	
	osEvent event;
	if(!rbuf)return 0;

	nb_process_disable_sleep_in_lwp();
	m_mem_set(rbuf,'\0',ssize);
	uartRceivedBufCountUser=0;
	if(cmd)m_lpusart1_send_str(cmd);
	tmSec=nb_time_ms_2_s(tm);
	for(i=0;i<tmSec;i++){
		if(pwrStatus==POWER_STATUS_DOWN){ret=-1;break;};
		nb_process_disable_sleep_in_lwp();
		ret=0;
		event=osSignalWait(flg_NB_MODULE_ALL_BITS,1000);
		if(event.status!=osEventSignal){	
			continue;
		}
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
				}else{
					ret= t16;
					break;
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

int16_t nb_coap_verify_baudrete(void)
{
	int16_t ret=0;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	
	nb_process_disable_sleep_in_lwp();

	m_lpusart1_deinit();
	osDelay(100);
	m_lpusart1_init(4800);
	osDelay(100);

	nb_at_cmd((uint8_t*)"AT\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
	ret=nb_at_cmd((uint8_t*)"AT\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);	
	if(ret<0)return -1;
	nb_at_cmd((uint8_t*)"AT+NATSPEED=9600,30,1,0,2\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
	
	m_lpusart1_deinit();
	osDelay(100);
	m_lpusart1_init(9600);
	osDelay(100);	
	
	nb_at_cmd((uint8_t*)"AT\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
	nb_at_cmd((uint8_t*)"AT\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
	//nb_module_nbr();
	
	osDelay(100);
	//nb_power_on_process_waite_ok();	
	nb_at_cmd((uint8_t*)"AT+NBR?\r\n",NULL,recbuf,reclen,2*configTICK_RATE_HZ);
	nb_at_cmd(NULL,(uint8_t*)"OK\r\n",recbuf,reclen,10*configTICK_RATE_HZ);

	return 1;
}

bool nbReset=false;
uint16_t nb_coap_psm_send_ready(void)
{
	uint32_t tm,subTm;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	uint16_t tryTm=0,t16;	
	int16_t ret;
	bool readInf=true;
	uint16_t ncdprTry;
//	menu=0x00;
//	subMenu=subMENU_MAIN_HOME_NB_ATTACH;	
//	ui_disp_menu(0);	
	
	//nbPsmStateMachine=NB_PSM_SM_AT;
	m_lpusart1_deinit();
	osDelay(50);
	m_lpusart1_init(9600);
	if(nbPsmStateMachine== NB_PSM_SM_NRZ){
		
	}else if(nbPsmStateMachine==NB_PSM_SM_PSM){
		nbPsmStateMachine=NB_PSM_SM_NCDPR;
		readInf=false;
	}else{nbPsmStateMachine=NB_PSM_SM_AT;}
	__nop();
	tm=osKernelSysTick();
	while(tryTm<=20){
		//tryTm++;
		if((osKernelSysTick()-tm)>180000){
			nbPsmStateMachine=NB_PSM_SM_NO_NET;break;
		}
		if(pwrStatus<POWER_STATUS_LOW){
			nbPsmStateMachine= NB_PSM_SM_NRZ;break;
		}
		switch(nbPsmStateMachine){
		case NB_PSM_SM_NRZ:
			readInf=true;
			nbReset=false;
			//t16=0;
			for(t16=0;t16<3;t16++){
				//ret=nb_at_cmd_ex(&AT_CMD_AT,recbuf,reclen);
				ret=nb_at_cmd((uint8_t*)"AT\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
				if(ret)break;
			}
			if(!ret){
				/*
				tryTm++;
				if(!m_str_match(nbAplReceivedBuffer,(uint8_t*)"ERROR")){
					ret=nb_coap_verify_baudrete();
					if(ret<=0){
						m_uart4_deinit();
						osDelay(100);
						m_uart4_init(9600);
						osDelay(100);	
						//nb_at_cmd_ex(&AT_CMD_AT,recbuf,reclen);		
						nb_at_cmd((uint8_t*)"AT\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
					}					
				}

				ret=nb_at_cmd((uint8_t*)"AT+NRB\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
				osDelay(1000);
				//nb_power_on_process_waite_ok();	

				nb_at_cmd(NULL,(uint8_t*)"OK\r\n",recbuf,reclen,10*configTICK_RATE_HZ);				
				break;	
				*/
			}
			
			//nb_at_cmd_ex(&AT_CMD_CFUN_0,recbuf,reclen);
			nb_at_cmd((uint8_t*)"AT+CFUN=0\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
			nb_get_mver();
			if(bc95MainVer >= BC95B5JB){
				ret=nb_at_cmd((uint8_t*)"AT+QREGSWT?\r\n",(uint8_t*)":1",recbuf,reclen,2*configTICK_RATE_HZ);
				if(ret<=0){
					if(!m_str_match(nbAplReceivedBuffer,(uint8_t*)"=1")){
						nb_at_cmd((uint8_t*)"AT+QREGSWT=1\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
						nbReset=true;
					}
				}
				ret=nb_at_cmd((uint8_t*)"AT+NPOWERCLASS?\r\n",(uint8_t*)"5,5",recbuf,reclen,2*configTICK_RATE_HZ);
				if(ret<=0){
					nb_at_cmd((uint8_t*)"AT+NPOWERCLASS=5,5\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
					//nbReset=true;
				}
			}
			//"AT+NCDP?\r\n"
			ret=nb_at_cmd((uint8_t*)"AT+NCDP?\r\n",(uint8_t*)IOT_IP,recbuf,reclen,2*configTICK_RATE_HZ);
			if(ret<=0){
				m_str_cpy(nbAplSendBuffer,(uint8_t*)"AT+NCDP=");
				m_str_cpy(nbAplSendBuffer+sizeof("AT+NCDP=")-1,(uint8_t*)IOT_IP);
				m_str_cpy(nbAplSendBuffer+sizeof("AT+NCDP=")+sizeof(IOT_IP)-2,(uint8_t*)"\r\n");
				ret=nb_at_cmd(nbAplSendBuffer,(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
				nbReset=true;
			}
			if(nbReset){
				nbReset=false;
				
				tryTm++;
				//ret=nb_module_nbr();
				ret=nb_at_cmd((uint8_t*)"AT+NRB\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
				osDelay(1000);
				//nb_power_on_process_waite_ok();		
				ret=nb_at_cmd(NULL,(uint8_t*)"OK\r\n",recbuf,reclen,10*configTICK_RATE_HZ);
				break;				
			}else{
				nbPsmStateMachine=NB_PSM_SM_ATI;
				break;
			}
		case NB_PSM_SM_AT:
			//nb_at_cmd_ex(&AT_CMD_AT,recbuf,reclen);
			//ret=nb_at_cmd_ex(&AT_CMD_AT,recbuf,reclen);
			nb_at_cmd((uint8_t*)"AT\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
			if(ret){nbPsmStateMachine=NB_PSM_SM_ATI;}
			else{tryTm++;nbPsmStateMachine=NB_PSM_SM_NRZ;}
			break;
		case NB_PSM_SM_ATI:		
			//nb_set_pwr(0);
			//nb_at_cmd_ex(&AT_CMD_CMEE,recbuf,reclen);
			nb_at_cmd((uint8_t*)"AT+CMEE=1\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
			//nb_at_cmd_ex(&AT_CMD_ATI,recbuf,reclen);
			//nb_get_mver();
			/*<<
			if(bc95MainVer < BC95B5JB){
				nb_at_cmd_ex(&AT_CMD_NNMI0,recbuf,reclen);
			}else{
				nb_at_cmd_ex(&AT_CMD_NNMI1,recbuf,reclen);
			}	
			*/
			
			//nb_at_cmd_ex(&AT_CMD_NNMI1,recbuf,reclen);
			//"AT+NNMI=0\r\n"
			ret=nb_at_cmd((uint8_t*)"AT+NNMI=1\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
			//>>
			//nb_at_cmd_ex(&AT_CMD_CGMR,recbuf,reclen);
			nb_at_cmd((uint8_t*)"AT+CGMR\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
			ret=m_str_match_ex(recbuf,(uint8_t*)"SP",&t16);
			if(!ret){nbPsmStateMachine=NB_PSM_SM_CMEE;break;}
			m_mem_cpy_len(SPVER,recbuf+t16-3,6);
			SPVER[6]='\0';
			if(SPVER[5]-'0'>7){
				bc95SubVer=SP7;
			}else{
				bc95SubVer=(bc95Ver_t)(SPVER[5]-'0');
			}
			//add for bc95hc
			nb_coap_clear_freq_point();
			__nop();
			nb_coap_psm_close_erda();
			nb_coap_psm_enable();
			nb_coap_psm_reselect_enable();
			
			nbPsmStateMachine=NB_PSM_SM_CMEE;
			break;
		case NB_PSM_SM_CMEE:
			//nb_at_cmd_ex(&AT_CMD_CMEE,recbuf,reclen);
			nbPsmStateMachine=NB_PSM_SM_CGMR;
			break;
		case NB_PSM_SM_CGMR:
			//nb_at_cmd_ex(&AT_CMD_CGMR,recbuf,reclen);
			nbPsmStateMachine=NB_PSM_SM_CFUNR;
			break;
		case NB_PSM_SM_CFUN:
			//nb_at_cmd_ex(&AT_CMD_CFUN,recbuf,reclen);
			nb_at_cmd((uint8_t*)"AT+CFUN=1\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
			nbPsmStateMachine=NB_PSM_SM_CFUNR;
			break;
		case NB_PSM_SM_CFUNR:
			//ret=nb_at_cmd_ex(&AT_CMD_CFUN_R,recbuf,reclen);
			ret=nb_at_cmd((uint8_t*)"AT+CFUN?\r\n",(uint8_t*)"+CFUN:1",recbuf,reclen,2*configTICK_RATE_HZ);
			if(ret){nbPsmStateMachine=NB_PSM_SM_CGCCONT_R;}
			else{tryTm++;nbPsmStateMachine=NB_PSM_SM_CFUN;}
			break;
		case NB_PSM_SM_CGCCONT:
		
			//nb_at_cmd_ex(&AT_CMD_NCCID,recbuf,reclen);
			ret=nb_at_cmd((uint8_t*)"AT+NCCID\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
			nb_extract_nccid(recbuf,reclen,ICCID);	
			__nop();
			
			//ret=nb_at_cmd_ex(&AT_CMD_CGDCONT_COAP,recbuf,reclen);
			ret=nb_at_cmd((uint8_t*)"AT+CGDCONT=1,\"IP\",\"ctnb\"\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
			nbPsmStateMachine=NB_PSM_SM_CGCCONT_R;
			break;
		case NB_PSM_SM_CGCCONT_R:
			//ret=nb_at_cmd_ex(&AT_CMD_CGDCONT_R,recbuf,reclen);
			ret=nb_at_cmd((uint8_t*)"AT+CGDCONT?\r\n",(uint8_t*)"1,\"IP\",\"ctnb\"",recbuf,reclen,2*configTICK_RATE_HZ);
			if(ret>0){nbPsmStateMachine=NB_PSM_SM_NCDPR;}
			else{tryTm++;nbPsmStateMachine=NB_PSM_SM_CGCCONT;}
			break;
		
		case NB_PSM_SM_NCDP:
			//nb_at_cmd_ex(&AT_CMD_CFUN_0,recbuf,reclen);
			//ret=nb_coap_ncbp();
			//tryTm++;
			nbPsmStateMachine=NB_PSM_SM_NRZ;
			break;
		case NB_PSM_SM_NCDPR:
		
		/*
			ncdprTry=0;
			while(1){
				ret=nb_at_cmd_ex(&AT_CMD_NCDP_R,recbuf,reclen);	
				ncdprTry++;
				if(bc95MainVer < BC95B5JB){
					if(!ret){nbPsmStateMachine=NB_PSM_SM_NCDP;break;}
				}
				
				if(m_str_match(recbuf,(uint8_t*)IOT_IP)){
					nbPsmStateMachine=NB_PSM_SM_CGATTR_FAST;break;
				}else{
					//continue;
				}
				if(ncdprTry<3){
					osDelay(1000);
					continue;
				}else{
					nbPsmStateMachine=NB_PSM_SM_NCDP;
					break;
				}
			}
			*/
			nbPsmStateMachine=NB_PSM_SM_CGATTR_FAST;
			break;
		
		case NB_PSM_SM_CGATT:
			//ret=nb_at_cmd_ex(&AT_CMD_CGATT,recbuf,reclen);
			ret=nb_at_cmd((uint8_t*)"AT+CGATT=1\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
			nbPsmStateMachine=NB_PSM_SM_CGATTR_SLOW;
			break;
		case NB_PSM_SM_CGATTR_SLOW:
			subTm=osKernelSysTick();
			while(1){
				//ret=nb_at_cmd_ex(&AT_CMD_CGATT_R,recbuf,reclen);
				ret=nb_at_cmd((uint8_t*)"AT+CGATT?\r\n",(uint8_t*)"CGATT:1",recbuf,reclen,2*configTICK_RATE_HZ);
				if(ret){nbPsmStateMachine=NB_PSM_SM_CIMI;break;}
				else{
					osDelay(2000);
					nb_process_disable_sleep_in_lwp();
					m_lpusart1_deinit();
					osDelay(50);
					m_lpusart1_init(9600);	
					osDelay(50);
					//nb_at_cmd_ex(&AT_CMD_AT,recbuf,reclen);	
					nb_at_cmd((uint8_t*)"AT\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
					if(osKernelSysTick()-subTm>=100*1000){
						nbPsmStateMachine=NB_PSM_SM_NO_NET;break;
					}
					continue;
				}
			}
			break;			
		case NB_PSM_SM_CGATTR_FAST:
			//ret=nb_at_cmd_ex(&AT_CMD_CGATT_R,recbuf,reclen);
			ret=nb_at_cmd((uint8_t*)"AT+CGATT?\r\n",(uint8_t*)"CGATT:1",recbuf,reclen,2*configTICK_RATE_HZ);
			if(ret){nbPsmStateMachine=NB_PSM_SM_CIMI;}
			else{nbPsmStateMachine=NB_PSM_SM_CGATT;}
			break;
		case NB_PSM_SM_CIMI:
			//nb_at_cmd_ex(&AT_CMD_CSCON_0,recbuf,reclen);
			nb_at_cmd((uint8_t*)"AT+CSCON=0\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
			__nop();
			if(readInf){
				//nb_at_cmd_ex(&AT_CMD_CMI,recbuf,reclen);
				nb_at_cmd((uint8_t*)"AT+CIMI\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
				nb_extract_cimi(recbuf,reclen,CIMI);
			}
			nbPsmStateMachine=NB_PSM_SM_NCCID;
			break;
		case NB_PSM_SM_NCCID:
			if(readInf){
				//nb_at_cmd_ex(&AT_CMD_NCCID,recbuf,reclen);
				nb_at_cmd((uint8_t*)"AT+NCCID\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
				nb_extract_nccid(recbuf,reclen,ICCID);	
			}
			nbPsmStateMachine=NB_PSM_SM_CGSN;
			break;
		case NB_PSM_SM_CGSN:
			if(readInf){
				//nb_at_cmd_ex(&AT_CMD_CGSN,recbuf,reclen);
				nb_at_cmd((uint8_t*)"AT+CGSN=1\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
				nb_extract_cgsn(recbuf,reclen,CGSN);
			}
			nbPsmStateMachine=NB_PSM_SM_CSQ;
			break;
		case NB_PSM_SM_CSQ:
			//nb_at_cmd_ex(&AT_CMD_CSQ,recbuf,reclen);
			nb_at_cmd((uint8_t*)"AT+CSQ\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
			rssi=nb_get_csq();
			nbPsmStateMachine=NB_PSM_SM_NUESTATS;
			break;
		case NB_PSM_SM_NUESTATS:
			//nb_at_cmd_ex(&AT_CMD_NUESTATS,recbuf,reclen);
			nb_at_cmd((uint8_t*)"AT+NUESTATS\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
			ret=m_str_match_ex(recbuf,(uint8_t*)"EARFCN:",&t16);
			if(!ret){nbPsmStateMachine=NB_PSM_SM_QSECSWT;break;}
			m_mem_cpy_len(EARFCN,recbuf+t16+4,7);
			EARFCN[0]='c';EARFCN[1]='n';
			
			//ret=m_str_match_ex(recbuf,(uint8_t*)"SNR:",&t16);
			nb_get_snr();
			nbPsmStateMachine=NB_PSM_SM_QSECSWT;
			break;
		case NB_PSM_SM_QSECSWT:
			//nb_at_cmd_ex(&AT_CMD_QSECSWT_S,recbuf,reclen);
			nbPsmStateMachine=NB_PSM_SM_QSECSWT_R;
			break;
		case NB_PSM_SM_QSECSWT_R:
			//nb_at_cmd_ex(&AT_CMD_QSECSWT,recbuf,reclen);
			nbPsmStateMachine=NB_PSM_SM_SEND;
			break;
		default:
			nbPsmStateMachine=NB_PSM_SM_NO_NET;
			break;
		}
		
		if(nbPsmStateMachine==NB_PSM_SM_SEND || nbPsmStateMachine==NB_PSM_SM_NO_NET)break;
	};
	if(nbPsmStateMachine!=NB_PSM_SM_SEND){
		nb_module_cfun_off();
		nbPsmStateMachine=NB_PSM_SM_NRZ;
		return 0;
	}
	return 1;
}

int16_t nb_fota_process(uint8_t* rbuf,uint16_t rlen)
{
	int ret=0;
	uint32_t tm;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);	
	
	tm=osKernelSysTick();
	if(!(m_str_match(recbuf,(uint8_t*)"FIRMWARE")))return 0;
	if(m_str_match(recbuf,(uint8_t*)"FIRMWARE UPDATE OVER"))return 0;
	while(1){
		if(osKernelSysTick()-tm>600*configTICK_RATE_HZ)break;

		nb_at_cmd(NULL,(uint8_t*)"FIRMWARE",recbuf,reclen,2*configTICK_RATE_HZ);


		if(m_str_match(recbuf,(uint8_t*)"FIRMWARE UPDATE OVER")){
			nbPsmStateMachine=NB_PSM_SM_NRZ;
			ret=1;
		};
	}
	return ret;
}
uint16_t nb_coap_enter_psm(void)
{
	uint32_t tm=0;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	uint16_t ret;
	//if(bc95Ver==BC95B5HB)return 1;
	menu=0x00;
	subMenu=subMENU_HOME_NB_END;		
	tm=osKernelSysTick();
	nb_process_disable_sleep_in_lwp();
	//nb_at_cmd_ex(&AT_CMD_CSCON_0,recbuf,reclen);
	//osDelay(4000);
	do{
		osDelay(1000);
		//ret=nb_at_cmd_ex(&AT_CMD_CSCON_R,recbuf,reclen);
		ret=nb_at_cmd((uint8_t*)"AT+CSCON?\r\n",NULL,recbuf,reclen,2*configTICK_RATE_HZ);
		if(ret){
			if(m_str_match(recbuf,(uint8_t*)"FIRMWARE"))nb_fota_process(recbuf,ret);
		}
		if(m_str_match(recbuf,(uint8_t*)"+CSCON:0,0"))break;
		__nop();
		//if(ret)break;
		
		if(osKernelSysTick()-tm>24000)break;
	}while(1);
	if(ret==0){
		nb_module_cfun_off();
		nbPsmStateMachine=NB_PSM_SM_NRZ;
	}
	//if(nbPsmStateMachine!=NB_PSM_SM_NRZ)nbPsmStateMachine=NB_PSM_SM_PSM;
	//osDelay(1000);
	return 1;
}
bool nbRun=false;
volatile int16_t baudOffsetBuf[16];
volatile uint16_t buadOfsetNum=0;
volatile int16_t baudRateOffset=0;
/*
void nb_baudrate_offset_scan(void)
{
	uint16_t i,j,tblen;
	int16_t ret;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);	
	tblen=sizeof(baudRateOffsetTable)/sizeof(baudRateOffsetTable[0]);
	
	buadOfsetNum=0;
	baudRateOffset=0;
	for(i=0;i<tblen;i++){
		ret=0;
		baudRateOffset=baudRateOffsetTable[i];
		
		nb_process_disable_sleep_in_lwp();
		m_uart4_deinit();
		osDelay(50);
		m_uart4_init(9600);	
		
		for(j=0;j<3;j++){
			ret=0;
			ret=nb_at_cmd((uint8_t*)"AT\r\n",(uint8_t*)"OK",recbuf,reclen,1*configTICK_RATE_HZ);
			
			//osDelay(200);
			if(ret != 0)break;
		}
		//if(ret>0)break;
		if(ret>0){
			baudOffsetBuf[buadOfsetNum]=baudRateOffset;
			buadOfsetNum++;
		}
	}

	if(buadOfsetNum==0){
		baudRateOffset=0;
		return;
	}
	ret=0;
	for(i=0;i<buadOfsetNum;i++){
		ret+=baudOffsetBuf[i];
	}
	ret/=buadOfsetNum;
	baudRateOffset=ret;
	
	m_uart4_deinit();
	osDelay(50);
	m_uart4_init(9600);	
	
	nb_at_cmd((uint8_t*)"AT\r\n",(uint8_t*)"OK",recbuf,reclen,1*configTICK_RATE_HZ);	
}
*/
/*
void nb_baudrate_offset_scan(void)
{
	uint16_t i,j,tblen;
	int16_t ret;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);	
	tblen=sizeof(baudRateOffsetTable)/sizeof(baudRateOffsetTable[0]);

	for(i=0;i<tblen;i++){
		ret=0;
		baudRateOffset=baudRateOffsetTable[i];
		
		m_uart4_deinit();
		osDelay(200);
		m_uart4_init(9600);	
		
		for(j=0;j<4;j++){
			ret=0;
			ret=nb_at_cmd((uint8_t*)"AT\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
			nb_process_disable_sleep_in_lwp();
			osDelay(200);
			if(ret)break;
		}
		if(ret>0)break;
	}
	if(ret<=0 || i>=tblen){
		baudRateOffset=0;
	}

}
*/
void vTheadCoapPsm(void * pvParameters)
{
	osEvent event;
	uint8_t buf[128];
	uint16_t redo,ret,sendNum=0;
	uint16_t len;//,t16=10;
	(void)pvParameters;	
	while(1){
		
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
#endif


void m_thread_create_nb_udp(void)
{
	#if config_NB_PSM_EN ==0 
	osThreadDef(TheadUdp, vTheadUdp, osPriorityNormal, 0, configMINIMAL_STACK_SIZE*4);
	idTheadUdpId=osThreadCreate(osThread(TheadUdp), NULL);	
	#else
	osThreadDef(TheadCoap, vTheadCoapPsm, osPriorityNormal, 0, configMINIMAL_STACK_SIZE*4);
	idTheadUdpId=osThreadCreate(osThread(TheadCoap), NULL);			
	#endif
}

//file end
