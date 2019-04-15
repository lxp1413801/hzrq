#include "./includes/includes.h"
volatile uint8_t __hzrqMsgSendSn=0x00;
volatile uint8_t __hzrqMid=0xff;
//volatile int16_t hzrqCommFailTimes=0x00;
volatile int16_t hzrqRsrp=0;
volatile int16_t hzrqSnr=0;
volatile int8_t hzrqEclLevel=0;
uint8_t hzrqCellId[6]={0};
volatile uint16_t hzrqRealNearFcn=0x00;
uint8_t hzrqNbModuleType[10]={0};
uint8_t hzrqNbModuleSoftVer[20]={0};

uint8_t hzrqComBuf[256];

volatile uint16_t __hzrqEventCode=0x00;
volatile uint16_t __hzrqErrorCode=0x00;
volatile uint16_t __hzrqUnSendNum=0x00;

#define __hzrq_MAC_LEN 32
#define __hzrq_DFID_LEN 2

uint16_t __hzrq_crc_sum(uint8_t* puchMsg,uint16_t usDataLen)
{  
	uint16_t wCRCin = 0x0000;  
	uint16_t wCPoly = 0x1021;  
	uint8_t wChar = 0;  

	while (usDataLen--)     
	{  
		wChar = *(puchMsg++);  
		wCRCin ^= (wChar << 8);  
		for(int i = 0;i < 8;i++)  
		{  
			if(wCRCin & 0x8000)  
				wCRCin = (wCRCin << 1) ^ wCPoly;  
			else  
				wCRCin = wCRCin << 1;  
		}  
	}  
	return (wCRCin) ;  
}

uint16_t __hzrq_crc_verify(uint8_t* message, uint16_t n)
{

    
    uint16_t expected,crccal;
	//m_mem_cpy_len((uint8_t*)(&expected),message+n-2,2);
	expected=message[n-2];
	expected<<=8;
	expected |= message[n-1];
    crccal=__hzrq_crc_sum(message,n - 2);
	if(expected!=crccal)return 0;
	return 1;	
}

void __hzrq_crc_append(uint8_t* message, uint16_t n)
{
    uint16_t crc;
    crc = __hzrq_crc_sum(message, n);
    message[n+1] = (uint8_t)(crc & 0xff);
    message[n] = (uint8_t)((crc >> 8) & 0xff);
}

//hatq api
void __hzrq_load_rtc(uint8_t* buf)
{
	sysDataTime_t dt={0};
	m_rtc_get(&dt);
	buf[0]=dt.YY;
	buf[1]=dt.MM;
	buf[2]=dt.DD;
	buf[3]=dt.hh;
	buf[4]=dt.mm;
	buf[5]=dt.ss;
}

uint32_t __hzrq_download_rtc(uint8_t* dtbuf)
{

	uint32_t t32;
	sysDataTime_t dt={0};
	dt.YYhi=0x20;
	dt.YY=dtbuf[0];
	dt.MM=dtbuf[1];
	dt.DD=dtbuf[2];
	dt.hh=dtbuf[3];
	dt.mm=dtbuf[4];
	dt.ss=dtbuf[5];
	if(dtbuf[0]==0xff)return ~0x00UL;
	t32=system_dt_to_time_stamp(&dt);
	return t32;
}

/*
void __hzrq_load_dfid(uint8_t* buf,uint16_t dfid)
{
	buf[0]=(uint8_t)(dfid>>0x08);
	buf[1]=(uint8_t)(dfid&0xff);
}

uint16_t __hzrq_swap_t16(uint16_t x)
{
	uint16_t t16=(x&0x00ff)<<8;
	t16+=(x&0xff00)>>8;
    return t16;   
}
*/

void __hzrq_swap_load_t16(uint8_t* dest,uint16_t x)
{
	dest[0]=(uint8_t)(x>>0x08);
	dest[1]=(uint8_t)(x&0xff);
}

void __hzrq_swap_load_t32(uint8_t* dest,uint32_t x)
{
	uint32_t t32=x;
	uint8_t* pSrc=(uint8_t*)&t32;
	dest[0]=pSrc[3];
	dest[1]=pSrc[2];
	dest[2]=pSrc[1];
	dest[3]=pSrc[0];
	//return 
}

int16_t __hzrq_swap_get_t16(uint8_t* src)
{
	uint16_t ret=src[0];
	ret<<=8;
	ret+=src[1];
	return (int16_t)ret;
}

uint32_t __hzrq_swap_get_t32(uint8_t* src)
{
	int32_t t32=0L;
	uint8_t* pSrc=(uint8_t*)&t32;
	pSrc[3]=src[0];
	pSrc[2]=src[1];
	pSrc[1]=src[2];
	pSrc[0]=src[3];
	return t32;
}

uint16_t __hzrq_gas_log_format(__hzrq_gasLog_t* hzrqGasLog,consumeLog_t* commGasLog)
{
	uint32_t t32=commGasLog->volume;
	t32*=10;
	__hzrq_swap_load_t32(hzrqGasLog->volume,t32);
	return sizeof(__hzrq_gasLog_t);
}

uint16_t __hzrq_event_log_format(__hzrq_eventLog_t* hzrqEventLog,eventLog_t* commEventLog)
{
	uint32_t t32;
	uint16_t t16;
	//uint8_t buf[16];
	sysDataTime_t dt={0};
	t32=commEventLog->ts;
	time_stamp_to_system_dt(t32,&dt);
	//YYMMDDhhmmss
	hzrqEventLog->dt[0]=dt.YY;
	hzrqEventLog->dt[1]=dt.MM;
	hzrqEventLog->dt[2]=dt.DD;
	hzrqEventLog->dt[3]=dt.hh;
	hzrqEventLog->dt[4]=dt.mm;	
	hzrqEventLog->dt[5]=dt.ss;	
	
	t16=commEventLog->eventCode;
	//m_mem_cpy_len(syEventLog->eventType,(uint8_t*)&t16,sizeof(uint16_t));
	__hzrq_swap_load_t16(hzrqEventLog->eventType,t16);
	//hzrqEventLog->times[0]=0x00;
	//hzrqEventLog->times[1]=0x01;
	
	hzrqEventLog->xx=0x00;
	return sizeof(__hzrq_eventLog_t);
}


uint16_t __hzrq_pkcs7padding(uint8_t* buf,uint16_t bufLen,uint16_t kcpsLen)
{
	uint16_t i,t16,retlen;
	t16=kcpsLen-(bufLen % kcpsLen);
	retlen=bufLen+t16;
	for(i=bufLen;i<retlen;i++){
		buf[i]=t16;
	}
	return retlen;
}

void __hzrq_get_encrypt_key(uint8_t* encryptKey,uint8_t* random, uint16_t randomLen)
{
	//“会话加密密钥”由用“主密钥”对注册数据中的“通信随机码”进行HMAC-SHA256计算得到。
	//用于加解密通信数据。本协议支持最大密钥长度为32字节。
	uint8_t buf[64];
	uint8_t out[64];	
	//uint16_t len;
	m_mem_cpy_len(buf,random,randomLen);
	hmac_sha256(out,buf,randomLen,sysData.key,sysData.hzrqKeyLen);
	
	m_mem_cpy_len(encryptKey,out,16);
}

void __hzrq_get_mac_key(uint8_t* macKey,uint8_t* random,uint16_t randomLen)
{
	//“会话MAC密钥”由用“主密钥”对注册数据中的“通信随机码”进行加密结果的前16字节。
	//用于对通信数据进行MAC计算。
	uint8_t buf[64];
	uint8_t out[64];
	uint16_t len;
	m_mem_cpy_len(buf,random,16);//sysData.key,sysData.hzrqKeyLen);
	len=__hzrq_pkcs7padding(buf,16,16);
	m_ebc_encrypt(out,buf,sysData.key,len);
	m_mem_cpy_len(macKey,out,16);
}

uint16_t __hzrq_load_frame_mod_len(uint8_t* sbuf,uint16_t len)
{
	__hzrq_frameHerder_t* sth=(__hzrq_frameHerder_t*)sbuf;
	__hzrq_swap_load_t16(sth->frameLen,len);
	return len;
}

uint16_t __hzrq_load_frame_mod_hasmore_mid(uint8_t* sbuf,uint16_t len,uint8_t hasMore,uint8_t mid)
{
	__hzrq_frameHerder_t* sth=(__hzrq_frameHerder_t*)sbuf;
	__hzrq_ctrlByteDef_t cb;
	cb.b=sth->ctrlByte;
	cb.bits.bhasMore=hasMore;
	sth->ctrlByte=cb.b;
	sth->msgIndex=__hzrqMid;
	
	return len;
}


uint16_t __hzrq_load_frame_mod_mid(uint8_t* sbuf,uint16_t len,uint8_t hasMore)
{
	__hzrq_frameHerder_t* sth=(__hzrq_frameHerder_t*)sbuf;
	__hzrq_ctrlByteDef_t cb;
	cb.b=sth->ctrlByte;
	cb.bits.bhasMore=hasMore;
	sth->ctrlByte=cb.b;
	return len;
}
uint16_t _hzrq_crc_append_send(uint8_t* sbuf,uint16_t slen)
{
	__hzrq_crc_append(sbuf+sizeof(__hzrq_frameHerder_t)-2,slen- (sizeof(__hzrq_frameHerder_t)-2) -3 );
	return slen;
}

void _hzrq_load_device_sta(uint8_t* buf)
{
	__hzrq_deviceStaByteDef_t sta={0};
	
	buf[0]=0;
	buf[1]=0;
	if(vavleState==VALVE_ON || vavleState==VALVE_OPERATION_ON){
		sta.bits.bVavleStaFiOpen=1;
	}
	if(sysData.lockReason.t32 && vavleState==VALVE_OFF){
		sta.bits.bVavleStaFiLock=1;
	}
	
	if(sysData.devStatus.bits.bPwrStatus==POWER_STATUS_LOW || sysData.devStatus.bits.bPwrStatus==POWER_STATUS_DOWN){
		sta.bits.bBatPowerLow=1;
	}
	if(sysData.devStatus.bits.bQcFlowHi){
		sta.bits.bFlowOver=1;
	}
	if(sysData.devStatus.bits.bValveStatus==VALVE_FAULT){
		sta.bits.bVavleErr=1;
	}
	if(sysData.devStatus.bits.bLeakage){
		sta.bits.bAlarm=1;
	}
	if(sysData.devStatus.bits.bQcFlowTmout){
		sta.bits.bNoFlowTimeOut=1;
	}
	if(sysData.devStatus.bits.bNoNetWork){
		sta.bits.bNoConnectTimeOut=1;
	}
	if(sysData.devStatus.bits.bStrongMagnetic){
		sta.bits.bSte=1;
	}
	//__hzrq_swap_load_t16(buf,sta.t16);
	buf[0]=(uint8_t)(sta.t16 &  0xff);
	buf[1]=(uint8_t)(sta.t16 >> 8);
}

uint16_t _hzrq_load_frame_header(uint8_t* sbuf,uint16_t ssize,uint16_t len,uint8_t cb,uint8_t mid)
{
	__hzrq_frameHerder_t* sth=(__hzrq_frameHerder_t*)sbuf;
	sth->startChar=__hzrq_FRAME_START_CHAR;
	sth->protocolType=__hzrq_PROTOCOL_TYPE_DEFAULT;
	sth->protocolFrameVer=__hzrq_PROTOCOL_FRAME_VER_DEFAULT;
	sth->frameLen[0]=(uint8_t)(len>>8);
	sth->frameLen[1]=(uint8_t)(len&0x00ff);
	sth->msgIndex=mid;
	//__hzrqMsgSendSn++;
	sth->ctrlByte=cb;
	
	return sizeof(__hzrq_frameHerder_t);
}

uint16_t _hzrq_load_frame_mac(uint8_t* sbuf,uint16_t len)
{
	//调用此函数必须填充正确的帧长度
	uint16_t t16;
	uint8_t tmpBuf[32];
	uint16_t frmLen;
	__hzrq_frameHerder_t* sth=(__hzrq_frameHerder_t*)sbuf;
	frmLen=__hzrq_swap_get_t16(sth->frameLen);	
	if(frmLen!=len){
		return 0;
	}
	
	t16=m_mem_cpy_len_ex(hzrqComBuf,sysData.hzrqRandomCode,16);
	t16+=m_mem_cpy_len_ex(hzrqComBuf+16,sbuf+sizeof(__hzrq_frameHerder_t)+2,frmLen-(sizeof(__hzrq_frameHerder_t)+2)-3);
	uint8_t macKey[16];
	__hzrq_get_mac_key(macKey,sysData.hzrqRandomCode,16);
	hmac_sha256(tmpBuf,hzrqComBuf,t16,macKey,16);
	m_mem_cpy_len(sbuf+frmLen-3,tmpBuf,sizeof(tmpBuf));	
	
	frmLen=frmLen+32;
	__hzrq_load_frame_mod_len(sbuf,frmLen);
	sbuf[frmLen-1]=__hzrq_FRAME_END_CHAR;
	return frmLen;
}	

uint16_t _hzrq_load_frame_encrypt(uint8_t* sbuf,uint16_t len)
{
	//调用此函数必须填充原始的正确正确的帧长度
	uint16_t t16;
	uint16_t frmLen;
	__hzrq_frameHerder_t* sth=(__hzrq_frameHerder_t*)sbuf;
	frmLen=__hzrq_swap_get_t16(sth->frameLen);	
	if(frmLen!=len){
		return 0;
	}	
	t16=frmLen-(sizeof(__hzrq_frameHerder_t)+2)-3;
	t16=__hzrq_pkcs7padding(sbuf+sizeof(__hzrq_frameHerder_t)+2,t16,16);
	uint8_t encryKey[16];
	__hzrq_get_encrypt_key(encryKey,sysData.hzrqRandomCode,16);	
	
	m_ebc_encrypt(hzrqComBuf,sbuf+sizeof(__hzrq_frameHerder_t)+2,encryKey,t16);
	m_mem_cpy_len(sbuf+sizeof(__hzrq_frameHerder_t)+2,hzrqComBuf,t16);	
	frmLen=sizeof(__hzrq_frameHerder_t)+2+t16+3;
	
	__hzrq_load_frame_mod_len(sbuf,frmLen);
	sbuf[frmLen-1]=__hzrq_FRAME_END_CHAR;
	return frmLen;	
}

