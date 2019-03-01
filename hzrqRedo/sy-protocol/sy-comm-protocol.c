#include "./includes/includes.h"
#include "float.h"
//sha256,https://github.com/jb55/sha256.c/blob/master/sha256.h
/*
数据生成过程说明：
1,数据对齐填充;
2,数据加密;
3,生成sha256;
4,计算并且附着mac;
5,计算并且附着crc16;
6,结束字节;
数据解析过程
1,计算验证CRC
2,计算sha256
2,计算验证MAC
4,数据解密
5,提取数据

数据加密区域和mac计算从数据标识码之后开始
*/
uint8_t	syCmdSn=0x00;
__sy_CommStatus_t syCommStatus=__sy_COMMUNICATION_STATUS_IDLE;
/*
typedef union{
	uint8_t snBuf[16];
	struct{
		uint8_t factoryCode[3];
		uint8_t	productionDate[4];
		uint8_t	sn[7];
		uint8_t	ver[2];
	}stru;
}snStrFormat_t;
*/

uint8_t __syID[8]={0};
uint8_t keyCrypt=1;
uint8_t keyMac=1;
uint8_t frameBuf[SY_COMM_FRAME_BUF_LEN];

__sy_errCode_t __syErrorCode=__SY_ERRCODE_NO_ERR;

void __sy_err_code_set(__sy_errCode_t ec)
{
	__syErrorCode=ec;
}

uint8_t __sy_err_code_get(void)
{
	return (uint8_t)__syErrorCode;
}

uint16_t __sy_get_sys_dt(uint8_t* dt)
{
	__nop();
	*dt++=sysRtcDataTime.YY;
	*dt++=sysRtcDataTime.MM;
	*dt++=sysRtcDataTime.DD;
	*dt++=sysRtcDataTime.hh;
	*dt++=sysRtcDataTime.mm;
	*dt  =sysRtcDataTime.ss;//m_hex_2_bcd(stRtcDataTime.YY);
	return 6;
}



uint16_t __sy_get_random(uint8_t* buf,uint8_t len)
{
	m_mem_set(buf,0,len);
	return len;
}

uint16_t __sy_data_pkcs7padding(uint8_t* buf,uint16_t len)
{
	uint16_t i,t16,retlen;
	t16=(16-(len&0x0f));
	retlen=(len & ~0x0f) +16;
	for(i=len;i<retlen;i++){
		buf[i]=t16;
	}
	return retlen;
}

uint16_t __sy_crypt(uint8_t* buf,uint16_t len,uint8_t cryptkey)
{
	uint16_t t16=len;
	uint8_t key[16]={0x00,0x11};
	t16=__sy_data_pkcs7padding(buf,len);
	//esam_sm4_crypt(buf,buf,t16,cryptkey,false);	
	m_ebc_encrypt(buf,buf,key,t16);
	return t16;
}	

uint16_t __sy_crypt_append_mac(uint8_t* buf,uint16_t len,uint8_t cryptkey,uint8_t mackey)
{
	uint8_t tempbuf[32];
	uint16_t t16=len;
	uint8_t key[16]={0x00,0x11};
	t16=__sy_data_pkcs7padding(buf,len);
	
	//esam_sm4_crypt(buf,buf,t16,cryptkey,false);
	m_ebc_encrypt(buf,buf,key,t16);
	//sha256
	sha256_hash(tempbuf,buf,len);
	m_mem_cpy_len(buf+t16,tempbuf,sizeof(tempbuf));
	t16+=32;
	//cal mac
	//esam_sm4_mac_cal(buf,tempbuf,t16,mackey);
	m_mem_cpy_len(buf+t16,tempbuf,4);
	return len;
}

uint16_t __sy_append_mac(uint8_t* buf,uint16_t len,uint8_t mackey)
{
	uint8_t tempbuf[32];
	uint16_t t16=len;
	t16=__sy_data_pkcs7padding(buf,len);
	len=t16;
	//sha256
	sha256_hash(tempbuf,buf,len);
	m_mem_cpy_len(buf+t16,tempbuf,sizeof(tempbuf));
		
	t16+=32;
	//cal mac
	//esam_sm4_mac_cal(buf,tempbuf,t16,mackey);
	m_mem_cpy_len(buf+len,tempbuf,4);

	return len+4;
}

uint16_t __sy_crc_verify(uint8_t* rbuf,uint16_t rlen)
{
	uint16_t t16=0;
	__sy_frameHerder_t* sth=(__sy_frameHerder_t*)rbuf;
	m_mem_cpy_len((uint8_t*)&t16,sth->len,sizeof(uint16_t));

	if(t16+2!=rlen)return 0;
	
	t16=crc_verify(rbuf+1,t16);
	__nop();
	return t16;
}

//从接受到的帧中提取数据域长度,不包括CRC
uint16_t __sy_get_frame_valid_datalen(uint8_t* rbuf,uint16_t rlen)
{
	uint16_t len=0;
	__sy_frameHerder_t* sth=(__sy_frameHerder_t*)rbuf;
	m_mem_cpy_len((uint8_t*)&len,sth->len,sizeof(uint16_t));
	//
	if(len<sizeof(__sy_frameHerder_t))return 0;
	len=len+2-sizeof(__sy_frameHerder_t) -2 /*ident*/ - 3 /*crc + endchar*/;
	return len;
}

uint16_t __sy_verify_mac(uint8_t* rbuf,uint16_t rlen,uint8_t mackey)
{
	uint8_t shaMac[32];
	uint8_t tmpBuf[64];
	uint16_t t16,dlen;
	
	t16=__sy_get_frame_valid_datalen(rbuf,rlen);
	if(t16%16 != 4){
		__nop();
		return 0;
	}
	dlen=t16-4;
	m_mem_cpy_len(tmpBuf,rbuf+sizeof(__sy_frameHerder_t)+2,dlen);
	
	sha256_hash(shaMac,tmpBuf,dlen);
	m_mem_cpy_len(tmpBuf+dlen,shaMac,32);
	t16=dlen+32;
	
	m_mem_cpy_len(shaMac,rbuf+sizeof(__sy_frameHerder_t)+2+dlen,4);
	//t16=esam_sm4_mac_verify(tmpBuf,shaMac,dlen,mackey);
	__nop();
	return t16;
}

void __sy_crc_append(uint8_t* buf,uint16_t len)
{
	if(len<2)return;
	crc_append(buf+1,len-1);
}

void __sy_add_event_record(uint8_t eventCode)
{
	__nop();
}

//in format:YYMMDDhhmmss,BCD
extern void m_rtc_set(sysDataTime_t* dt);
void __sy_set_rtc(uint8_t* buf)
{
	uint8_t DD,MM;
	uint16_t year;
	sysDataTime_t dt;
	dt.YY=*buf++;
	dt.MM=*buf++;
	dt.DD=*buf++;
	dt.hh=*buf++;
	dt.mm=*buf++;
	dt.ss=*buf;
	
	year=2000+m_bcd_2_hex(dt.YY);
	MM=m_bcd_2_hex(dt.MM);
	DD=m_bcd_2_hex(dt.DD);
	dt.day=day_of_week(year,MM,DD);
	m_rtc_set(&dt);
}

