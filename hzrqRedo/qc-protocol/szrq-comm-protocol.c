#include "includes.h"
#include "szrq-comm-protocol.h"

uint8_t severReqFreezeYY;
uint8_t severReqFreezeMM;

extern uint8_t szrqRssi=0;
//218.17.114.5,7701
severInf_t szrqDefaultSever={{218,17,114,5},7701};
void __szrq_disable_sleep_in_lwp(void)
{
	noEventTimeOut=NO_EVEN_MAX_TIME_OUT;
}

uint16_t __szrq_crc16(uint8_t *buf,uint16_t len)
{
	uint16_t crc;
	uint16_t i,j,k;
	crc=0xffff;
	for(i=0;i<len;i++){
		crc=crc^buf[i];
		for(j=0;j<8;j++){
			k=crc&1;
			crc=crc>>1;
			if(k)crc=crc^0xa001;
		}
	}
	return crc;
}

uint16_t __szrq_crc16_verify(uint8_t* message, uint16_t n)
{
    uint16_t expected;
	m_mem_cpy_len((uint8_t*)(&expected),message+n-2,2);
	if(expected!=__szrq_crc16(message,n - 2))return 0;
	return 1;	
}

void __szrq_crc16_append(uint8_t* message, uint16_t n)
{
    uint16_t crc;
    crc = __szrq_crc16(message, n);
    message[n] = (uint8_t)(crc & 0xff);
    message[n+1] = (uint8_t)((crc >> 8) & 0xff);
}


void __szrq_get_sys_rtc(uint8_t *bcdRtc,sysDataTime_t* rtcDt)
{
	sysDataTime_t* dt;
	if(rtcDt){
		dt=rtcDt;
	}else{
		dt=&stRtcDataTime;
	}
	bcdRtc[0]=dt->YY;
	bcdRtc[1]=dt->MM;
	bcdRtc[2]=dt->DD;
	bcdRtc[3]=dt->hh;
	bcdRtc[4]=dt->mm;
	bcdRtc[5]=dt->ss;	
}

uint8_t __szrq_get_valve_status(void)
{
	uint8_t ret=0;
	if(vavleState==VALVE_ON ){
		ret=SZRQ_VALVE_ON;
	}else{
		if(sysData.lockReason.bits.bShellOpen || sysData.lockReason.bits.bSeverOff || sysData.lockReason.bits.bBalance || \
		sysData.lockReason.bits.bStrongMagnetic>=MAX_STE_OFF_TIMES){
			ret=SZRQ_VALVE_OFF_FORCE;
		}else{
			ret=SZRQ_VALVE_OFF;
		}
	}
	return ret;
}

uint8_t __szrq_get_pwr_type(void)
{
	return 2;
}

int8_t __szrq_get_batPercent0(void)
{
	//return 80;
	if(pwrStatus==POWER_STATUS_NORMAL || pwrStatus==POWER_STATUS_OVER){
		return 80;
	}else if(pwrStatus==POWER_STATUS_LOW){
		return 40;
	}else{
		return 0;
	}
}

int8_t __szrq_get_batPercent1(void)
{
	return -1;
}

uint16_t __szrq_get_cimi(uint8_t* szrqCimi)
{
	uint16_t i=0;
	uint8_t t8;
	for(i=0;i<sizeof(CIMI);){
		if(CIMI[i]>='0' && CIMI[i]<='9'){
			t8=CIMI[i]-'0';
			t8<<=4;
			*szrqCimi=t8;
		}else{
			break;
		}
		i++;
		if(CIMI[i]>='0' && CIMI[i]<='9'){
			t8+=(CIMI[i]-'0');

			*szrqCimi=t8;
		}else{
			break;
		}
		i++	;
		szrqCimi++;
	}
	//for()
	return i;
}

int16_t __szrq_modify_hase_more(uint8_t* buf,uint16_t len,uint8_t haseMore)
{
	if(len>MAX_UART_TX_BUFFER_LEN)return 0;
	__szrq_framHeader_t* sth=(__szrq_framHeader_t*)buf;
	sth->hasMore=haseMore;
	__szrq_crc16_append(buf,len-3);	
	return len;
}

int16_t __szrq_load_frame_header(uint8_t* buf,uint16_t ssize,uint8_t dsr,uint8_t cmd)
{
	uint16_t t16=0x00;
	__szrq_framHeader_t* sth;
	if(!buf)return 0;
	if(ssize<sizeof(__szrq_framHeader_t))return 0;
	
	//m_mem_set(buf,0,sizeof(__szrq_framHeader_t));
	sth=(__szrq_framHeader_t*)buf;
	sth->startChr=SZRQ_FRAME_START_CHR;
	sth->frameSource=dsr;
	sth->hasMore=0;
	
	
	sth->mid[0]=0;sth->mid[1]=0;
	sth->protocolVer=SZRQ_PROTOCOL_VER;
	sth->vendorCode=SZRQ_VENDOR_CODE;
	sth->cmd=cmd;
	
	sth->encryptFlg=SZRQ_PLAIN_FLG;
	
	m_mem_cpy_len(sth->iccid,ICCID,20);
	return sizeof(__szrq_framHeader_t);
}

int16_t __szrq_load_frame_pop_s(uint8_t* buf,uint16_t ssize,uint8_t popPeriodType)
{
	__szrq_ddfPopS_t* stb;
	
	int16_t len,t16=0x00;
	int32_t t32;
	
	if(!buf)return 0;
	len=sizeof(__szrq_ddfPopS_t)+sizeof(__szrq_framHeader_t);
	if(ssize<len)return 0;	
	m_mem_set(buf,0,ssize);
	
	__szrq_load_frame_header(buf,ssize,EXINF_DS_deviceReq,SZRQ_CMD_POP_SINGLE);
	stb=(__szrq_ddfPopS_t*)(buf+sizeof(__szrq_framHeader_t));
	
	m_mem_cpy_len(stb->dataLen,(uint8_t*)&len,sizeof(uint16_t));
	m_mem_cpy_len(stb->commSn,sysData.commSN,16);
	
	if(popPeriodType==0){
		stb->reportPeriod=popPeriodType;
	}else{
		stb->reportPeriod=sysData.szrqRoportPeriodType;
	}
	
	if(stb->reportPeriod>=5)return 0;
	stb->reportPeriodValue=sysData.szrqtPeriodValue;
	stb->reportTime[0]=stRtcDataTime.hh;
	stb->reportTime[1]=stRtcDataTime.mm;
	__szrq_get_sys_rtc(stb->readingTime,NULL);
	
	//t32=SZRQ_DATA_INVILID;
	//t32=totalVolume;
	#if __my_SZRQ_DEVCLASS==SZRQ_DEVCLASS_CIVIL
	t32=totalVolume/10;
	if(totalVolume%10>=5)t32++;
	#else
	t32=totalVolume/100;
	if(totalVolume%100>=5)t32++;
	#endif
	m_mem_cpy_len(stb->totalVol,(uint8_t*)&t32,sizeof(int32_t));
	//t32>>=1;
	m_mem_cpy_len(stb->stpTotaleVol,(uint8_t*)&t32,sizeof(int32_t));
	
	
	//t32=SZRQ_DATA_INVILID;
	float f=(float)SZRQ_DATA_INVILID;
	m_mem_cpy_len((uint8_t*)&t32,(uint8_t*)&f,sizeof(float));
	t32=swap_uint32(t32);
	m_mem_cpy_len(stb->rtFlow,(uint8_t*)&t32,sizeof(int32_t));
	
	t16=SZRQ_DATA_INVILID;
	m_mem_cpy_len(stb->rtTemp,(uint8_t*)&t16,sizeof(int16_t));
	t16=SZRQ_DATA_INVILID;
	m_mem_cpy_len(stb->rtPressure,(uint8_t*)&t16,sizeof(int16_t));	
	
	stb->rssi=szrqRssi;
	stb->deviceType=SZRQ_DEVTYPE_EPI;//皮膜燃气表
	stb->deviceClass=__my_SZRQ_DEVCLASS;
	
	stb->valveStatus=__szrq_get_valve_status();
	stb->pwrType=__szrq_get_pwr_type();//
	stb->batPercent0=__szrq_get_batPercent0();
	stb->batPercent1=__szrq_get_batPercent1();
	
	t16=__szrq_get_cimi(stb->simCode);
	
	stb->simCodeLen=(uint8_t)t16;
	//__szrq_get_cimi(stb->simCode);
	
	t32=sysData.szrqBalance;
	m_mem_cpy_len(stb->balance,(uint8_t*)&t32,sizeof(int32_t));	
	
	t32=sysData.szrqSeverVolume;
	m_mem_cpy_len(stb->settleVol,(uint8_t*)&t32,sizeof(int32_t));	
	
	m_mem_cpy_len(stb->settleDt,sysData.szrqSeverSettleTime,6);
	
	
	__szrq_crc16_append(buf,len-3);
	stb->endChr=SZRQ_FRAME_END_CHR;
	return len;
}
/*
		uint8_t	reportPeriod;
		uint8_t	reportDataInterval;
		uint8_t	reportPeriodValue;
		uint8_t	reportTime[2];
		uint8_t	readingTime[6];	

*/
uint32_t __nextCollectTm=0x00UL;
uint16_t szrqMultSendNum=0x00;
uint8_t  szrqMulHasMore=0;
uint8_t szrqRecHasMore=0;
// uint8_t  __lastPeriodType=0x00;
// uint8_t  __lastDataInterval=0x00;
// uint8_t  __lastPeriodValue=0x00;
uint32_t __uint32_diff_abs(uint32_t x,uint32_t y)
{
	if(x>y){
		return x-y;
	}else{
		return y-x;
	}
}