uint16_t _hzrq_load_frame_encrypt_mac(uint8_t* sbuf,uint16_t len)
{
	//uint16_t t16;
	uint16_t frmLen;
	__hzrq_frameHerder_t* sth=(__hzrq_frameHerder_t*)sbuf;
	frmLen=__hzrq_swap_get_t16(sth->frameLen);	
	if(frmLen!=len){
		return 0;
	}	
	frmLen=_hzrq_load_frame_encrypt(sbuf,frmLen);
	frmLen=_hzrq_load_frame_mac(sbuf,frmLen);
	
	__hzrq_load_frame_mod_len(sbuf,frmLen);
	sbuf[frmLen-1]=__hzrq_FRAME_END_CHAR;
	return frmLen;		
}

uint16_t _hzrq_load_frame_crc_append(uint8_t* sbuf,uint16_t len)
{
	//调用此函数必须填充正确的帧长度
	//uint16_t t16;
	uint16_t frmLen;
	__hzrq_frameHerder_t* sth=(__hzrq_frameHerder_t*)sbuf;
	frmLen=__hzrq_swap_get_t16(sth->frameLen);	
	if(frmLen!=len){
		return 0;
	}
	__hzrq_crc_append(sbuf+sizeof(__hzrq_frameHerder_t)-2,frmLen- (sizeof(__hzrq_frameHerder_t)-2) -3 );
	sbuf[frmLen-1]=__hzrq_FRAME_END_CHAR;
	return frmLen;
}

uint16_t _hzrq_load_frame_mod_send_index(uint8_t* sbuf,uint16_t len)
{
	uint16_t frmLen,ret;
	__hzrq_frameHerder_t* sth=(__hzrq_frameHerder_t*)sbuf;
	frmLen=__hzrq_swap_get_t16(sth->frameLen);	
	if(frmLen!=len){
		return 0;
	}	
	sth->msgIndex=__hzrqMsgSendSn;
	__hzrqMsgSendSn++;
	ret=_hzrq_load_frame_crc_append(sbuf,len);
	return ret;
}

uint8_t _hzrq_get_func_code(uint8_t* buf)
{
	__hzrq_ctrlByteDef_t cb;
	__hzrq_frameHerder_t* sth=(__hzrq_frameHerder_t*)buf;
	cb.b=sth->ctrlByte;
	return cb.bits.bFuncCode;
}