uint32_t __sy_dt_t0_ts(uint8_t* buf)
{
	uint32_t t32;
	sysDataTime_t dt;
	dt.YY=*buf++;
	dt.MM=*buf++;
	dt.DD=*buf++;
	dt.hh=*buf++;
	dt.mm=*buf++;
	dt.ss=*buf;	
	t32=system_dt_to_time_stamp(&dt);
	return t32;
}

uint16_t __sy_load_meter_id(uint32_t id,uint16_t fc,uint16_t softVer,uint16_t pd,uint8_t* syid)
{
	uint8_t buf[16];
	m_int_2_str(buf+2,fc,3);
	//m_int_2_str(buf+2,fc,3);
	m_int_2_str(buf+6,pd,4);

	m_int_2_str(buf+13,id,7);
	m_int_2_str(buf+15,softVer,2);
	m_str_h2b(syid,buf,16);
	m_mem_cpy_len(__syID,syid,8);
	return 8;
}

uint16_t __sy_load_frame_header(uint8_t* sbuf,uint16_t ssize,uint8_t cmdsn,uint8_t cb)
{
	uint16_t t16;
	__sy_frameHerder_t* sth=(__sy_frameHerder_t*)sbuf;
	if(ssize<sizeof(__sy_frameHerder_t))return 0;
	

	sth->startChar=__nSY_FRAME_START_CHAR;

	t16=sizeof(__sy_frameHerder_t)+sizeof(__sy_dfRegisterUp_t)+sizeof(__sy_frameTail_t)-2;
	m_mem_cpy_len(sth->len,(uint8_t*)&t16,sizeof(uint16_t));

	//__sy_load_meter_id(sysData.ID,sysData.VenderCode,sysData.swVer,sysData.productionDate,sbuf+3);

	sth->protocolCode=__SY_PROTOCOL_CODE;
	sth->protocolVer=__SY_PROTOCOL_VER;
	sth->cmdSn=cmdsn;
	
	sth->ctrlByte=cb;
	return sizeof(__sy_frameHerder_t);
}


uint16_t __sy_gas_log_format(__sy_gasLog_t* syGasLog,consumeLog_t* commGasLog)
{

	uint32_t t32;
	uint8_t buf[16];
	sysDataTime_t* pdt=(sysDataTime_t*)buf;
	t32=commGasLog->ts;
	time_stamp_to_system_dt(t32,pdt);
	//YYMMDDhhmm
	syGasLog->dt[0]=pdt->YY;
	syGasLog->dt[1]=pdt->MM;
	syGasLog->dt[2]=pdt->DD;
	syGasLog->dt[3]=pdt->hh;
	syGasLog->dt[4]=pdt->mm;
	//XXXXXXXX.XX
	t32=commGasLog->volume;
	t32=t32/(sysData.QS);
	m_mem_set(buf,0,sizeof(buf));
	m_int_2_str(buf+7,t32,8);
	
	t32=t32%(sysData.QS);
	t32=(t32*100)/(sysData.QS);
	m_int_2_str(buf+9,t32,2);
	//hehe~~~
	m_str_h2b(syGasLog->volume,buf,10);
	return sizeof(__sy_gasLog_t);
}

uint16_t __sy_event_log_format(__sy_eventLog_t* syEventLog,eventLog_t* commEventLog)
{
	uint32_t t32;
	uint16_t t16;
	uint8_t buf[16];
	sysDataTime_t* pdt=(sysDataTime_t*)buf;
	t32=commEventLog->ts;
	time_stamp_to_system_dt(t32,pdt);
	//YYMMDDhhmmss
	syEventLog->dt[0]=pdt->YY;
	syEventLog->dt[1]=pdt->MM;
	syEventLog->dt[2]=pdt->DD;
	syEventLog->dt[3]=pdt->hh;
	syEventLog->dt[4]=pdt->mm;	
	syEventLog->dt[5]=pdt->ss;	
	
	t16=commEventLog->eventCode;
	m_mem_cpy_len(syEventLog->eventType,(uint8_t*)&t16,sizeof(uint16_t));
	
	syEventLog->times[0]=0x00;
	syEventLog->times[1]=0x01;
	
	syEventLog->xx=0x00;
	return sizeof(__sy_eventLog_t);
}

uint8_t __sy_status_byte_cross_map(void)
{
	__sy_runStatusByteDef_t sta;
	sta.b=0x00;
	if(pwrStatus==POWER_STATUS_DOWN){
		sta.bits.bMstBatStatus=2;
	}else if(pwrStatus==POWER_STATUS_LOW){
		sta.bits.bMstBatStatus=1;
	}
	//
	if(vavleState==VALVE_ON){
		sta.bits.bValveStatus=1;
	}
	if(sysData.lockReason.bits.bStrongMagnetic>=MAX_STE_OFF_TIMES || sysData.lockReason.bits.bNoflow>=4 
	|| sysData.lockReason.bits.bSeverOff){
		sta.bits.bValveLocked=1;
	}
//if(sysData.devStatus.bits.bValveStatus==){
//	sta.bits.bMeterAbnormal=1;
//}
	return sta.b;
}

/*
typedef struct{
	uint8_t iden[2];
	uint8_t	esamId[8];
	uint8_t	keyCrypt;
	uint8_t	keyMac;
	uint8_t	dt[6];
	uint8_t	random[8];
	uint8_t	mac[4];
}__sy_dfRegisterUp_t;
*/
uint16_t __sy_load_frame_register(uint8_t* sbuf,uint16_t ssize,uint8_t keyCrypt,uint8_t keyMac)
{
	uint8_t* p;
	uint16_t len=0;
	uint16_t t16;
	
	if(ssize<sizeof(__sy_frameHerder_t)+sizeof(__sy_dfRegisterUp_t)+3+48)return 0;
	
	__sy_frameHerder_t* sth=(__sy_frameHerder_t*)sbuf;
	
	__sy_dfRegisterUp_t* stb;
	len=__sy_load_frame_header(sbuf,ssize,syCmdSn,0xc1);
	p=sbuf+len;
	
	stb=(__sy_dfRegisterUp_t*)p;
	
	//load body
	t16=__fdfSY_DFID_REGISTER;
	m_mem_cpy_len(stb->iden,(uint8_t*)&t16,sizeof(t16));
	
	//get esamid
	//esam_apl_setup();
	//m_mem_cpy_len(stb->esamId,esamID,8);
	
	keyCrypt=1;keyMac=1;
	stb->keyCrypt=keyCrypt;
	stb->keyMac=keyMac;
	
	__sy_get_sys_dt(stb->dt);	
	__sy_get_random(stb->random,8);
	
	//从什么位置开始计算mac???,-4:mac占4字节,-2数据标准码占用2字节,加密以及mac计算不包含数据标识码
	len+=2;	//不包含数据标志码
	p=sbuf+len;
	t16=__sy_append_mac(p,sizeof(__sy_dfRegisterUp_t)-4-2,keyMac);
	len=len+t16;
	
	//p=sbuf+len;
	t16=len+3-2;
	m_mem_cpy_len(sth->len,(uint8_t*)&t16,sizeof(uint16_t));	
	
	__sy_crc_append(sbuf,len);
	p=sbuf+len+2;
	*p= __nSY_FRAME_END_CHAR;
	len+=3;

	return len;
}

