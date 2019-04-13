#include "./includes/includes.h"


#define nbAplSendBuffer 	uartSendBuf

//TaskHandle_t idTheadUdpId=(osThreadId)NULL;

uint8_t CIMI[16]={0};
uint8_t	CGSN[16]={0};
uint8_t	ICCID[21]={0};
uint8_t SPVER[8]={0};
uint8_t EARFCN[8]="cn:0000";



//bool nbRun=false;
//extern const uint8_t HexTable[];
__mtkStateMachine_t mtkStateMachine=MTK_POWER_OFF;
/*
osMessageQDef(nbSendMsgQ, 8,sizeof(uint32_t));
osMessageQId nbSendMsgQ;

*/

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
extern uint16_t nbAplReceivedCount;
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
	m_lpusart1_deinit();
	osDelay(200);
	m_lpusart1_init(9600);		
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
	m_lpusart1_deinit();
	osDelay(200);
	m_lpusart1_init(9600);	
	
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
int16_t nb_get_module_type(void)
{
	uint16_t t16;
	int16_t ret=0;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint8_t* p;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);	
	ret=mtk_at_cmd((uint8_t*)"ATI\r\n",(uint8_t*)"OK",recbuf,reclen,1*configTICK_RATE_HZ);
	if(ret<=0)return ret;
	ret=m_str_match_ex(recbuf,(uint8_t*)"LYNQ\r\n",(uint16_t*)&t16);
	if(ret<=0)return ret;
	p=recbuf+t16+sizeof("LYNQ\r\n")-1;
	m_str_match_ex(p,(uint8_t*)"\r\n",(uint16_t*)&t16);
	m_mem_cpy_len(hzrqNbModuleType,p,t16);
	//
	p=p+t16+2;
	//m_str_match_ex(p,(uint8_t*)"Revision:",(uint16_t*)&t16);
	//p=p+t16+sizeof("Revision:")-1;
	
	m_str_match_ex(p,(uint8_t*)"\r\n",(uint16_t*)&t16);
	m_mem_cpy_len(hzrqNbModuleSoftVer,p,t16);
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
/*
int16_t m_str_loc_char(uint8_t* str,uint16_t len,uint8_t chr,uint8_t tms)
{
	uint16_t i;
	int ret=-1;
	uint8_t t8=0;
	for(i=0;i<len;i++){
		if(str[i]==chr)t8++;
		if(t8>=tms){
			ret=i;
			break;
		}
	}
	return ret;
}
*/
uint8_t __x_chr2hex(uint8_t c) 
{
	uint8_t h;
    if(c>='0' && c <='9')h|=(c-'0'); \
    else if(c>='A' && c<='F')h|=(c-'A'+0x0a); 
    else if(c>='a' && c<='f')h|=(c-'a'+0x0a); 
    else h|=0; 
	return h;
}

uint32_t __x_hex_2__int32(uint8_t* hex,uint16_t len)
{
	uint16_t i;
	uint32_t t32=0UL;
	for(i=0;i<len;i++){
		t32<<=4;
		t32|=__x_chr2hex(hex[i]);
	}
	return t32;
}

int16_t nb_get_nuestatus_cell(void)
{
	int32_t t32;
	uint8_t* p;
	int16_t ret=0,l1,l2,rlen;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t t16,reclen=sizeof(nbAplReceivedBuffer);
	ret=mtk_at_cmd((uint8_t*)"AT*MENGINFO=0\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
	if(ret<=0)return ret;	
	rlen=ret;
	//NUESTATS:CELL,2504,87,1,-804,-108,-745,231
	//<earfcn>,<fcn offset>,<pci>,<cellid>,<rsrp>,<rsrq>,<ssi>,<snr>,<band>
	//git :MENGINFOSC: 2506,2,37,"E978052",-100,-7,-93,5,5,"9B95",0,150
	ret=m_str_match_ex(recbuf,(uint8_t*)"*MENGINFOSC: ",&t16);
	if(ret<=0)return ret;	
	p=recbuf+t16+sizeof("*MENGINFOSC: ")-1;
	
	//fcn
	hzrqRealNearFcn=(uint16_t)nb_scanf_int32(p,',',&t16);
	//fcn offset
	p=p+t16+1;
	t32=nb_scanf_int32(p,',',&t16);
	//pci
	p=p+t16+1;
	t32=nb_scanf_int32(p,',',&t16);
	
	
	//cellid
	l1=m_str_loc_char(recbuf,rlen,'"',1);
	if(l1<0)return 0;
	l2=m_str_loc_char(recbuf,rlen,'"',2);
	if(l2<0)return 0;
	t32=__x_hex_2__int32(recbuf+l1+1,l2-l1-1);
	m_uint32_2_bcd(hzrqCellId,sizeof(hzrqCellId),t32);
	

	//rsrp
	p=recbuf+l2+2;
	hzrqRsrp=nb_scanf_int32(p,',',&t16);
	
	//rsrq
	p=p+t16+1;
	t32=nb_scanf_int32(p,',',&t16);	
	//rssi
	p=p+t16+1;
	t32=nb_scanf_int32(p,',',&t16);
	//snr
	p=p+t16+1;
	hzrqSnr=nb_scanf_int32(p,',',&t16);
	//<band>
	p=p+t16+1;
	t32=nb_scanf_int32(p,',',&t16);
	//<tac>

	l2=m_str_loc_char(recbuf,rlen,'"',4);
	p=recbuf+l2+2;	
	if(l2<0)return 0;
	//<tac>
	//p=p+t16+1;
	t32=nb_scanf_int32(p,',',&t16);	
	hzrqEclLevel=(int8_t)t32;	
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
	//
//	ret=mtk_at_cmd((uint8_t*)"AT+CREG?\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);	
//	if(ret<=0)return ret;	
	
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
	nb_get_nuestatus_cell();
	l620_get_ver();
	nb_get_module_type();
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
	subMenu=subMENU_HOME_NB_SEND;	
	ui_disp_menu();	
	
	//m_uart_send_str((uint8_t*)"at+m2mclisend=00");
	m_lpusart1_send_str((uint8_t*)"at+m2mclisend=");
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

int16_t l620_received(uint8_t* rbuf,uint16_t rsize)
{
	int16_t ret;
	menu=MENU_HOME;
	subMenu=subMENU_HOME_NB_REC;	
	ui_disp_menu();	
	
	ret=mtk_at_cmd(NULL,(uint8_t*)"M2MCLIRECV:",rbuf,rsize,16*configTICK_RATE_HZ);
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

int16_t l620_cfun_off(void)
{
	int16_t ret=0;
	uint8_t* recbuf=nbAplReceivedBuffer;
	uint16_t reclen=sizeof(nbAplReceivedBuffer);
	ret=mtk_at_cmd((uint8_t*)"AT+CFUN=0\r\n",(uint8_t*)"OK",recbuf,reclen,2*configTICK_RATE_HZ);
	return ret;	
}

int16_t l620_send_ready(void)
{
	int16_t ret;
//	uint8_t* recbuf=nbAplReceivedBuffer;
//	uint16_t reclen=sizeof(nbAplReceivedBuffer);
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
			subMenu=subMENU_HOME_NB_END;				
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
			subMenu=subMENU_HOME_NB_END;			
			l620_hal_power_off();
			mtkStateMachine=MTK_POWER_OFF;
			ret=0;
			break;
		}		
	}
	return ret;
}





#elif NB_MODULE_VENDOR==BC26
	#error "defined nb vendor error!!"
#else
	#error "defined nb vendor error!!"
#endif

//file end