uint16_t hzrq_load_rw_common_Reply(uint8_t* sbuf,uint16_t ssize,uint8_t cb,uint16_t dfid,
	uint16_t retValue,bool encrypt,bool mac)
{
	uint16_t len,t16;
	uint8_t* p;	
	len=_hzrq_load_frame_header(sbuf,ssize,0,cb,__hzrqMid);	
	__hzrq_dfdCommonReply_t* stb=(__hzrq_dfdCommonReply_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
	__hzrq_swap_load_t16(stb->iden,dfid);	
	__hzrq_swap_load_t16(stb->retCode,retValue);	
	
	len+=sizeof(__hzrq_dfdCommonReply_t);
	len+=3;	
	len=__hzrq_load_frame_mod_len(sbuf,len);
	if(encrypt){
		len=_hzrq_load_frame_encrypt(sbuf,len);
	}
	if(mac){
		len=_hzrq_load_frame_mac(sbuf,len);
	}
	
	len=_hzrq_load_frame_crc_append(sbuf,len);
	return len;		
}
//明文+mac
uint16_t hzrq_load_register_frame(uint8_t* sbuf,uint16_t ssize)
{
	uint16_t len;
	//uint8_t tmpBuf[32];
	//uint8_t* p;
	
	len=_hzrq_load_frame_header(sbuf,ssize,0,0x01,__hzrqMsgSendSn);
	//__hzrqMsgSendSn++;
	__hzrq_dfdRegisterReq_t* stb=(__hzrq_dfdRegisterReq_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
	

	__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_REGISTER);
	__hzrq_load_rtc(stb->dt);
	__hzrq_swap_load_t16(stb->venderCode,sysData.vendorCode);
	__hzrq_swap_load_t16(stb->deviceType,sysData.hzrqDeviceType);
	
	stb->hazqMeterID[0]=sysData.hzrqMeterIdLen;
	m_mem_cpy_len(stb->hazqMeterID+1,sysData.hzrqMeterId,32);
	stb->accountSta=sysData.hzrqAccountSta;
	stb->mobleSeverInf=sysData.hzrqMobleSeverInf;
	stb->commWay=sysData.hzrqCommWay;
	
	m_mem_cpy_len(stb->appSoftVer,sysData.hzrqAppSwVer,4);
	m_mem_cpy_len(stb->appProtocolVer,sysData.hzrqAppProtocolVer,2);
	m_mem_cpy_len(stb->randomCode,sysData.hzrqRandomCode,16);
	
	__hzrq_swap_load_t16(stb->rsrp,hzrqRsrp);
	__hzrq_swap_load_t16(stb->snr,hzrqSnr);
	
	stb->eclLevel=hzrqEclLevel;
	m_mem_cpy_len(stb->cellID,hzrqCellId,6);
	__hzrq_swap_load_t16(stb->realNearfcn,hzrqRealNearFcn);
	
	m_mem_cpy_len(stb->imei,CGSN,15);

	
	m_mem_cpy_len(stb->nbModuleType,hzrqNbModuleType,10);
	m_mem_cpy_len(stb->nbModuleSoftVer,hzrqNbModuleSoftVer,20);
	stb->keyVer=sysData.hzrqKeyVer;
	
	len+=sizeof(__hzrq_dfdRegisterReq_t);
	len+=3;
	
	
	//p=sbuf+len;
	len=__hzrq_load_frame_mod_len(sbuf,len);
	len=_hzrq_load_frame_mac(sbuf,len);
	len=_hzrq_load_frame_crc_append(sbuf,len);
	return len;
}
#define SECS_ON_DAY (3600UL*24UL)
uint16_t __hzrq_load_pop_get_hour_log(uint8_t* logbuf,uint16_t logbufLen,uint32_t ts)
{
	uint8_t buf[32];
	uint16_t loc,t16,partStartAddr,limitsItem,bodyItemAddr,ItemSize;
	uint32_t t32=0UL;
	exEepromPartDesc_t* pDesc;
	consumeLog_t*	pVlog;
	
	m_mem_set(logbuf,0,logbufLen);
	ts=ts/SECS_ON_DAY;
	if(ts<1)return 0;
	ts--;
	ts=ts*SECS_ON_DAY;
	
	//从写入的位置-1开始想向前查找,从buf的最后端开始写入
	ex_data_get_part_inf(PART_SN_CONSTLOG_HOUR,buf,sizeof(exEepromPartDesc_t));
	t16=crc_verify(buf,sizeof(exEepromPartDesc_t));
	if(!t16)return 0;	

	pDesc=(exEepromPartDesc_t*)buf;

	ItemSize=pDesc->recordSize;
	limitsItem=pDesc->partSize/ItemSize;
	partStartAddr=pDesc->partStartAddr;	
	
	loc=pDesc->recordWriteLoc;
	loc+=limitsItem;loc--;loc%=limitsItem;
	do{
		bodyItemAddr=partStartAddr+loc*ItemSize;
		ex_eeprom_read(SLV_ADDR,bodyItemAddr,buf,ItemSize);
		t16=crc_verify(buf,ItemSize);	
		if(!t16)break;
		//
		pVlog=(consumeLog_t*)buf;
		t32=pVlog->ts;
		t32/=SECS_ON_DAY;
		t32*=SECS_ON_DAY;
		if(t32<ts){
			break;
		}else if(t32==ts){
			//日期正确，填充数据
			if(logbufLen==0)break;
			logbufLen-=4;
			__hzrq_swap_load_t32(logbuf+logbufLen,pVlog->volume);
		}
		loc+=limitsItem;loc--;loc%=limitsItem;
		
	}while(1);
	return t16;
}

uint16_t __hzrq_load_pop_get_day_log(uint8_t* logbuf,uint16_t logbufLen,uint32_t ts)
{
	uint8_t buf[32];
	uint16_t loc,t16,partStartAddr,limitsItem,bodyItemAddr,ItemSize;
	uint32_t t32=0UL;
	exEepromPartDesc_t* pDesc;
	consumeLog_t*	pVlog;
	
	m_mem_set(logbuf,0,logbufLen);
	ts=ts/SECS_ON_DAY;
	if(ts<1)return 0;
	ts--;
	ts=ts*SECS_ON_DAY;
	
	//从写入的位置-1开始想向前查找,从buf的最后端开始写入
	ex_data_get_part_inf(PART_SN_CONSTLOG_DAY,buf,sizeof(exEepromPartDesc_t));
	t16=crc_verify(buf,sizeof(exEepromPartDesc_t));	
	if(!t16)return 0;	

	pDesc=(exEepromPartDesc_t*)buf;

	ItemSize=pDesc->recordSize;
	limitsItem=pDesc->partSize/ItemSize;
	partStartAddr=pDesc->partStartAddr;	
	
	loc=pDesc->recordWriteLoc;
	loc+=limitsItem;loc--;loc%=limitsItem;
	do{
		bodyItemAddr=partStartAddr+loc*ItemSize;
		ex_eeprom_read(SLV_ADDR,bodyItemAddr,buf,ItemSize);
		t16=crc_verify(buf,ItemSize);	
		if(!t16)break;
		//
		pVlog=(consumeLog_t*)buf;
		t32=pVlog->ts;
		t32/=SECS_ON_DAY;
		t32*=SECS_ON_DAY;
		if(t32<ts){
			break;
		}else if(t32==ts){
			//日期正确，填充数据
			if(logbufLen==0)break;
			logbufLen-=4;
			__hzrq_swap_load_t32(logbuf+logbufLen,pVlog->volume);
			ts-=SECS_ON_DAY;
		}
		loc+=limitsItem;loc--;loc%=limitsItem;
		
	}while(1);
	return t16;
}

uint16_t hzrq_load_pop_frame(uint8_t* sbuf,uint16_t ssize,uint8_t popType,uint16_t eventCode,uint8_t eventFlg)
{
	//
	uint16_t len,t16;
	//uint8_t tmpBuf[32];
	uint8_t* p;
	sysDataTime_t dt;
	uint32_t t32;
	
	
	len=_hzrq_load_frame_header(sbuf,ssize,0,0x01,__hzrqMsgSendSn);	
	//__hzrqMsgSendSn++;
	__hzrq_dfPop_t* stb=(__hzrq_dfPop_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
	
	__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_POP);
	__hzrq_load_rtc(stb->dt);	
	stb->popType=popType;
	__hzrq_swap_load_t32(stb->curTotalVol,totalVolume*10);
	_hzrq_load_device_sta(stb->deviceSta);
	//__hzrq_swap_load_t32(stb->customDeviceSta,sysData.devStatus.t32);
	stb->reverse=0x00;
	if(popType==__hzrq_POP_TYPE_EVENT){
		__hzrq_swap_load_t16(stb->eventCode,eventCode);
		stb->eventFlg=eventFlg;
	}else{
		__hzrq_swap_load_t16(stb->eventCode,0);
		stb->eventFlg=0;
	}
	stb->powerType=__hzrq_POWER_TYPE_LISOCL;
	__hzrq_swap_load_t16(stb->batVoltage,voltBat);
	if(pwrStatus==POWER_STATUS_LOW){
		stb->batPercent=20;
	}else if(pwrStatus==POWER_STATUS_DOWN){
		stb->batPercent=10;
	}else{
		stb->batPercent=90;
	}
	
	m_rtc_get(&sysRtcDataTime);
	m_mem_cpy_len((uint8_t*)&dt,(uint8_t*)&sysRtcDataTime,sizeof(sysDataTime_t));
	t32=system_dt_to_time_stamp(&dt);	
	__hzrq_load_pop_get_hour_log(stb->hourVolLogBuf,100,t32);
	__hzrq_load_pop_get_day_log(stb->dayVolLogBuf,24,t32);

	len+=sizeof(__hzrq_dfPop_t);
	len+=3;	
	len=__hzrq_load_frame_mod_len(sbuf,len);
	
	//len=_hzrq_load_frame_encrypt(sbuf,len);
	//len=_hzrq_load_frame_mac(sbuf,len);
	len=_hzrq_load_frame_crc_append(sbuf,len);
	return len;	

}

uint16_t __hzrq_load_rw_fb_insert_retvalue(uint8_t* sbuf,uint16_t len,uint16_t retvalue)
{
	uint16_t frmLen;
	uint8_t* p;
	__hzrq_frameHerder_t* sth=(__hzrq_frameHerder_t*)sbuf;
	frmLen=__hzrq_swap_get_t16(sth->frameLen);	
	
	p=hzrqComBuf;
	m_mem_cpy_len(p,sbuf,sizeof(__hzrq_frameHerder_t)+2);
	p=hzrqComBuf+sizeof(__hzrq_frameHerder_t)+2;
	__hzrq_swap_load_t16(p,retvalue);
	p+=2;
	m_mem_cpy_len(p,sbuf+sizeof(__hzrq_frameHerder_t)+2,frmLen-sizeof(__hzrq_frameHerder_t)-2);
	
	frmLen+=2;
	m_mem_cpy_len(sbuf,hzrqComBuf,frmLen);
	__hzrq_load_frame_mod_len(sbuf,frmLen);
	return frmLen;
}

uint8_t __hzrq_fi_valve_lock(void)
{
	uint8_t ret=0;
	
	do{
		if(sysData.lockReason.bits.bSeverOff )ret=1;
		if(sysData.lockReason.bits.bBalance )ret=1;;
		if(sysData.lockReason.bits.bShellOpen)ret=1;;
		uint8_t tmtry=sysData.qcNoTryTimes;
		if(tmtry==1)tmtry=2;		
		if(sysData.lockReason.bits.bNoflow>=tmtry && tmtry!=0)ret=1;;
		
		//if(lockReason.bits.bSmallFlow)return 0;
		if(sysData.lockReason.bits.bStrongMagnetic >=MAX_STE_OFF_TIMES)ret=1;;		
		
	}while(0);
	return ret;
}



uint16_t hzrq_load_rw_valve(uint8_t* sbuf,uint16_t ssize,uint8_t cb)
{
	uint16_t len,t16;
	uint8_t* p;	
	__hzrq_ctrlByteDef_t __cb;
	__cb.b=cb;
	
	len=_hzrq_load_frame_header(sbuf,ssize,0,cb,__hzrqMid);	
	__hzrq_dfdValveCtrl_t* stb=(__hzrq_dfdValveCtrl_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
	__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_VALVE_CTRL);
	if(vavleState==VALVE_ON){
		stb->valveSta=0;
	}else{
		if(__hzrq_fi_valve_lock()){
			stb->valveSta=2;
		}else{
			stb->valveSta=1;
		}
	}
	len+=sizeof(__hzrq_dfdValveCtrl_t);
	len+=3;
	len=__hzrq_load_frame_mod_len(sbuf,len);
	
	if(__cb.bits.bFuncCode==__bHZRQ_CBFC_DATA_READ){
		
	}else if(__cb.bits.bFuncCode==__bHZRQ_CBFC_WRITE_FEEDBACK){
		//
		len=__hzrq_load_rw_fb_insert_retvalue(sbuf,len,0);
		len=_hzrq_load_frame_encrypt(sbuf,len);
	}

	len=_hzrq_load_frame_crc_append(sbuf,len);
	return len;		
}

uint16_t hzrq_load_rw_rtc(uint8_t* sbuf,uint16_t ssize,uint8_t cb)
{
	uint16_t len,t16;
	uint8_t* p;	
	__hzrq_ctrlByteDef_t __cb;
	__cb.b=cb;
	len=_hzrq_load_frame_header(sbuf,ssize,0,cb,__hzrqMid);	
	__hzrq_dfdDateTime_t* stb=(__hzrq_dfdDateTime_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
	__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_DATETIME);	
	__hzrq_load_rtc(stb->dt);
	
	len+=sizeof(__hzrq_dfdDateTime_t);
	len+=3;
	len=__hzrq_load_frame_mod_len(sbuf,len);	
	
	if(__cb.bits.bFuncCode==__bHZRQ_CBFC_DATA_READ){
		
	}else if(__cb.bits.bFuncCode==__bHZRQ_CBFC_WRITE_FEEDBACK){
		len=__hzrq_load_rw_fb_insert_retvalue(sbuf,len,0);
		
		len=_hzrq_load_frame_encrypt(sbuf,len);
	}

	len=_hzrq_load_frame_crc_append(sbuf,len);
	return len;		
}

uint16_t hzrq_load_rw_total_vol(uint8_t* sbuf,uint16_t ssize,uint8_t cb)
{
	uint16_t len,t16;
	uint8_t* p;	
	__hzrq_ctrlByteDef_t __cb;
	__cb.b=cb;
	len=_hzrq_load_frame_header(sbuf,ssize,0,cb,__hzrqMid);	
	__hzrq_dfdCurToltalVol_t* stb=(__hzrq_dfdCurToltalVol_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
	__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_CUR_TOLTAL_VOL);	
	//__hzrq_load_rtc(stb->dt);
	__hzrq_swap_load_t32(stb->curTotalVol,totalVolume*10);
	len+=sizeof(__hzrq_dfdCurToltalVol_t);
	len+=3;
	len=__hzrq_load_frame_mod_len(sbuf,len);	
	
	if(__cb.bits.bFuncCode==__bHZRQ_CBFC_DATA_READ){
		len=_hzrq_load_frame_encrypt(sbuf,len);
		
	}else if(__cb.bits.bFuncCode==__bHZRQ_CBFC_WRITE_FEEDBACK){
		len=__hzrq_load_rw_fb_insert_retvalue(sbuf,len,0);
		
		len=_hzrq_load_frame_encrypt(sbuf,len);
	}
	len=_hzrq_load_frame_crc_append(sbuf,len);
	return len;		
}

uint16_t hzrq_load_rw_reverse_vol(uint8_t* sbuf,uint16_t ssize,uint8_t cb)
{
	uint16_t len,t16;
	uint8_t* p;	
	__hzrq_ctrlByteDef_t __cb;
	__cb.b=cb;
	len=_hzrq_load_frame_header(sbuf,ssize,0,cb,__hzrqMid);	
	__hzrq_dfdReverseVolUnknow_t* stb=(__hzrq_dfdReverseVolUnknow_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
	__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_CUR_TOLTAL_VOL);	
	//__hzrq_load_rtc(stb->dt);
	int32_t t32=sysData.hzrqReverseVol;
	__hzrq_swap_load_t32(stb->reverseVol,t32*10);
	len+=sizeof(__hzrq_dfdReverseVolUnknow_t);
	len+=3;
	len=__hzrq_load_frame_mod_len(sbuf,len);	
	
	if(__cb.bits.bFuncCode==__bHZRQ_CBFC_DATA_READ){
		len=_hzrq_load_frame_encrypt(sbuf,len);
		
	}else if(__cb.bits.bFuncCode==__bHZRQ_CBFC_WRITE_FEEDBACK){
		len=__hzrq_load_rw_fb_insert_retvalue(sbuf,len,0);
		
		len=_hzrq_load_frame_encrypt(sbuf,len);
	}
	len=_hzrq_load_frame_crc_append(sbuf,len);
	return len;		
}
//__hzrq_dfdStatusDataCommonn_t32_t
uint16_t hzrq_load_rw_t32(uint8_t* sbuf,uint16_t ssize,uint8_t cb,uint16_t dfid,
						int32_t t32,bool encrypt,bool mac)
{
	uint16_t len,t16;
	uint8_t* p;	
	__hzrq_ctrlByteDef_t __cb;
	__cb.b=cb;
	len=_hzrq_load_frame_header(sbuf,ssize,0,cb,__hzrqMid);	
	__hzrq_dfdStatusDataCommonn_t32_t* stb=(__hzrq_dfdStatusDataCommonn_t32_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
	__hzrq_swap_load_t16(stb->iden,dfid);	
	__hzrq_swap_load_t32(stb->data,t32);
	len+=sizeof(__hzrq_dfdStatusDataCommonn_t32_t);
	len+=3;
	len=__hzrq_load_frame_mod_len(sbuf,len);	
	
	if(__cb.bits.bFuncCode==__bHZRQ_CBFC_DATA_READ){
		if(encrypt)len=_hzrq_load_frame_encrypt(sbuf,len);
		
	}else if(__cb.bits.bFuncCode==__bHZRQ_CBFC_WRITE_FEEDBACK){
		len=__hzrq_load_rw_fb_insert_retvalue(sbuf,len,0);
		len=_hzrq_load_frame_encrypt(sbuf,len);
		//if(encrypt)len=_hzrq_load_frame_encrypt(sbuf,len);
	}
	if(mac)len=_hzrq_load_frame_mac(sbuf,len);
	len=_hzrq_load_frame_crc_append(sbuf,len);
	return len;		
}

void _hzrq_ins_exe_sync_rtc(uint8_t* rtcBuf)
{
	sysDataTime_t dt;
	dt.YY=rtcBuf[0];
	dt.MM=rtcBuf[1];
	dt.DD=rtcBuf[2];
	dt.hh=rtcBuf[3];
	dt.mm=rtcBuf[4];
	dt.ss=rtcBuf[5];
	dt.YYhi=0x20;
	uint16_t year=m_bcd_2_hex(0x20);
	year*=100;
	year+=m_bcd_2_hex(dt.YY);
	dt.day=day_of_week(year,m_bcd_2_hex(dt.MM),m_bcd_2_hex(dt.DD));
	api_rtc_set(&dt);
}
uint16_t _hzrq_received_crc_verify(uint8_t* rbuf,uint16_t rlen)
{
	uint16_t ret;
	uint16_t frmLen;
	__hzrq_frameHerder_t* sth=(__hzrq_frameHerder_t*)rbuf;
	frmLen=__hzrq_swap_get_t16(sth->frameLen);
	if(frmLen>rlen)return 0;
	ret=__hzrq_crc_verify(rbuf+sizeof(__hzrq_frameHerder_t)-2,frmLen-(sizeof(__hzrq_frameHerder_t)-2)-1);
	return ret;
}

uint16_t _hzrq_received_mac_verify(uint8_t* rbuf,uint16_t rlen)
{
	uint8_t tmpBuf[32];
	uint16_t ret;
	uint16_t frmLen;
	uint16_t objLen;
	__hzrq_frameHerder_t* sth=(__hzrq_frameHerder_t*)rbuf;
	frmLen=__hzrq_swap_get_t16(sth->frameLen);
	//mac 验证
	objLen=frmLen-sizeof(__hzrq_frameHerder_t)- __hzrq_DFID_LEN -sizeof(__hzrq_frameTail_t)-__hzrq_MAC_LEN;
	m_mem_cpy_len(hzrqComBuf,sysData.hzrqRandomCode,16);
	m_mem_cpy_len(hzrqComBuf+16,rbuf+sizeof(__hzrq_frameHerder_t) + __hzrq_DFID_LEN ,objLen);	
	//
	uint8_t macKey[16];
	__hzrq_get_mac_key(macKey,sysData.hzrqRandomCode,16);
	hmac_sha256(tmpBuf,hzrqComBuf,objLen+16,macKey,16);
	ret=m_str_cmp_len(tmpBuf,rbuf+sizeof(__hzrq_frameHerder_t)+__hzrq_DFID_LEN+objLen,__hzrq_MAC_LEN);
	if(ret==0)return ret;
	//去掉mac部分，位解密用
	frmLen-=32;
	__hzrq_load_frame_mod_len(rbuf,frmLen);
	
	return frmLen;
}

uint16_t _hzrq_received_decrypt(uint8_t* rbuf,uint16_t rlen)
{
	//调用此函数必须去掉mac长度，并且修正帧头中的len字段
	uint16_t frmLen;
	uint16_t objLen;
	
	__hzrq_frameHerder_t* sth=(__hzrq_frameHerder_t*)rbuf;
	frmLen=__hzrq_swap_get_t16(sth->frameLen);	
	objLen=frmLen-sizeof(__hzrq_frameHerder_t) - __hzrq_DFID_LEN -sizeof(__hzrq_frameTail_t);
	//if(fimac)objLen-=__hzrq_MAC_LEN;
	
	uint8_t ecryptKey[16];
	__hzrq_get_encrypt_key(ecryptKey,sysData.hzrqRandomCode,16);
	
	m_ebc_decrypt(hzrqComBuf,rbuf+sizeof(__hzrq_frameHerder_t) + __hzrq_DFID_LEN,ecryptKey,objLen);
	m_mem_cpy_len(rbuf+sizeof(__hzrq_frameHerder_t) + __hzrq_DFID_LEN,hzrqComBuf,objLen);
	return frmLen;
}


//明文+mac
int16_t hzrq_ins_exe_register(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize,uint8_t popType)
{
	//uint16_t frmLen;
	int16_t t16;
	

	__hzrq_dfdRegisteReply* stb=(__hzrq_dfdRegisteReply*)(rbuf+sizeof(__hzrq_frameHerder_t));
	//crc 验证
	
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}
	/*
	//mac 验证
	uint8_t tmpBuf[32];
	m_mem_cpy_len(hzrqComBuf,sysData.hzrqRandomCode,16);
	m_mem_cpy_len(hzrqComBuf+16,(uint8_t*)stb+2,sizeof(__hzrq_dfdRegisteReply)-2);	
	//sha256(tmpBuf,hzrqComBuf,sizeof(__hzrq_dfdRegisteReply)-2+16);
	


	uint8_t macKey[16];
	_hzrq_get_mac_key(macKey,sysData.hzrqRandomCode,16);
	hmac_sha256(tmpBuf,hzrqComBuf,sizeof(__hzrq_dfdRegisteReply)-2+16,macKey,16);
	
	t16=m_str_cmp_len(tmpBuf,rbuf+sizeof(__hzrq_frameHerder_t)+sizeof(__hzrq_dfdRegisteReply),32);
	if(t16==0){return -1;}
	*/
	t16=_hzrq_received_mac_verify(rbuf,rlen);
 	if(t16==0){return -1;}
	//do 
	_hzrq_ins_exe_sync_rtc(stb->dt);
	//rtc
	__hzrqErrorCode=__hzrq_swap_get_t16(stb->retCode);
	//if(errCode=)
	//load pop frame buffer;
	//uint16_t unSendItem;
	
	t16=rf_send_fifo_get_tail(sbuf,(uint16_t*)&__hzrqUnSendNum);
	if(__hzrqUnSendNum>0){
		t16=__hzrq_load_frame_mod_hasmore_mid(sbuf,t16,__bHZRQ_CBHASMORE_OVER,__hzrqMsgSendSn);
		t16=_hzrq_load_frame_encrypt(sbuf,t16);
		t16=_hzrq_crc_append_send(sbuf,t16);		
	}else{
		t16=0;
	}
	/*
	if(unSendItem>1){
		//t16=__hzrq_load_frame_mod_hasmore_mid(sbuf,t16,__bHZRQ_CBHASMORE_HASMORE,__hzrqMid);
		t16=__hzrq_load_frame_mod_hasmore_mid(sbuf,t16,__bHZRQ_CBHASMORE_OVER,__hzrqMsgSendSn);
		t16=_hzrq_crc_append_send(sbuf,t16);
		
	}else if(unSendItem==1){
		t16=__hzrq_load_frame_mod_hasmore_mid(sbuf,t16,__bHZRQ_CBHASMORE_OVER,__hzrqMsgSendSn);
		t16=_hzrq_crc_append_send(sbuf,t16);
		//t16=hzrq_load_pop_frame(sbuf,ssize,popType);
	}else{
		t16=0;
	}
	*/
	//t16=hzrq_load_pop_frame(sbuf,ssize,popType);
	sendStaMichine=__hzrq_SEND_SM_POP;
	rtcSync=true;
	return t16;
}

int16_t hzrq_ins_exe_pop(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	/*
	rf_send_fifo_delete_tail();
	uint16_t t16,unSendItem;
	t16=rf_send_fifo_get_tail(sbuf,&unSendItem);
	if(unSendItem>1){
		//t16=__hzrq_load_frame_mod_hasmore_mid(sbuf,t16,__bHZRQ_CBHASMORE_HASMORE,__hzrqMid);
		t16=__hzrq_load_frame_mod_hasmore_mid(sbuf,t16,__bHZRQ_CBHASMORE_HASMORE,__hzrqMsgSendSn);
		t16=_hzrq_crc_append_send(sbuf,t16);
		
	}else if(unSendItem==1){
		t16=__hzrq_load_frame_mod_hasmore_mid(sbuf,t16,__bHZRQ_CBHASMORE_OVER,__hzrqMsgSendSn);
		t16=_hzrq_crc_append_send(sbuf,t16);
		//t16=hzrq_load_pop_frame(sbuf,ssize,popType);
		//t16=0;
	}else{
		t16=0;
	}
	*/
	uint16_t t16;
	t16=rf_send_fifo_get_tail(sbuf,(uint16_t*)&__hzrqUnSendNum);
	if(__hzrqUnSendNum>0){
		t16=__hzrq_load_frame_mod_hasmore_mid(sbuf,t16,__bHZRQ_CBHASMORE_OVER,__hzrqMsgSendSn);
		t16=_hzrq_load_frame_encrypt(sbuf,t16);
		t16=_hzrq_crc_append_send(sbuf,t16);		
	}else{
		t16=0;
	}	
	return t16;
}
/*

		uint8_t iden[2];
		uint8_t retCode[2];					//2	错误码
		uint8_t dt[6];						//6	时钟
		uint8_t balanceVol[4];				//4	剩余气量
		uint8_t overdraftStat;				//1	透支状态
		uint8_t balanceVolSta;				//1	余量状态
		uint8_t price[4];					//4	单价
		uint8_t balanceMoney[4];			//4	剩余金额
*/

int16_t hzrq_ins_exe_end(uint8_t* rbuf,uint16_t rlen)
{
	//uint16_t frmLen;
	int16_t t16;
	//uint8_t tmpBuf[32];
	//crc
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}
	//mac
	t16=_hzrq_received_mac_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	//解密
	_hzrq_received_decrypt(rbuf,rlen);
	__hzrq_dfdCommEnd_t* stb=(__hzrq_dfdCommEnd_t*)(rbuf+sizeof(__hzrq_frameHerder_t));
	
	//__hzrq_swap_get_t32
	__hzrqErrorCode=__hzrq_swap_get_t16(stb->retCode);
	if(__hzrqErrorCode ==__hzrq_ERR_NONE){
		api_calc_all();
		sysData.crBalance=__hzrq_swap_get_t32(stb->balanceMoney);
		int32_t t32=__hzrq_swap_get_t32(stb->balanceVol);
		sysData.crBalanceVol=t32/10;
		sysData.hzrqOverdraftStat=stb->overdraftStat;
		sysData.hzrqBalanceVolSta=stb->balanceVolSta;
		sysData.crPrice=__hzrq_swap_get_t32(stb->price);
		api_calc_all();
	}
	sendStaMichine=__hzrq_SEND_SM_END;
	return 0;
}