uint16_t __sy_load_frame_whole_inf(uint8_t* sbuf,uint16_t ssize,uint8_t keyCrypt,uint8_t keyMac)
{
	uint8_t* p;
	uint16_t len=0;
	uint16_t t16;	
	uint8_t tbuf[32];
	if(ssize<sizeof(__sy_frameHerder_t)+sizeof(__sy_dfRegisterUp_t)+3+48)return 0;
	__sy_frameHerder_t* sth=(__sy_frameHerder_t*)sbuf;
	len=__sy_load_frame_header(sbuf,ssize,syCmdSn,0x82);
	p=sbuf+len;

	t16=ssize-len;
	if(t16<sizeof(__sy_gasLog_t)+sizeof(__sy_eventLog_t)+6)return 0;
	
	__sy_fdfWholeInf_t* stb=(__sy_fdfWholeInf_t*)p;
	t16=__fdfSY_DFID_WHOLE_INF;
	m_mem_cpy_len(stb->iden,(uint8_t*)&t16,sizeof(uint16_t));
	stb->rssi=rssi;
	stb->battaryCap=batCapacity;
	stb->sta=__sy_status_byte_cross_map();
	
	ex_data_read_record(PART_SN_CONSTLOG_DAY,tbuf,sizeof(tbuf),&t16);
	if(t16>1)t16-=1;
	m_mem_cpy_len(stb->usendGasLogNm,(uint8_t*)&t16,sizeof(uint16_t));
	
	t16=1;
	m_mem_cpy_len(stb->sendGasLogNm,(uint8_t*)&t16,sizeof(uint16_t));
	
	__sy_gas_log_format((__sy_gasLog_t*)(&(stb->gasLog_1st)),(consumeLog_t*)tbuf);
	
	len+=sizeof(__sy_fdfWholeInf_t);
	p=sbuf+len;
	__sy_fdfWholeInfEx_t* stbex=(__sy_fdfWholeInfEx_t*)p;
	
	ex_data_read_record(PART_SN_EVENTLOG,tbuf,sizeof(tbuf),&t16);
	if(t16>1)t16-=1;
	if(t16>0xff)t16=0xff;
	stbex->usendEventLogNm=(uint8_t)t16;
	stbex->sendEventLogNm=1;

	__sy_event_log_format(&(stbex->eventLog_1st),(eventLog_t*)tbuf);
	len+=sizeof(__sy_fdfWholeInfEx_t);
	
	//无mac 密文
	p=sbuf+sizeof(__sy_frameHerder_t)+2;
	t16=len-sizeof(__sy_frameHerder_t)-2;
	
	t16=__sy_crypt(p,t16,keyCrypt);
	len=sizeof(__sy_frameHerder_t)+2+t16;
	
	t16=len+3-2;
	m_mem_cpy_len(sth->len,(uint8_t*)&t16,sizeof(uint16_t));	
	
	__sy_crc_append(sbuf,len);
	p=sbuf+len+2;
	*p= __nSY_FRAME_END_CHAR;
	len+=3;
	return len;
}

//0x1001
uint16_t __sy_load_frame_run_status(uint8_t* sbuf,uint16_t ssize)
{
	uint8_t* p;
	uint16_t t16,len;
	__sy_frameHerder_t* sth=(__sy_frameHerder_t*)sbuf;
	
	if(ssize<sizeof(__sy_frameHerder_t)+3+3)return 0;
	len=__sy_load_frame_header(sbuf,ssize,syCmdSn,0x82);
	p=sbuf+len;	
	
	__sy_fdfRunStatus_t* stb=(__sy_fdfRunStatus_t*)p;
	t16=__fdfSY_DFID_RUN_STATUS;
	m_mem_cpy_len(stb->iden,(uint8_t*)&t16,sizeof(uint16_t));
	
	stb->sta=__sy_status_byte_cross_map();
	len+=3;
	t16=len+3-2;
	
	m_mem_cpy_len(sth->len,(uint8_t*)&t16,sizeof(uint16_t));	
	
	__sy_crc_append(sbuf,len);
	p=sbuf+len+2;
	*p= __nSY_FRAME_END_CHAR;
	len+=3;
	return len;	
}

//0x1002
uint16_t __sy_load_frame_stell_status(uint8_t* sbuf,uint16_t ssize)
{
	uint8_t* p;
	uint16_t t16,len;
	__sy_frameHerder_t* sth=(__sy_frameHerder_t*)sbuf;
	
	if(ssize<sizeof(__sy_frameHerder_t)+3+3)return 0;
	len=__sy_load_frame_header(sbuf,ssize,syCmdSn,0x82);
	p=sbuf+len;	
	
	__sy_fdfSetlleSta_t* stb=(__sy_fdfSetlleSta_t*)p;
	t16=__fdfSY_DFID_SETLLE_STATUS;
	m_mem_cpy_len(stb->iden,(uint8_t*)&t16,sizeof(uint16_t));
	
	//stb->setlleSta=sysData.__syStellStatus;
	len+=3;
	t16=len+3-2;
	
	m_mem_cpy_len(sth->len,(uint8_t*)&t16,sizeof(uint16_t));	
	
	__sy_crc_append(sbuf,len);
	p=sbuf+len+2;
	*p= __nSY_FRAME_END_CHAR;
	len+=3;
	return len;	
}

uint16_t __sy_load_frame_voltage(uint8_t* sbuf,uint16_t ssize,uint16_t voltage)
{
	uint8_t* p;
	uint16_t t16,len;
	__sy_frameHerder_t* sth=(__sy_frameHerder_t*)sbuf;
	
	if(ssize<sizeof(__sy_frameHerder_t)+4+3)return 0;
	len=__sy_load_frame_header(sbuf,ssize,syCmdSn,0x82);
	p=sbuf+len;	
	
	__sy_fdfVoltage_t* stb=(__sy_fdfVoltage_t*)p;
	t16=__fdfSY_DFID_VOLTAGE;
	m_mem_cpy_len(stb->iden,(uint8_t*)&t16,sizeof(uint16_t));
	
	t16=voltage/1000;
	stb->voltage[0]=m_hex_2_bcd((uint8_t)t16);
	
	t16=voltage%1000;
	t16/=10;
	stb->voltage[0]=m_hex_2_bcd((uint8_t)t16);
	
	len+=4;
	t16=len+3-2;
	m_mem_cpy_len(sth->len,(uint8_t*)&t16,sizeof(uint16_t));	

	__sy_crc_append(sbuf,len);
	p=sbuf+len+2;
	*p= __nSY_FRAME_END_CHAR;
	len+=3;
	return len;		
}