void __szrq_load_frame_pop_m_body_swap(uint8_t* buf,uint16_t len)
{
	
	uint16_t t16;
	__szrq_ddfPopMult_b_t *bodyBuf;
	uint8_t __buf[sizeof(__szrq_ddfPopMult_b_t)];
	t16=len-(sizeof(__szrq_ddfPopMult_h_t)+sizeof(__szrq_framHeader_t)+sizeof(__szrq_ddfPopMult_tail_t));
	t16=t16/sizeof(__szrq_ddfPopMult_b_t);
	
	uint16_t i=0;
	bodyBuf=(__szrq_ddfPopMult_b_t*)(buf+sizeof(__szrq_ddfPopMult_h_t)+sizeof(__szrq_framHeader_t));
	for(i=0;i<t16/2;i++){
		m_mem_cpy_len(__buf,(uint8_t*)&bodyBuf[t16-1-i],sizeof(__szrq_ddfPopMult_b_t));
		m_mem_cpy_len((uint8_t*)&bodyBuf[t16-1-i],(uint8_t*)&bodyBuf[i],sizeof(__szrq_ddfPopMult_b_t));
		m_mem_cpy_len((uint8_t*)&bodyBuf[i],__buf,sizeof(__szrq_ddfPopMult_b_t));
	}
}
int16_t __szrq_load_frame_pop_m(uint8_t* buf,uint16_t ssize)
{
	__szrq_framHeader_t* sth;
	sysDataTime_t dt;
	int16_t item,len,t16=0x00;
	int32_t t32,vol;
	item=szrq_item_get_unread_num();
	if(item==0){	return 0;}
	if(!buf)return 0;

	len=sizeof(__szrq_ddfPopMult_h_t)+sizeof(__szrq_framHeader_t)+sizeof(__szrq_ddfPopMult_tail_t);
	if(ssize<len)return 0;	
	m_mem_set(buf,0,ssize);
	__szrq_load_frame_header(buf,ssize,EXINF_DS_deviceReq,SZRQ_CMD_POP_MULT);
	
	__szrq_ddfPopMult_h_t* stbh;
	stbh=(__szrq_ddfPopMult_h_t*)(buf+sizeof(__szrq_framHeader_t));
	
	m_mem_cpy_len(stbh->dataLen,(uint8_t*)&len,sizeof(uint16_t));
	m_mem_cpy_len(stbh->commSn,sysData.commSN,16);
	// stbh->reportPeriod=sysData.szrqRoportPeriodType;
	// stbh->reportDataInterval=sysData.szrqRoportDataInterval;
	// stbh->reportPeriodValue=sysData.szrqtPeriodValue;
	
	 stbh->reportTime[0]=stRtcDataTime.hh;
	 stbh->reportTime[1]=stRtcDataTime.mm;
	//__szrq_get_sys_rtc(stbh->readingTime,NULL);
	//volItem_t
	int16_t i;
	szrqVolLogItem_t volItemBuf;
	uint8_t* pb0=buf+sizeof(__szrq_framHeader_t)+sizeof(__szrq_ddfPopMult_h_t);
	uint8_t* pb;
	__szrq_ddfPopMult_b_t* stbb;
	float f;
	for(i=0;i<item;i++){
		t16=szrq_item_read((uint8_t*)&volItemBuf,sizeof(szrqVolLogItem_t),i);
		if(t16!=sizeof(szrqVolLogItem_t)){
			return 0;
		}
		pb=pb0+i*sizeof(__szrq_ddfPopMult_b_t);
		stbb=(__szrq_ddfPopMult_b_t*)pb;
		//判断是否为第一包
		//<<-- load data
		if(i==0){
			stbh->reportPeriod=volItemBuf.szrqRoportPeriodType;
			stbh->reportDataInterval=volItemBuf.szrqRoportDataInterval;
			stbh->reportPeriodValue=volItemBuf.szrqtPeriodValue;	
			// time_stamp_to_system_dt(volItemBuf.ts,&dt);
			// __szrq_get_sys_rtc(stbh->readingTime,&dt);			
		}else{
			//if(__nextCollectTm)
				t32=__uint32_diff_abs(volItemBuf.ts,__nextCollectTm);
				if(t32>60)break;//时间差对不上
				if(stbh->reportPeriod != volItemBuf.szrqRoportPeriodType || \
				stbh->reportDataInterval!=volItemBuf.szrqRoportDataInterval || \
				stbh->reportPeriodValue!=volItemBuf.szrqtPeriodValue){
					break;
				}
		}
		__nextCollectTm=volItemBuf.szrqcollectNextTm;
		time_stamp_to_system_dt(volItemBuf.ts,&dt);
		__szrq_get_sys_rtc(stbh->readingTime,&dt);	
		vol=volItemBuf.vol;
		#if __my_SZRQ_DEVCLASS==SZRQ_DEVCLASS_CIVIL
		t32=vol/10;
		if(vol%10>=5)t32++;
		#else
		t32=vol/100;
		if(vol%100>=5)t32++;
		#endif	
		
		m_mem_cpy_len(stbb->totalVol,(uint8_t*)&t32,sizeof(int32_t));
		//t32>>=1;
		m_mem_cpy_len(stbb->stpTotaleVol,(uint8_t*)&t32,sizeof(int32_t));
		

		f=(float)SZRQ_DATA_INVILID;
		m_mem_cpy_len((uint8_t*)&t32,(uint8_t*)&f,sizeof(float));
		t32=swap_uint32(t32);
		m_mem_cpy_len(stbb->rtFlow,(uint8_t*)&t32,sizeof(int32_t));		
		
		t16=SZRQ_DATA_INVILID;
		m_mem_cpy_len(stbb->rtTemp,(uint8_t*)&t16,sizeof(int16_t));
		t16=SZRQ_DATA_INVILID;
		m_mem_cpy_len(stbb->rtPressure,(uint8_t*)&t16,sizeof(int16_t));		
		//load data-->>
		len+=sizeof(__szrq_ddfPopMult_b_t);
		if(len>=ssize-32){
			i+=1;
			break;
		}
	}
	if(i<item){
		szrqMulHasMore=1;
		sth=(__szrq_framHeader_t*)buf;
		sth->hasMore=1;
	}else{
		szrqMulHasMore=0;
	}
	
	szrqMultSendNum=i;
	pb=pb0+i*sizeof(__szrq_ddfPopMult_b_t);
	__szrq_ddfPopMult_tail_t* stbt;
	stbt=(__szrq_ddfPopMult_tail_t*)pb;
	
	
	stbt->rssi=szrqRssi;
	stbt->deviceType=SZRQ_DEVTYPE_EPI;//皮膜燃气表
	stbt->deviceClass=__my_SZRQ_DEVCLASS;
	
	stbt->valveStatus=__szrq_get_valve_status();
	stbt->pwrType=__szrq_get_pwr_type();//
	stbt->batPercent0=__szrq_get_batPercent0();
	stbt->batPercent1=__szrq_get_batPercent1();
	
	t16=__szrq_get_cimi(stbt->simCode);
	stbt->simCodeLen=(uint8_t)t16;
	
	t32=sysData.szrqBalance;
	m_mem_cpy_len(stbt->balance,(uint8_t*)&t32,sizeof(int32_t));	
	
	t32=sysData.szrqSeverVolume;
	m_mem_cpy_len(stbt->settleVol,(uint8_t*)&t32,sizeof(int32_t));	
	
	m_mem_cpy_len(stbt->settleDt,sysData.szrqSeverSettleTime,6);
	//
	m_mem_cpy_len(stbh->dataLen,(uint8_t*)&len,sizeof(uint16_t));
	
	__szrq_crc16_append(buf,len-3);
	stbt->endChr=SZRQ_FRAME_END_CHR;
	__szrq_load_frame_pop_m_body_swap(buf,len);
	
	__szrq_crc16_append(buf,len-3);
	stbt->endChr=SZRQ_FRAME_END_CHR;	
	__nop();
	__nop();
	return len;	
}