int16_t hzrq_ins_rw_valve_sta(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	//
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	__hzrq_dfdValveCtrl_t* stb;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	if(fc==__bHZRQ_CBFC_DATA_READ){
		
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		//mac
		t16=_hzrq_received_mac_verify(rbuf,rlen);
		if(t16==0){return -1;}	
		//解密
		_hzrq_received_decrypt(rbuf,rlen);	
		stb=(__hzrq_dfdValveCtrl_t*)(rbuf+sizeof(__hzrq_frameHerder_t));
		if(stb->valveCtrl==0){
			//开
			sysData.offReason=OFF_REASON_NONE;
			sysData.lockReason.t32=0x00UL;
			api_sysdata_save();			
		}else if(stb->valveCtrl==1){
			if(vavleState==VALVE_OFF || vavleState==VALVE_OPERATION_OFF){
				retValue=__hzrq_ERR_WRITE_PARAM;
			}else{
				vavle_off_from_app(OFF_REASON_TEMP);
			}
			//临时关
		}else if(stb->valveCtrl==2){
			//
			sysData.lockReason.bits.bSeverOff=1;
			if(vavleState==VALVE_OFF || vavleState==VALVE_OPERATION_OFF){
				api_sysdata_save();
			}else{
				vavle_off_from_app(OFF_REASON_FORCE);
			}			
		}	
	}
	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if((fc==__bHZRQ_CBFC_DATA_READ || fc==__bHZRQ_CBFC_WRITE_FEEDBACK) && (retValue==__hzrq_ERR_NONE)){
		t16=hzrq_load_rw_valve(sbuf,ssize,cb.b);
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_VALVE_CTRL,retValue,false,false);
	}
	return t16;

}

int16_t hzrq_ins_rw_rtc(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	//
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	__hzrq_dfdDateTime_t* stb;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	if(fc==__bHZRQ_CBFC_DATA_READ){
		
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		//mac
		t16=_hzrq_received_mac_verify(rbuf,rlen);
		if(t16==0){return -1;}	
		//解密
		_hzrq_received_decrypt(rbuf,rlen);	
		
		stb=(__hzrq_dfdDateTime_t*)(rbuf+sizeof(__hzrq_frameHerder_t));
		_hzrq_ins_exe_sync_rtc(stb->dt);		
	}
	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if(fc==__bHZRQ_CBFC_DATA_READ || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		t16=hzrq_load_rw_rtc(sbuf,ssize,cb.b);
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_DATETIME,retValue,false,false);
	}
	return t16;	
}

int16_t hzrq_ins_rw_total_vol(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	int32_t t32;
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	__hzrq_dfdCurToltalVol_t* stb;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	
	if(fc==__bHZRQ_CBFC_DATA_READ){
		_hzrq_received_decrypt(rbuf,rlen);	
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		t16=_hzrq_received_mac_verify(rbuf,rlen);
		if(t16==0){return -1;}	
		_hzrq_received_decrypt(rbuf,rlen);	
		stb=(__hzrq_dfdCurToltalVol_t*)(rbuf+sizeof(__hzrq_frameHerder_t));
		t32=__hzrq_swap_get_t32(stb->curTotalVol);
		t32/=10;
		api_sysdata_save();
		__disable_irq();
		sysData.totalVolume=t32;
		__enable_irq();	
		api_sysdata_save();
		
	}
	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if(fc==__bHZRQ_CBFC_DATA_READ || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		t16=hzrq_load_rw_total_vol(sbuf,ssize,cb.b);
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_CUR_TOLTAL_VOL,retValue,false,false);
	}
	return t16;		
}
//

int16_t hzrq_ins_rw_bat_voltage(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize,uint16_t dfid)
{
	//int32_t t32;
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	if(dfid==__hzrq_DFID_BAT_PERCENT_EX){
		retValue=__hzrq_ERR_DFID;
	}else if(fc==__bHZRQ_CBFC_DATA_READ ){
		__nop();
	}else{
		
		retValue=__hzrq_ERR_WRITE_PARAM;
	}
	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if(retValue==__hzrq_ERR_NONE){
		t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);	
		_hzrq_dfdBtVoltage_t* stb=(_hzrq_dfdBtVoltage_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
		__hzrq_swap_load_t16(stb->iden,dfid);	
		__hzrq_swap_load_t16(stb->batVoltage,voltBat);	
		
		t16+=sizeof(_hzrq_dfdBtVoltage_t);
		t16+=3;
		t16=__hzrq_load_frame_mod_len(sbuf,t16);	
		
		t16=_hzrq_load_frame_crc_append(sbuf,t16);				
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,dfid,retValue,false,false);
	}
	return t16;		
}
//
int16_t hzrq_ins_rw_bat_percent(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize,uint16_t dfid)
{
	//int32_t t32;
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	if(dfid==__hzrq_DFID_BAT_PERCENT_EX){
		retValue=__hzrq_ERR_DFID;
	}else if(fc==__bHZRQ_CBFC_DATA_READ ){
		__nop();
	}else{
		
		retValue=__hzrq_ERR_WRITE_PARAM;
	}
	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if(retValue==__hzrq_ERR_NONE){
		t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);	
		_hzrq_dfdBtPercent_t* stb=(_hzrq_dfdBtPercent_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
		__hzrq_swap_load_t16(stb->iden,dfid);
		stb->batPercent=90;	
		
		t16+=sizeof(_hzrq_dfdBtPercent_t);
		t16+=3;
		t16=__hzrq_load_frame_mod_len(sbuf,t16);	
		
		t16=_hzrq_load_frame_crc_append(sbuf,t16);				
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,dfid,retValue,false,false);
	}
	return t16;		
}

int16_t hzrq_ins_rw_reverse_vol(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	int32_t t32;
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	__hzrq_dfdReverseVolUnknow_t* stb;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	
	if(fc==__bHZRQ_CBFC_DATA_READ){
		_hzrq_received_decrypt(rbuf,rlen);	
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		t16=_hzrq_received_mac_verify(rbuf,rlen);
		if(t16==0){return -1;}	
		_hzrq_received_decrypt(rbuf,rlen);	
		stb=(__hzrq_dfdReverseVolUnknow_t*)(rbuf+sizeof(__hzrq_frameHerder_t));
		t32=__hzrq_swap_get_t32(stb->reverseVol);
		t32/=10;
		api_sysdata_save();
		__disable_irq();
		sysData.hzrqReverseVol=t32;
		__enable_irq();	
		api_sysdata_save();
	}
	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if(fc==__bHZRQ_CBFC_DATA_READ || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		//t16=hzrq_load_rw_total_vol(sbuf,ssize,cb.b);
		t32=sysData.hzrqReverseVol;
		t32*=10;
		//t16=hzrq_load_rw_t32(sbuf,ssize,cb.b,__hzrq_DFID_REVERSE_UKN,t32,true,false);
		t16=hzrq_load_rw_t32(sbuf,ssize,cb.b,__hzrq_DFID_REVERSE_UKN,t32,false,false);
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_REVERSE_UKN,retValue,false,false);
	}
	return t16;		
}

//
int16_t hzrq_ins_rw_balence_vol(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	int32_t t32;
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	__hzrq_dfdBalanceVol_t* stb;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	
	if(fc==__bHZRQ_CBFC_DATA_READ){
		_hzrq_received_decrypt(rbuf,rlen);	
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		t16=_hzrq_received_mac_verify(rbuf,rlen);
		if(t16==0){return -1;}	
		_hzrq_received_decrypt(rbuf,rlen);	
		stb=(__hzrq_dfdBalanceVol_t*)(rbuf+sizeof(__hzrq_frameHerder_t));
		t32=__hzrq_swap_get_t32(stb->balanceVol);
		t32/=10;
		api_sysdata_save();
		__disable_irq();
		sysData.crBalanceVol=t32;
		__enable_irq();	
		api_sysdata_save();
	}
	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if(fc==__bHZRQ_CBFC_DATA_READ || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		t32=sysData.crBalanceVol;
		t32*=10;
		t16=hzrq_load_rw_t32(sbuf,ssize,cb.b,__hzrq_DFID_BALANCE_VOL,t32,false,false);
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_BALANCE_VOL,retValue,false,false);
	}
	return t16;		
}

int16_t hzrq_ins_rw_overdraft_sta(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	//
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	__hzrq_dfdOverdraftStat_t* stb;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	if(fc==__bHZRQ_CBFC_DATA_READ){
		
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		//mac
		//t16=_hzrq_received_mac_verify(rbuf,rlen);
		//if(t16==0){return -1;}	
		//解密
		_hzrq_received_decrypt(rbuf,rlen);	
		stb=(__hzrq_dfdOverdraftStat_t*)(rbuf+sizeof(__hzrq_frameHerder_t));
		//sysData.hzrqOverdraftStat=stb->overdraftStat;
		api_sysdata_save();
		__disable_irq();
		sysData.hzrqOverdraftStat=stb->overdraftStat;
		__enable_irq();	
		api_sysdata_save();	
	}
	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	if((fc==__bHZRQ_CBFC_DATA_READ || fc==__bHZRQ_CBFC_WRITE_FEEDBACK) && (retValue==__hzrq_ERR_NONE)){
		//t16=hzrq_load_rw_valve(sbuf,ssize,cb.b);
		t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);	
		stb=(__hzrq_dfdOverdraftStat_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
		__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_OVERDRAFT_STA);	
		//__hzrq_swap_load_t16(stb->batVoltage,voltBat);	
		stb->overdraftStat=sysData.hzrqOverdraftStat;
		t16+=sizeof(__hzrq_dfdOverdraftStat_t);
		t16+=3;
		t16=__hzrq_load_frame_mod_len(sbuf,t16);		
		if(fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
				//
			t16=__hzrq_load_rw_fb_insert_retvalue(sbuf,t16,0);
			t16=_hzrq_load_frame_encrypt(sbuf,t16);
		}		
		t16=_hzrq_load_frame_crc_append(sbuf,t16);	
		
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_OVERDRAFT_STA,retValue,false,false);
	}
	return t16;

}


int16_t hzrq_ins_rw_device_sta(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	//int32_t t32;
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	if(fc==__bHZRQ_CBFC_DATA_READ){
		
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		retValue=__hzrq_ERR_WRITE_PARAM;
	}	
	

	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if(retValue==__hzrq_ERR_NONE){
		t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);	
		__hzrq_dfdDeviceStat_t* stb=(__hzrq_dfdDeviceStat_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
		__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_DEVICE_STA);	
		_hzrq_load_device_sta(stb->deviceSta);
		
		t16+=sizeof(__hzrq_dfdDeviceStat_t);
		t16+=3;
		t16=__hzrq_load_frame_mod_len(sbuf,t16);	
		
		t16=_hzrq_load_frame_crc_append(sbuf,t16);				
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_DEVICE_STA,retValue,false,false);
	}
	return t16;		
}

int16_t hzrq_ins_rw_random_code(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	//int32_t t32;
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	if(fc==__bHZRQ_CBFC_DATA_READ){
		
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		retValue=__hzrq_ERR_WRITE_PARAM;
	}	
	

	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if(retValue==__hzrq_ERR_NONE){
		t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);	
		__hzrq_dfdRandonCode_t* stb=(__hzrq_dfdRandonCode_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
		__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_RANDOM_CODE);	
		//_hzrq_load_device_sta(stb->deviceSta);
		m_mem_cpy_len(stb->randomCode,sysData.hzrqRandomCode,16);
		
		t16+=sizeof(__hzrq_dfdRandonCode_t);
		t16+=3;
		t16=__hzrq_load_frame_mod_len(sbuf,t16);	
		
		t16=_hzrq_load_frame_crc_append(sbuf,t16);				
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_RANDOM_CODE,retValue,false,false);
	}
	return t16;		
}

int16_t hzrq_ins_rw_price(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	int32_t t32;
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	__hzrq_dfdPrice_t* stb;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	
	if(fc==__bHZRQ_CBFC_DATA_READ){
		_hzrq_received_decrypt(rbuf,rlen);	
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		t16=_hzrq_received_mac_verify(rbuf,rlen);
		if(t16==0){return -1;}	
		_hzrq_received_decrypt(rbuf,rlen);	
		stb=(__hzrq_dfdPrice_t*)(rbuf+sizeof(__hzrq_frameHerder_t));
		t32=__hzrq_swap_get_t32(stb->price);
		
		api_sysdata_save();
		__disable_irq();
		sysData.crPrice=t32;
		__enable_irq();	
		api_sysdata_save();
	}
	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if(fc==__bHZRQ_CBFC_DATA_READ || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		t32=sysData.crPrice;
		//t32*=10;
		t16=hzrq_load_rw_t32(sbuf,ssize,cb.b,__hzrq_DFID_PRICE,t32,false,false);
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_PRICE,retValue,false,false);
	}
	return t16;		
}

