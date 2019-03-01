//file :czsn-comm-protocol.c,潮州申能定制协议
#include "includes.h"
//一些潮州申能的专用数据定义
int16_t czsnRssi=0x00;
uint8_t czsnSendFrameIndex=0x01;
uint8_t czsnFlowStatus=CZSN_FLOW_NORMAL;
uint16_t czsnLastSendSampleRecordNum=0x00;
uint8_t czsnMid[4];

uint32_t noNetworkTimer=0x00L;
uint32_t warningOffTimer=0x00L;
//两位小数，低位在前
uint32_t czsn_bcd_2_uint32(uint8_t* bcd)
{
	uint32_t t32=0;
	uint16_t i;
	for(i=0;i<4;i++){
		t32*=100;
		t32+=m_bcd_2_hex(bcd[3-i]);
	}
	return t32;
}

//两位小数，低位在前
void czsn_uint32_2_bcd(uint8_t* bcd,uint32_t t32)
{
	uint16_t i;
	uint8_t t8;
	for(i=0;i<4;i++){
		__nop();
		t8=(uint8_t)(t32%(100L));
		bcd[i]=(m_hex_2_bcd(t8));
		t32/=(100L);
	}
}

//YY,MM is bcd format
void czsn_combine_id_ex(uint8_t* czsnId,uint32_t id,uint8_t type,uint8_t YY,uint32_t MM)
{
	uint8_t bcd[4];
	m_mem_set(czsnId,0x00,8);
	czsnId[2]=type;
	czsnId[3]=YY;
	czsnId[4]=MM;
	if(id>999999UL)id=999999UL;
	czsn_uint32_2_bcd(bcd,id);
	//id的高位在前
	czsnId[5]=bcd[2];
	czsnId[6]=bcd[1];
	czsnId[7]=bcd[0];
	//m_mem_cpy_len(czsnId+5,bcd+1,3);
}

void czsn_combine_id(uint8_t* czsnId)
{
	uint8_t YY,MM,type;
	uint32_t id;
	type=sysData.czsnDeviceType;
	YY=sysData.PD[0];
	MM=sysData.PD[1];
	id=sysData.ID;
	czsn_combine_id_ex(czsnId,id,type,YY,MM);
}

//从协议中获取id后续补充
uint8_t czsn_set_command_mode_byte(uint8_t dir,uint8_t mulFrame,uint8_t encrype,uint8_t meterType)
{
	__czsn_cmdModeByte_t cmb;
	cmb.t8=0x00;
	if(dir)cmb.bits.dir=1;
	if(mulFrame)cmb.bits.nMultFrameFlg=1;
	if(encrype)cmb.bits.bEncrypt=1;
	
	cmb.bits.bMeterType=meterType;
	return cmb.t8;
}

uint8_t czsn_get_encrype(uint8_t cmdMode)
{
	__czsn_cmdModeByte_t cmd;
	cmd.t8=cmdMode;
	return cmd.bits.bEncrypt;
}
/*
typedef union{
	uint16_t t16;
	struct{
		uint16_t	bReadErr:1;			//0,直读异常
		uint16_t	bBatVoltLow:1;		//1,	
		uint16_t	bSaveDataErr:1;		//2,存储故障
		uint16_t	bRtcErr:1;			//3,
		uint16_t	bBatVoltDown:1;		//4,
		uint16_t	bValveStatus:2;		//6-5
		uint16_t	bNoFlowTimeOut:1;	//7
		uint16_t	bValveCtrlMode:2;	//9-8
		uint16_t	bFlowStatus:2;		//11-10
		uint16_t	bPulseSenStatus:2;	//13-12
		uint16_t	:2;//
	}bits;
}__czsn_meterStatusDef_t;
*/
uint16_t __czsn_meter_status_cross_map(void)
{
	uint16_t t16;
	__czsn_meterStatusDef_t msta;
	msta.t16=0x00;
	if(pwrStatus==POWER_STATUS_LOW)msta.bits.bBatVoltLow=1;
	if(pwrStatus<POWER_STATUS_LOW)msta.bits.bBatVoltDown=1;
	//if(nowDevStatus.bits.bValveErr)msta.bits.bValveStatus=CZSN_VALVE_ERR;
	else{
		if(vavleState==VALVE_ON)msta.bits.bValveStatus=CZSN_VALVE_OPEN;
		else{
			msta.bits.bValveStatus=CZSN_VALVE_CLOSE;
		}
	}
	if(lockReason.bits.bNoflow)msta.bits.bNoFlowTimeOut=1;
	
	msta.bits.bValveCtrlMode=sysData.czsnValveCtrlMode;
	msta.bits.bFlowStatus=czsnFlowStatus;
	
	if(nowDevStatus.bits.bStrongMagnetic)msta.bits.bPulseSenStatus=(CZSN_PULSE_STE);
	t16=msta.t16;
	//t16=swap_uint16(t16);
	return t16;
	//return msta.t16;
}

