#include "./includes/includes.h"
volatile uint8_t __hzrqMsgSendSn=0x00;
volatile uint8_t __hzrqMid=0xff;
volatile int16_t hzrqRsrp=0;
volatile int16_t hzrqSnr=0;
volatile int8_t hzrqEclLevel=0;
uint8_t hzrqCellId[6]={0};
volatile uint16_t hzrqRealNearFcn=0x00;
uint8_t hzrqNbModuleType[10]={0};
uint8_t hzrqNbModuleSoftVer[20]={0};

uint8_t hzrqComBuf[256];

uint16_t hzrqErrorCode=0x00;
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
	
uint16_t _hzrq_crc_append_send(uint8_t* sbuf,uint16_t slen)
{
	__hzrq_crc_append(sbuf+sizeof(__hzrq_frameHerder_t)-2,slen- (sizeof(__hzrq_frameHerder_t)-2) -3 );
	return slen;
}



//encrypt key
//AES128 ECB
/*
uint16_t _hzrq_pkcs7padding(uint8_t* buf,uint16_t len)
{
	uint16_t i,t16,retlen;
	t16=(16-(len&0x0f));
	retlen=(len & ~0x0f) +16;
	for(i=len;i<retlen;i++){
		buf[i]=t16;
	}
	return retlen;
}
*/

void _hzrq_load_device_sta(uint8_t* buf)
{
	__hzrq_deviceStaByteDef_t sta={0};
	
	buf[0]=0;
	buf[1]=0;
	if(vavleState==VALVE_ON){
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
	__hzrqMsgSendSn++;
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

uint16_t hzrq_load_pop_frame(uint8_t* sbuf,uint16_t ssize,uint8_t popType)
{
	//
	uint16_t len,t16;
	//uint8_t tmpBuf[32];
	uint8_t* p;
	
	len=_hzrq_load_frame_header(sbuf,ssize,0,0x01,__hzrqMsgSendSn);	
	__hzrqMsgSendSn++;
	__hzrq_dfPop_t* stb=(__hzrq_dfPop_t*)(sbuf+sizeof(__hzrq_frameHerder_t));
	
	__hzrq_swap_load_t16(stb->iden,__hzrq_DFID_POP);
	__hzrq_load_rtc(stb->dt);	
	stb->popType=popType;
	__hzrq_swap_load_t32(stb->curTotalVol,totalVolume);
	_hzrq_load_device_sta(stb->deviceSta);
	__hzrq_swap_load_t32(stb->customDeviceSta,sysData.devStatus.t32);
	stb->powerType=__hzrq_POWER_TYPE_LISOCL;
	__hzrq_swap_load_t16(stb->batVoltage,voltBat);
	stb->batPercent=90;
	m_mem_set(stb->hourVolLogBuf,0,100);
	m_mem_set(stb->dayVolLogBuf,0,24);
	
	len+=sizeof(__hzrq_dfPop_t);
	len+=3;	
	len=__hzrq_load_frame_mod_len(sbuf,len);
	len=_hzrq_load_frame_encrypt(sbuf,len);
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
		stb->valveSta=1;
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
	hzrqErrorCode=__hzrq_swap_get_t16(stb->retCode);
	//if(errCode=)
	//load pop frame buffer;
	t16=hzrq_load_pop_frame(sbuf,ssize,popType);
	sendStaMichine=__hzrq_SEND_SM_POP;
	szrqRtcSync=true;
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
	hzrqErrorCode=__hzrq_swap_get_t16(stb->retCode);
	if(hzrqErrorCode ==__hzrq_ERR_NONE){
		api_calc_all();
		sysData.hzrqBalanceMoney=__hzrq_swap_get_t32(stb->balanceMoney);
		sysData.hzrqBalanceVol=__hzrq_swap_get_t32(stb->balanceVol);
		sysData.hzrqOverdraftStat=stb->overdraftStat;
		sysData.hzrqBalanceVolSta=stb->balanceVolSta;
		sysData.hzrqPrice=__hzrq_swap_get_t32(stb->price);
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
		case __hzrq_DFID_REGISTER:		ret=hzrq_ins_exe_register(rbuf,rlen,sbuf,ssize,popType);	break;
		case __hzrq_DFID_END:			ret=hzrq_ins_exe_end(rbuf,rlen);							break;
		//表A.1.1 状态数据对象
		case __hzrq_DFID_VALVE_CTRL:	ret=hzrq_ins_rw_valve_sta(rbuf,rlen,sbuf,ssize);			break;
		case __hzrq_DFID_DATETIME:		ret=hzrq_ins_rw_rtc(rbuf,rlen,sbuf,ssize);					break;
		default:ret=0;break;
		
	}
	return ret;
}
//

//file end