int16_t hzrq_ins_rw_account_sta(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	//
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	__hzrq_dfdAccountSTa_t* stb;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	if(sysData.DLCS==DLC_STATUS_A){
		retValue=__hzrq_ERR_WRITE_PARAM;
		
	}else if(fc==__bHZRQ_CBFC_DATA_READ){
		
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		//mac
		t16=_hzrq_received_mac_verify(rbuf,rlen);
		if(t16==0){return -1;}	
		//解密
		_hzrq_received_decrypt(rbuf,rlen);	
		stb=(__hzrq_dfdAccountSTa_t*)(rbuf+sizeof(__hzrq_frameHerder_t));
		
		api_sysdata_save();
		sysData.hzrqAccountSta=stb->accountSta;
		if(stb->accountSta==0){
			m_flow_all_data_init();
			qc_data_realtime_data_clear();
			qc_data_device_even_lock_clear();
			sysData.DLCS=DLC_STATUS_B;
			
			sysData.OVerageVM=default_OVERAGE_VM_B;
			sysData.warnSetOverageVM=default_WARNING_VOLUMING;
			sysData.offSetOverageVM=0;
			sysData.limitOffsetVM=-1000;			
			
		}else{
			m_flow_all_data_init();
			qc_data_realtime_data_clear();
			qc_data_device_even_lock_clear();	
			sysData.DLCS=DLC_STATUS_C;			
		}
		
		//__disable_irq();
		sysData.hzrqAccountSta=stb->accountSta;
		//__enable_irq();	
		api_sysdata_save();		
	}
	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if((fc==__bHZRQ_CBFC_DATA_READ || fc==__bHZRQ_CBFC_WRITE_FEEDBACK) && (retValue==__hzrq_ERR_NONE)){
		//t16=hzrq_load_rw_valve(sbuf,ssize,cb.b);
		t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);	
		stb=(__hzrq_dfdAccountSTa_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
		__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_ACCOUNT_STA);
		stb->accountSta=sysData.hzrqAccountSta;

		t16+=sizeof(__hzrq_dfdAccountSTa_t);
		t16+=3;
		t16=__hzrq_load_frame_mod_len(sbuf,t16);
		
		if(fc==__bHZRQ_CBFC_DATA_READ){
			
		}else if(fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
			//
			t16=__hzrq_load_rw_fb_insert_retvalue(sbuf,t16,0);
			t16=_hzrq_load_frame_encrypt(sbuf,t16);
		}

		t16=_hzrq_load_frame_crc_append(sbuf,t16);
			
		
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_ACCOUNT_STA,retValue,false,false);
	}
	return t16;

}

int16_t hzrq_ins_rw_balance_money(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	int32_t t32;
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	__hzrq_dfdBalanceMoney_t* stb;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	
	if(fc==__bHZRQ_CBFC_DATA_READ){
		_hzrq_received_decrypt(rbuf,rlen);	
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		t16=_hzrq_received_mac_verify(rbuf,rlen);
		if(t16==0){return -1;}	
		_hzrq_received_decrypt(rbuf,rlen);	
		stb=(__hzrq_dfdBalanceMoney_t*)(rbuf+sizeof(__hzrq_frameHerder_t));
		t32=__hzrq_swap_get_t32(stb->balanceMoney);
		//t32/=10;
		api_sysdata_save();
		__disable_irq();
		sysData.crBalance=t32;
		__enable_irq();	
		api_sysdata_save();
	}
	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if(fc==__bHZRQ_CBFC_DATA_READ || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		t32=sysData.crBalance;
		//t32*=10;
		t16=hzrq_load_rw_t32(sbuf,ssize,cb.b,__hzrq_DFID_BALANCE_M,t32,false,false);
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_BALANCE_M,retValue,false,false);
	}
	return t16;		
}


int16_t hzrq_ins_rw_balance_sta(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	//
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	__hzrq_dfdAccountSTa_t* stb;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	if(fc==__bHZRQ_CBFC_DATA_READ){
		
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		//mac
		t16=_hzrq_received_mac_verify(rbuf,rlen);
		if(t16==0){return -1;}	
		//解密
		_hzrq_received_decrypt(rbuf,rlen);	
		stb=(__hzrq_dfdAccountSTa_t*)(rbuf+sizeof(__hzrq_frameHerder_t));
		
		api_sysdata_save();
		__disable_irq();
		sysData.hzrqAccountSta=stb->accountSta;
		__enable_irq();	
		api_sysdata_save();		
	
	}
	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if((fc==__bHZRQ_CBFC_DATA_READ || fc==__bHZRQ_CBFC_WRITE_FEEDBACK) && (retValue==__hzrq_ERR_NONE)){
		//t16=hzrq_load_rw_valve(sbuf,ssize,cb.b);
		t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);	
		stb=(__hzrq_dfdAccountSTa_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
		__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_ACCOUNT_STA);
		stb->accountSta=sysData.hzrqAccountSta;

		t16+=sizeof(__hzrq_dfdAccountSTa_t);
		t16+=3;
		t16=__hzrq_load_frame_mod_len(sbuf,t16);
		
		if(fc==__bHZRQ_CBFC_DATA_READ){
			
		}else if(fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
			//
			t16=__hzrq_load_rw_fb_insert_retvalue(sbuf,t16,0);
			t16=_hzrq_load_frame_encrypt(sbuf,t16);
		}

		t16=_hzrq_load_frame_crc_append(sbuf,t16);
			
		
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_ACCOUNT_STA,retValue,false,false);
	}
	return t16;

}
int16_t hzrq_ins_rw_rssi(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	//int32_t t32;
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	if(fc==__bHZRQ_CBFC_DATA_READ){
		
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		retValue=__hzrq_ERR_WRITE_PARAM;
	}	
	

	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if(retValue==__hzrq_ERR_NONE){
		t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);	
		__hzrq_dfdRssi_t* stb=(__hzrq_dfdRssi_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
		__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_RSSI);
		__hzrq_swap_load_t16(stb->rsrp,hzrqRsrp);
		__hzrq_swap_load_t16(stb->snr,hzrqSnr);

		
		t16+=sizeof(__hzrq_dfdRssi_t);
		t16+=3;
		t16=__hzrq_load_frame_mod_len(sbuf,t16);	
		
		t16=_hzrq_load_frame_crc_append(sbuf,t16);				
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_RSSI,retValue,false,false);
	}
	return t16;		
}

int16_t hzrq_ins_rw_comm_fails_times(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	//int32_t t32;
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	if(fc==__bHZRQ_CBFC_DATA_READ){
		
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		retValue=__hzrq_ERR_WRITE_PARAM;
	}	
	

	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if(retValue==__hzrq_ERR_NONE){
		t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);	
		__hzrq_dfdCommFailTimes_t* stb=(__hzrq_dfdCommFailTimes_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
		__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_COMM_FAIL_TIMES);
		__hzrq_swap_load_t16(stb->commFailTimes,sysData.hzrqCommFailTimes);

		t16+=sizeof(__hzrq_dfdCommFailTimes_t);
		t16+=3;
		t16=__hzrq_load_frame_mod_len(sbuf,t16);	
		
		t16=_hzrq_load_frame_crc_append(sbuf,t16);				
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_COMM_FAIL_TIMES,retValue,false,false);
	}
	return t16;		
}

int16_t hzrq_ins_rw_ecl_level(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	//int32_t t32;
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	if(fc==__bHZRQ_CBFC_DATA_READ){
		
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		retValue=__hzrq_ERR_WRITE_PARAM;
	}	
	

	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if(retValue==__hzrq_ERR_NONE){
		t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);	
		__hzrq_dfdEclLevel_t* stb=(__hzrq_dfdEclLevel_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
		__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_ECL_LEVEL);
		__hzrq_swap_load_t16(stb->eclLevel,hzrqEclLevel);

		t16+=sizeof(__hzrq_dfdEclLevel_t);
		t16+=3;
		t16=__hzrq_load_frame_mod_len(sbuf,t16);	
		
		t16=_hzrq_load_frame_crc_append(sbuf,t16);				
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_ECL_LEVEL,retValue,false,false);
	}
	return t16;		
}

int16_t hzrq_ins_rw_cell_id(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	//int32_t t32;
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	if(fc==__bHZRQ_CBFC_DATA_READ){
		
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		retValue=__hzrq_ERR_WRITE_PARAM;
	}	
	

	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if(retValue==__hzrq_ERR_NONE){
		t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);	
		__hzrq_dfdCellId_t* stb=(__hzrq_dfdCellId_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
		__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_CELL_ID);
		//__hzrq_swap_load_t16(stb->eclLevel,hzrqEclLevel);
		m_mem_cpy_len(stb->cellId,hzrqCellId,6);
		t16+=sizeof(__hzrq_dfdCellId_t);
		t16+=3;
		t16=__hzrq_load_frame_mod_len(sbuf,t16);	
		
		t16=_hzrq_load_frame_crc_append(sbuf,t16);				
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_CELL_ID,retValue,false,false);
	}
	return t16;		
}

int16_t hzrq_ins_rw_near_fcn(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	//int32_t t32;
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	if(fc==__bHZRQ_CBFC_DATA_READ){
		
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		retValue=__hzrq_ERR_WRITE_PARAM;
	}	
	

	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if(retValue==__hzrq_ERR_NONE){
		t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);	
		__hzrq_dfdRealNerafcn_t* stb=(__hzrq_dfdRealNerafcn_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
		__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_REAL_NEARFCN);
		__hzrq_swap_load_t16(stb->realNearfcn,hzrqRealNearFcn);
		//m_mem_cpy_len(stb->cellId,hzrqCellId,6);
		t16+=sizeof(__hzrq_dfdRealNerafcn_t);
		t16+=3;
		t16=__hzrq_load_frame_mod_len(sbuf,t16);	
		
		t16=_hzrq_load_frame_crc_append(sbuf,t16);				
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_REAL_NEARFCN,retValue,false,false);
	}
	return t16;		
}


int16_t hzrq_ins_rw_imei(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	//int32_t t32;
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	if(fc==__bHZRQ_CBFC_DATA_READ){
		
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		retValue=__hzrq_ERR_WRITE_PARAM;
	}	
	

	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if(retValue==__hzrq_ERR_NONE){
		t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);	
		__hzrq_dfdImei_t* stb=(__hzrq_dfdImei_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
		__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_IMEI);
		//__hzrq_swap_load_t16(stb->realNearfcn,hzrqRealNearFcn);
		m_mem_cpy_len(stb->imei,CGSN,15);
		t16+=sizeof(__hzrq_dfdImei_t);
		t16+=3;
		t16=__hzrq_load_frame_mod_len(sbuf,t16);	
		
		t16=_hzrq_load_frame_crc_append(sbuf,t16);				
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_IMEI,retValue,false,false);
	}
	return t16;		
}


int16_t hzrq_ins_rw_nb_swver(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	//int32_t t32;
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	if(fc==__bHZRQ_CBFC_DATA_READ){
		
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		retValue=__hzrq_ERR_WRITE_PARAM;
	}	
	

	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if(retValue==__hzrq_ERR_NONE){
		t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);	
		__hzrq_dfdNbModuleSwVer_t* stb=(__hzrq_dfdNbModuleSwVer_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
		__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_NBMODULE_SWVER);
		//__hzrq_swap_load_t16(stb->realNearfcn,hzrqRealNearFcn);
		m_mem_cpy_len(stb->nbModuleSoftVer,hzrqNbModuleSoftVer,20);
		t16+=sizeof(__hzrq_dfdNbModuleSwVer_t);
		t16+=3;
		t16=__hzrq_load_frame_mod_len(sbuf,t16);	
		
		t16=_hzrq_load_frame_crc_append(sbuf,t16);				
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_NBMODULE_SWVER,retValue,false,false);
	}
	return t16;		
}

int16_t hzrq_ins_rw_nb_module_type(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	//int32_t t32;
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	if(fc==__bHZRQ_CBFC_DATA_READ){
		
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		retValue=__hzrq_ERR_WRITE_PARAM;
	}	
	

	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if(retValue==__hzrq_ERR_NONE){
		t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);	
		__hzrq_dfdNbModuleType_t* stb=(__hzrq_dfdNbModuleType_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
		__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_NBMODULE_TYPE);
		//__hzrq_swap_load_t16(stb->realNearfcn,hzrqRealNearFcn);
		m_mem_cpy_len(stb->nbModuleType,hzrqNbModuleType,10);
		t16+=sizeof(__hzrq_dfdNbModuleType_t);
		t16+=3;
		t16=__hzrq_load_frame_mod_len(sbuf,t16);	
		
		t16=_hzrq_load_frame_crc_append(sbuf,t16);				
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_NBMODULE_TYPE,retValue,false,false);
	}
	return t16;		
}

int16_t hzrq_ins_rw_cust_dev_sta(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	//int32_t t32;
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	if(fc==__bHZRQ_CBFC_DATA_READ){
		
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		retValue=__hzrq_ERR_WRITE_PARAM;
	}	
	

	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if(retValue==__hzrq_ERR_NONE){
		t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);	
		__hzrq_dfdCustomDeviceSta_t* stb=(__hzrq_dfdCustomDeviceSta_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
		__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_CUSTOM_DEVICE_STA);
		__hzrq_swap_load_t32(stb->customDeviceSta,sysData.devStatus.t32);
		//m_mem_cpy_len(stb->nbModuleType,hzrqNbModuleType,10);
		t16+=sizeof(__hzrq_dfdCustomDeviceSta_t);
		t16+=3;
		t16=__hzrq_load_frame_mod_len(sbuf,t16);	
		
		t16=_hzrq_load_frame_crc_append(sbuf,t16);				
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_CUSTOM_DEVICE_STA,retValue,false,false);
	}
	return t16;		
}
int16_t hzrq_ins_rw_vendor_id(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	//int32_t t32;
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	if(fc==__bHZRQ_CBFC_DATA_READ){
		
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		retValue=__hzrq_ERR_WRITE_PARAM;
	}	
	

	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if(retValue==__hzrq_ERR_NONE){
		t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);	
		__hzrq_dfdVenderId_t* stb=(__hzrq_dfdVenderId_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
		__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_VENDER_ID);
		__hzrq_swap_load_t16(stb->venderCode,sysData.vendorCode);
		t16+=sizeof(__hzrq_dfdVenderId_t);
		t16+=3;
		t16=__hzrq_load_frame_mod_len(sbuf,t16);	
		
		t16=_hzrq_load_frame_crc_append(sbuf,t16);				
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_VENDER_ID,retValue,false,false);
	}
	return t16;		
}
int16_t hzrq_ins_rw_app_swver(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	//int32_t t32;
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	if(fc==__bHZRQ_CBFC_DATA_READ){
		
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		retValue=__hzrq_ERR_WRITE_PARAM;
	}	
	

	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if(retValue==__hzrq_ERR_NONE){
		t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);	
		__hzrq_dfdAppSwVer_t* stb=(__hzrq_dfdAppSwVer_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
		__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_APPSW_VER);
		//__hzrq_swap_load_t16(stb->venderCode,sysData.vendorCode);
		//m_mem_cpy_len(stb->appSoftVer,sysData.hzrqAppSwVer,4);
		stb->appSoftVer[0]=0;
		stb->appSoftVer[1]=m_hex_2_bcd(NB20E11_SUB_VER);
		stb->appSoftVer[2]=m_hex_2_bcd(config_NB_PLAT + 1);
		stb->appSoftVer[3]=m_hex_2_bcd(sw_VER_NUM);
		t16+=sizeof(__hzrq_dfdAppSwVer_t);
		t16+=3;
		t16=__hzrq_load_frame_mod_len(sbuf,t16);	
		
		t16=_hzrq_load_frame_crc_append(sbuf,t16);				
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_APPSW_VER,retValue,false,false);
	}
	return t16;		
}