uint16_t __sy_load_frame_rtc(uint8_t* sbuf,uint16_t ssize)
{
	uint8_t* p;
	uint16_t t16,len;
	__sy_frameHerder_t* sth=(__sy_frameHerder_t*)sbuf;
	
	if(ssize<sizeof(__sy_frameHerder_t)+4+3)return 0;
	len=__sy_load_frame_header(sbuf,ssize,syCmdSn,0x82);
	p=sbuf+len;	

	__sy_fdfRtc_t* stb=(__sy_fdfRtc_t*)p;
	t16=__fdfSY_DFID_RTC;
	m_mem_cpy_len(stb->iden,(uint8_t*)&t16,sizeof(uint16_t));	
	stb->dt[0]=sysRtcDataTime.YY;
	stb->dt[1]=sysRtcDataTime.MM;
	stb->dt[2]=sysRtcDataTime.DD;
	stb->dt[3]=sysRtcDataTime.hh;
	stb->dt[4]=sysRtcDataTime.mm;
	stb->dt[5]=sysRtcDataTime.ss;	
	
	len+=8;
	t16=len+3-2;
	m_mem_cpy_len(sth->len,(uint8_t*)&t16,sizeof(uint16_t));	

	__sy_crc_append(sbuf,len);
	p=sbuf+len+2;
	*p= __nSY_FRAME_END_CHAR;
	len+=3;
	return len;		
} 

uint16_t __sy_ins_exe_rtc(uint8_t* sbuf,uint16_t len)
{
	
	return 1;
}

//0x2004
uint16_t __sy_load_frame_rssi(uint8_t* sbuf,uint16_t ssize)
{
	uint8_t* p;
	uint16_t t16,len;
	__sy_frameHerder_t* sth=(__sy_frameHerder_t*)sbuf;
	
	if(ssize<sizeof(__sy_frameHerder_t)+3+3)return 0;
	len=__sy_load_frame_header(sbuf,ssize,syCmdSn,0x82);
	p=sbuf+len;	
	
	__sy_fdfRssi_t* stb=(__sy_fdfRssi_t*)p;
	t16=__fdfSY_DFID_RSSI;
	m_mem_cpy_len(stb->iden,(uint8_t*)&t16,sizeof(uint16_t));
	
	stb->rssi=rssi;
	len+=3;
	t16=len+3-2;
	
	m_mem_cpy_len(sth->len,(uint8_t*)&t16,sizeof(uint16_t));	
	
	__sy_crc_append(sbuf,len);
	p=sbuf+len+2;
	*p= __nSY_FRAME_END_CHAR;
	len+=3;
	return len;	
}
//0x2005
uint16_t __sy_load_frame_total_volume(uint8_t* sbuf,uint16_t ssize,uint8_t keyCrypt,uint8_t keyMac)
{
	
	uint8_t* p;
	uint32_t t32;
	uint16_t t16,len;
	uint8_t tbuf[16];
	__sy_frameHerder_t* sth=(__sy_frameHerder_t*)sbuf;
	
	//if(ssize<sizeof(__sy_frameHerder_t)+3+3)return 0;
	len=__sy_load_frame_header(sbuf,ssize,syCmdSn,0x82);
	p=sbuf+len;	
	
	__sy_fdfTotaleVlume_t* stb=(__sy_fdfTotaleVlume_t*)p;
	t16=__fdfSY_DFID_TOTAL_VOL;
	m_mem_cpy_len(stb->iden,(uint8_t*)&t16,sizeof(uint16_t));
	
	
	t32=totalVolume;
	t32=t32/(sysData.QS);
	m_mem_set(tbuf,0,sizeof(tbuf));
	m_int_2_str(tbuf+7,t32,8);
	
	t32=totalVolume;
	t32=t32%(sysData.QS);
	t32=t32*10000/(sysData.QS);
	m_int_2_str(tbuf+11,t32,4);
	
	m_str_h2b(stb->tov,tbuf,12);
	
	p=sbuf+sizeof(__sy_frameHerder_t)+2;
	t16=sizeof(__sy_fdfTotaleVlume_t)-2;
	t16=__sy_crypt_append_mac(p,t16,keyCrypt,keyMac);
	len=len+t16;

	t16=len+3-2;
	m_mem_cpy_len(sth->len,(uint8_t*)&t16,sizeof(uint16_t));		
	__sy_crc_append(sbuf,len);
	p=sbuf+len+2;
	*p= __nSY_FRAME_END_CHAR;
	len+=3;
	return len;	
}

//0x2006
uint16_t __sy_load_frame_ccid(uint8_t* sbuf,uint16_t ssize)
{
	uint8_t* p;
	uint16_t t16,len;

	__sy_frameHerder_t* sth=(__sy_frameHerder_t*)sbuf;
	
	//if(ssize<sizeof(__sy_frameHerder_t)+3+3)return 0;
	len=__sy_load_frame_header(sbuf,ssize,syCmdSn,0x82);
	p=sbuf+len;	
	
	__sy_fdfCcid_t* stb=(__sy_fdfCcid_t*)p;
	t16=__fdfSY_DFID_CCID;
	m_mem_cpy_len(stb->iden,(uint8_t*)&t16,sizeof(uint16_t));
	
	m_mem_cpy_len(stb->ccid,0,15);
	m_str_h2b(stb->ccid,ICCID,20);

	len=len+sizeof(__sy_fdfCcid_t);
	t16=len+3-2;
	
	m_mem_cpy_len(sth->len,(uint8_t*)&t16,sizeof(uint16_t));	
	
	__sy_crc_append(sbuf,len);
	p=sbuf+len+2;
	*p= __nSY_FRAME_END_CHAR;
	len+=3;
	return len;	
}

//0x3001
//0x3002
//0x3003
//0x3004

//0x5031
//0x5032
uint16_t __sy_load_frame_sever_setting(uint8_t* sbuf,uint16_t ssize,uint16_t ident)
{
	uint8_t* p;
	uint16_t t16,len;
	//uint8_t t8;
	__sy_frameHerder_t* sth=(__sy_frameHerder_t*)sbuf;
	
	if(ssize<sizeof(__sy_frameHerder_t)+3+3)return 0;
	len=__sy_load_frame_header(sbuf,ssize,syCmdSn,0x82);
	p=sbuf+len;	
	
	__sy_fdfServerSetting_t* stb=(__sy_fdfServerSetting_t*)p;
	if(ident==__fdfSY_DFID_MSTSEVER_IP){
		t16=__fdfSY_DFID_MSTSEVER_IP;
		m_mem_cpy_len(stb->iden,(uint8_t*)&t16,sizeof(uint16_t));
		
		//m_mem_cpy_len(stb->ip,sysData.masterSeverIP,6);

	}else{
		t16=__fdfSY_DFID_SLVSEVER_IP;
		m_mem_cpy_len(stb->iden,(uint8_t*)&t16,sizeof(uint16_t));
		
		//m_mem_cpy_len(stb->ip,sysData.slaverSeverIp,6);

	}
	m_mem_cpy_len((uint8_t*)&t16,stb->port,sizeof(uint16_t));
	t16=swap_uint16(t16);
	m_mem_cpy_len(stb->port,(uint8_t*)&t16,sizeof(uint16_t));
	

	len=len+sizeof(__sy_fdfServerSetting_t);
	t16=len+3-2;
	
	m_mem_cpy_len(sth->len,(uint8_t*)&t16,sizeof(uint16_t));	
	
	__sy_crc_append(sbuf,len);
	p=sbuf+len+2;
	*p= __nSY_FRAME_END_CHAR;
	len+=3;
	return len;	
}

