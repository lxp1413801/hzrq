#include "./includes/includes.h"

uint8_t CIMI[16]={0};
uint8_t	CGSN[16]={0};
uint8_t	ICCID[21]={0};
uint8_t SPVER[8]={0};
uint8_t EARFCN[8]="cn:0000";
uint8_t SNR[8]="snr:---";
volatile bool nbFirstCattch=true;	

bc95Ver_t bc95SubVer=SP3;
em_bc95MasVer_t bc95MainVer=BC95B5HB;

int16_t nb_at_cmd(uint8_t* cmd,uint8_t* expResp,uint8_t* rbuf,uint16_t ssize,uint32_t tm);

void nb_process_disable_sleep_in_lwp(void)
{
	if(noEventTimeOut<NO_EVEN_MAX_TIME_OUT)noEventTimeOut=NO_EVEN_MAX_TIME_OUT;
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

int16_t nb_get_module_type(void)
{
	uint16_t t16;
	int16_t ret=0;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint8_t* p;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);	
	ret=nb_at_cmd((uint8_t*)"ATI\r\n",(uint8_t*)"OK",recbuf,reclen,1*configTICK_RATE_HZ);
	if(ret<=0)return ret;
	ret=m_str_match_ex(recbuf,(uint8_t*)"Quectel\r\n",(uint16_t*)&t16);
	if(ret<=0)return ret;
	p=recbuf+t16+sizeof("Quectel\r\n")-1;
	m_str_match_ex(p,(uint8_t*)"\r\n",(uint16_t*)&t16);
	m_mem_cpy_len(hzrqNbModuleType,p,t16);
	//
	p=p+t16+2;
	m_str_match_ex(p,(uint8_t*)"Revision:",(uint16_t*)&t16);
	p=p+t16+sizeof("Revision:")-1;
	
	m_str_match_ex(p,(uint8_t*)"\r\n",(uint16_t*)&t16);
	m_mem_cpy_len(hzrqNbModuleSoftVer,p,t16);
	return 1;
}

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



int16_t nb_udp_cereg_coap(void)
{
	uint16_t ret;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	uint16_t tyrtm=10;	
	while(tyrtm){
		//ret=nb_at_cmd_ex(&AT_CMD_CGPADDR,recbuf,reclen);
		ret=nb_at_cmd((uint8_t*)"AT+CGPADDR\r\n",(uint8_t*)"OK",recbuf,reclen,1*configTICK_RATE_HZ);
		if(ret!=0)break;

		osDelay(1000);
		tyrtm--;

	}
	return ret;
}	


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

#else

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

#else

#endif
#if config_NB_PLAT==UDP

#else