int16_t hzrq_ins_rw_device_type(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	//int32_t t32;
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	if(fc==__bHZRQ_CBFC_DATA_READ){
		
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		retValue=__hzrq_ERR_WRITE_PARAM;
	}	
	

	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if(retValue==__hzrq_ERR_NONE){
		t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);	
		__hzrq_dfdDeviceType_t* stb=(__hzrq_dfdDeviceType_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
		__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_DEVICE_TYPE);
		__hzrq_swap_load_t16(stb->deviceType,sysData.hzrqDeviceType);
		//m_mem_cpy_len(stb->appSoftVer,sysData.hzrqAppSwVer,4);
		t16+=sizeof(__hzrq_dfdDeviceType_t);
		t16+=3;
		t16=__hzrq_load_frame_mod_len(sbuf,t16);	
		
		t16=_hzrq_load_frame_crc_append(sbuf,t16);				
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_DEVICE_TYPE,retValue,false,false);
	}
	return t16;		
}
int16_t hzrq_ins_rw_device_id(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	//int32_t t32;
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	if(fc==__bHZRQ_CBFC_DATA_READ){
		
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		retValue=__hzrq_ERR_WRITE_PARAM;
	}	
	

	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if(retValue==__hzrq_ERR_NONE){
		t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);	
		__hzrq_dfdDeviceId_t* stb=(__hzrq_dfdDeviceId_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
		__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_DEVICE_ID);
		
		stb->deviceIdLen=sysData.hzrqMeterIdLen;
		m_mem_cpy_len(stb->hazqMeterID,sysData.hzrqMeterId,32);
		t16+=sizeof(__hzrq_dfdDeviceId_t);
		t16+=3;
		t16=__hzrq_load_frame_mod_len(sbuf,t16);	
		
		t16=_hzrq_load_frame_crc_append(sbuf,t16);				
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_DEVICE_ID,retValue,false,false);
	}
	return t16;		
}

int16_t hzrq_ins_rw_valve_has(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	//int32_t t32;
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	if(fc==__bHZRQ_CBFC_DATA_READ){
		
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		retValue=__hzrq_ERR_WRITE_PARAM;
	}	
	

	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if(retValue==__hzrq_ERR_NONE){
		t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);	
		__hzrq_dfdHasValve_t* stb=(__hzrq_dfdHasValve_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
		__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_VALVE_HAS);
		stb->fiHasValve=__hzrq_HAS_VALVE;

		t16+=sizeof(__hzrq_dfdHasValve_t);
		t16+=3;
		t16=__hzrq_load_frame_mod_len(sbuf,t16);	
		
		t16=_hzrq_load_frame_crc_append(sbuf,t16);				
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_VALVE_HAS,retValue,false,false);
	}
	return t16;		
}

int16_t hzrq_ins_rw_comm_way(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	//int32_t t32;
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	if(fc==__bHZRQ_CBFC_DATA_READ){
		
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		retValue=__hzrq_ERR_WRITE_PARAM;
	}	
	

	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if(retValue==__hzrq_ERR_NONE){
		t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);	
		__hzrq_dfdCommWay_t* stb=(__hzrq_dfdCommWay_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
		__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_COMM_WAY);
		stb->commWay=__hzrq_DFID_COMM_WAY_NB;

		t16+=sizeof(__hzrq_dfdCommWay_t);
		t16+=3;
		t16=__hzrq_load_frame_mod_len(sbuf,t16);	
		
		t16=_hzrq_load_frame_crc_append(sbuf,t16);				
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_COMM_WAY,retValue,false,false);
	}
	return t16;		
}

int16_t hzrq_ins_rw_report_param(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	//
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	__hzrq_dfdReportParam_t* stb;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	if(fc==__bHZRQ_CBFC_DATA_READ){
		
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		//mac
		t16=_hzrq_received_mac_verify(rbuf,rlen);
		if(t16==0){return -1;}	
		//解密
		_hzrq_received_decrypt(rbuf,rlen);	
		stb=(__hzrq_dfdReportParam_t*)(rbuf+sizeof(__hzrq_frameHerder_t));
		api_sysdata_save();
		__disable_irq();
		//sysData.hzrqAccountSta=stb->accountSta;
		sysData.hzrqPeriodType=stb->periodType;
		sysData.hzrqPeriodValue=stb->periodValue;
		sysData.hzrqReportHour=stb->hour;
		sysData.hzrqReportMinute=stb->minute;
		__enable_irq();	
		api_sysdata_save();		
	
	}
	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if((fc==__bHZRQ_CBFC_DATA_READ || fc==__bHZRQ_CBFC_WRITE_FEEDBACK) && (retValue==__hzrq_ERR_NONE)){
		//t16=hzrq_load_rw_valve(sbuf,ssize,cb.b);
		t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);	
		stb=(__hzrq_dfdReportParam_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
		__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_REPORT_PARAM);
		stb->periodType=sysData.hzrqPeriodType;
		stb->periodValue=sysData.hzrqPeriodValue;
		stb->hour=sysData.hzrqReportHour;
		stb->minute=sysData.hzrqReportMinute;

		t16+=sizeof(__hzrq_dfdReportParam_t);
		t16+=3;
		t16=__hzrq_load_frame_mod_len(sbuf,t16);
		
		if(fc==__bHZRQ_CBFC_DATA_READ){
			
		}else if(fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
			//
			t16=__hzrq_load_rw_fb_insert_retvalue(sbuf,t16,0);
			t16=_hzrq_load_frame_encrypt(sbuf,t16);
		}

		t16=_hzrq_load_frame_crc_append(sbuf,t16);
			
		
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_REPORT_PARAM,retValue,false,false);
	}
	return t16;

}

int16_t hzrq_ins_rw_sever_addr(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	//
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	__hzrq_dfdSeversParam_t* stb;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	if(fc==__bHZRQ_CBFC_DATA_READ){
		
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		//mac
		t16=_hzrq_received_mac_verify(rbuf,rlen);
		if(t16==0){return -1;}	
		//解密
		_hzrq_received_decrypt(rbuf,rlen);	
		stb=(__hzrq_dfdSeversParam_t*)(rbuf+sizeof(__hzrq_frameHerder_t));
		api_sysdata_save();
		__disable_irq();
		sysData.hzrqSeverPort=__hzrq_swap_get_t16(stb->port);
		m_mem_cpy_len(sysData.hzrqSeverAddr,stb->addr,16);
		__enable_irq();	
		api_sysdata_save();		
	
	}
	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if((fc==__bHZRQ_CBFC_DATA_READ || fc==__bHZRQ_CBFC_WRITE_FEEDBACK) && (retValue==__hzrq_ERR_NONE)){
		//t16=hzrq_load_rw_valve(sbuf,ssize,cb.b);
		t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);	
		stb=(__hzrq_dfdSeversParam_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
		__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_SEVERS_ADDR_PARAM);
		__hzrq_swap_load_t16(stb->port,sysData.hzrqSeverPort);
		m_mem_cpy_len(stb->addr,sysData.hzrqSeverAddr,16);

		t16+=sizeof(__hzrq_dfdSeversParam_t);
		t16+=3;
		t16=__hzrq_load_frame_mod_len(sbuf,t16);
		
		if(fc==__bHZRQ_CBFC_DATA_READ){
			
		}else if(fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
			//
			t16=__hzrq_load_rw_fb_insert_retvalue(sbuf,t16,0);
			t16=_hzrq_load_frame_encrypt(sbuf,t16);
		}

		t16=_hzrq_load_frame_crc_append(sbuf,t16);
			
		
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_SEVERS_ADDR_PARAM,retValue,false,false);
	}
	return t16;
}

int16_t hzrq_ins_rw_settle_mode(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	//int32_t t32;
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	if(fc==__bHZRQ_CBFC_DATA_READ){
		
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		retValue=__hzrq_ERR_WRITE_PARAM;
	}	
	

	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if(retValue==__hzrq_ERR_NONE){
		t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);	
		__hzrq_dfdSettleMode_t* stb=(__hzrq_dfdSettleMode_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
		__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_SETTLE_MODE);
		stb->settleMode=__hzrq_SETTLE_MODE_M;
	
		t16+=sizeof(__hzrq_dfdSettleMode_t);
		t16+=3;
		t16=__hzrq_load_frame_mod_len(sbuf,t16);	
		
		t16=_hzrq_load_frame_crc_append(sbuf,t16);				
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_SETTLE_MODE,retValue,false,false);
	}
	return t16;		
}

int16_t hzrq_ins_rw_key(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	//
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	__hzrq_dfdKeyParam_t* stb;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	if(fc==__bHZRQ_CBFC_DATA_READ){
		
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		//mac
		t16=_hzrq_received_mac_verify(rbuf,rlen);
		if(t16==0){return -1;}	
		//解密
		_hzrq_received_decrypt(rbuf,rlen);	
		stb=(__hzrq_dfdKeyParam_t*)(rbuf+sizeof(__hzrq_frameHerder_t));
		api_sysdata_save();
		__disable_irq();
		sysData.hzrqKeyLen=stb->keyLen;
		sysData.hzrqKeyVer=stb->keyVer;
		m_mem_cpy_len(sysData.key,stb->key,32);
		__enable_irq();	
		api_sysdata_save();		
	
	}
	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if((fc==__bHZRQ_CBFC_DATA_READ || fc==__bHZRQ_CBFC_WRITE_FEEDBACK) && (retValue==__hzrq_ERR_NONE)){
		//t16=hzrq_load_rw_valve(sbuf,ssize,cb.b);
		t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);	
		stb=(__hzrq_dfdKeyParam_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
		__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_KEY_PARAM);
		stb->keyLen=sysData.hzrqKeyLen;
		stb->keyVer=sysData.hzrqKeyVer;
		m_mem_cpy_len(stb->key,sysData.key,32);

		t16+=sizeof(__hzrq_dfdKeyParam_t);
		t16+=3;
		t16=__hzrq_load_frame_mod_len(sbuf,t16);
		
		if(fc==__bHZRQ_CBFC_DATA_READ){
		}else if(fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
			//
			t16=__hzrq_load_rw_fb_insert_retvalue(sbuf,t16,0);
			t16=_hzrq_load_frame_encrypt(sbuf,t16);
		}
		t16=_hzrq_load_frame_crc_append(sbuf,t16);
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_KEY_PARAM,retValue,false,false);
	}
	return t16;

}
int16_t hzrq_ins_rw_iccid(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	//int32_t t32;
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	if(fc==__bHZRQ_CBFC_DATA_READ){
		
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		retValue=__hzrq_ERR_WRITE_PARAM;
	}	
	

	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if(retValue==__hzrq_ERR_NONE){
		t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);	
		__hzrq_dfdIccid_t* stb=(__hzrq_dfdIccid_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
		__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_ICCID);
		//stb->settleMode=__hzrq_SETTLE_MODE_M;
		m_mem_cpy_len(stb->iccid,ICCID,20);
		t16+=sizeof(__hzrq_dfdIccid_t);
		t16+=3;
		t16=__hzrq_load_frame_mod_len(sbuf,t16);	
		
		t16=_hzrq_load_frame_crc_append(sbuf,t16);				
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_ICCID,retValue,false,false);
	}
	return t16;		
}
int16_t hzrq_ins_rw_mb_sever_inf(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	//int32_t t32;
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	if(fc==__bHZRQ_CBFC_DATA_READ){
		
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		retValue=__hzrq_ERR_WRITE_PARAM;
	}	
	

	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if(retValue==__hzrq_ERR_NONE){
		t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);	
		__hzrq_dfdMobleSeverInf_t* stb=(__hzrq_dfdMobleSeverInf_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
		__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_MOBLE_SEVERS_INF);
		stb->mobleSeverInf=__hzrq_MOBLE_SEVERS_CTCC;

		t16+=sizeof(__hzrq_dfdMobleSeverInf_t);
		t16+=3;
		t16=__hzrq_load_frame_mod_len(sbuf,t16);	
		
		t16=_hzrq_load_frame_crc_append(sbuf,t16);				
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_MOBLE_SEVERS_INF,retValue,false,false);
	}
	return t16;		
}
int16_t hzrq_ins_rw_app_protoclo_ver(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	//int32_t t32;
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	if(fc==__bHZRQ_CBFC_DATA_READ){
		
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		retValue=__hzrq_ERR_WRITE_PARAM;
	}	
	

	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if(retValue==__hzrq_ERR_NONE){
		t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);	
		__hzrq_dfdAppProtocolVer_t* stb=(__hzrq_dfdAppProtocolVer_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
		__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_APP_PROTOCOL_VER);
		m_mem_cpy_len(stb->appProtocolVer,sysData.hzrqAppProtocolVer,2);

		t16+=sizeof(__hzrq_dfdAppProtocolVer_t);
		t16+=3;
		t16=__hzrq_load_frame_mod_len(sbuf,t16);	
		
		t16=_hzrq_load_frame_crc_append(sbuf,t16);				
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_APP_PROTOCOL_VER,retValue,false,false);
	}
	return t16;		
}