//应答帧和主动上报帧不需要加密。
uint16_t __czsn_load_body_pop_vol_log_item(uint8_t* popbuf,uint8_t* itembuf)
{
	
	__czsn_ddfSampleRecord_t* record;
	volItem_t* item;
	sysDataTime_t dt;
	uint32_t t32;
	
	record=(__czsn_ddfSampleRecord_t*)popbuf;
	item=(volItem_t*)itembuf;
	t32=item->ts;
	time_stamp_to_system_dt(t32,&dt);
	//分/时/日/月/年,非BCD码格式
	record->dt[0]=m_bcd_2_hex(dt.mm);
	record->dt[1]=m_bcd_2_hex(dt.hh);
	record->dt[2]=m_bcd_2_hex(dt.DD);
	record->dt[3]=m_bcd_2_hex(dt.MM);
	record->dt[4]=m_bcd_2_hex(dt.YY);
	
	t32=item->vol;
	//读数（4BCD）
	t32=(t32*100/sysData.QS);	
	czsn_uint32_2_bcd(record->vol,t32);
	//
	uint16_t t16=item->sta;
	
	m_mem_cpy_len(record->meterSta,(uint8_t*)&t16,sizeof(uint16_t));
	return sizeof(__czsn_ddfSampleRecord_t);
}

uint16_t __czsn_load_body_pop_vol_log(uint8_t* body,uint16_t ssize,uint16_t itemNum,uint16_t* readednum)
{
	uint8_t buf[16];
	uint16_t i;
	uint16_t t16,len;
	uint8_t* p;
	if(itemNum==0)return 0;
	
	len=0;
	p=body+sizeof(__czsn_ddfPop_t);
	for(i=0;i<1;i++){
	//for(i=0;i<itemNum;i++){
		if(len+sizeof(__czsn_ddfSampleRecord_t)>ssize)break;
		vol_log_item_read(buf,sizeof(buf),i);
		t16=__czsn_load_body_pop_vol_log_item(p,buf);
		p+=t16;
		len+=t16;
	}
	if(readednum)*readednum=i;
	return len;
}

uint16_t __czsn_load_body_pop(uint8_t* body,uint16_t ssize)
{
	uint32_t t32;
	uint16_t len,t16=0;
	__czsn_ddfPop_t* stbpop=(__czsn_ddfPop_t*)body;
	m_mem_cpy_len(stbpop->rssi ,(uint8_t*)&czsnRssi,sizeof(uint16_t));
	t16=voltBat/100;
	stbpop->batVolt=(uint8_t)(t16 & 0x00ff);
	//秒/分/时/日/月/年
	stbpop->dt[5]=m_bcd_2_hex(stRtcDataTime.YY);
	stbpop->dt[4]=m_bcd_2_hex(stRtcDataTime.MM);
	stbpop->dt[3]=m_bcd_2_hex(stRtcDataTime.DD);
	stbpop->dt[2]=m_bcd_2_hex(stRtcDataTime.hh);
	stbpop->dt[1]=m_bcd_2_hex(stRtcDataTime.mm);
	stbpop->dt[0]=m_bcd_2_hex(stRtcDataTime.ss);
	
	//表状态
	t16=__czsn_meter_status_cross_map();
	m_mem_cpy_len(stbpop->meterStatus,(uint8_t*)&t16,sizeof(uint16_t));
	//版本号
	t16=sw_VER_NUM;
	m_mem_cpy_len(stbpop->ver,(uint8_t*)&t16,sizeof(uint16_t));
	//当前读数,换算成BCD码
	t32=totalVolume;
	czsn_uint32_2_bcd(stbpop->currVol,t32);
	len=sizeof(__czsn_ddfPop_t);
	//采集记录
	t16=vol_log_item_get_unread_num();
	if(t16>10)t16=10;
	czsnLastSendSampleRecordNum=0x00;
	t16=__czsn_load_body_pop_vol_log(body,ssize,t16,&czsnLastSendSampleRecordNum);
	len+=t16;
	return len;
}


uint8_t czsnID[8];

uint16_t __czsn_load_frame_header(uint8_t* buf,uint16_t ssize,uint8_t* mid,uint8_t cmdCode,uint16_t dlen)
{
	uint8_t t8;
	//uint16_t t16=0x00;
	__czsn_frameHeader_t* sth=(__czsn_frameHeader_t*)buf;
	sth->sta=CZSN_FRAM_START_CHR;

	czsn_combine_id(sth->meterId);
	m_mem_cpy_len(czsnID,sth->meterId,8);
	t8=czsn_set_command_mode_byte(CZSN_DIR_SLV_POP,CZSN_SINGLE_FRAME,CZSN_FI_ENCRYPT_PLAINT,0x00);
	sth->cmdMode=t8;
	
	sth->cmdCode=cmdCode;
	//m_mem_set(sth->mid,0x00,4);
	m_mem_cpy_len(sth->mid,mid,4);
	
	sth->frameSn=czsnSendFrameIndex;
	czsnSendFrameIndex++;	
	m_mem_cpy_len(sth->dfLen,(uint8_t*)&dlen,sizeof(uint16_t));
	return sizeof(__czsn_frameHeader_t);
	
}

uint16_t __czsn_load_frame_pop(uint8_t* buf,uint16_t ssize)
{
	//uint32_t t32;
	uint8_t t8;
	uint16_t len,t16=0x00;
	__czsn_frameHeader_t* sth=(__czsn_frameHeader_t*)buf;
	sth->sta=CZSN_FRAM_START_CHR;

	czsn_combine_id(sth->meterId);
	m_mem_cpy_len(czsnID,sth->meterId,8);
	t8=czsn_set_command_mode_byte(CZSN_DIR_SLV_POP,CZSN_SINGLE_FRAME,CZSN_FI_ENCRYPT_PLAINT,0x00);
	sth->cmdMode=t8;
	
	sth->cmdCode=CZSN_CMD_POP;
	m_mem_set(sth->mid,0x00,4);
	
	sth->frameSn=czsnSendFrameIndex;
	czsnSendFrameIndex++;
	
	t16=__czsn_load_body_pop(buf+sizeof(__czsn_frameHeader_t),ssize-sizeof(__czsn_frameHeader_t));
	m_mem_cpy_len(sth->dfLen,(uint8_t*)&t16,sizeof(uint16_t));
	len=sizeof(__czsn_frameHeader_t)+t16;
	crc_append(buf,len);
	len+=2;
	//
	return len;
}