int16_t	__szrq_read_freeze(volItem_t* item,uint8_t fzYY,uint8_t fzMM)
{
	sysDataTime_t TsDt;
	uint16_t itemNum=freeze_item_get_num();
	uint32_t readAddr=FREEZE_VOLLOG_BODY_START_ADDR;
	uint32_t i;
	uint16_t rlen=0;
	//volItem_t item;
	for(i=0;i<itemNum;i++){
		rlen=0;
		qc_data_read_from_media((uint8_t*)item,readAddr+i*sizeof(volItem_t),sizeof(volItem_t));
		time_stamp_to_system_dt(item->ts,&TsDt);
		if(TsDt.YY==fzYY && TsDt.MM==fzMM){
			rlen=sizeof(volItem_t);
			break;
		}
	}
	//if(!rlen)return 0;
	return rlen;
}

int16_t __szrq_load_frame_freeze(uint8_t* buf,uint16_t ssize)
{
	__szrq_ddfFreezeRsp_t* stb;
	
	int16_t len,t16=0x00;
	int32_t t32,vol;
	
	if(!buf)return 0;
	len=sizeof(__szrq_ddfFreezeRsp_t)+sizeof(__szrq_framHeader_t);
	if(ssize<len)return 0;	
	m_mem_set(buf,0,ssize);
	
	__szrq_load_frame_header(buf,ssize,EXINF_DS_deviceRsp,SZRQ_CMD_FREEZE_DATA_RSP);
	stb=(__szrq_ddfFreezeRsp_t*)(buf+sizeof(__szrq_framHeader_t));
	
	m_mem_cpy_len(stb->dataLen,(uint8_t*)&len,sizeof(uint16_t));
	m_mem_cpy_len(stb->commSn,sysData.commSN,16);	
	
	volItem_t item;
	t16=__szrq_read_freeze(&item,severReqFreezeYY,severReqFreezeMM);
	if(t16){
		stb->rsp=1;
		sysDataTime_t dt;
		time_stamp_to_system_dt(item.ts,&dt);
		__szrq_get_sys_rtc(stb->readingTime,&dt);
		
		vol=item.vol;
		#if __my_SZRQ_DEVCLASS==SZRQ_DEVCLASS_CIVIL
		t32=vol/10;
		if(vol%10>=5)t32++;
		#else
		t32=vol/100;
		if(vol%100>=5)t32++;
		#endif			
		m_mem_cpy_len(stb->totalVol,(uint8_t*)&t32,sizeof(int32_t));
		//t32>>=1;
		m_mem_cpy_len(stb->stpTotaleVol,(uint8_t*)&t32,sizeof(int32_t));		
		t16=SZRQ_DATA_INVILID;
		m_mem_cpy_len(stb->rtTemp,(uint8_t*)&t16,sizeof(int16_t));
		t16=SZRQ_DATA_INVILID;
		m_mem_cpy_len(stb->rtPressure,(uint8_t*)&t16,sizeof(int16_t));	
			
		stb->deviceClass=__my_SZRQ_DEVCLASS;
		
	}else{
		stb->rsp=-3;
	}
	
	__szrq_crc16_append(buf,len-3);
	stb->endChr=SZRQ_FRAME_END_CHR;
	return len;	
}

uint8_t szrqWarnType=SZRQ_WARNVAL_PDBAT0;

uint8_t __szrq_get_warn_type(void)
{
	uint8_t ret=0;
	switch(VavleOffReason){
		case OFF_REASON_LO_VOLETAGE: ret=SZRQ_WARNVAL_PDBAT0;break;
		case OFF_REASON_HIGHT_GAS_FLOW: ret=SZRQ_WARNVAL_HF;break;
		case OFF_REASON_LOW_OV:
		case OFF_REASON_NO_OV:
		case OFF_REASON_LIMIT_OV:ret=SZRQ_WARNVAL_BALANCE_WARN;break;
		case OFF_REASON_STRONG_MAGTIC:ret=SZRQ_WARNVAL_STE;break;
		case OFF_REASON_LEAKAGE:ret=SZRQ_WARNVAL_LEAKAGE;break;
		case OFF_REASON_NONE:ret=0;break;
		default:	ret=SZRQ_WARNVAL_ABMORMAL_OFF;break;
		
	}
	szrqWarnType=ret;
	return ret;
}
uint8_t __szrq_get_off_reason(void)
{
	uint8_t ret=0;
	switch(VavleOffReason){
		case OFF_REASON_NONE:ret=0;break;
		case OFF_REASON_LOW_OV:ret=SZRQ_OFF_REASON_BLAN_LOW;break;
		case OFF_REASON_NO_OV:ret=SZRQ_OFF_REASON_BLAN_ZERO;break;
		case OFF_REASON_LIMIT_OV:ret=SZRQ_OFF_REASON_BLAN_OFF;break;
		case OFF_REASON_SEVERS:ret=SZRQ_OFF_REASON_REMOTE;break;
		case OFF_REASON_LO_VOLETAGE:ret=SZRQ_OFF_REASON_PWR_LOW;break;
		case OFF_REASON_STRONG_MAGTIC:ret=SZRQ_OFF_REASON_STE;break;
		default :ret=SZRQ_OFF_REASON_OTHER;break;
	}
	return ret;
}