int16_t hzrq_ins_rw_power_type(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	//int32_t t32;
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	if(fc==__bHZRQ_CBFC_DATA_READ){
		
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		retValue=__hzrq_ERR_WRITE_PARAM;
	}	
	

	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if(retValue==__hzrq_ERR_NONE){
		t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);	
		__hzrq_dfdPowerType_t* stb=(__hzrq_dfdPowerType_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
		__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_POWER_TYPE);
		stb->powerType=__hzrq_POWER_TYPE_LISOCL;
		t16+=sizeof(__hzrq_dfdPowerType_t);
		t16+=3;
		t16=__hzrq_load_frame_mod_len(sbuf,t16);	
		
		t16=_hzrq_load_frame_crc_append(sbuf,t16);				
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_POWER_TYPE,retValue,false,false);
	}
	return t16;		
}

int16_t hzrq_ins_rw_refusepeek(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	//
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	__hzrq_dfdRefusesPeekInterval_t* stb;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	if(fc==__bHZRQ_CBFC_DATA_READ){
		
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		//mac
		t16=_hzrq_received_mac_verify(rbuf,rlen);
		if(t16==0){return -1;}	
		//解密
		_hzrq_received_decrypt(rbuf,rlen);	
		stb=(__hzrq_dfdRefusesPeekInterval_t*)(rbuf+sizeof(__hzrq_frameHerder_t));
		api_sysdata_save();
		__disable_irq();
		sysData.hzrqRefusesPeekInterval=__hzrq_swap_get_t16(stb->refusesPeekInterval);;
		__enable_irq();	
		api_sysdata_save();		
	
	}
	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if((fc==__bHZRQ_CBFC_DATA_READ || fc==__bHZRQ_CBFC_WRITE_FEEDBACK) && (retValue==__hzrq_ERR_NONE)){
		//t16=hzrq_load_rw_valve(sbuf,ssize,cb.b);
		t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);	
		stb=(__hzrq_dfdRefusesPeekInterval_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
		__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_REFUSESPEEK_INTERVAL);
		__hzrq_swap_load_t16(stb->refusesPeekInterval,sysData.hzrqRefusesPeekInterval);

		t16+=sizeof(__hzrq_dfdRefusesPeekInterval_t);
		t16+=3;
		t16=__hzrq_load_frame_mod_len(sbuf,t16);
		
		if(fc==__bHZRQ_CBFC_DATA_READ){
			
		}else if(fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
			//
			t16=__hzrq_load_rw_fb_insert_retvalue(sbuf,t16,0);
			t16=_hzrq_load_frame_encrypt(sbuf,t16);
		}

		t16=_hzrq_load_frame_crc_append(sbuf,t16);
			
		
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_REFUSESPEEK_INTERVAL,retValue,false,false);
	}
	return t16;
}

int16_t hzrq_ins_rw_onflow_tmout(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	//
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	__hzrq_dfdNoFlowTimeOut_t* stb;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	if(fc==__bHZRQ_CBFC_DATA_READ){
		
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		//mac
		t16=_hzrq_received_mac_verify(rbuf,rlen);
		if(t16==0){return -1;}	
		//解密
		_hzrq_received_decrypt(rbuf,rlen);	
		stb=(__hzrq_dfdNoFlowTimeOut_t*)(rbuf+sizeof(__hzrq_frameHerder_t));
		api_sysdata_save();
		__disable_irq();
		sysData.qcNoFlowTimeOut=(uint16_t)(stb->noFlowTimeOut)*24;
		sysData.qcNoFlowTimeOutEx=0;
		__enable_irq();	
		api_sysdata_save();		
	
	}
	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if((fc==__bHZRQ_CBFC_DATA_READ || fc==__bHZRQ_CBFC_WRITE_FEEDBACK) && (retValue==__hzrq_ERR_NONE)){
		//t16=hzrq_load_rw_valve(sbuf,ssize,cb.b);
		t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);	
		stb=(__hzrq_dfdNoFlowTimeOut_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
		__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_NOFLOW_TIMEOUT);
		//__hzrq_swap_load_t16(stb->refusesPeekInterval,sysData.hzrqRefusesPeekInterval);
		stb->noFlowTimeOut=(uint8_t)(sysData.qcNoFlowTimeOut/24);
		t16+=sizeof(__hzrq_dfdNoFlowTimeOut_t);
		t16+=3;
		t16=__hzrq_load_frame_mod_len(sbuf,t16);
		
		if(fc==__bHZRQ_CBFC_DATA_READ){
			
		}else if(fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
			//
			t16=__hzrq_load_rw_fb_insert_retvalue(sbuf,t16,0);
			t16=_hzrq_load_frame_encrypt(sbuf,t16);
		}

		t16=_hzrq_load_frame_crc_append(sbuf,t16);
			
		
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_NOFLOW_TIMEOUT,retValue,false,false);
	}
	return t16;
}

int16_t hzrq_ins_rw_onconn_tmout(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	//
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	uint16_t tm;
	__hzrq_dfdNoConnectTimeOut_t* stb;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	if(fc==__bHZRQ_CBFC_DATA_READ){
		
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		//mac
		t16=_hzrq_received_mac_verify(rbuf,rlen);
		if(t16==0){return -1;}	
		//解密
		_hzrq_received_decrypt(rbuf,rlen);	
		stb=(__hzrq_dfdNoConnectTimeOut_t*)(rbuf+sizeof(__hzrq_frameHerder_t));
		api_sysdata_save();
		__disable_irq();
		tm=stb->noConnectTimeOut;
		tm*=24;
		sysData.unNetTimeOut=tm;
		__enable_irq();	
		api_sysdata_save();		
	
	}
	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if((fc==__bHZRQ_CBFC_DATA_READ || fc==__bHZRQ_CBFC_WRITE_FEEDBACK) && (retValue==__hzrq_ERR_NONE)){
		//t16=hzrq_load_rw_valve(sbuf,ssize,cb.b);
		t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);	
		stb=(__hzrq_dfdNoConnectTimeOut_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
		__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_NOCONN_TIMEOUT);
		tm=sysData.unNetTimeOut;
		tm/=24;
		stb->noConnectTimeOut=(uint8_t)(tm);
		t16+=sizeof(__hzrq_dfdNoConnectTimeOut_t);
		t16+=3;
		t16=__hzrq_load_frame_mod_len(sbuf,t16);
		
		if(fc==__bHZRQ_CBFC_DATA_READ){
			
		}else if(fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
			//
			t16=__hzrq_load_rw_fb_insert_retvalue(sbuf,t16,0);
			t16=_hzrq_load_frame_encrypt(sbuf,t16);
		}

		t16=_hzrq_load_frame_crc_append(sbuf,t16);
			
		
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_NOCONN_TIMEOUT,retValue,false,false);
	}
	return t16;
}

int16_t hzrq_ins_rw_hiflow_protect(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	//
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	uint16_t tm;
	__hzrq_dfdhiFlowWarningEn_t* stb;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	if(fc==__bHZRQ_CBFC_DATA_READ){
		
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		//mac
		t16=_hzrq_received_mac_verify(rbuf,rlen);
		if(t16==0){return -1;}	
		//解密
		_hzrq_received_decrypt(rbuf,rlen);	
		stb=(__hzrq_dfdhiFlowWarningEn_t*)(rbuf+sizeof(__hzrq_frameHerder_t));
		api_sysdata_save();
		__disable_irq();
		sysData.HightFlowProtectEn=stb->hiFlowWarningEn;
		__enable_irq();	
		api_sysdata_save();		
	
	}
	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if((fc==__bHZRQ_CBFC_DATA_READ || fc==__bHZRQ_CBFC_WRITE_FEEDBACK) && (retValue==__hzrq_ERR_NONE)){
		//t16=hzrq_load_rw_valve(sbuf,ssize,cb.b);
		t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);	
		stb=(__hzrq_dfdhiFlowWarningEn_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
		__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_HIFLOW_WARNING_EN);
		
		stb->hiFlowWarningEn=sysData.HightFlowProtectEn;
		
		t16+=sizeof(__hzrq_dfdhiFlowWarningEn_t);
		t16+=3;
		t16=__hzrq_load_frame_mod_len(sbuf,t16);
		
		if(fc==__bHZRQ_CBFC_DATA_READ){
			
		}else if(fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
			//
			t16=__hzrq_load_rw_fb_insert_retvalue(sbuf,t16,0);
			t16=_hzrq_load_frame_encrypt(sbuf,t16);
		}

		t16=_hzrq_load_frame_crc_append(sbuf,t16);
			
		
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_HIFLOW_WARNING_EN,retValue,false,false);
	}
	return t16;
}

int16_t hzrq_ins_rw_apn(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	//
	uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint8_t fc;
	uint16_t tm;
	__hzrq_dfdApn_t* stb;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}	
	fc=_hzrq_get_func_code(rbuf);
	if(fc==__bHZRQ_CBFC_DATA_READ){
		
	}else if(fc==__bHZRQ_CBFC_DATA_WRITE || fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
		//mac
		t16=_hzrq_received_mac_verify(rbuf,rlen);
		if(t16==0){return -1;}	
		//解密
		_hzrq_received_decrypt(rbuf,rlen);	
		stb=(__hzrq_dfdApn_t*)(rbuf+sizeof(__hzrq_frameHerder_t));
		api_sysdata_save();
		__disable_irq();
		m_mem_cpy_len(sysData.hzrqApn,stb->apn,32);
		__enable_irq();	
		api_sysdata_save();		
	
	}
	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	
	if((fc==__bHZRQ_CBFC_DATA_READ || fc==__bHZRQ_CBFC_WRITE_FEEDBACK) && (retValue==__hzrq_ERR_NONE)){
		//t16=hzrq_load_rw_valve(sbuf,ssize,cb.b);
		t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);	
		stb=(__hzrq_dfdApn_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
		__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_APN);
		m_mem_cpy_len(stb->apn,sysData.hzrqApn,32);
	
		t16+=sizeof(__hzrq_dfdApn_t);
		t16+=3;
		t16=__hzrq_load_frame_mod_len(sbuf,t16);
		
		if(fc==__bHZRQ_CBFC_DATA_READ){
			
		}else if(fc==__bHZRQ_CBFC_WRITE_FEEDBACK){
			//
			t16=__hzrq_load_rw_fb_insert_retvalue(sbuf,t16,0);
			t16=_hzrq_load_frame_encrypt(sbuf,t16);
		}

		t16=_hzrq_load_frame_crc_append(sbuf,t16);
			
		
	}else{
		t16=hzrq_load_rw_common_Reply(sbuf,ssize,cb.b,__hzrq_DFID_APN,retValue,false,false);
	}
	return t16;
}

int16_t hzrq_ins_rd_event_log(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	
	uint32_t tmStart,tmEnd;
	
	__hzrq_dfdReadEventLogReq_t* stb;
	//uint16_t t16,retValue=__hzrq_ERR_NONE;
	uint16_t logLen,t16,readEventCode;
	uint16_t tm;
	uint8_t fc,retNum,readNum;
	uint8_t* p;
	
	
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}		
	fc=_hzrq_get_func_code(rbuf);
	
	stb=(__hzrq_dfdReadEventLogReq_t*)(rbuf+sizeof(__hzrq_frameHerder_t));
	//计算起始时间和结束时间
	tmStart=__hzrq_download_rtc(stb->startDt);tmStart/=86400UL;tmStart*=86400UL;
	//tmStart-=(tmStart)
	tmEnd=__hzrq_download_rtc(stb->endDt);tmEnd/=86400UL;tmEnd*=86400UL;tmEnd+=86400UL;
	
	readEventCode=__hzrq_swap_get_t16(stb->eventCode);
	readNum=stb->readNum;
	
	//eventLog_t evetnlog;
	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);

	p=sbuf+sizeof(__hzrq_frameHerder_t);
	__hzrq_swap_load_t16(p,__hzrq_DFID_READ_EVENTLOG);
	t16+=2;
	
	p=sbuf+sizeof(__hzrq_frameHerder_t)+3;
	recordReadStartTs=0x00UL;
	logLen=record_read_eventlog_start_to_end(p,ssize-32,tmStart,tmEnd,readEventCode,readNum);
	
	p=sbuf+sizeof(__hzrq_frameHerder_t)+2;
	*p=logLen/sizeof(__hzrq_eventLog_t);
	t16+=1;
	
	t16=logLen+t16+3;

	t16=__hzrq_load_frame_mod_len(sbuf,t16);	
	t16=_hzrq_load_frame_crc_append(sbuf,t16);
	return t16;
}

int16_t hzrq_ins_rd_event_log_new(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	uint8_t* p;
	int16_t t16,logLen;
	uint8_t fc,readNum;
	int16_t readEventCode;
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}		
	fc=_hzrq_get_func_code(rbuf);
	
	__hzrq_dfdReadEventLogNewReq_t* stb=(__hzrq_dfdReadEventLogNewReq_t*)(rbuf+sizeof(__hzrq_frameHerder_t));
	readEventCode=__hzrq_swap_get_t16(stb->eventCode);
	readNum=stb->eventItemsNum;
	
	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);

	p=sbuf+sizeof(__hzrq_frameHerder_t);
	__hzrq_swap_load_t16(p,__hzrq_DFID_READ_EVENTLOG_NEW);
	t16+=2;	
	
	p=sbuf+sizeof(__hzrq_frameHerder_t)+3;
	recordReadStartTs=0x00UL;
	logLen=record_read_eventlog_new(p,ssize-32,readEventCode,readNum);	
	
	p=sbuf+sizeof(__hzrq_frameHerder_t)+2;
	*p=logLen/sizeof(__hzrq_eventLog_t);
	t16+=1;

	t16=logLen+t16+3;

	t16=__hzrq_load_frame_mod_len(sbuf,t16);	
	t16=_hzrq_load_frame_crc_append(sbuf,t16);
	return t16;
}

int16_t hzrq_ins_rd_vol_log_hour(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	uint8_t* p;
	uint32_t tmStart,tmEnd;
	int16_t t16,logLen,reqReadItems;
	uint8_t fc,readDay;
	uint8_t rtBuf[6]={0};
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}		
	fc=_hzrq_get_func_code(rbuf);	
	
	__hzrq_dfdReadVolLogDayReq_t* stb=(__hzrq_dfdReadVolLogDayReq_t*)(rbuf+sizeof(__hzrq_frameHerder_t));
	readDay=stb->days;
	reqReadItems=stb->days;
	reqReadItems*=24;
	m_mem_cpy_len(rtBuf,stb->startDate,3);
	tmStart=__hzrq_download_rtc(rtBuf);
	tmStart/=86400UL;
	tmStart*=86400UL;
	
	tmEnd=tmStart+(uint32_t)readDay*86400UL;
	
	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);
	
	
	__hzrq_dfdReadVolLogDayReply_t* sts=(__hzrq_dfdReadVolLogDayReply_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
	__hzrq_swap_load_t16(sts->iden,__hzrq_DFID_READ_VOLLOG_HOUR);

	m_mem_cpy_len(sts->startDate,stb->startDate,3);
	t16+=sizeof(__hzrq_dfdReadVolLogDayReply_t);
	
	
	p=sbuf+t16;
	recordReadStartTs=0x00UL;
	logLen=record_read_vol_log_hour_start_end(p,ssize-32,tmStart,tmEnd);	
	logLen-=(logLen%(24*4));
	t16+=logLen;
	
	sysDataTime_t dt={0};
	time_stamp_to_system_dt(recordReadStartTs,&dt);
	sts->startDate[0]=dt.YY;
	sts->startDate[1]=dt.MM;
	sts->startDate[2]=dt.DD;
	
	sts->days=(uint8_t)logLen/(24*4);
	t16+=3;

	t16=__hzrq_load_frame_mod_len(sbuf,t16);	
	t16=_hzrq_load_frame_encrypt(sbuf,t16);	
	t16=_hzrq_load_frame_crc_append(sbuf,t16);
	return t16;	
}