uint16_t __czsn_load_frame_valve_ctrl(uint8_t* buf,uint16_t ssize)
{
	uint8_t t8;
	uint16_t t16,len;
	__czsn_frameHeader_t* sth=(__czsn_frameHeader_t*)buf;
	sth->sta=CZSN_FRAM_START_CHR;

	czsn_combine_id(sth->meterId);
	m_mem_cpy_len(czsnID,sth->meterId,8);
	t8=czsn_set_command_mode_byte(CZSN_DIR_SLV_POP,CZSN_SINGLE_FRAME,CZSN_FI_ENCRYPT_PLAINT,0x00);
	sth->cmdMode=t8;
	
	sth->cmdCode=CZSN_CMD_VALVE_CTRL;
	m_mem_cpy_len(sth->mid,czsnMid,4);
	
	sth->frameSn=czsnSendFrameIndex;
	czsnSendFrameIndex++;	
	
	t16=2;
	m_mem_cpy_len(sth->dfLen,(uint8_t*)&t16,sizeof(uint16_t));

	t16=__czsn_meter_status_cross_map();
	len=sizeof(__czsn_frameHeader_t);
	
	m_mem_cpy_len(buf+len,(uint8_t*)&t16,sizeof(uint16_t));
	
	len+=2;
	crc_append(buf,len);
	len+=2;
	//
	return len;	
}



uint16_t __czsn_load_frame_read_meter(uint8_t* buf,uint16_t ssize,uint8_t cmdCode)
{
	int32_t t32;
	uint8_t t8;
	uint16_t t16,len;
	__czsn_frameHeader_t* sth=(__czsn_frameHeader_t*)buf;
	sth->sta=CZSN_FRAM_START_CHR;

	czsn_combine_id(sth->meterId);
	m_mem_cpy_len(czsnID,sth->meterId,8);
	t8=czsn_set_command_mode_byte(CZSN_DIR_SLV_POP,CZSN_SINGLE_FRAME,CZSN_FI_ENCRYPT_PLAINT,0x00);
	sth->cmdMode=t8;
	
	//sth->cmdCode=CZSN_CMD_READING;
	sth->cmdCode=cmdCode;
	m_mem_cpy_len(sth->mid,czsnMid,4);
	
	sth->frameSn=czsnSendFrameIndex;
	czsnSendFrameIndex++;	
	
	t16=sizeof(__czsn_ddfReading_t);
	m_mem_cpy_len(sth->dfLen,(uint8_t*)&t16,sizeof(uint16_t));	
	
	//
	len=sizeof(__czsn_frameHeader_t);
	__czsn_ddfReading_t* stb=(__czsn_ddfReading_t*)(buf+len);
	
	m_mem_cpy_len(stb->rssi ,(uint8_t*)&czsnRssi,sizeof(uint16_t));
	t16=voltBat/100;
	stb->batVolt=(uint8_t)(t16 & 0x00ff);
	//秒/分/时/日/月/年
	stb->dt[5]=m_bcd_2_hex(stRtcDataTime.YY);
	stb->dt[4]=m_bcd_2_hex(stRtcDataTime.MM);
	stb->dt[3]=m_bcd_2_hex(stRtcDataTime.DD);
	stb->dt[2]=m_bcd_2_hex(stRtcDataTime.hh);
	stb->dt[1]=m_bcd_2_hex(stRtcDataTime.mm);
	stb->dt[0]=m_bcd_2_hex(stRtcDataTime.ss);
	
	//表状态
	t16=__czsn_meter_status_cross_map();
	m_mem_cpy_len(stb->meterStatus,(uint8_t*)&t16,sizeof(uint16_t));
	//版本号
	t16=sw_VER_NUM;
	m_mem_cpy_len(stb->ver,(uint8_t*)&t16,sizeof(uint16_t));
	//当前读数,换算成BCD码
	t32=totalVolume;
	czsn_uint32_2_bcd(stb->currVol,t32);	
	
	len=len+sizeof(__czsn_ddfReading_t);
	crc_append(buf,len);
	len+=2;
	//
	return len;		
}

uint16_t __czsn_load_frame_sync_rtc(uint8_t* buf,uint16_t ssize)
{
	uint16_t len;
	len=sizeof(__czsn_ddfSyncRtc_t);
	len=__czsn_load_frame_header(buf,ssize,czsnMid,CZSN_CMD_SYNC_RTC,len);
	uint8_t* p=buf+sizeof(__czsn_frameHeader_t);
	p[5]=m_bcd_2_hex(stRtcDataTime.YY);
	p[4]=m_bcd_2_hex(stRtcDataTime.MM);
	p[3]=m_bcd_2_hex(stRtcDataTime.DD);
	p[2]=m_bcd_2_hex(stRtcDataTime.hh);
	p[1]=m_bcd_2_hex(stRtcDataTime.mm);
	p[0]=m_bcd_2_hex(stRtcDataTime.ss);
	len+=sizeof(__czsn_ddfSyncRtc_t);
	crc_append(buf,len);
	len+=2;
	return len;			
}