uint8_t szrqWarnReportSendOld=0;
uint8_t szrqWarnReportbuf[128];
int16_t __szrq_load_frame_warn_report_ex(uint8_t* buf,uint16_t ssize,uint8_t warnFlg,uint8_t value)
{
	__szrq_ddfWarningReport_t* stb;
	
	int16_t len,t16=0x00;
	int32_t t32;
	
	if(!buf)return 0;
	len=sizeof(__szrq_ddfWarningReport_t)+sizeof(__szrq_framHeader_t);
	if(ssize<len)return 0;	
	m_mem_set(buf,0,ssize);
	
	__szrq_load_frame_header(buf,ssize,EXINF_DS_deviceReq,SZRQ_CMD_WARNING_REPORT);
	stb=(__szrq_ddfWarningReport_t*)(buf+sizeof(__szrq_framHeader_t));
	
	m_mem_cpy_len(stb->dataLen,(uint8_t*)&len,sizeof(uint16_t));
	m_mem_cpy_len(stb->commSn,sysData.commSN,16);	
	//<--todo
	__szrq_get_sys_rtc(stb->warningDt,NULL);
	stb->warningFlg=warnFlg;
	stb->warnType=0;
	
	if(value){
		szrqWarnType=value;
		stb->warnType=value;
	}else{ 
		if(warnFlg){
			stb->warnType=__szrq_get_warn_type();
		}else{
			stb->warnType=szrqWarnType;
		}
	}

	t16=SZRQ_DATA_INVILID;
	m_mem_cpy_len(stb->rtPressure,(uint8_t*)&t16,sizeof(int16_t));		
	t16=SZRQ_DATA_INVILID;
	m_mem_cpy_len(stb->rtTemp,(uint8_t*)&t16,sizeof(int16_t));
	
	t32=SZRQ_DATA_INVILID;
	m_mem_cpy_len(stb->speed,(uint8_t*)&t32,sizeof(int32_t));
	
	t32=SZRQ_DATA_INVILID;
	m_mem_cpy_len(stb->strength,(uint8_t*)&t32,sizeof(int32_t));	

	
	
	float f=(float)SZRQ_DATA_INVILID;
	m_mem_cpy_len((uint8_t*)&t32,(uint8_t*)&f,sizeof(float));
	t32=swap_uint32(t32);
	m_mem_cpy_len(stb->rtFlow,(uint8_t*)&t32,sizeof(int32_t));	
		

	stb->batPercent0=__szrq_get_batPercent0();
	stb->batPercent1=-1;
	
	t32=sysData.szrqBalance;
	m_mem_cpy_len(stb->balance,(uint8_t*)&t32,sizeof(int32_t));	
	stb->valveStatus=__szrq_get_valve_status();
	if(warnFlg){
		stb->offReason=__szrq_get_off_reason();
	}else{
		stb->offReason=0;
	}
	
	
	#if __my_SZRQ_DEVCLASS==SZRQ_DEVCLASS_CIVIL
	t32=totalVolume/10;
	if(totalVolume%10>=5)t32++;
	#else
	t32=totalVolume/100;
	if(totalVolume%100>=5)t32++;
	#endif
	m_mem_cpy_len(stb->totalVol,(uint8_t*)&t32,sizeof(int32_t));
	//t32>>=1;
	m_mem_cpy_len(stb->stpTotaleVol,(uint8_t*)&t32,sizeof(int32_t));
	
	
	stb->deviceClass=__my_SZRQ_DEVCLASS;

	//-->
	__szrq_crc16_append(buf,len-3);
	stb->endChr=SZRQ_FRAME_END_CHR;
	return len;		
}
int16_t __szrq_load_frame_warn_report(uint8_t* buf,uint16_t ssize,uint8_t warnFlg,uint8_t value)
{
	__szrq_ddfWarningReport_t* stb;
	
	int16_t len,t16=0x00;
	int32_t t32;
	
	if(!buf)return 0;
	len=sizeof(__szrq_ddfWarningReport_t)+sizeof(__szrq_framHeader_t);
	if(ssize<len)return 0;	
	m_mem_set(buf,0,ssize);
	
	__szrq_load_frame_header(buf,ssize,EXINF_DS_deviceReq,SZRQ_CMD_WARNING_REPORT);
	stb=(__szrq_ddfWarningReport_t*)(buf+sizeof(__szrq_framHeader_t));
	
	m_mem_cpy_len(stb->dataLen,(uint8_t*)&len,sizeof(uint16_t));
	m_mem_cpy_len(stb->commSn,sysData.commSN,16);	
	//<--todo
	__szrq_get_sys_rtc(stb->warningDt,NULL);
	stb->warningFlg=warnFlg;
	stb->warnType=0;
	
	if(value){
		stb->warnType=value;
		__szrq_framHeader_t* sth;
		sth=(__szrq_framHeader_t*)buf;
		sth->hasMore=1;
		szrqWarnType=value;
	}else{ 
		if(warnFlg){
			stb->warnType=__szrq_get_warn_type();
		}else{
			stb->warnType=szrqWarnType;
		}
	}

	t16=SZRQ_DATA_INVILID;
	m_mem_cpy_len(stb->rtPressure,(uint8_t*)&t16,sizeof(int16_t));		
	t16=SZRQ_DATA_INVILID;
	m_mem_cpy_len(stb->rtTemp,(uint8_t*)&t16,sizeof(int16_t));
	
	t32=SZRQ_DATA_INVILID;
	m_mem_cpy_len(stb->speed,(uint8_t*)&t32,sizeof(int32_t));
	
	t32=SZRQ_DATA_INVILID;
	m_mem_cpy_len(stb->strength,(uint8_t*)&t32,sizeof(int32_t));	

	
	//float f=SZRQ_DATA_INVILID;
	//m_mem_cpy_len(stb->rtFlow,(uint8_t*)&f,sizeof(float));	
	float f=(float)SZRQ_DATA_INVILID;
	m_mem_cpy_len((uint8_t*)&t32,(uint8_t*)&f,sizeof(float));
	t32=swap_uint32(t32);
	m_mem_cpy_len(stb->rtFlow,(uint8_t*)&t32,sizeof(int32_t));	
		
//	if(stb->warnType == SZRQ_WARNVAL_PDBAT0 || stb->warnType == SZRQ_WARNVAL_PDBAT1 && warnFlg){
//		stb->batPercent0=0;
//	}else{
//		stb->batPercent0=__szrq_get_batPercent0();
//	}
	stb->batPercent0=__szrq_get_batPercent0();
	stb->batPercent1=-1;
	
	t32=sysData.szrqBalance;
	m_mem_cpy_len(stb->balance,(uint8_t*)&t32,sizeof(int32_t));	
	stb->valveStatus=__szrq_get_valve_status();
	if(warnFlg){
		stb->offReason=__szrq_get_off_reason();
	}else{
		stb->offReason=0;
	}
	
	
	#if __my_SZRQ_DEVCLASS==SZRQ_DEVCLASS_CIVIL
	t32=totalVolume/10;
	if(totalVolume%10>=5)t32++;
	#else
	t32=totalVolume/100;
	if(totalVolume%100>=5)t32++;
	#endif
	m_mem_cpy_len(stb->totalVol,(uint8_t*)&t32,sizeof(int32_t));
	//t32>>=1;
	m_mem_cpy_len(stb->stpTotaleVol,(uint8_t*)&t32,sizeof(int32_t));
	
	
	stb->deviceClass=__my_SZRQ_DEVCLASS;

	//-->
	__szrq_crc16_append(buf,len-3);
	stb->endChr=SZRQ_FRAME_END_CHR;
	return len;		
}

int16_t __szrq_load_frame_valve_ctrl(uint8_t* buf,uint16_t ssize,uint8_t rsp)
{
	__szrq_ddfValveCtrlRsp_t* stb;
	
	int16_t len,t16=0x00;
	int32_t t32;
	
	if(!buf)return 0;
	len=sizeof(__szrq_ddfValveCtrlRsp_t)+sizeof(__szrq_framHeader_t);
	if(ssize<len)return 0;	
	m_mem_set(buf,0,ssize);
	
	__szrq_load_frame_header(buf,ssize,EXINF_DS_deviceRsp,SZRQ_CMD_VALVE_CTRL_RSP);
	stb=(__szrq_ddfValveCtrlRsp_t*)(buf+sizeof(__szrq_framHeader_t));
	
	m_mem_cpy_len(stb->dataLen,(uint8_t*)&len,sizeof(uint16_t));
	m_mem_cpy_len(stb->commSn,sysData.commSN,16);	
	//<--todo
	stb->rsp=rsp;
	while(vavleState==VALVE_OPERATION_OFF || vavleState==VALVE_OPERATION_ON){
		__szrq_disable_sleep_in_lwp();
		osDelay(500);
	}
	stb->valveStatus=__szrq_get_valve_status();
	//-->
	__szrq_crc16_append(buf,len-3);
	stb->endChr=SZRQ_FRAME_END_CHR;
	return len;		
}

/*
int16_t __szrq_load_frame_valve_ctrl(uint8_t* buf,uint16_t ssize)
{
	__szrq_ddfValveCtrlRsp_t* stb;
	
	int16_t len,t16=0x00;
	int32_t t32;
	
	if(!buf)return 0;
	len=sizeof(__szrq_ddfValveCtrlRsp_t)+sizeof(__szrq_framHeader_t);
	if(ssize<len)return 0;	
	m_mem_set(buf,0,ssize);
	
	__szrq_load_frame_header(buf,ssize,EXINF_DS_deviceRsp,SZRQ_CMD_VALVE_CTRL_RSP);
	stb=(__szrq_ddfValveCtrlRsp_t*)(buf+sizeof(__szrq_framHeader_t));
	
	m_mem_cpy_len(stb->dataLen,(uint8_t*)&len,sizeof(uint16_t));
	m_mem_cpy_len(stb->commSn,sysData.commSN,16);	
	//<--todo
	stb->rsp=1;
	while(vavleState==VALVE_OPERATION_OFF || vavleState==VALVE_OPERATION_ON){
		__szrq_disable_sleep_in_lwp();
		osDelay(500);
	}
	stb->valveStatus=__szrq_get_valve_status();
	//-->
	__szrq_crc16_append(buf,len-3);
	stb->endChr=SZRQ_FRAME_END_CHR;
	return len;		
}
*/