int16_t hzrq_ins_rd_vol_log_day(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	uint8_t* p;
	uint32_t tmStart,tmEnd;
	int16_t t16,logLen,reqReadItems;
	uint8_t fc,readDay;
	uint8_t rtBuf[6]={0};
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}		
	fc=_hzrq_get_func_code(rbuf);	
	
	__hzrq_dfdReadVolLogDayReq_t* stb=(__hzrq_dfdReadVolLogDayReq_t*)(rbuf+sizeof(__hzrq_frameHerder_t));
	readDay=stb->days;
	reqReadItems=stb->days;
	reqReadItems*=24;
	m_mem_cpy_len(rtBuf,stb->startDate,3);
	tmStart=__hzrq_download_rtc(rtBuf);
	tmStart/=86400UL;
	tmStart*=86400UL;
	
	tmEnd=tmStart+(uint32_t)readDay*86400UL;
	
	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);
	
	
	__hzrq_dfdReadVolLogDayReply_t* sts=(__hzrq_dfdReadVolLogDayReply_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
	__hzrq_swap_load_t16(sts->iden,__hzrq_DFID_READ_VOLLOG_DAY);

	m_mem_cpy_len(sts->startDate,stb->startDate,3);
	t16+=sizeof(__hzrq_dfdReadVolLogDayReq_t);
	
	
	p=sbuf+t16;
	recordReadStartTs=0x00UL;
	logLen=record_read_vol_log_day_start_end(p,ssize-32,tmStart,tmEnd);	
	logLen-=(logLen%(1*4));
	t16+=logLen;

	sysDataTime_t dt={0};
	time_stamp_to_system_dt(recordReadStartTs,&dt);
	sts->startDate[0]=dt.YY;
	sts->startDate[1]=dt.MM;
	sts->startDate[2]=dt.DD;

	sts->days=(uint8_t)logLen/(1*4);
	t16+=3;

	t16=__hzrq_load_frame_mod_len(sbuf,t16);	
	t16=_hzrq_load_frame_encrypt(sbuf,t16);	
	t16=_hzrq_load_frame_crc_append(sbuf,t16);
	return t16;	
}

int16_t hzrq_ins_rd_vol_log_month(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	uint8_t* p;
	uint32_t tmStart,tmEnd;
	int16_t t16,logLen,reqReadItems;
	uint8_t fc;
	uint8_t rtBuf[6]={0x00,0x01,0x01,0x00,0x00,0x00};
	t16=_hzrq_received_crc_verify(rbuf,rlen);
	if(t16==0){return -1;}		
	fc=_hzrq_get_func_code(rbuf);	
	
	__hzrq_dfdReadVolLogMonthReq_t* stb=(__hzrq_dfdReadVolLogMonthReq_t*)(rbuf+sizeof(__hzrq_frameHerder_t));
	//readDay=stb->days;
	//reqReadItems=stb->days;
	//reqReadItems*=24;
	reqReadItems=12;
	//m_mem_cpy_len(rtBuf,stb->startDate,3);
	rtBuf[0]=stb->year;
	tmStart=__hzrq_download_rtc(rtBuf);
	rtBuf[1]=0x12;
	tmEnd=__hzrq_download_rtc(rtBuf);
	
	__hzrq_ctrlByteDef_t cb;
	cb.b=0;
	cb.bits.bFuncCode=fc;
	cb.bits.bDir=__bHZRQ_CBDIR_UP;
	cb.bits.bhasMore=__bHZRQ_CBHASMORE_OVER;
	t16=_hzrq_load_frame_header(sbuf,ssize,0,cb.b,__hzrqMid);
	
	
	__hzrq_dfdReadVolLogMonthReply_t* sts=(__hzrq_dfdReadVolLogMonthReply_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
	__hzrq_swap_load_t16(sts->iden,__hzrq_DFID_READ_VOLLOG_MONTH);

	//m_mem_cpy_len(sts->startDate,stb->startDate,3);
	sts->year=stb->year;
	t16+=sizeof(__hzrq_dfdReadVolLogMonthReply_t);
	
	
	p=sbuf+t16;
	recordReadStartTs=0x00UL;
	logLen=record_read_vol_log_month_start_end(p,ssize-32,tmStart,tmEnd);	
	//logLen-=(logLen%(1*4));
	t16+=logLen;

	sysDataTime_t dt={0};
	time_stamp_to_system_dt(recordReadStartTs,&dt);
	sts->year=dt.YY;


	//sts->days=(uint8_t)logLen/(1*4);
	t16+=3;

	t16=__hzrq_load_frame_mod_len(sbuf,t16);	
	t16=_hzrq_load_frame_encrypt(sbuf,t16);	
	t16=_hzrq_load_frame_crc_append(sbuf,t16);
	return t16;		
	
	return 0;
}

int16_t hzrq_ins_default(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	return 0;
}

int16_t hzrq_comm_received_process(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize,uint8_t popType)
{
	int16_t ret;
	uint16_t dfid=0xffff;
	uint8_t* pdfid=rbuf+sizeof(__hzrq_frameHerder_t);
	if(rlen<sizeof(__hzrq_frameHerder_t)+sizeof(__hzrq_frameTail_t)+1)return -1;
	//dfid=pdfid[0];dfid<<=8;dfid+=pdfid[1];
	dfid=(pdfid[0]<<8) + pdfid[1];
	__hzrqMid=rbuf[4];
	switch(dfid){
		case __hzrq_DFID_REGISTER:				ret=hzrq_ins_exe_register(rbuf,rlen,sbuf,ssize,popType);	break;
		case __hzrq_DFID_POP:					ret=hzrq_ins_exe_pop(rbuf,rlen,sbuf,ssize);					break;
		case __hzrq_DFID_END:					ret=hzrq_ins_exe_end(rbuf,rlen);							break;
		//表A.1.1 状态数据对象
		case __hzrq_DFID_VALVE_CTRL:			ret=hzrq_ins_rw_valve_sta(rbuf,rlen,sbuf,ssize);			break;
		case __hzrq_DFID_DATETIME:				ret=hzrq_ins_rw_rtc(rbuf,rlen,sbuf,ssize);					break;
		case __hzrq_DFID_CUR_TOLTAL_VOL:		ret=hzrq_ins_rw_total_vol(rbuf,rlen,sbuf,ssize);			break;
		case __hzrq_DFID_BAT_VOLTAGE:
		case __hzrq_DFID_BAT_VOLTAGE_EX:		ret=hzrq_ins_rw_bat_voltage(rbuf,rlen,sbuf,ssize,dfid);		break;
		case __hzrq_DFID_BAT_PERCENT:
		case __hzrq_DFID_BAT_PERCENT_EX:		ret=hzrq_ins_rw_bat_percent(rbuf,rlen,sbuf,ssize,dfid);		break;
		case __hzrq_DFID_REVERSE_UKN:			ret=hzrq_ins_rw_reverse_vol(rbuf,rlen,sbuf,ssize);			break;
		case __hzrq_DFID_BALANCE_VOL:			ret=hzrq_ins_rw_balence_vol(rbuf,rlen,sbuf,ssize);			break;
		case __hzrq_DFID_OVERDRAFT_STA:			ret=hzrq_ins_rw_overdraft_sta(rbuf,rlen,sbuf,ssize);		break;
		case __hzrq_DFID_DEVICE_STA:			ret=hzrq_ins_rw_device_sta(rbuf,rlen,sbuf,ssize);			break;
		case __hzrq_DFID_RANDOM_CODE:			ret=hzrq_ins_rw_random_code(rbuf,rlen,sbuf,ssize);			break;
		case __hzrq_DFID_PRICE:					ret=hzrq_ins_rw_price(rbuf,rlen,sbuf,ssize);				break;
		case __hzrq_DFID_ACCOUNT_STA:			ret=hzrq_ins_rw_account_sta(rbuf,rlen,sbuf,ssize);			break;
		case __hzrq_DFID_BALANCE_M:				ret=hzrq_ins_rw_balance_money(rbuf,rlen,sbuf,ssize);		break;
		case __hzrq_DFID_BALANCE_STA:			ret=hzrq_ins_rw_balance_sta(rbuf,rlen,sbuf,ssize);			break;
		case __hzrq_DFID_RSSI:					ret=hzrq_ins_rw_rssi(rbuf,rlen,sbuf,ssize);					break;
		case __hzrq_DFID_COMM_FAIL_TIMES:		ret=hzrq_ins_rw_comm_fails_times(rbuf,rlen,sbuf,ssize);		break;
		case __hzrq_DFID_ECL_LEVEL:				ret=hzrq_ins_rw_ecl_level(rbuf,rlen,sbuf,ssize);			break;
		case __hzrq_DFID_CELL_ID:				ret=hzrq_ins_rw_cell_id(rbuf,rlen,sbuf,ssize);				break;
		case __hzrq_DFID_REAL_NEARFCN:			ret=hzrq_ins_rw_near_fcn(rbuf,rlen,sbuf,ssize);				break;
		case __hzrq_DFID_IMEI:					ret=hzrq_ins_rw_imei(rbuf,rlen,sbuf,ssize);					break;
		case __hzrq_DFID_NBMODULE_SWVER:		ret=hzrq_ins_rw_nb_swver(rbuf,rlen,sbuf,ssize);				break;
		case __hzrq_DFID_NBMODULE_TYPE:			ret=hzrq_ins_rw_nb_module_type(rbuf,rlen,sbuf,ssize);		break;
		case __hzrq_DFID_CUSTOM_DEVICE_STA:		ret=hzrq_ins_rw_cust_dev_sta(rbuf,rlen,sbuf,ssize);			break;
		//A.3　设置数据
		case __hzrq_DFID_VENDER_ID:				ret=hzrq_ins_rw_vendor_id(rbuf,rlen,sbuf,ssize);			break;
		case __hzrq_DFID_APPSW_VER:				ret=hzrq_ins_rw_app_swver(rbuf,rlen,sbuf,ssize);			break;
		case __hzrq_DFID_DEVICE_TYPE:			ret=hzrq_ins_rw_device_type(rbuf,rlen,sbuf,ssize);			break;
		case __hzrq_DFID_DEVICE_ID:				ret=hzrq_ins_rw_device_id(rbuf,rlen,sbuf,ssize);			break;
		case __hzrq_DFID_VALVE_HAS:				ret=hzrq_ins_rw_valve_has(rbuf,rlen,sbuf,ssize);			break;
		case __hzrq_DFID_COMM_WAY:				ret=hzrq_ins_rw_comm_way(rbuf,rlen,sbuf,ssize);				break;
		case __hzrq_DFID_REPORT_PARAM:			ret=hzrq_ins_rw_report_param(rbuf,rlen,sbuf,ssize);			break;
		case __hzrq_DFID_SEVERS_ADDR_PARAM:		ret=hzrq_ins_rw_sever_addr(rbuf,rlen,sbuf,ssize);			break;
		case __hzrq_DFID_SETTLE_MODE:			ret=hzrq_ins_rw_settle_mode(rbuf,rlen,sbuf,ssize);			break;
		case __hzrq_DFID_KEY_PARAM:				ret=hzrq_ins_rw_key(rbuf,rlen,sbuf,ssize);					break;
		case __hzrq_DFID_ICCID:					ret=hzrq_ins_rw_iccid(rbuf,rlen,sbuf,ssize);				break;
		case __hzrq_DFID_MOBLE_SEVERS_INF:		ret=hzrq_ins_rw_mb_sever_inf(rbuf,rlen,sbuf,ssize);			break;
		case __hzrq_DFID_APP_PROTOCOL_VER:		ret=hzrq_ins_rw_app_protoclo_ver(rbuf,rlen,sbuf,ssize);		break;
		case __hzrq_DFID_POWER_TYPE:			ret=hzrq_ins_rw_power_type(rbuf,rlen,sbuf,ssize);			break;
		case __hzrq_DFID_REFUSESPEEK_INTERVAL:	ret=hzrq_ins_rw_refusepeek(rbuf,rlen,sbuf,ssize);			break;
		case __hzrq_DFID_NOFLOW_TIMEOUT:		ret=hzrq_ins_rw_onflow_tmout(rbuf,rlen,sbuf,ssize);			break;
		case __hzrq_DFID_NOCONN_TIMEOUT:		ret=hzrq_ins_rw_onconn_tmout(rbuf,rlen,sbuf,ssize);			break;
		case __hzrq_DFID_HIFLOW_WARNING_EN:		ret=hzrq_ins_rw_hiflow_protect(rbuf,rlen,sbuf,ssize);		break;
		case __hzrq_DFID_APN:					ret=hzrq_ins_rw_apn(rbuf,rlen,sbuf,ssize);					break;
		////A.2　记录数据
		case __hzrq_DFID_READ_EVENTLOG:			ret=hzrq_ins_rd_event_log(rbuf,rlen,sbuf,ssize);			break;
		case __hzrq_DFID_READ_EVENTLOG_NEW:		ret=hzrq_ins_rd_event_log_new(rbuf,rlen,sbuf,ssize);		break;
		case __hzrq_DFID_READ_VOLLOG_HOUR:		ret=hzrq_ins_rd_vol_log_hour(rbuf,rlen,sbuf,ssize);			break;
		case __hzrq_DFID_READ_VOLLOG_DAY:		ret=hzrq_ins_rd_vol_log_day(rbuf,rlen,sbuf,ssize);			break;
		case __hzrq_DFID_READ_VOLLOG_MONTH:		ret=hzrq_ins_rd_vol_log_month(rbuf,rlen,sbuf,ssize);		break;
		
		default:								ret=hzrq_ins_default(rbuf,rlen,sbuf,ssize);					break;
		
	}
	return ret;
}



//

//file end