uint16_t __czsn_load_frame_apn(uint8_t* buf,uint16_t ssize,uint8_t cmdCode)
{
	uint16_t t16,len;
	len=1;		//先随便填一个长度
	len=__czsn_load_frame_header(buf,ssize,czsnMid,cmdCode,len);	
	uint8_t* p=buf+sizeof(__czsn_frameHeader_t);	
	severInf_t* stSevInf;
	switch(sysData.severIndex){
		case 0:stSevInf=&(sysData.severTest);break;
		case 1:stSevInf=&(sysData.severRelease);break;
		default:stSevInf=&(sysData.severUser);break;
	}
	t16=ipv4_bin_to_str(p,stSevInf->ip);
	p[t16-1]='\t';
	p+=t16;
	len+=t16;
	
	t16=m_str_int32_2_str(p,stSevInf->port);
	p[t16-1]='\t';
	p+=t16;
	len+=t16;
	
	t16=m_str_cpy(p,(uint8_t*)"ctnb");
	p[t16-1]='\t';
	p+=t16;
	len+=t16;
	
	t16=m_str_cpy(p,(uint8_t*)"xxxxx");
	p[t16-1]='\t';
	p+=t16;
	len+=t16;	
	
	t16=m_str_cpy(p,(uint8_t*)"x");
	p[t16-1]='\t';
	p+=t16;
	len+=t16;
	
	t16=m_str_cpy(p,(uint8_t*)"x");
	//p[t16-1]='\t';
	//p+=t16;
	//len+=t16;	
	t16--;
	len+=t16;
	
	t16=len-sizeof(__czsn_frameHeader_t);
	__czsn_frameHeader_t* sth=(__czsn_frameHeader_t*)buf;
	m_mem_cpy_len(sth->dfLen,(uint8_t*)&t16,sizeof(uint16_t));
	crc_append(buf,len);
	len+=2;
	return len;			

}

uint16_t __czsn_load_frame_base_param(uint8_t* buf,uint16_t ssize,uint8_t cmdCode)
{
	uint16_t len;
	len=sizeof(__czsn_ddfbaseParam_t);
	len=__czsn_load_frame_header(buf,ssize,czsnMid,cmdCode,len);
	__czsn_ddfbaseParam_t* stb=(__czsn_ddfbaseParam_t*)(buf+sizeof(__czsn_frameHeader_t));	
	m_mem_cpy_len(stb->sendParam,(uint8_t*)(sysData.czsnSendParam),24);
	m_mem_cpy_len(stb->sampleParam,(uint8_t*)(sysData.czsnSampleParam),24);
	stb->settleDate=sysData.czsnSettleDate;
	
	len+=(sizeof(__czsn_ddfbaseParam_t));
	crc_append(buf,len);
	len+=2;
	return len;			
}

uint16_t __czsn_load_frame_settle_inf(uint8_t* buf,uint16_t ssize)
{
	uint16_t len;
	len=0;
	len=__czsn_load_frame_header(buf,ssize,czsnMid,CZSN_CMD_SYNC_RTC,len);
	crc_append(buf,len);
	len+=2;
	return len;			
}

uint16_t __czsn_load_frame_warnclose_mode(uint8_t* buf,uint16_t ssize,uint8_t cmdCode)
{
	uint16_t t16,len;
	
	len=1;
	len=__czsn_load_frame_header(buf,ssize,czsnMid,cmdCode,len);
	uint8_t* p=buf+sizeof(__czsn_frameHeader_t);	
	
	p[0]=sysData.czsnWarnCloseMode;
	len+=1;
	crc_append(buf,len);
	len+=2;
	return len;		
}


uint16_t __czsn_load_frame_noflow(uint8_t* buf,uint16_t ssize,uint8_t cmdCode)
{
	uint16_t t16,len;
	
	len=1;
	len=__czsn_load_frame_header(buf,ssize,czsnMid,cmdCode,len);
	uint8_t* p=buf+sizeof(__czsn_frameHeader_t);	
	t16=sysData.ConstNoFlowTimeOut;
	t16/=24;
	p[0]=(uint8_t)t16;
	
	len+=1;
	crc_append(buf,len);
	len+=2;
	return len;		
}

uint16_t __czsn_load_frame_nonet(uint8_t* buf,uint16_t ssize,uint8_t cmdCode)
{
	uint16_t t16,len;
	
	len=1;
	len=__czsn_load_frame_header(buf,ssize,czsnMid,cmdCode,len);
	uint8_t* p=buf+sizeof(__czsn_frameHeader_t);	
	t16=sysData.czsnUnNetTimeOut;
	t16/=24;
	p[0]=(uint8_t)t16;
	
	len+=1;
	crc_append(buf,len);
	len+=2;
	return len;		
}

uint16_t __czsn_load_frame_sync_balance(uint8_t* buf,uint16_t ssize)
{
	uint16_t t16,len;
	len=2;
	len=__czsn_load_frame_header(buf,ssize,czsnMid,CZSN_CMD_SYNC_BALANCE,len);

	t16=__czsn_meter_status_cross_map();
	len=sizeof(__czsn_frameHeader_t);
	m_mem_cpy_len(buf+len,(uint8_t*)&t16,sizeof(uint16_t));	
	
	len+=2;
	crc_append(buf,len);
	len+=2;
	return len;			
}