int16_t __szrq_load_frame_base_param(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	__szrq_ddfSetBaseParmRsp_t* stbRsp;
	__szrq_ddfSetBaseParm_t*	stbRec;
	int16_t len,t16=0x00;
	int32_t t32;
	//buf
	if(!sbuf)return 0;
	len=sizeof(__szrq_ddfSetBaseParmRsp_t)+sizeof(__szrq_framHeader_t);
	if(ssize<len)return 0;	
	//m_mem_set(sbuf,0,ssize);
	
	__szrq_load_frame_header(sbuf,ssize,EXINF_DS_deviceRsp,SZRQ_CMD_SET_BASE_PRAM_RSP);
	stbRsp=(__szrq_ddfSetBaseParmRsp_t*)(sbuf+sizeof(__szrq_framHeader_t));
	stbRec=(__szrq_ddfSetBaseParm_t*)(rbuf+sizeof(__szrq_framHeader_t));
	
	
	m_mem_cpy_len(stbRsp->dataLen,(uint8_t*)&len,sizeof(uint16_t));
	m_mem_cpy_len(stbRsp->commSn,sysData.commSN,16);	
	//<--todo
	m_mem_cpy_len((uint8_t*)stbRsp,(uint8_t*)stbRec,sizeof(__szrq_ddfSetBaseParm_t));
	stbRsp->qsSetFlg		=stbRec->qsSetFlg;
	stbRsp->commSnSetFlg	=stbRec->commSnSetFlg;
	stbRsp->volSetFlg		=stbRec->volSetFlg;
	//-->
	__szrq_crc16_append(sbuf,len-3);
	stbRsp->endChr=SZRQ_FRAME_END_CHR;
	return len;		
}

int16_t __szrq_load_frame_report_param(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	//buf
	__szrq_ddfSetReportParmRsp_t* stbRsp;
	__szrq_ddfSetReportParm_t*	stbRec;	
	
	int16_t len,t16=0x00;
	int32_t t32;
	
	if(!sbuf)return 0;
	len=sizeof(__szrq_ddfSetReportParmRsp_t)+sizeof(__szrq_framHeader_t);
	if(ssize<len)return 0;	
	//m_mem_set(sbuf,0,ssize);
	
	__szrq_load_frame_header(sbuf,ssize,EXINF_DS_deviceRsp,SZRQ_CMD_SET_REPORT_PRAM_RSP);
	stbRsp=(__szrq_ddfSetReportParmRsp_t*)(sbuf+sizeof(__szrq_framHeader_t));
	stbRec=(__szrq_ddfSetReportParm_t*)(rbuf+sizeof(__szrq_framHeader_t));
	
	//m_mem_cpy_len(stbRsp->dataLen,(uint8_t*)&len,sizeof(uint16_t));
	//m_mem_cpy_len(stbRsp->commSn,sysData.commSN,16);	
	//<--todo
	m_mem_cpy_len((uint8_t*)stbRsp,(uint8_t*)stbRec,sizeof(__szrq_ddfSetReportParmRsp_t));
	stbRsp->periodFlg			=stbRec->periodFlg;	
	stbRsp->intervalFlg			=stbRec->intervalFlg;	
	stbRsp->periodValueFlg		=stbRec->periodValueFlg;	
	stbRsp->reportTimeFlg		=stbRec->reportTimeFlg;	
	stbRsp->freezeDayFlg		=stbRec->freezeDayFlg;	
	stbRsp->freezeHourFlg		=stbRec->freezeHourFlg;		

	//-->
	__szrq_crc16_append(sbuf,len-3);
	stbRsp->endChr=SZRQ_FRAME_END_CHR;
	return len;		
}

int16_t __szrq_load_frame_comm_param(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	//buf
	__szrq_ddfSetIotParmRsp_t* stbRsp;
	__szrq_ddfSetIotParm_t* stbRec;
	int16_t len,t16=0x00;
	int32_t t32;
	
	if(!sbuf)return 0;
	len=sizeof(__szrq_ddfSetIotParmRsp_t)+sizeof(__szrq_framHeader_t);
	if(ssize<len)return 0;	
	m_mem_set(sbuf,0,ssize);
	
	__szrq_load_frame_header(sbuf,ssize,EXINF_DS_deviceRsp,SZRQ_CMD_SET_IOT_PRAM_RSP);
	stbRsp=(__szrq_ddfSetIotParmRsp_t*)(sbuf+sizeof(__szrq_framHeader_t));
	stbRec=(__szrq_ddfSetIotParm_t*)(rbuf+sizeof(__szrq_framHeader_t));
	
	//m_mem_cpy_len(stbRsp->dataLen,(uint8_t*)&len,sizeof(uint16_t));
	//m_mem_cpy_len(stbRsp->commSn,sysData.commSN,16);	
	//<--todo
	m_mem_cpy_len((uint8_t*)stbRsp,(uint8_t*)stbRec,sizeof(__szrq_ddfSetIotParm_t));
	//-->
	__szrq_crc16_append(sbuf,len-3);
	stbRsp->endChr=SZRQ_FRAME_END_CHR;
	return len;		
}

int16_t __szrq_load_frame_warn_param(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize)
{
	__szrq_ddfSetWarnParmRsp_t* stbRsp;
	__szrq_ddfSetWarnParm_t*	stbRec;
	
	int16_t len,t16=0x00;
	int32_t t32;
	
	if(!sbuf)return 0;
	len=sizeof(__szrq_ddfSetWarnParmRsp_t)+sizeof(__szrq_framHeader_t);
	if(ssize<len)return 0;	
	m_mem_set(sbuf,0,ssize);
	
	__szrq_load_frame_header(sbuf,ssize,EXINF_DS_deviceRsp,SZRQ_CMD_SET_WARN_PRAM_RSP);
	stbRsp=(__szrq_ddfSetWarnParmRsp_t*)(sbuf+sizeof(__szrq_framHeader_t));
	stbRec=(__szrq_ddfSetWarnParm_t*)(rbuf+sizeof(__szrq_framHeader_t));
	//m_mem_cpy_len(stbRsp->dataLen,(uint8_t*)&len,sizeof(uint16_t));
	//m_mem_cpy_len(stbRsp->commSn,sysData.commSN,16);	
	//<--todo
	m_mem_cpy_len((uint8_t*)stbRsp,(uint8_t*)stbRec,sizeof(__szrq_ddfSetWarnParm_t));
	stbRsp->pressHiFlg		=stbRec->pressHiFlg;
	stbRsp->pressLoFlg		=stbRec->pressLoFlg;
	stbRsp->tempHiFlg		=stbRec->tempHiFlg;
	stbRsp->tempLoFlg		=stbRec->tempLoFlg;
	//-->
	__szrq_crc16_append(sbuf,len-3);
	stbRsp->endChr=SZRQ_FRAME_END_CHR;
	return len;		
}