//0x5033
uint16_t __sy_load_frame_reading_period(uint8_t* sbuf,uint16_t ssize)
{
	uint8_t* p;
	uint16_t t16,len;
	__sy_frameHerder_t* sth=(__sy_frameHerder_t*)sbuf;
	
	//if(ssize<sizeof(__sy_frameHerder_t)+3+3)return 0;
	len=__sy_load_frame_header(sbuf,ssize,syCmdSn,0x82);
	p=sbuf+len;
	
	__sy_fdfPopPeriod_t* stb=(__sy_fdfPopPeriod_t*)p;
	t16=__fdfSY_DFID_POP_PERIOD;
	m_mem_cpy_len(stb->iden,(uint8_t*)&t16,sizeof(uint16_t));	
	
//	stb->periodType=m_hex_2_bcd(sysData.rdPeriodType);
//	stb->periodValue=m_hex_2_bcd(sysData.rdPeriodValue);
//	
//	stb->dd=m_hex_2_bcd(sysData.ReadTime[0]);
//	stb->hh=m_hex_2_bcd(sysData.ReadTime[1]);
//	stb->mm=m_hex_2_bcd(sysData.ReadTime[2]);
	
	len=len+sizeof(__sy_fdfPopPeriod_t);
	t16=len+3-2;
	
	m_mem_cpy_len(sth->len,(uint8_t*)&t16,sizeof(uint16_t));	
	
	__sy_crc_append(sbuf,len);
	p=sbuf+len+2;
	*p= __nSY_FRAME_END_CHAR;
	len+=3;
	return len;		
}

//0x5034
uint16_t __sy_load_frame_apn(uint8_t* sbuf,uint16_t ssize)
{
	uint8_t* p;
	uint16_t t16,len;
	__sy_frameHerder_t* sth=(__sy_frameHerder_t*)sbuf;
	
	//if(ssize<sizeof(__sy_frameHerder_t)+3+3)return 0;
	len=__sy_load_frame_header(sbuf,ssize,syCmdSn,0x82);
	p=sbuf+len;
	
	__sy_fdfAPN_t* stb=(__sy_fdfAPN_t*)p;
	t16=__fdfSY_DFID_APN;
	m_mem_cpy_len(stb->iden,(uint8_t*)&t16,sizeof(uint16_t));	
	
	//m_mem_cpy_len(stb->apn,sysData.apn,30);
	
	len=len+sizeof(__sy_fdfAPN_t);
	t16=len+3-2;
	
	m_mem_cpy_len(sth->len,(uint8_t*)&t16,sizeof(uint16_t));	
	
	__sy_crc_append(sbuf,len);
	p=sbuf+len+2;
	*p= __nSY_FRAME_END_CHAR;
	len+=3;
	return len;		
}

//0x5035
uint16_t __sy_load_frame_noflow_timeout(uint8_t* sbuf,uint16_t ssize)
{
	uint8_t* p;
	uint16_t t16,len;
	__sy_frameHerder_t* sth=(__sy_frameHerder_t*)sbuf;
	
	//if(ssize<sizeof(__sy_frameHerder_t)+3+3)return 0;
	len=__sy_load_frame_header(sbuf,ssize,syCmdSn,0x82);
	p=sbuf+len;
	
	__sy_fdfNoFlowProtect_t* stb=(__sy_fdfNoFlowProtect_t*)p;
	t16=__fdfSY_DFID_NOFLOW_PROTECT;
	m_mem_cpy_len(stb->iden,(uint8_t*)&t16,sizeof(uint16_t));	
	
	//todo
//	stb->noFlowProtectEn=sysData.noFlowProtectEn;
//	t16=sysData.ConstNoFlowTimeOut/24;
	stb->timeOut=(uint8_t)t16;
	
	len=len+sizeof(__sy_fdfNoFlowProtect_t);
	t16=len+3-2;
	
	m_mem_cpy_len(sth->len,(uint8_t*)&t16,sizeof(uint16_t));	
	
	__sy_crc_append(sbuf,len);
	p=sbuf+len+2;
	*p= __nSY_FRAME_END_CHAR;
	len+=3;
	return len;		
}

//0x5036
uint16_t __sy_load_frame_commfail_timeout(uint8_t* sbuf,uint16_t ssize)
{
	uint8_t* p;
	uint16_t t16,len;
	__sy_frameHerder_t* sth=(__sy_frameHerder_t*)sbuf;
	
	//if(ssize<sizeof(__sy_frameHerder_t)+3+3)return 0;
	len=__sy_load_frame_header(sbuf,ssize,syCmdSn,0x82);
	p=sbuf+len;
	
	__sy_fdfCommFailProtect_t* stb=(__sy_fdfCommFailProtect_t*)p;
	t16=__fdfSY_DFID_COMMFAIL_PROTECT;
	m_mem_cpy_len(stb->iden,(uint8_t*)&t16,sizeof(uint16_t));	
	
	//todo
//	stb->commFailProtectEn=sysData.commFailProtectEn;
//	stb->timeOut=sysData.commFailProtectTimout;
	
	len=len+sizeof(__sy_fdfCommFailProtect_t);
	t16=len+3-2;
	
	m_mem_cpy_len(sth->len,(uint8_t*)&t16,sizeof(uint16_t));	
	
	__sy_crc_append(sbuf,len);
	p=sbuf+len+2;
	*p= __nSY_FRAME_END_CHAR;
	len+=3;
	return len;		
}

//0x6001
uint16_t __sy_load_frame_gas_log(uint8_t* sbuf,uint16_t ssize,uint32_t staTs,uint32_t endTs,uint8_t keycry)
{
	uint8_t* p;
	uint16_t t16,len;
	uint8_t partNb;
	__sy_frameHerder_t* sth=(__sy_frameHerder_t*)sbuf;
	
	//if(ssize<sizeof(__sy_frameHerder_t)+3+3)return 0;
	len=__sy_load_frame_header(sbuf,ssize,syCmdSn,0x82);
	p=sbuf+len;	
	
	t16=__fdfSY_DFID_GAS_LOG;
	m_mem_cpy_len(p,(uint8_t*)&t16,sizeof(uint16_t));
	p+=2;
	len+=2;
	
	if(endTs-staTs <= 24*3600)partNb=0;
	else if(endTs-staTs <= 30*24*3600 )partNb=1;
	else 
		partNb=2;
	t16=record_read_start_to_end(partNb,staTs,endTs,p,ssize-len);
	len+=t16;
	
	p=sbuf+sizeof(__sy_frameHerder_t)+2;
	t16=len-sizeof(__sy_frameHerder_t)-2;
	
	t16=__sy_crypt(p,t16,keycry);
	len=sizeof(__sy_frameHerder_t)+2+t16;	
	
	t16=len+3-2;
	m_mem_cpy_len(sth->len,(uint8_t*)&t16,sizeof(uint16_t));	
	
	__sy_crc_append(sbuf,len);
	p=sbuf+len+2;
	*p= __nSY_FRAME_END_CHAR;
	len+=3;
	return len;		
}