uint16_t __czsn_load_frame_get_modinf(uint8_t* buf,uint16_t ssize)
{
	uint16_t len;
	
	len=sizeof(CGSN)+1+sizeof(ICCID);
	len=__czsn_load_frame_header(buf,ssize,czsnMid,CZSN_CMD_GET_MODULE_INF,len);
	
	//len=sizeof(__czsn_frameHeader_t);
	uint8_t* p=buf+sizeof(__czsn_frameHeader_t);
	
	m_mem_cpy_len(p,CGSN,sizeof(CGSN));
	len+=sizeof(CGSN);
	p+=sizeof(CGSN);
	
	*p='\t';
	p++;
	len++;
	
	m_mem_cpy_len(p,ICCID,sizeof(ICCID));
	len+=sizeof(ICCID);
	
	crc_append(buf,len);
	len+=2;
	return len;		
}

uint16_t __czsn_load_frame_update_key(uint8_t* buf,uint16_t ssize,uint8_t ret)
{
	uint16_t len;
	
	len=1;
	len=__czsn_load_frame_header(buf,ssize,czsnMid,CZSN_CMD_UPDATE_KEY,len);
	uint8_t* p=buf+sizeof(__czsn_frameHeader_t);	
	p[0]=ret;
	len+=1;
	crc_append(buf,len);
	len+=2;
	return len;		
}

uint16_t __czsn_load_frame_set_id(uint8_t* buf,uint16_t ssize)
{
	uint16_t len;
	
	len=8;
	len=__czsn_load_frame_header(buf,ssize,czsnMid,CZSN_CMD_SET_METER_ID,len);
	uint8_t* p=buf+sizeof(__czsn_frameHeader_t);	
	
	czsn_combine_id(p);

	len+=8;
	crc_append(buf,len);
	len+=2;
	return len;			
}

uint16_t __czsn_load_frame_mod_vol(uint8_t* buf,uint16_t ssize)
{
	uint16_t t16;
	t16=__czsn_load_frame_read_meter(buf,sizeof(frameBuf),CZSN_CMD_MOD_VOL);
	return t16;
}

uint16_t czsn_ins_pop_ack(uint8_t*rbuf,uint16_t rlen)
{
	sysDataTime_t dt;
	uint8_t* p=rbuf+sizeof(__czsn_frameHeader_t);
	dt.ss=m_hex_2_bcd(p[0]);
	dt.mm=m_hex_2_bcd(p[1]);
	dt.hh=m_hex_2_bcd(p[2]);
	dt.DD=m_hex_2_bcd(p[3]);
	dt.MM=m_hex_2_bcd(p[4]);
	dt.YY=m_hex_2_bcd(p[5]);
	dt.YYhi=0x20;
	dt.day=0;
	m_rtc_set(&dt);
	__nop();
	__nop();
	//return __czsn_load_frame_pop(frameBuf,sizeof(frameBuf));
	czsnNbCommStatus=CZSN_NB_COMM_POP_ACK;
	rtcSync=true;
	return 1;
}

uint16_t __czsn_valve_ctrl(uint8_t ctrlMode)
{
	uint8_t ret;
	sysData.czsnValveCtrlMode=ctrlMode;
	switch(ctrlMode){
		case CZSN_VALVE_CTRL_OPEN:
			qc_data_device_even_lock_clear();
			if(sysData.DLCS>=DLC_STATUS_C)sysData.DLCS=DLC_STATUS_C;
			qc_data_save_sys_data(true);
			ret=1;
			break;
		case CZSN_VALVE_CTRL_FORCE_CLOSE:
			sysData.lockReason.bits.bSeverOff=1;
			lockReason.bits.bSeverOff=1;
			qc_data_save_sys_data(true);
			vavle_off_from_app(OFF_REASON_SEVERS);
			osDelay(2000);
			ret=1;
			break;
		case CZSN_VALVE_CTRL_WARN_CLOSE:
			sysData.lockReason.bits.bSeverOff=0;
			lockReason.bits.bSeverOff=0;
			qc_data_save_sys_data(true);
			vavle_off_from_app(OFF_REASON_CZSN_SEVER_WARN);
			osDelay(2000);
			ret=1;
			break;		
		default:
			ret=0;
	}
	return ret;
}



uint16_t czsn_ins_read_meter(uint8_t* rbuf,uint16_t rlen)
{
	uint16_t t16;
	czsnNbCommStatus=CZSN_NB_COMM_CMD;
	t16=__czsn_load_frame_read_meter(rbuf,sizeof(frameBuf),CZSN_CMD_READING);
	return t16;
}

uint16_t czsn_ins_valve_ctrl(uint8_t* rbuf,uint16_t rlen)
{
	uint16_t ret,t16;
	__czsn_ddfValveCtrl_t* stb=(__czsn_ddfValveCtrl_t*)(rbuf+ sizeof(__czsn_frameHeader_t));
	ret=m_str_cmp_len(czsnID,stb->meterId,8);
	if(!ret)return 0;
	qc_data_save_sys_data(true);
	
	sysData.czsnValveCtrlMode=stb->valveCtrlByte;
	ret=0;
	ret=__czsn_valve_ctrl(stb->valveCtrlByte);
	if(!ret)return ret;
	
	czsnNbCommStatus=CZSN_NB_COMM_CMD;
	t16=__czsn_load_frame_valve_ctrl(rbuf,sizeof(frameBuf));
	__nop();
	return t16;
}