int16_t __szrq_load_frame_all_param(uint8_t* buf,uint16_t ssize)
{
	__szrq_ddfGetAllParmRsp_t* stb;
	
	int16_t len,t16=0x00;
	int32_t t32;
	
	if(!buf)return 0;
	len=sizeof(__szrq_ddfGetAllParmRsp_t)+sizeof(__szrq_framHeader_t);
	if(ssize<len)return 0;	
	m_mem_set(buf,0,ssize);
	
	__szrq_load_frame_header(buf,ssize,EXINF_DS_deviceRsp,SZRQ_CMD_GET_ALL_PRAM_RSP);
	stb=(__szrq_ddfGetAllParmRsp_t*)(buf+sizeof(__szrq_framHeader_t));
	
	m_mem_cpy_len(stb->dataLen,(uint8_t*)&len,sizeof(uint16_t));
	m_mem_cpy_len(stb->commSn,sysData.commSN,16);	
	//<--todo
	stb->qsSetFlg=1;					//	uint8_t		qsSetFlg;
	t32=(int32_t)sysData.QS;			//	uint8_t		qs[4];
	m_mem_cpy_len(stb->qs,(uint8_t*)&t32,sizeof(int32_t));
	
	stb->volSetFlg=1;					//	uint8_t		volSetFlg;
	float f=0.0;
	f=(float)(sysData.initVolume)/100;
	//t32=sysData.initVolume;				//	uint8_t		volume[4];
	m_mem_cpy_len((uint8_t*)&t32,(uint8_t*)&f,sizeof(uint32_t));
	t32=swap_uint32(t32);
	m_mem_cpy_len(stb->volume,(uint8_t*)&t32,sizeof(uint32_t));
		
	stb->periodFlg=1;						//	uint8_t		periodFlg;
	stb->period=sysData.szrqRoportPeriodType;//	uint8_t		period;
		
	stb->intervalFlg=1;						//	uint8_t		intervalFlg;
	stb->interval=sysData.szrqRoportDataInterval;//	uint8_t		interval;
		
	stb->periodValueFlg=1;//	uint8_t		periodValueFlg;
	stb->periodValue=sysData.szrqtPeriodValue;//	uint8_t		periodValue;
		
	stb->reportTimeFlg=1;//	uint8_t		reportTimeFlg;
	m_mem_cpy_len(stb->reportTime,sysData.szrqtPeriodTime,2);//	uint8_t		reportTime[2];
		
	stb->freezeDayFlg=1;//	uint8_t		freezeDayFlg;
	stb->freezeDay=sysData.szrqtFreezeDay;//	uint8_t		freezeDay;
		
	stb->freezeHourFlg=1;//	uint8_t		freezeHourFlg;
	stb->freezeHour=sysData.szrqtFreezeHour;//	uint8_t		freezeHour;			
		
	stb->iotIp0Flg=1;//	uint8_t		iotIp0Flg;
	m_mem_cpy_len(stb->iotIp0,sysData.severRelease.ip,4);//	uint8_t		iotIp0[4];
		
	stb->iotPort0Flg=1;//	uint8_t		iotPort0Flg;
	t16=sysData.severRelease.port;//	uint8_t		iotPort0[2];
	m_mem_cpy_len(stb->iotPort0,(uint8_t*)&t16,2);

	stb->iotPort1Flg=1;//	uint8_t		iotIp1Flg;
	m_mem_cpy_len(stb->iotIp1,sysData.severUser.ip,4);//	uint8_t		iotIp1[4];
		
	stb->iotPort1Flg=1;//	uint8_t		iotPort1Flg;
	t16=sysData.severUser.port;//	uint8_t		iotPort1[2];	
	m_mem_cpy_len(stb->iotPort1,(uint8_t*)&t16,2);	
	
	stb->pressHiFlg=-1;//	uint8_t		pressHiFlg;
	//	uint8_t		pressHi[4];
		
	stb->pressLoFlg=-1;//	uint8_t		pressLoFlg;
	//	uint8_t		pressLo[4];	

	stb->tempHiFlg=-1;//	uint8_t		tempHiFlg;
	//	uint8_t		tempHi[4];		

	stb->tempLoFlg=-1;//	uint8_t		tempLoFlg;
	//	uint8_t		tempLo[4];	

	stb->balanceWarnFlg=1;//	uint8_t		balanceWarnFlg;
	t32=sysData.warnSetOverageVM;//	uint8_t		balanceWarn[4];	
	m_mem_cpy_len(stb->balanceWarn,(uint8_t*)&t32,sizeof(uint32_t));
	

	stb->balanceOffFlg=1;//	uint8_t		balanceOffFlg;
	t32=sysData.offSetOverageVM;//	uint8_t		balanceOff[4];	
	m_mem_cpy_len(stb->balanceOff,(uint8_t*)&t32,sizeof(uint32_t));
	
	stb->balanceLimitOffFlg=1;//	uint8_t		balanceLimitOffFlg;
	t32=sysData.limitOffsetVM;//	uint8_t		balanceLimitOff[4];		
	m_mem_cpy_len(stb->balanceLimitOff,(uint8_t*)&t32,sizeof(uint32_t));
		
	stb->valveCtrlByteFlg=1;//	uint8_t		valveCtrlByteFlg;
	stb->valveCtrlByte=sysData.szrqValveCtrlByte;//	uint8_t		valveCtrlByte;	
	//-->
	__szrq_crc16_append(buf,len-3);
	stb->endChr=SZRQ_FRAME_END_CHR;
	return len;		
}

int16_t __szrq_load_sync_balance_rsp(uint8_t* buf,uint16_t ssize)
{
	__szrq_ddfBalanceSyncRsp_t* stb;
	
	int16_t len,t16=0x00;
	int32_t t32;
	
	if(!buf)return 0;
	len=sizeof(__szrq_ddfBalanceSyncRsp_t)+sizeof(__szrq_framHeader_t);
	if(ssize<len)return 0;	
	m_mem_set(buf,0,ssize);
	
	__szrq_load_frame_header(buf,ssize,EXINF_DS_deviceRsp,SZRQ_CMD_BALANCE_SYN_RSP);
	stb=(__szrq_ddfBalanceSyncRsp_t*)(buf+sizeof(__szrq_framHeader_t));
	
	m_mem_cpy_len(stb->dataLen,(uint8_t*)&len,sizeof(uint16_t));
	m_mem_cpy_len(stb->commSn,sysData.commSN,16);	
	//<--todo
	stb->rsp=1;
	
	t32=sysData.szrqBalance;
	m_mem_cpy_len(stb->balance,(uint8_t*)&t32,sizeof(t32));
	
	t32=sysData.szrqBalanceVol;
	#if __my_SZRQ_DEVCLASS==SZRQ_DEVCLASS_NONCIVIL
	t32/=100;
	#else
	t32/=10;
	#endif
	m_mem_cpy_len(stb->balanceVol,(uint8_t*)&t32,sizeof(t32));
	
	t32=sysData.szrqSeverVolume;
	#if __my_SZRQ_DEVCLASS==SZRQ_DEVCLASS_NONCIVIL
	t32/=100;
	#else
	t32/=10;
	#endif
	m_mem_cpy_len(stb->severVolume,(uint8_t*)&t32,sizeof(t32));
	
	t32=sysData.szrqSeverSetpStartVol;
	m_mem_cpy_len(stb->severSetpStartVol,(uint8_t*)&t32,sizeof(t32));

	m_mem_cpy_len(stb->severSettleTime,sysData.szrqSeverSettleTime,6);
	m_mem_cpy_len(stb->severSetpStartDt,sysData.szrqSeverSetpStartDt,6);
	//-->
	__szrq_crc16_append(buf,len-3);
	stb->endChr=SZRQ_FRAME_END_CHR;
	return len;		
}

int16_t __szrq_load_frame_step_slu(uint8_t* buf,uint16_t ssize,uint8_t cmd)
{
	__szrq_ddfSetpPrice_t* stb;
	
	int16_t len,t16=0x00;
	int32_t t32;
	
	if(!buf)return 0;
	len=sizeof(__szrq_ddfSetpPrice_t)+sizeof(__szrq_framHeader_t);
	if(ssize<len)return 0;	
	//m_mem_set(buf,0,ssize);
	
	__szrq_load_frame_header(buf,ssize,EXINF_DS_deviceRsp,cmd);
	stb=(__szrq_ddfSetpPrice_t*)(buf+sizeof(__szrq_framHeader_t));
	
	m_mem_cpy_len(stb->dataLen,(uint8_t*)&len,sizeof(uint16_t));
	m_mem_cpy_len(stb->commSn,sysData.commSN,16);	
	//<--todo

	//-->
	__szrq_crc16_append(buf,len-3);
	stb->endChr=SZRQ_FRAME_END_CHR;
	return len;		
}