//0x6002
uint16_t __sy_load_frame_event_log(uint8_t* sbuf,uint16_t ssize,uint32_t staTs,uint32_t endTs)
{
	
	uint8_t* p;
	uint16_t t16,len;

	__sy_frameHerder_t* sth=(__sy_frameHerder_t*)sbuf;
	
	//if(ssize<sizeof(__sy_frameHerder_t)+3+3)return 0;
	len=__sy_load_frame_header(sbuf,ssize,syCmdSn,0x82);
	p=sbuf+len;	
	
	t16=__fdfSY_DFID_EVENT_LOG;
	m_mem_cpy_len(p,(uint8_t*)&t16,sizeof(uint16_t));
	p+=2;
	len+=2;
	

	t16=record_read_start_to_end(3,staTs,endTs,p,ssize-len);
	
	len+=t16;
	t16=len+3-2;
	m_mem_cpy_len(sth->len,(uint8_t*)&t16,sizeof(uint16_t));	
	
	__sy_crc_append(sbuf,len);
	p=sbuf+len+2;
	*p= __nSY_FRAME_END_CHAR;
	len+=3;
	return len;			
	
}

//一般应答
uint16_t __sy_load_frame_common_raply(uint8_t* sbuf,uint16_t ssize,uint16_t dataIden,uint8_t ret)
{
	uint8_t* p;
	uint16_t t16,len;
	__sy_frameHerder_t* sth=(__sy_frameHerder_t*)sbuf;
	
	if(ssize<sizeof(__sy_frameHerder_t)+3+3)return 0;
	len=__sy_load_frame_header(sbuf,ssize,syCmdSn,0x82);
	p=sbuf+len;	
	
	__sy_fdfComRet_t* stb=(__sy_fdfComRet_t*)p;
	t16=dataIden;
	m_mem_cpy_len(stb->iden,(uint8_t*)&t16,sizeof(uint16_t));
	
	stb->ret=ret;
	len+=3;
	t16=len+3-2;
	
	m_mem_cpy_len(sth->len,(uint8_t*)&t16,sizeof(uint16_t));	
	
	__sy_crc_append(sbuf,len);
	p=sbuf+len+2;
	*p= __nSY_FRAME_END_CHAR;
	len+=3;
	return len;	
}

uint8_t __sy_get_op(uint8_t* rbuf,uint16_t rlen)
{
	//写操作返回__bSY_CBOP_WRITE	1
	//读操作返回__bSY_CBOP_READ		0
	__sy_ctrlByteDef_t cb;
	__sy_frameHerder_t* sth=(__sy_frameHerder_t*)rbuf;
	cb.b=sth->ctrlByte;
	return cb.bits.bOp;
}

uint16_t __sy_exe_register(uint8_t* rbuf,uint16_t rlen)
{
	uint16_t t16;
	uint8_t* p;
	
	p=rbuf+sizeof(__sy_frameHerder_t);
	__sy_fdfRegisterDown_t* stb=(__sy_fdfRegisterDown_t*)p;
	
	keyCrypt=stb->keyCrypt;
	keyMac=stb->keyMac;
	
	t16=__sy_verify_mac(rbuf,rlen,keyMac);
	if(!t16){
		__sy_add_event_record(0);
		return 0;
	}
	
	if(stb->ret == __fdfSY_RET_REG_SUCCESS && sysData.DLCS == DLC_STATUS_B){
		//启用燃气表
		api_sysdata_save();
		osMutexWait(osMutexSysData,osWaitForever);
		m_flow_all_data_init();
		//data_api_calc_all_ex();
		osMutexRelease(osMutexSysData);
		
		api_calc_all();
		
		osMutexWait(osMutexSysData,osWaitForever);
		qc_data_realtime_data_clear();
		qc_data_device_even_lock_clear();	
		sysData.DLCS=DLC_STATUS_C;
		sysData.DWM=DWM_COMMON_MODE;
		osMutexRelease(osMutexSysData);
		
		api_sysdata_save();
	}
	
	if(stb->ret == __fdfSY_RET_REG_SUCCESS || stb->ret==__fdfSY_RET_REG_NO_ACCONTED){
		//写入时间
		__sy_set_rtc(stb->dt);
		
		return 1;
	}
	return 0;
}

uint16_t __sy_exe_whole_inf(uint8_t* rbuf,uint16_t rlen)
{
	//只读
	return 0;
}

uint16_t __sy_exe_run_sta(uint8_t* rbuf,uint16_t rlen)
{
	//只读
	
	return __sy_load_frame_run_status(frameBuf,sizeof(frameBuf));
}

uint16_t __sy_exe_setlle_sta(uint8_t* rbuf,uint16_t rlen)
{
	//读写
	uint16_t t16;
	uint8_t op;
	op=__sy_get_op(rbuf,rlen);
	__sy_fdfSetlleSta_t* stb=(__sy_fdfSetlleSta_t*)(rbuf+sizeof(__sy_frameHerder_t));
	if(op==__bSY_CBOP_WRITE){
		api_sysdata_save();
		//sysData.__syStellStatus=stb->setlleSta;
		api_sysdata_save();
		t16=__sy_load_frame_common_raply(frameBuf,sizeof(frameBuf),__fdfSY_DFID_SETLLE_STATUS,__SY_OP_RET_SUCCESS);
	}else{
		t16=__sy_load_frame_stell_status(frameBuf,sizeof(frameBuf));
	}
	return t16;
}

uint16_t __sy_exe_voltage(uint8_t* rbuf,uint16_t rlen)
{
	return __sy_load_frame_voltage(frameBuf,sizeof(frameBuf),voltBat);
	//return 0;
}

uint16_t __sy_exe_rtc(uint8_t* rbuf,uint16_t rlen)
{
	uint16_t t16;
	uint8_t op;
	op=__sy_get_op(rbuf,rlen);
	__sy_fdfRtc_t* stb=(__sy_fdfRtc_t*)(rbuf+sizeof(__sy_frameHerder_t));
	if(op==__bSY_CBOP_WRITE){
		//mac
		t16=__sy_verify_mac(rbuf,rlen,keyMac);
		if(!t16){
			__sy_add_event_record(0);
			return 0;
		}		
		__sy_set_rtc(stb->dt);
		t16=__sy_load_frame_common_raply(frameBuf,sizeof(frameBuf),__fdfSY_DFID_RTC,__SY_OP_RET_SUCCESS);		
	}else{
		t16=__sy_load_frame_rtc(frameBuf,sizeof(frameBuf));
	}
	return t16;
}

uint16_t __sy_exe_rssi(uint8_t* rbuf,uint16_t rlen)
{
	return __sy_load_frame_rssi(frameBuf,sizeof(frameBuf));
	//return 0;
}

uint16_t __sy_exe_total_vol(uint8_t* rbuf,uint16_t rlen)
{
	//return 0;
	return __sy_load_frame_total_volume(frameBuf,sizeof(frameBuf),keyCrypt,keyMac);
}

uint16_t __sy_exe_ccid(uint8_t* rbuf,uint16_t rlen)
{
	//return 0;
	return __sy_load_frame_ccid(frameBuf,sizeof(frameBuf));
}