uint16_t czsn_ins_sync_rtc(uint8_t* rbuf,uint16_t rlen)
{
	uint16_t t16;
	czsnNbCommStatus=CZSN_NB_COMM_CMD;	
	
	sysDataTime_t dt;
	uint8_t* p=rbuf+sizeof(__czsn_frameHeader_t);
	dt.ss=m_hex_2_bcd(p[0]);
	dt.mm=m_hex_2_bcd(p[1]);
	dt.hh=m_hex_2_bcd(p[2]);
	dt.DD=m_hex_2_bcd(p[3]);
	dt.MM=m_hex_2_bcd(p[4]);
	dt.YY=m_hex_2_bcd(p[5]);
	dt.day=0;
	dt.YYhi=0x00;
	m_rtc_set(&dt);	
	rtcSync=true;
	t16=__czsn_load_frame_sync_rtc(rbuf,sizeof(frameBuf));
	return t16;
}

uint16_t czsn_ins_set_apn(uint8_t* rbuf,uint16_t rlen)
{
	uint16_t ret,len;
	int16_t t16;
	uint8_t buf[16]; 
	uint8_t ip[4];
	czsnNbCommStatus=CZSN_NB_COMM_CMD;	
	qc_data_save_sys_data(true);
	uint8_t* p=(rbuf+sizeof(__czsn_frameHeader_t));
	len=sizeof(__czsn_frameHeader_t);
	ret=0;
	do{
		if(len>=rlen)break;
		t16=m_str_chr_loc_ex(p,'\t',rlen-len);
		if(t16<0)break;

		m_mem_cpy_len(buf,p,t16);
		buf[t16]='\0';
		ipv4_str_to_bin(ip,buf);
		__nop();
		m_mem_cpy_len(sysData.severUser.ip,ip,4);

		t16++;
		p+=t16;
		len+=t16;
		if(len>=rlen)break;
		
		t16=m_str_chr_loc_ex(p,'\t',rlen-len);
		if(t16<0)break;
		m_mem_cpy_len(buf,p,t16);
		buf[t16]='\0';
		int16_t port=(int16_t)m_str_dec_2_int32(buf);
		sysData.severUser.port=port;
		ret=1;
	}while(0);
	if(ret)qc_data_save_sys_data(true);
	
	
	t16=__czsn_load_frame_apn(rbuf,sizeof(frameBuf),CZSN_CMD_SET_APN);
	return t16;	
}

uint16_t czsn_ins_get_apn(uint8_t* rbuf,uint16_t rlen)
{
	uint16_t t16,len;
	czsnNbCommStatus=CZSN_NB_COMM_CMD;	
	
	t16=__czsn_load_frame_apn(rbuf,sizeof(frameBuf),CZSN_CMD_GET_APN);
	return t16;	
}

uint16_t czsn_ins_set_base_param(uint8_t* rbuf,uint16_t rlen)
{
	uint16_t t16,len;
	czsnNbCommStatus=CZSN_NB_COMM_CMD;	
	qc_data_save_sys_data(true);
	__czsn_ddfbaseParam_t* stb=(__czsn_ddfbaseParam_t*)(rbuf+sizeof(__czsn_frameHeader_t));
	m_mem_cpy_len((uint8_t*)sysData.czsnSendParam,stb->sendParam,24);
	m_mem_cpy_len((uint8_t*)sysData.czsnSampleParam,stb->sampleParam,24);
	sysData.czsnSettleDate=stb->settleDate;
	qc_data_save_sys_data(true);
	
	czsn_data_load_sample_send_time(&czsnSampleTime,sysData.czsnSampleParam);
	czsn_data_load_sample_send_time(&czsnSendTime,sysData.czsnSendParam);
	t16=__czsn_load_frame_base_param(rbuf,sizeof(frameBuf),CZSN_CMD_SET_BASE_PARAM);
	return t16;	
}

uint16_t czsn_ins_get_base_param(uint8_t* rbuf,uint16_t rlen)
{
	uint16_t t16,len;
	czsnNbCommStatus=CZSN_NB_COMM_CMD;	
	t16=__czsn_load_frame_base_param(rbuf,sizeof(frameBuf),CZSN_CMD_GET_BASE_PARAM);
	return t16;
}

uint16_t czsn_ins_get_settle_inf(uint8_t* rbuf,uint16_t rlen)
{
	uint16_t t16,len;
	czsnNbCommStatus=CZSN_NB_COMM_CMD;	
	
	t16=__czsn_load_frame_settle_inf(rbuf,sizeof(frameBuf));
	return t16;	
}

uint16_t czsn_ins_set_warn_close(uint8_t* rbuf,uint16_t rlen)
{
	uint16_t t16,len;
	czsnNbCommStatus=CZSN_NB_COMM_CMD;	
	
	qc_data_save_sys_data(true);
	uint8_t* p=rbuf+sizeof(__czsn_frameHeader_t);
	sysData.czsnWarnCloseMode=p[0];
	qc_data_save_sys_data(true);
	
	t16=__czsn_load_frame_warnclose_mode(rbuf,sizeof(frameBuf),CZSN_CMD_SET_WARN_CLOSE);
	return t16;	
}

uint16_t czsn_ins_get_warn_close(uint8_t* rbuf,uint16_t rlen)
{
	uint16_t t16,len;
	czsnNbCommStatus=CZSN_NB_COMM_CMD;
	
	t16=__czsn_load_frame_warnclose_mode(rbuf,sizeof(frameBuf),CZSN_CMD_GET_WARN_CLOSE);
	return t16;	
}