int16_t nb_send_process(uint8_t* sbuf,uint16_t slen)
{
	uint8_t t8,chr;
	uint8_t tmpbuf[8]={0};
	uint16_t i;
	uint16_t redo=0;
	int16_t ret;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);	
	menu=MENU_HOME;
	subMenu=subMENU_HOME_NB_SEND;	
	ui_disp_menu();
	if(slen>500)slen=500;
	do{
		redo++;
		m_lpusart1_send_str((uint8_t*)"AT+NMGS=");
		ret=m_int_2_str_ex(tmpbuf,slen,7);
		tmpbuf[ret-1]=',';
		tmpbuf[ret]='\0';
		m_lpusart1_send_str(tmpbuf);
		for(i=0;i<slen;i++)
		{
			nb_process_disable_sleep_in_lwp();
			t8=sbuf[i];
			chr=HexTable[(t8&0xf0)>>4];
			m_lpusart1_send_byte_poll(chr);
			chr=HexTable[(t8&0x0f)];
			m_lpusart1_send_byte_poll(chr);
		}
		
		m_lpusart1_send_str((uint8_t*)"\r\n");
		ret=nb_at_cmd(NULL,(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
		if(ret !=0 )break;
		if(m_str_match(recbuf,(uint8_t*)"ERROR:513")){
			nb_process_disable_sleep_in_lwp();
			osDelay(10000);
			continue;
		}
		break;
	}while(redo<4);
	return ret;	
}
	
int16_t nb_send_process_release_ra(uint8_t* sbuf,uint16_t slen)
{
	uint8_t t8,chr;
	uint8_t tmpbuf[16]={0};
	uint16_t i;
	uint16_t redo=0;
	int16_t ret;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);	
//	menu=MENU_HOME;
//	subMenu=subMENU_HOME_NB_SEND;	
//	ui_disp_menu();
	if(slen>500)slen=500;
	do{
		redo++;
		m_lpusart1_send_str((uint8_t*)"AT+QLWULDATAEX=");
		ret=m_int_2_str_ex(tmpbuf,slen,7);
		tmpbuf[ret-1]=',';
		tmpbuf[ret]='\0';
		m_lpusart1_send_str(tmpbuf);
		for(i=0;i<slen;i++)
		{
			nb_process_disable_sleep_in_lwp();
			t8=sbuf[i];
			chr=HexTable[(t8&0xf0)>>4];
			m_lpusart1_send_byte_poll(chr);
			chr=HexTable[(t8&0x0f)];
			m_lpusart1_send_byte_poll(chr);
		}
		
		m_lpusart1_send_str((uint8_t*)",0x0001\r\n");
		ret=nb_at_cmd(NULL,(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
		if(ret !=0 )break;
		if(m_str_match(recbuf,(uint8_t*)"ERROR:513")){
			nb_process_disable_sleep_in_lwp();
			osDelay(10000);
			continue;
		}
		break;
	}while(redo<4);
	return ret;	
}

#endif

#if config_NB_PLAT==UDP

#else

int16_t np_udp_received_extract_coap(uint8_t* str,uint8_t* bin,uint16_t len)
{
	uint16_t i;
	uint16_t t16,ret,loc;
	t16=0;
	m_str_match_ex(str,(uint8_t*)("+NNMI:"),&loc);
	str=str+sizeof("+NNMI:")-1+loc;
	t16=my_scanf_int16(str,',',&i);
	t16<<=1;
	str=str+i+1;
	ret=m_str_h2b(bin,str,t16);		
	return ret;
}

int16_t nb_module_receive_buffer_clear(void)
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
		osDelay(200);
		//ret=nb_at_cmd_ex(&AT_CMD_NQMGR,recbuf,reclen);
		ret=nb_at_cmd((uint8_t*)"AT+NQMGR\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
		if(!ret)continue;
		m_str_match_ex(recbuf,(uint8_t*)("BUFFERED="),&t16);
		uint16_t t=my_scanf_int16(recbuf+9+t16,',',NULL);
		if(t==0)break;		
		osDelay(200);
		m_mem_set(nbAplReceivedBuffer,0,sizeof(nbAplReceivedBuffer));
		//ret=nb_at_cmd_ex(&AT_CMD_NMGR,recbuf,reclen);
		ret=nb_at_cmd((uint8_t*)"AT+NMGR\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
	}while(1);
	return ret;
}

int16_t nb_udp_received_coap_nnmi1(void)
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
		if(t16!=0)break;
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

int16_t np_received_extract_process(uint8_t* str,uint8_t* bin,uint16_t len)
{
	uint16_t ret;
	ret=np_udp_received_extract_coap(str,bin,len);
	return ret;
}

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


int16_t nb_received_process(void)
{
	int16_t ret;
	menu=MENU_HOME;
	subMenu=subMENU_HOME_NB_REC;	
	ui_disp_menu();

	//ret=nb_udp_received_coap();	

	ret=nb_udp_received_coap_nnmi1();	
	//>>


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

int16_t nb_coap_psm_release_ra(void)
{
	int16_t ret,len=0;
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

	ret=nb_send_process_release_ra(nbAplSendBuffer,len);

	return ret;
}


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

int32_t nb_scanf_int32(uint8_t* str,uint8_t eof,uint16_t* loc)
{
	uint16_t t16,i=0;
	int32_t ret=0x00;
	int32_t x=1L;
	uint8_t chr;
	if(str[0]=='-'){
		str++;
		x=-1L;
		i++;
	}
	do{
		
		chr=*str;
		if(chr<'0' || chr>'9')break;
		if(chr==eof)break;
		ret*=10;
		
		t16=0;
		_chr2hex(t16,chr);
		ret += t16;
		str++;
		i++;
		if(i>=12)break;
		//;
	}while(1);
	if(loc){
		*loc=i;
	}
	return ret*x;
}

void m_uint32_2_bcd(uint8_t* bcd,uint16_t ssize,uint32_t x)
{
	uint8_t buf[6]={0};
	uint8_t t8,i=0;
	uint16_t loc=ssize-1;
	while(x>0){

		t8=(uint8_t)(x%10);
		if(i%2==1){
			//buf[i]|=(t8<<4)
			t8<<=4;
		}	
		buf[loc-(i/2)]|=t8;
		i++;
		x/=10;

	}
	m_mem_cpy_len(bcd,buf,ssize);

}

int16_t nb_get_nuestatus(void)
{
	int32_t t32;
	uint8_t* p;
	int16_t ret=0;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t t16,reclen=sizeof(nbAplReceivedBuffer);
	ret=nb_at_cmd((uint8_t*)"AT+NUESTATS\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);	
	if(ret<=0)return ret;		
	
	ret=m_str_match_ex(recbuf,(uint8_t*)"EARFCN:",&t16);
	if(ret<=0)return ret;	
	//if(!ret){nbPsmStateMachine=NB_PSM_SM_QSECSWT;break;}
	m_mem_cpy_len(EARFCN,recbuf+t16+4,7);
	EARFCN[0]='c';EARFCN[1]='n';
	//nb_get_snr();	
	//ECL:
	ret=m_str_match_ex(recbuf,(uint8_t*)"ECL:",&t16);
	t32=nb_scanf_int32(recbuf+t16+4,'\r',NULL);
	hzrqEclLevel=(int8_t)t32;
	//Cell ID:
	ret=m_str_match_ex(recbuf,(uint8_t*)"Cell ID:",&t16);
	t32=nb_scanf_int32(recbuf+t16+sizeof("Cell ID:")-1,'\r',NULL);
	m_uint32_2_bcd(hzrqCellId,sizeof(hzrqCellId),t32);
	return 1;
}
int16_t nb_get_nuestatus_cell(void)
{
	int32_t t32;
	uint8_t* p;
	int16_t ret=0;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t t16,reclen=sizeof(nbAplReceivedBuffer);
	ret=nb_at_cmd((uint8_t*)"AT+NUESTATS=CELL\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);	
	if(ret<=0)return ret;	
	//NUESTATS:CELL,2504,87,1,-804,-108,-745,231
	//<earfcn>,<physical cell id>,<primarycell>,<rsrp>,<rsrq>,<rssi>,<snr>
	//git 
	ret=m_str_match_ex(recbuf,(uint8_t*)"NUESTATS:CELL,",&t16);
	if(ret<=0)return ret;	
	p=recbuf+t16+sizeof("NUESTATS:CELL,")-1;
	hzrqRealNearFcn=(uint16_t)nb_scanf_int32(p,',',&t16);
	//physical cell id
	p=p+t16+1;
	t32=nb_scanf_int32(p,',',&t16);
	//m_uint32_2_bcd(hzrqCellId,t32);
	//primarycell
	p=p+t16+1;
	t32=nb_scanf_int32(p,',',&t16);
	
	//rsrp
	p=p+t16+1;
	hzrqRsrp=nb_scanf_int32(p,',',&t16);	
	//rsrq
	p=p+t16+1;
	t32=nb_scanf_int32(p,',',&t16);
	//rssi
	p=p+t16+1;
	t32=nb_scanf_int32(p,',',&t16);
	//snr
	p=p+t16+1;
	hzrqSnr=nb_scanf_int32(p,'\r,',&t16);
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
int16_t nb_coap_psm_send_ready(void)
{
	uint32_t tm,subTm;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	uint16_t tryTm=0,t16;	
	int16_t ret;
	bool readInf=true;
	uint16_t ncdprTry;
	menu=0x00;
	subMenu=subMENU_HOME_NB_CONN;	
	ui_disp_menu();	
	
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
		if(pwrStatus==POWER_STATUS_DOWN){
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
			nb_get_module_type();
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
			
			/*
			//nb_at_cmd_ex(&AT_CMD_NUESTATS,recbuf,reclen);
			nb_at_cmd((uint8_t*)"AT+NUESTATS\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
			ret=m_str_match_ex(recbuf,(uint8_t*)"EARFCN:",&t16);
			if(!ret){nbPsmStateMachine=NB_PSM_SM_QSECSWT;break;}
			m_mem_cpy_len(EARFCN,recbuf+t16+4,7);
			EARFCN[0]='c';EARFCN[1]='n';
			*/
			nb_get_nuestatus();
			nb_get_snr();
			nb_get_nuestatus_cell();
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

int16_t nb_coap_enter_psm(void)
{
	uint32_t tm=0;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	uint16_t ret;
	//if(bc95Ver==BC95B5HB)return 1;
//	menu=MENU_HOME;
//	subMenu=subMENU_HOME_NB_END;		
//	ui_disp_menu();
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
	return ret;
}

volatile int16_t baudOffsetBuf[16];
volatile uint16_t buadOfsetNum=0;
volatile int16_t baudRateOffset=0;

#endif

//file end