int16_t __szrq_ins_common_rsp(uint8_t* rbuf, uint16_t rlen, uint8_t* sbuf,uint16_t ssize)
{
	int16_t t16;
	__szrq_framHeader_t* sth;
	sth=(__szrq_framHeader_t*)rbuf;

	sysDataTime_t dt={0};
	__szrq_ddfCloudRsp_t* stb;
	

	if(rlen<sizeof(__szrq_framHeader_t)+sizeof(__szrq_ddfCloudRsp_t))return 0;
	stb=(__szrq_ddfCloudRsp_t*)(rbuf+sizeof(__szrq_framHeader_t));
	stb->rsp=stb->rsp;
	api_sysdata_save();	
	dt.YY=stb->dt[0];
	dt.MM=stb->dt[1];
	dt.DD=stb->dt[2];
	dt.hh=stb->dt[3];
	dt.mm=stb->dt[4];
	dt.ss=stb->dt[5];
	api_rtc_set(&dt);
	szrqRtcSync=true;
	__nop();
	
	//if(sth->cmd==SZRQ_CMD_POP_MULT_RSP && stb->rsp==1){
	if(sth->cmd==SZRQ_CMD_POP_MULT_RSP){
		szrq_item_set_read_loc(szrqMultSendNum);
		if(szrqMulHasMore){
			//szrqSendStaMichine=__15S_SEND_SM_POP;
			return 1;
		}
	}else{
		t16=rf_send_fifo_delete_tail();
		if(t16)return 1;
	}		
	/*
	//if(sth->cmd==SZRQ_CMD_WARNING_REPORT_RSP && stb->rsp==1){
	if(sth->cmd==SZRQ_CMD_WARNING_REPORT_RSP){
		if(szrqWarnReportSendOld){
			szrqWarnReportSendOld=0;
			szrqSendStaMichine=__15S_SEND_SM_POP;
			return 1;			
		}
	}
	*/
	return 0;
}

int16_t __szrq_ins_read_freeze(uint8_t* rbuf, uint16_t rlen, uint8_t* sbuf,uint16_t ssize)
{
	int16_t ret=0;
	__szrq_ddfFreezeReq_t* stb;
	if(rlen<sizeof(__szrq_framHeader_t)+sizeof(__szrq_ddfFreezeReq_t))return 0;
	stb=(__szrq_ddfFreezeReq_t*)(rbuf+sizeof(__szrq_framHeader_t));
	//<<--todo
	severReqFreezeYY=stb->freezeDate[0];
	severReqFreezeMM=stb->freezeDate[1];
	//-->>
	ret=__szrq_load_frame_freeze(sbuf,ssize);
	return ret;
	
}

int16_t __szrq_ins_sync_balance(uint8_t* rbuf, uint16_t rlen, uint8_t* sbuf,uint16_t ssize)
{
	int16_t ret=0;
	int32_t t32;
	__szrq_ddfBalanceSync_t* stb;
	if(rlen<sizeof(__szrq_framHeader_t)+sizeof(__szrq_ddfBalanceSync_t))return 0;
	stb=(__szrq_ddfBalanceSync_t*)(rbuf+sizeof(__szrq_framHeader_t));
	api_sysdata_save();	
	
	m_mem_cpy_len((uint8_t*)&t32,stb->balance,sizeof(int32_t));
	sysData.szrqBalance=t32;
	
	m_mem_cpy_len((uint8_t*)&t32,stb->balanceVol,sizeof(int32_t));
	#if __my_SZRQ_DEVCLASS==SZRQ_DEVCLASS_NONCIVIL
		t32*=100;
	#else
		t32*=10;
	#endif	
	sysData.szrqBalanceVol=t32;	
	
	//
	m_mem_cpy_len((uint8_t*)&t32,stb->severVolume,sizeof(int32_t));
	#if __my_SZRQ_DEVCLASS==SZRQ_DEVCLASS_NONCIVIL
		t32*=100;
	#else
		t32*=10;
	#endif	
	sysData.szrqSeverVolume=t32;
	//
	m_mem_cpy_len((uint8_t*)&t32,stb->severSetpStartVol,sizeof(int32_t));
	sysData.szrqSeverSetpStartVol=t32;	
	
	m_mem_cpy_len(sysData.szrqSeverSettleTime,stb->severSettleTime,6);
	m_mem_cpy_len(sysData.szrqSeverSetpStartDt,stb->severSetpStartDt,6);
	api_sysdata_save();	
	
	ret=__szrq_load_sync_balance_rsp(sbuf,ssize);
	return ret;
}

int16_t __szrq_ins_valve_ctrl(uint8_t* rbuf, uint16_t rlen, uint8_t* sbuf,uint16_t ssize)
{
	int8_t rsp;
	int16_t ret=0;
	int32_t t32;
	__szrq_ddfValveCtrl_t* stb;
	if(rlen<sizeof(__szrq_framHeader_t)+sizeof(__szrq_ddfValveCtrl_t))return 0;
	stb=(__szrq_ddfValveCtrl_t*)(rbuf+sizeof(__szrq_framHeader_t));
	//api_sysdata_save();
	uint16_t t16;
	//m_mem_cpy_len((uint8_t*)&t16,stb->valveCtrl,sizeof(uint16_t));
	t16=stb->valveCtrl[0];
	t16<<=8;
	t16|=stb->valveCtrl[1];
	
	rsp=1;
	if(t16==SZRQ_VALVE_CTRL_OPEN){
		//stlockReasonBits_t.t32=0;
		sysData.lockReason.t32=0x00UL;
		api_sysdata_save();
		// if((void*)vTheadEvenID){
			// osSignalSet( vTheadEvenID, flg_EVENT_KEY_DOWN);
		// }
	}else if(t16==SZRQ_VALVE_CTRL_CLOSE){
		if(sysData.lockReason.bits.bSeverOff && vavleState==VALVE_OFF){
			rsp=-5;
		}else{
			vavle_off_from_app(OFF_REASON_TEMP);
		}
	}else if(t16==SZRQ_VALVE_CTRL_LOCK){
		sysData.lockReason.bits.bSeverOff=1;
		if(vavleState==VALVE_OFF || vavleState==VALVE_OPERATION_OFF){
			api_sysdata_save();
		}else{
			vavle_off_from_app(OFF_REASON_FORCE);
		}
	}else{
		rsp=-8;
	}
	ret=__szrq_load_frame_valve_ctrl(sbuf,ssize,rsp);
	return ret;
}

int16_t __szrq_ins_set_pram(uint8_t* rbuf, uint16_t rlen, uint8_t* sbuf,uint16_t ssize)
{
	bool saveFlg=false;
	int16_t t16,ret=0;
	int32_t t32;
	__szrq_ddfSetBaseParm_t* stb;
	if(rlen<sizeof(__szrq_framHeader_t)+sizeof(__szrq_ddfSetBaseParm_t))return 0;
	stb=(__szrq_ddfSetBaseParm_t*)(rbuf+sizeof(__szrq_framHeader_t));
	api_sysdata_save();	
	//if(stb->)
	if(stb->qsSetFlg){
		m_mem_cpy_len((uint8_t*)&t32,stb->qs,sizeof(uint32_t));
		t16=(int16_t)t32;
		if(t16==QS0_1m3 || t16==QS0_01m3 || t16==QS0_001m3){
			stb->qsSetFlg=1;
			sysData.QS=t16;
			saveFlg=true;
		}else{
			stb->qsSetFlg=-12;
		}
	}
	float f=0.0f;
	if(stb->volSetFlg){
		m_mem_cpy_len((uint8_t*)&t32,stb->volume,sizeof(uint32_t));
		t32=swap_uint32(t32);
		m_mem_cpy_len((uint8_t*)&f,(uint8_t*)&t32,sizeof(uint32_t));
		t32=(int32_t)(100*f);
		sysData.initVolume=t32;
		//
		sysData.totalVolume=t32;
		saveFlg=true;
	}
	
	if(stb->commSnSetFlg){
		m_mem_cpy_len(sysData.commSN,stb->commSnNew,16);
		sysData.commSN[16]='\0';
		saveFlg=true;
	}
	if(saveFlg)api_sysdata_save();	
	
	ret=__szrq_load_frame_base_param(rbuf,rlen,sbuf,ssize);
	return ret;	
}
extern uint32_t szrqRoportNextTm;
int16_t  __szrq_ins_set_report_pram(uint8_t* rbuf, uint16_t rlen, uint8_t* sbuf,uint16_t ssize)
{
	bool saveFlg=false;
	int16_t t16,ret=0;
	int32_t t32;
	__szrq_ddfSetReportParm_t* stb;
	if(rlen<sizeof(__szrq_framHeader_t)+sizeof(__szrq_ddfSetReportParm_t))return 0;
	stb=(__szrq_ddfSetReportParm_t*)(rbuf+sizeof(__szrq_framHeader_t));
	api_sysdata_save();	
	if(stb->periodFlg){
		sysData.szrqRoportPeriodType=stb->period;
		saveFlg=true;
	}
	
	if(stb->intervalFlg){
		sysData.szrqRoportDataInterval=stb->interval;
		saveFlg=true;
	}

	if(stb->periodValueFlg){
		sysData.szrqtPeriodValue=stb->periodValue;
		saveFlg=true;		
	}
	
	if(stb->reportTimeFlg){
		m_mem_cpy_len(sysData.szrqtPeriodTime,stb->reportTime,2);
		saveFlg=true;		
	}	
	
	if(stb->freezeDayFlg){
		sysData.szrqtFreezeDay=stb->freezeDay;
		saveFlg=true;		
	}	
	
	if(stb->freezeHourFlg){
		sysData.szrqtFreezeHour=stb->freezeHour;
		saveFlg=true;		
	}	
	if(saveFlg){
		szrqRoportNextTm=0;
		api_sysdata_save();	
	}
	
	ret=__szrq_load_frame_report_param(rbuf,rlen,sbuf,ssize);
	return ret;		
}