uint16_t czsn_ins_set_noflow_tm(uint8_t* rbuf,uint16_t rlen)
{
	uint16_t t16;
	czsnNbCommStatus=CZSN_NB_COMM_CMD;	
	qc_data_save_sys_data(true);
		
	uint8_t* p=rbuf+sizeof(__czsn_frameHeader_t);
	t16=p[0];
	if(t16>31)t16=31;
	t16*=24;
	sysData.ConstNoFlowTimeOut=t16;
	qc_data_save_sys_data(true);
	t16=__czsn_load_frame_noflow(rbuf,sizeof(frameBuf),CZSN_CMD_SET_NOFLOW_TIMEOUT);
	return t16;
}

uint16_t czsn_ins_get_noflow_tm(uint8_t* rbuf,uint16_t rlen)
{
	uint16_t t16;
	czsnNbCommStatus=CZSN_NB_COMM_CMD;	
	t16=__czsn_load_frame_noflow(rbuf,sizeof(frameBuf),CZSN_CMD_GET_NOFLOW_TIMEOUT);
	return t16;	
}

uint16_t czsn_ins_set_nonet_tm(uint8_t* rbuf,uint16_t rlen)
{
	uint16_t t16;
	czsnNbCommStatus=CZSN_NB_COMM_CMD;	
	qc_data_save_sys_data(true);
	
	uint8_t* p=rbuf+sizeof(__czsn_frameHeader_t);
	t16=p[0];
	if(t16>31)t16=31;
	t16*=24;
	sysData.czsnUnNetTimeOut=t16;
	qc_data_save_sys_data(true);
	
	t16=__czsn_load_frame_nonet(rbuf,sizeof(frameBuf),CZSN_CMD_SET_NONET_TIMEOUT);
	return t16;
}

uint16_t czsn_ins_get_nonet_tm(uint8_t* rbuf,uint16_t rlen)
{
	uint16_t t16;
	czsnNbCommStatus=CZSN_NB_COMM_CMD;		
	t16=__czsn_load_frame_nonet(rbuf,sizeof(frameBuf),CZSN_CMD_GET_NONET_TIMEOUT);
	return t16;	
}

uint16_t czsn_ins_sync_balance(uint8_t* rbuf,uint16_t rlen)
{
	int32_t t32;
	uint16_t t16;
	czsnNbCommStatus=CZSN_NB_COMM_CMD;	
	qc_data_save_sys_data(true);
	
	__czsn_ddfSyncBalance_t* stb=(__czsn_ddfSyncBalance_t*)(rbuf+sizeof(__czsn_frameHeader_t));
	m_mem_cpy_len(sysData.czsnLastSettlleDt,stb->preSettleDt,3);
	m_mem_cpy_len((uint8_t*)&t32,stb->preSettleM,sizeof(int32_t));
	sysData.czsnLastSettlleM=t32;
	m_mem_cpy_len(sysData.czsnLastPayDt,stb->lastChargeDt,3);
	m_mem_cpy_len((uint8_t*)&t32,stb->lastChargeM,sizeof(int32_t));
	sysData.czsnLastPayM=t32;
	sysData.czsnValveCtrlMode=stb->valveCtrlMode;
	sysData.czsnSyncShowSta=stb->syncShowSta;
	qc_data_save_sys_data(true);
	osDelay(100);
	//控制阀门
	__czsn_valve_ctrl(sysData.czsnValveCtrlMode);
	
	t16=__czsn_load_frame_sync_balance(rbuf,sizeof(frameBuf));
	return t16;
}

uint16_t czsn_ins_get_module_inf(uint8_t* rbuf,uint16_t rlen)
{
	uint16_t t16,len;
	czsnNbCommStatus=CZSN_NB_COMM_CMD;	
	t16=__czsn_load_frame_get_modinf(rbuf,sizeof(frameBuf));
	return t16;
}
uint16_t czsn_ins_update_key(uint8_t* rbuf,uint16_t rlen)
{
	uint16_t t16;
	uint8_t ret=0;
	czsnNbCommStatus=CZSN_NB_COMM_CMD;	
	
	do{
		t16=sizeof(__czsn_frameHeader_t)+8+2;
		if(rlen<=t16)break;
		
		__czsn_frameHeader_t* sth=(__czsn_frameHeader_t*)rbuf;
		m_mem_cpy_len((uint8_t*)&t16,sth->dfLen,sizeof(uint16_t));
		if(t16!=16)break;
		
		qc_data_save_sys_data(true);
		m_mem_cpy_len(sysData.aesKey,rbuf+sizeof(__czsn_frameHeader_t),16);
		qc_data_save_sys_data(true);
		ret=1;
	}while(0);
	
	t16=__czsn_load_frame_update_key(rbuf,sizeof(frameBuf),ret);
	return t16;
}

uint16_t czsn_ins_set_meter_id(uint8_t* rbuf,uint16_t rlen)
{
	uint32_t t32;
	uint8_t xid[4];
	uint16_t t16,len;
	czsnNbCommStatus=CZSN_NB_COMM_CMD;	
	uint8_t* p=rbuf+sizeof(__czsn_frameHeader_t);
	
	qc_data_save_sys_data(true);
	sysData.czsnDeviceType=p[2];
	sysData.PD[0]=p[3];
	sysData.PD[1]=p[4];
	
	xid[0]=p[7];
	xid[1]=p[6];
	xid[2]=p[5];
	xid[3]=0;
	t32=czsn_bcd_2_uint32(xid);
	sysData.ID=t32;
	qc_data_save_sys_data(true);
	
	t16=__czsn_load_frame_set_id(rbuf,sizeof(frameBuf));
	return t16;
	//return 0;
}