uint16_t __sy_exe_valve_ctrl(uint8_t* rbuf,uint16_t rlen)
{
	uint16_t t16;
	__sy_fdfValveCtrl_t* stb=(__sy_fdfValveCtrl_t*)(rbuf+sizeof(__sy_frameHerder_t));
	//mac
	t16=__sy_verify_mac(rbuf,rlen,keyMac);
	if(!t16){
		__sy_add_event_record(0);
		return 0;
	}
	
	m_mem_cpy_len((uint8_t*)&t16,stb->iden,sizeof(uint16_t));
	if(t16==__fdfSY_DFID_OPEN_VALVE){
		qc_data_device_even_lock_clear();
		if(sysData.DLCS>=DLC_STATUS_C)sysData.DLCS=DLC_STATUS_C;
		api_sysdata_save();		
		if(vavleState==VALVE_ON || vavleState==VALVE_OPERATION_ON){
			t16=__sy_load_frame_common_raply(frameBuf,sizeof(frameBuf),t16,__SY_OP_RET_ERROR);			
		}else{
			if((void*)vTheadEvenID){
				osSignalSet( vTheadEvenID, flg_EVENT_UNLOCK_ON);
			}
			t16=__sy_load_frame_common_raply(frameBuf,sizeof(frameBuf),t16,__SY_OP_RET_SUCCESS);
		}	
	}else if(t16== __fdfSY_DFID_FORCE_CLOSE){
		//sysData.lockReason.bits.bSeverOff=1;
		sysData.lockReason.bits.bSeverOff=1;
		vavle_off_from_app(OFF_REASON_SEVERS);	
		t16=__sy_load_frame_common_raply(frameBuf,sizeof(frameBuf),t16,__SY_OP_RET_SUCCESS);			
	}else{
		if(vavleState==VALVE_OFF || vavleState==VALVE_OPERATION_OFF){
			t16=__sy_load_frame_common_raply(frameBuf,sizeof(frameBuf),t16,__SY_OP_RET_ERROR);
		}else{
			vavle_off_from_app(OFF_REASON_SEVERS);	
			t16=__sy_load_frame_common_raply(frameBuf,sizeof(frameBuf),t16,__SY_OP_RET_SUCCESS);
		}				
	}
	return t16;
}

uint16_t __sy_exe_end_frame(uint8_t* rbuf,uint16_t rlen)
{
	uint16_t t16;
	__sy_fdfEnd_t* stb;
	t16=__sy_verify_mac(rbuf,rlen,keyMac);
	if(!t16){
		__sy_add_event_record(0);
		return 0;
	}
	stb=(__sy_fdfEnd_t*)(rbuf+sizeof(__sy_frameHerder_t));
	
	__sy_set_rtc(stb->dt);
	syCommStatus=__sy_COMMUNICATION_STATUS_END;
	return 1;	
}

uint16_t __sy_exe_sever_ip(uint8_t* rbuf,uint16_t rlen)
{
	uint16_t t16,ret;
	uint8_t op,t8;
	op=__sy_get_op(rbuf,rlen);	
	__sy_fdfServerSetting_t* stb=(__sy_fdfServerSetting_t*)(rbuf+sizeof(__sy_frameHerder_t));
	
	m_mem_cpy_len((uint8_t*)&t16,stb->iden,sizeof(uint16_t));
	
	if(op==__bSY_CBOP_WRITE){
		ret=__sy_verify_mac(rbuf,rlen,keyMac);
		if(!ret){
			__sy_add_event_record(0);
			return 0;
		}
		api_sysdata_save();
		osMutexWait(osMutexSysData,osWaitForever);
		
		if(t16==__fdfSY_DFID_MSTSEVER_IP){
			//m_mem_cpy_len(sysData.masterSeverIP,stb->ip,6);	
			//swap
			//t8=sysData.masterSeverIP[5];
			//sysData.masterSeverIP[5]=sysData.masterSeverIP[4];
			//sysData.masterSeverIP[4]=t8;
		}else{
			//m_mem_cpy_len(sysData.slaverSeverIp,stb->ip,6);	
			//swap
			//t8=sysData.slaverSeverIp[5];
			//sysData.slaverSeverIp[5]=sysData.slaverSeverIp[4];
			//sysData.slaverSeverIp[4]=t8;			
		}
		
		osMutexRelease(osMutexSysData);
		api_sysdata_save();
		ret=__sy_load_frame_common_raply(frameBuf,sizeof(frameBuf),t16,__SY_OP_RET_SUCCESS);		
	}else{
		ret= __sy_load_frame_sever_setting(frameBuf,sizeof(frameBuf),t16);
	}
	return ret;
}

uint16_t __sy_exe_pop_period(uint8_t* rbuf,uint16_t rlen)
{
	uint16_t t16;
	uint8_t op;
	op=__sy_get_op(rbuf,rlen);
	__sy_fdfPopPeriod_t* stb=(__sy_fdfPopPeriod_t*)(rbuf+sizeof(__sy_frameHerder_t));
	if(op==__bSY_CBOP_WRITE){
		//mac
		t16=__sy_verify_mac(rbuf,rlen,keyMac);
		if(!t16){
			__sy_add_event_record(0);
			return 0;
		}		
		api_sysdata_save();
		osMutexWait(osMutexSysData,osWaitForever);
		
//		sysData.rdPeriodType=stb->periodType;
//		sysData.rdPeriodValue=stb->periodValue;
//		sysData.ReadTime[0]=stb->dd;
//		sysData.ReadTime[1]=stb->hh;
//		sysData.ReadTime[2]=stb->mm;
//		
		osMutexRelease(osMutexSysData);
		api_sysdata_save();	
		t16=__sy_load_frame_common_raply(frameBuf,sizeof(frameBuf),__fdfSY_DFID_POP_PERIOD,__SY_OP_RET_SUCCESS);		
	}else{
		t16=__sy_load_frame_reading_period(frameBuf,sizeof(frameBuf));
	}
	return t16;
}

uint16_t __sy_exe_apn(uint8_t* rbuf,uint16_t rlen)
{
	uint16_t t16;
	uint8_t op;
	op=__sy_get_op(rbuf,rlen);
	__sy_fdfAPN_t* stb=(__sy_fdfAPN_t*)(rbuf+sizeof(__sy_frameHerder_t));
	if(op==__bSY_CBOP_WRITE){
		//mac
		t16=__sy_verify_mac(rbuf,rlen,keyMac);
		if(!t16){
			__sy_add_event_record(0);
			return 0;
		}		
		api_sysdata_save();
		osMutexWait(osMutexSysData,osWaitForever);
		//m_mem_cpy_len(sysData.apn,stb->apn,30);
		
		osMutexRelease(osMutexSysData);
		api_sysdata_save();		
		t16=__sy_load_frame_common_raply(frameBuf,sizeof(frameBuf),__fdfSY_DFID_APN,__SY_OP_RET_SUCCESS);		
	}else{
		t16=__sy_load_frame_apn(frameBuf,sizeof(frameBuf));
	}
	return t16;	

}

uint16_t __sy_exe_noflow(uint8_t* rbuf,uint16_t rlen)
{
	uint16_t t16;
	uint8_t op;
	uint16_t tm;
	op=__sy_get_op(rbuf,rlen);
	__sy_fdfNoFlowProtect_t* stb=(__sy_fdfNoFlowProtect_t*)(rbuf+sizeof(__sy_frameHerder_t));
	if(op==__bSY_CBOP_WRITE){
		//mac
		t16=__sy_verify_mac(rbuf,rlen,keyMac);
		if(!t16){
			__sy_add_event_record(0);
			return 0;
		}		
		api_sysdata_save();
		osMutexWait(osMutexSysData,osWaitForever);

//		sysData.noFlowProtectEn=stb->noFlowProtectEn;
//		tm=stb->timeOut;
//		tm*=(24);
//		sysData.ConstNoFlowTimeOut=tm;
//		sysData.ConstNoFlowTimeOutEx=0x00;
		
		osMutexRelease(osMutexSysData);
		api_sysdata_save();		
		t16=__sy_load_frame_common_raply(frameBuf,sizeof(frameBuf),__fdfSY_DFID_NOFLOW_PROTECT,__SY_OP_RET_SUCCESS);		
	}else{
		t16=__sy_load_frame_noflow_timeout(frameBuf,sizeof(frameBuf));
	}
	return t16;	
}