int16_t __szrq_ins_set_comm_param(uint8_t* rbuf, uint16_t rlen, uint8_t* sbuf,uint16_t ssize)
{
	bool saveFlg=false;
	int16_t t16,ret=0;
	int32_t t32;
	__szrq_ddfSetIotParm_t* stb;
	if(rlen<sizeof(__szrq_framHeader_t)+sizeof(__szrq_ddfSetIotParm_t))return 0;
	stb=(__szrq_ddfSetIotParm_t*)(rbuf+sizeof(__szrq_framHeader_t));
	api_sysdata_save();	
	//<<to do
	if(stb->iotIp0Flg){
		m_mem_cpy_len(sysData.severRelease.ip,stb->iotIp0,4);
		saveFlg=true;	
	}
	if(stb->iotIp1Flg){
		m_mem_cpy_len(sysData.severUser.ip,stb->iotIp1,4);
		saveFlg=true;	
	}	
	
	if(stb->iotPort0Flg){
		m_mem_cpy_len((uint8_t*)&t16,stb->iotPort0,sizeof(uint16_t));
		sysData.severRelease.port=t16;
		saveFlg=true;	
	}
	
	if(stb->iotPort1Flg){
		m_mem_cpy_len((uint8_t*)&t16,stb->iotPort1,sizeof(uint16_t));
		sysData.severUser.port=t16;
		saveFlg=true;	
	}	
	//>>
	if(saveFlg)api_sysdata_save();	
	ret=__szrq_load_frame_comm_param(rbuf,rlen,sbuf,ssize);
	return ret;				
}

/*
		uint8_t		pressHiFlg;
		uint8_t		pressHi[4];
		
		uint8_t		pressLoFlg;
		uint8_t		pressLo[4];	

		uint8_t		tempHiFlg;
		uint8_t		tempHi[4];		

		uint8_t		tempLoFlg;
		uint8_t		tempLo[4];	

		uint8_t		balanceWarnFlg;
		uint8_t		balanceWarn[4];	

		uint8_t		balanceOffFlg;
		uint8_t		balanceOff[4];	

		uint8_t		balanceLimitOffFlg;
		uint8_t		balanceLimitOff[4];			
		
		uint8_t		valveCtrlByteFlg;
		uint8_t		valveCtrlByte;	

*/

int16_t __szrq_ins_set_warn_param(uint8_t* rbuf, uint16_t rlen, uint8_t* sbuf,uint16_t ssize)
{
	bool saveFlg=false;
	int16_t t16,ret=0;
	int32_t t32;
	__szrq_ddfSetWarnParm_t* stb;
	if(rlen<sizeof(__szrq_framHeader_t)+sizeof(__szrq_ddfSetWarnParm_t))return 0;
	stb=(__szrq_ddfSetWarnParm_t*)(rbuf+sizeof(__szrq_framHeader_t));
	api_sysdata_save();	
	//<<to do
	if(stb->pressHiFlg){
		stb->pressHiFlg=(int8_t)-12;
	}
	if(stb->pressLoFlg){
		stb->pressHiFlg=(int8_t)-12;
	}
	if(stb->tempHiFlg){
		stb->pressHiFlg=(int8_t)-12;
	}
	if(stb->tempLoFlg){
		stb->pressHiFlg=(int8_t)-12;
	}
	
	if(stb->balanceWarnFlg){
		m_mem_cpy_len((uint8_t*)&t32,stb->balanceWarn,sizeof(uint32_t));
		sysData.warnSetOverageVM=t32;
		saveFlg=true;	
	}
	if(stb->balanceWarnOffFlg){
		m_mem_cpy_len((uint8_t*)&t32,stb->balanceWarnOff,sizeof(uint32_t));
		sysData.warnOffSetOverageVM=t32;
		saveFlg=true;	
	}	
	
	if(stb->balanceOffFlg){
		m_mem_cpy_len((uint8_t*)&t32,stb->balanceOff,sizeof(uint32_t));
		sysData.offSetOverageVM=t32;
		saveFlg=true;	
	}		
	
	if(stb->valveCtrlByteFlg){
		sysData.szrqValveCtrlByte=stb->valveCtrlByte;
		saveFlg=true;	
	}
	//>>
	if(saveFlg)api_sysdata_save();	
	ret=__szrq_load_frame_warn_param(rbuf,rlen,sbuf,ssize);
	return ret;			
}

int16_t __szrq_ins_read_all_param(uint8_t* rbuf, uint16_t rlen, uint8_t* sbuf,uint16_t ssize)
{
	int16_t ret=0;
	//__szrq_ddfGetAllParm_t* stb;
	if(rlen<sizeof(__szrq_framHeader_t)+sizeof(__szrq_ddfGetAllParm_t))return 0;
	//stb=(__szrq_ddfGetAllParm_t*)(rbuf+sizeof(__szrq_framHeader_t));
	//<<todo
	//>>
	ret=__szrq_load_frame_all_param(sbuf,ssize);
	return ret;
}

int16_t __szrq_received_process(uint8_t* rbuf, uint16_t rlen, uint8_t* sbuf,uint16_t ssize)
{
	int16_t ret=0;
	__szrq_framHeader_t* sth;
	//__szrq_ddfCommonBody_t* stb;
	
	if(rlen<sizeof(__szrq_framHeader_t)+16)return 0;
	sth=(__szrq_framHeader_t*)rbuf;
	ret=0;
	szrqSendStaMichine=__15S_SEND_SM_CMD;
	szrqRecHasMore=sth->hasMore;
	switch(sth->cmd){
		case SZRQ_CMD_POP_MULT_RSP:
		case SZRQ_CMD_POP_SINGLE_RSP:
		case SZRQ_CMD_WARNING_REPORT_RSP:	
			szrqSendStaMichine=__15S_SEND_SM_POP;
			ret=__szrq_ins_common_rsp(rbuf,rlen,sbuf,ssize);break;
			
		case SZRQ_CMD_FREEZE_DATA:			ret=__szrq_ins_read_freeze(rbuf,rlen,sbuf,ssize);break;
		case SZRQ_CMD_BALANCE_SYNC:			
			szrqSendStaMichine=__15S_SEND_SM_END_AFTER_SEND;
			ret=__szrq_ins_sync_balance(rbuf,rlen,sbuf,ssize);break;
			
			
		case SZRQ_CMD_VALVE_CTRL:			ret=__szrq_ins_valve_ctrl(rbuf,rlen,sbuf,ssize);break;
		case SZRQ_CMD_SET_BASE_PRAM:		ret=__szrq_ins_set_pram(rbuf,rlen,sbuf,ssize);break;
		case SZRQ_CMD_SET_REPORT_PRAM:		ret=__szrq_ins_set_report_pram(rbuf,rlen,sbuf,ssize);break;
		case SZRQ_CMD_SET_IOT_PRAM:			ret=__szrq_ins_set_comm_param(rbuf,rlen,sbuf,ssize);break;
		case SZRQ_CMD_SET_WARN_PRAM:		ret=__szrq_ins_set_warn_param(rbuf,rlen,sbuf,ssize);break;
		case SZRQ_CMD_GET_ALL_PRAM:			ret=__szrq_ins_read_all_param(rbuf,rlen,sbuf,ssize);break;
		default:szrqSendStaMichine=__15S_SEND_SM_END;break;
	}
	return ret;
}
//file end