uint16_t czsn_ins_set_volume(uint8_t* rbuf,uint16_t rlen)
{
	int32_t t32;
	uint16_t t16,len;
	czsnNbCommStatus=CZSN_NB_COMM_CMD;
	
	qc_data_save_sys_data(true);
	len=sizeof(__czsn_frameHeader_t);
	t32=czsn_bcd_2_uint32(rbuf+len);
	t32=t32*sysData.QS/100;
	sysData.totalVolume=t32;
	//
	qc_data_save_sys_data(true);
	//t16=__czsn_load_frame_read_meter(rbuf,sizeof(frameBuf),CZSN_CMD_MOD_VOL);
	t16=__czsn_load_frame_mod_vol(rbuf,sizeof(frameBuf));
	return t16;	
}


uint8_t testKey[]={0x6D,0x0D,0x16,0x64,0x64,0x45,0x6B,0x00,0x67,0x52,0x7B,0x7A,0x14,0x15,0x7E,0x10};
uint16_t czsn_comm_recevied_process(uint8_t*rbuf,uint16_t rlen)
{
	uint16_t ret,t16;
	__czsn_frameHeader_t* sth=(__czsn_frameHeader_t*)rbuf;
	if(rlen<sizeof(__czsn_frameHeader_t)+2)return 0;
	
	ret=m_str_cmp_len(czsnID,sth->meterId,8);
	if(!ret)return 0;
	
	ret=crc_verify(rbuf,rlen);
	if(!ret)return 0;
	
	m_mem_cpy_len(czsnMid,sth->mid,4);
	m_mem_cpy_len((uint8_t*)&t16,sth->dfLen,sizeof(uint16_t));
	if(czsn_get_encrype(sth->cmdMode)){
		t16=m_ebc_decrypt(nbAplReceivedBuffer,rbuf+sizeof(__czsn_frameHeader_t),sysData.aesKey,t16);
		//t16=m_ebc_decrypt(nbAplReceivedBuffer,rbuf+sizeof(__czsn_frameHeader_t),testKey,t16);
		__nop();
		m_mem_cpy_len(rbuf+sizeof(__czsn_frameHeader_t),nbAplReceivedBuffer,t16);
	}
	ret=0;
	switch(sth->cmdCode){
		case CZSN_CMD_POP:					ret=czsn_ins_pop_ack(rbuf,rlen);		break;
		case CZSN_CMD_VALVE_CTRL:			ret=czsn_ins_valve_ctrl(rbuf,rlen);		break;
		case CZSN_CMD_READING:				ret=czsn_ins_read_meter(rbuf,rlen);		break;
		case CZSN_CMD_SYNC_RTC:				ret=czsn_ins_sync_rtc(rbuf,rlen);		break;
		
		case CZSN_CMD_SET_APN:				ret=czsn_ins_set_apn(rbuf,rlen);		break;
		case CZSN_CMD_GET_APN:				ret=czsn_ins_get_apn(rbuf,rlen);		break;
		case CZSN_CMD_SET_BASE_PARAM:		ret=czsn_ins_set_base_param(rbuf,rlen);	break;
		case CZSN_CMD_GET_BASE_PARAM:		ret=czsn_ins_get_base_param(rbuf,rlen);	break;
		
		case CZSN_CMD_GET_SETTLE_INF:		ret=czsn_ins_get_settle_inf(rbuf,rlen);	break;
		
		case CZSN_CMD_SET_WARN_CLOSE:		ret=czsn_ins_set_warn_close(rbuf,rlen);	break;
		case CZSN_CMD_GET_WARN_CLOSE:		ret=czsn_ins_get_warn_close(rbuf,rlen);	break;
		
		case CZSN_CMD_SET_NOFLOW_TIMEOUT:	ret=czsn_ins_set_noflow_tm(rbuf,rlen);	break;
		case CZSN_CMD_GET_NOFLOW_TIMEOUT:	ret=czsn_ins_get_noflow_tm(rbuf,rlen);	break;		
		
		case CZSN_CMD_SET_NONET_TIMEOUT:	ret=czsn_ins_set_nonet_tm(rbuf,rlen);	break;
		case CZSN_CMD_GET_NONET_TIMEOUT:	ret=czsn_ins_get_nonet_tm(rbuf,rlen);	break;

		case CZSN_CMD_SYNC_BALANCE:			ret=czsn_ins_sync_balance(rbuf,rlen);	break;
		
		case CZSN_CMD_GET_MODULE_INF:		ret=czsn_ins_get_module_inf(rbuf,rlen);	break;
		
		case CZSN_CMD_UPDATE_KEY:			ret=czsn_ins_update_key(rbuf,rlen);		break;
		
		case CZSN_CMD_SET_METER_ID:			ret=czsn_ins_set_meter_id(rbuf,rlen);	break;
		
		case CZSN_CMD_MOD_VOL:				ret=czsn_ins_set_volume(rbuf,rlen);		break;
		default:
			__nop();
			break;
	}
	return ret;
}