uint16_t __sy_exe_commfail(uint8_t* rbuf,uint16_t rlen)
{
	uint16_t t16;
	uint8_t op;
	op=__sy_get_op(rbuf,rlen);
	__sy_fdfCommFailProtect_t* stb=(__sy_fdfCommFailProtect_t*)(rbuf+sizeof(__sy_frameHerder_t));
	if(op==__bSY_CBOP_WRITE){
		//mac
		t16=__sy_verify_mac(rbuf,rlen,keyMac);
		if(!t16){
			__sy_add_event_record(0);
			return 0;
		}		
		api_sysdata_save();
		osMutexWait(osMutexSysData,osWaitForever);
		
//		sysData.commFailProtectEn=stb->commFailProtectEn;
//		sysData.commFailProtectTimout=stb->timeOut;
		
		osMutexRelease(osMutexSysData);
		api_sysdata_save();		
		t16=__sy_load_frame_common_raply(frameBuf,sizeof(frameBuf),__fdfSY_DFID_COMMFAIL_PROTECT,__SY_OP_RET_SUCCESS);		
	}else{
		t16=__sy_load_frame_commfail_timeout(frameBuf,sizeof(frameBuf));
	}
	return t16;	
}

uint16_t __sy_exe_event_log(uint8_t* rbuf,uint16_t rlen)
{
	uint8_t buf[8];
	uint16_t t16;
	uint32_t tsSta,tsEnd;
	__sy_fdfEventLogUp_t* stb=(__sy_fdfEventLogUp_t*)(rbuf+sizeof(__sy_frameHerder_t));
	
	m_mem_set(buf,0,sizeof(buf));
	m_mem_cpy_len(buf,stb->startDt,5);
	tsSta=__sy_dt_t0_ts(buf);
	
	
	m_mem_set(buf,0,sizeof(buf));
	m_mem_cpy_len(buf,stb->endDt,5);
	tsEnd=__sy_dt_t0_ts(buf);
	
	t16=__sy_load_frame_event_log(frameBuf,sizeof(frameBuf)-19,tsSta,tsEnd);
	return t16;
}

uint16_t __sy_exe_gas_log(uint8_t* rbuf,uint16_t rlen)
{
	uint8_t buf[8];
	uint16_t t16;
	uint32_t tsSta,tsEnd;
	__sy_fdfGasLog_t* stb=(__sy_fdfGasLog_t*)(rbuf+sizeof(__sy_frameHerder_t));
	
	m_mem_set(buf,0,sizeof(buf));
	m_mem_cpy_len(buf,stb->startDt,5);
	tsSta=__sy_dt_t0_ts(buf);
	
	
	m_mem_set(buf,0,sizeof(buf));
	m_mem_cpy_len(buf,stb->endDt,5);
	tsEnd=__sy_dt_t0_ts(buf);
	
	t16=__sy_load_frame_gas_log(frameBuf,sizeof(frameBuf)-19,tsSta,tsEnd,keyCrypt);
	
	return t16;
}

uint16_t __sy_comm_received_process(uint8_t* rbuf,uint16_t rlen)
{
	uint8_t* p;
	uint16_t t16,cmdIdent;
	__sy_frameHerder_t* sth;
	if(rlen<sizeof(__sy_frameHerder_t)+2+3)return 0;
	
	t16=__sy_crc_verify(rbuf,rlen);
	if(!t16)return 0;
	
	sth=(__sy_frameHerder_t*)sth;
	if(sth->startChar!=__nSY_FRAME_START_CHAR)return 0;
	
	t16=m_str_cmp_len(sth->syID,__syID,8);
	if(!t16)return 0;
	
	p=rbuf+sizeof(__sy_frameHerder_t);
	m_mem_cpy_len((uint8_t*)&cmdIdent,p,sizeof(uint16_t));
	t16=0;
	switch(cmdIdent){
		case __fdfSY_DFID_REGISTER:			t16=__sy_exe_register(rbuf,rlen);	break;
		case __fdfSY_DFID_WHOLE_INF:		t16=__sy_exe_whole_inf(rbuf,rlen);	break;
		case __fdfSY_DFID_RUN_STATUS:		t16=__sy_exe_run_sta(rbuf,rlen);	break;
		case __fdfSY_DFID_SETLLE_STATUS:	t16=__sy_exe_setlle_sta(rbuf,rlen);	break;
		case __fdfSY_DFID_VOLTAGE:			t16=__sy_exe_voltage(rbuf,rlen);	break;
		case __fdfSY_DFID_RTC:				t16=__sy_exe_rtc(rbuf,rlen);		break;
		case __fdfSY_DFID_RSSI:				t16=__sy_exe_rssi(rbuf,rlen);		break;
		case __fdfSY_DFID_TOTAL_VOL:		t16=__sy_exe_total_vol(rbuf,rlen);	break;
		case __fdfSY_DFID_CCID:				t16=__sy_exe_ccid(rbuf,rlen);		break;
		case __fdfSY_DFID_OPEN_VALVE:	
		case __fdfSY_DFID_FORCE_CLOSE:
		case __fdfSY_DFID_CLOSE_VALVE:		t16=__sy_exe_valve_ctrl(rbuf,rlen);	break;
		case __fdfSY_DFID_END_FRAME:		t16=__sy_exe_end_frame(rbuf,rlen);	break;
		case __fdfSY_DFID_MSTSEVER_IP:
		case __fdfSY_DFID_SLVSEVER_IP:		t16=__sy_exe_sever_ip(rbuf,rlen);	break;
		case __fdfSY_DFID_POP_PERIOD:		t16=__sy_exe_pop_period(rbuf,rlen);	break;
		case __fdfSY_DFID_APN:				t16=__sy_exe_apn(rbuf,rlen);		break;
		case __fdfSY_DFID_NOFLOW_PROTECT:	t16=__sy_exe_noflow(rbuf,rlen);		break;
		case __fdfSY_DFID_COMMFAIL_PROTECT:	t16=__sy_exe_commfail(rbuf,rlen);	break;
		case __fdfSY_DFID_EVENT_LOG:		t16=__sy_exe_event_log(rbuf,rlen);	break;
		case __fdfSY_DFID_GAS_LOG:			t16=__sy_exe_gas_log(rbuf,rlen);	break;
		default:
			break;
	}
	
	return t16;
}

uint16_t __sy_test(void)
{
	uint16_t t16;
	uint8_t buf[128]; 
	//esam_apl_setup();
	__nop();
	t16=__sy_load_frame_register(buf,sizeof(buf),1,1);
	__nop();
	//esam_apl_remove();
	return t16;
}
//system_dt_to_time_stamp
//file 
