#include "./includes/includes.h"
#include <float.h>

extern stDeviceInf_t sysData;
extern stDeviceInf_t* stpSysData;
extern const stDeviceInf_t sysDataDefault;
uint32_t adcValue = 1ul;
uint32_t voltBat=0x01;
uint32_t voltAvdd=0x01;	
osMutexId osMutexSysData=NULL;
#define MANUAL_ID 60051
void qc_data_mutex_create(void)
{
	osMutexDef(osMutexSysData);
	osMutexSysData = osMutexCreate(osMutex(osMutexSysData));	
}

void qc_data_read_from_media(uint8_t* d,uint32_t mediaAddr,uint16_t len)
{
	m_mem_cpy_len(d,(uint8_t*)mediaAddr,len);
}

void qc_data_write_to_media(uint32_t mediaAddr,uint8_t* s,uint16_t len)
{
	m_flash_write_eeprom(mediaAddr,s,len);
}

void qc_data_set_rtc(uint8_t* dt)
{
	uint8_t t8;
	for(t8=0;t8<sizeof(sysDataTime_t);t8++){
		dt[t8]=m_hex_2_bcd(dt[t8]);
	}
	__nop();
	api_rtc_set((sysDataTime_t*)dt);
}

uint16_t qc_data_sysdata_set_default(void)
{
	uint16_t ret=0;
	uint8_t* buf;
	buf=(uint8_t*)(&sysData);	

	qc_data_read_from_media(buf,(uint32_t)(&sysDataDefault),sizeof(stDeviceInf_t));
	//add 
	sysData.ID=default_CONST_ID;
	//sysData.ID=defaultID;
	#if config_NB_PLAT==UDP
	if(sysData.ID== ~0x00ul || sysData.ID== 0x00ul)sysData.ID=MANUAL_ID;
	#else
	if(sysData.ID== ~0x00ul || sysData.ID== 0x00ul)sysData.ID=MANUAL_ID;
	#endif
	crc_append(buf,sizeof(stDeviceInf_t)-2);
	qc_data_write_to_media(SYS_DATA_MAIN_ADDR,buf,sizeof(stDeviceInf_t));
	
	qc_data_read_from_media(buf,SYS_DATA_MAIN_ADDR,sizeof(stDeviceInf_t));
	ret=crc_verify(buf,sizeof(stDeviceInf_t));		

	return ret;
}

uint16_t qc_data_sysdata_write_only(uint8_t* tbuf)
{
	uint16_t ret=0;
	uint8_t* buf;
	buf=(uint8_t*)(&sysData);	
	m_mem_cpy_len(buf,tbuf,sizeof(stDeviceInf_t));
	crc_append(buf,sizeof(stDeviceInf_t)-2);
	qc_data_write_to_media(SYS_DATA_MAIN_ADDR,buf,sizeof(stDeviceInf_t));

	qc_data_read_from_media(buf,SYS_DATA_MAIN_ADDR,sizeof(stDeviceInf_t));
	ret=crc_verify(buf,sizeof(stDeviceInf_t));		
	return ret;	
}

uint16_t qc_data_sysdata_init(void)
{
	float f=1.0;
	uint16_t ret=0;
	uint8_t* buf;
	uint32_t t32;
	buf=(uint8_t*)(&sysData);

	qc_data_read_from_media(buf,SYS_DATA_MAIN_ADDR,sizeof(stDeviceInf_t));
	ret=crc_verify(buf,sizeof(stDeviceInf_t));
	if(!ret){
		qc_data_read_from_media(buf,(uint32_t)(&sysDataDefault),sizeof(stDeviceInf_t));
		
		sysData.ID=default_CONST_ID;
		//sysData.ID=defaultID;
		#if config_NB_PLAT==UDP
		if(sysData.ID== ~0x00ul || sysData.ID== 0x00ul)sysData.ID=MANUAL_ID;
		#else
		if(sysData.ID== ~0x00ul || sysData.ID== 0x00ul)sysData.ID=MANUAL_ID;
		#endif	
		
		crc_append(buf,sizeof(stDeviceInf_t)-2);
		qc_data_write_to_media(SYS_DATA_MAIN_ADDR,buf,sizeof(stDeviceInf_t));
		
		qc_data_read_from_media(buf,SYS_DATA_MAIN_ADDR,sizeof(stDeviceInf_t));
		ret=crc_verify(buf,sizeof(stDeviceInf_t));		
	}
	
	if(sysData.DLCS==DLC_STATUS_A && sysData.resetPayNums < 9 && sysData.OVerageVM<=0x00l){
		sysData.OVerageVM=default_OVERAGE_VM_A;
		sysData.resetPayNums++;
	}
	
	
	//DeviceEvent.data=sysData.deviceEvent.data;
	//nowDevStatus.t32=0x00;
	//lastDevStatus.t32=sysData.devStatus.t32;
	//lockReason.t32=sysData.lockReason.t32;
	rtPrice=sysData.curPrice;
	//
	m_rng_init();
	m_rng_generation(&f);
	m_rng_reinit();
	t32=(22*3600);
	//
	t32=(uint32_t)(f*t32);
	popTimesEveryday=t32+1000;
	//popTimesEveryday+=t32;
	//<<--
	//freeze_part_init();
	//czsn_data_load_sample_send_time(&sampleTimeTab,sysData.sampleParam);
	//czsn_data_load_sample_send_time(&sendTimeTab,sysData.sendParam);
	//-->>
	//
	freeze_part_init();
	rf_send_fifo_init();
	return ret;
}


//<<--for szrq

uint16_t freeze_part_format(void)
{

	//uint16_t t16;
	uint8_t buf[16];
	volLogHeader_t* sth;
	sth=(volLogHeader_t*)buf;

	sth->writeLoc=0x00;
	sth->unreadNum=0x00;
	sth->itemNum=0x00;
	crc_append(buf,sizeof(volLogHeader_t)-2);
	qc_data_write_to_media(FREEZE_VOLLOG_HEADER_START_ADDR,buf,sizeof(volLogHeader_t));
	return 0;

}

uint16_t freeze_part_init(void)
{
	//uint16_t len;
	uint16_t t16;
	uint8_t buf[16];
	volLogHeader_t* sth;
	sth=(volLogHeader_t*)buf;
	qc_data_read_from_media(buf,FREEZE_VOLLOG_HEADER_START_ADDR,sizeof(volLogHeader_t));
	t16=crc_verify(buf,sizeof(volLogHeader_t));
	if(!t16){
		sth->writeLoc=0x00;
		sth->unreadNum=0x00;
		sth->itemNum=0x00;
		crc_append(buf,sizeof(volLogHeader_t)-2);
		qc_data_write_to_media(FREEZE_VOLLOG_HEADER_START_ADDR,buf,sizeof(volLogHeader_t));
	}
	return t16;
}

uint16_t freeze_item_save(void)
{
	uint16_t t16;
	uint32_t t32,itemAddr;
	//itemAddr=SYS_DATA_VOLLOG_ADDR+sizeof(volLogHeader_t);
	volLogHeader_t partHeader;
	volItem_t	item;
	sysDataTime_t dt;
	
	if(!szrqRtcSync)return 0;
	
	qc_data_read_from_media((uint8_t*)&partHeader,FREEZE_VOLLOG_HEADER_START_ADDR,sizeof(volLogHeader_t));
	
	t32=sizeof(volItem_t)*partHeader.writeLoc;
	itemAddr=FREEZE_VOLLOG_BODY_START_ADDR+t32;

	m_rtc_get(&dt);
	t32=system_dt_to_time_stamp(&dt);
	item.ts=t32;
	item.vol=totalVolume;
	//t16=__czsn_meter_status_cross_map();
	item.sta=t16;
	crc_append((uint8_t*)&item,sizeof(volItem_t)-2);
	qc_data_write_to_media(itemAddr,(uint8_t*)&item,sizeof(volItem_t));
	//
	if(partHeader.itemNum<FREEZE_VOLLOG_ITEM_NUM_LIMITS)partHeader.itemNum++;
	if(partHeader.unreadNum<FREEZE_VOLLOG_ITEM_NUM_LIMITS)partHeader.unreadNum++;
	t16=partHeader.writeLoc;
	t16++;t16%=FREEZE_VOLLOG_ITEM_NUM_LIMITS;
	partHeader.writeLoc=t16;
	crc_append((uint8_t*)&partHeader,sizeof(volLogHeader_t)-2);
	qc_data_write_to_media(FREEZE_VOLLOG_HEADER_START_ADDR,(uint8_t*)&partHeader,sizeof(volLogHeader_t));
	return partHeader.unreadNum;
}

uint16_t freeze_item_read(uint8_t* buf,uint16_t ssize,uint16_t itemOffset)
{
	uint32_t t32,itemAddr;
	//uint16_t len;
	uint16_t t16;
	volLogHeader_t partHeader;
	qc_data_read_from_media((uint8_t*)&partHeader,FREEZE_VOLLOG_HEADER_START_ADDR,sizeof(volLogHeader_t));	
	if(partHeader.itemNum==0)return 0;
	if(ssize<sizeof(volItem_t))return 0;
	//找到读起始位置
	t16=partHeader.writeLoc;
	t16=t16+FREEZE_VOLLOG_ITEM_NUM_LIMITS;
	uint16_t temp=partHeader.unreadNum;
	if(temp==0)temp=1;
	t16=t16-temp;
	//t16=t16-partHeader.unreadNum==0?1:partHeader.unreadNum;
	t16%=FREEZE_VOLLOG_ITEM_NUM_LIMITS;
	//加上偏移量
	if(partHeader.unreadNum==0){
		itemOffset=0;
	}else if(itemOffset>=partHeader.unreadNum){
		itemOffset=partHeader.unreadNum-1;
	}
	t16+=itemOffset;
	t16%=FREEZE_VOLLOG_ITEM_NUM_LIMITS;
	itemOffset=t16;	//保存偏移量
	//计算item地址
	t32=sizeof(volItem_t)*itemOffset;
	itemAddr=FREEZE_VOLLOG_BODY_START_ADDR+t32;
	
	qc_data_read_from_media(buf,itemAddr,sizeof(volItem_t));
	return sizeof(volItem_t);
}

uint16_t freeze_item_unread_num_offset(uint16_t itemOffset)
{
	uint16_t t16;
	volLogHeader_t partHeader;
	if(itemOffset==0)return 0;
	qc_data_read_from_media((uint8_t*)&partHeader,FREEZE_VOLLOG_HEADER_START_ADDR,sizeof(volLogHeader_t));	
	if(partHeader.itemNum==0)return 0;
	
	if(itemOffset>partHeader.unreadNum)itemOffset=partHeader.unreadNum;
	t16=partHeader.unreadNum;
	t16+=FREEZE_VOLLOG_ITEM_NUM_LIMITS;
	t16-=itemOffset;
	t16%=FREEZE_VOLLOG_ITEM_NUM_LIMITS;
	
	partHeader.unreadNum=t16;
	
	crc_append((uint8_t*)&partHeader,sizeof(volLogHeader_t)-2);
	qc_data_write_to_media(FREEZE_VOLLOG_HEADER_START_ADDR,(uint8_t*)&partHeader,sizeof(volLogHeader_t));
	return partHeader.unreadNum;	
}

uint16_t freeze_item_get_unread_num(void)
{
	volLogHeader_t partHeader;
	qc_data_read_from_media((uint8_t*)&partHeader,FREEZE_VOLLOG_HEADER_START_ADDR,sizeof(volLogHeader_t));	
	if(partHeader.itemNum==0)return 0;
	return partHeader.unreadNum==0?1:partHeader.unreadNum;
}

uint16_t freeze_item_get_num(void)
{
	volLogHeader_t partHeader;
	qc_data_read_from_media((uint8_t*)&partHeader,FREEZE_VOLLOG_HEADER_START_ADDR,sizeof(volLogHeader_t));	
	if(partHeader.itemNum==0)return 0;
	//return partHeader.unreadNum==0?1:partHeader.unreadNum;
	return partHeader.itemNum;
}

void freeze_item_save_test(void)
{
	freeze_item_save();
}

//-->>

//<<--for szrq
//szrqVolLogHeader_t
//volLogHeader_t
uint16_t szrq_part_format(void)
{
	//uint16_t t16;
	uint8_t buf[16];
	szrqVolLogHeader_t* sth;
	sth=(szrqVolLogHeader_t*)buf;

	sth->writeLoc=0x00;
	sth->readLoc=0x00;
	sth->itemNum=0x00;
	crc_append(buf,sizeof(szrqVolLogHeader_t)-2);
	qc_data_write_to_media(SZRQ_VOLLOG_HEADER_START_ADDR,buf,sizeof(szrqVolLogHeader_t));
	return 0;
}

uint16_t szrq_item_save(void)
{
	//FREEZE
	//volItem_t
	uint16_t t16=0x00;
	uint32_t t32,itemAddr;
	szrqVolLogHeader_t partHeader;
	szrqVolLogItem_t	item;
	sysDataTime_t dt;
	qc_data_read_from_media((uint8_t*)&partHeader,SZRQ_VOLLOG_HEADER_START_ADDR,sizeof(szrqVolLogHeader_t));
	t32=partHeader.itemNum;
	if(t32>=SZRQ_VOLLOG_ITEM_NUM_LIMITS)return (uint16_t)t32;
	
	t32=sizeof(szrqVolLogItem_t)*t32;
	itemAddr=SZRQ_VOLLOG_BODY_START_ADDR+t32;

	m_rtc_get(&dt);
	t32=system_dt_to_time_stamp(&dt);
	item.ts=t32;
	item.vol=totalVolume;
	//<<--
	item.szrqRoportPeriodType=sysData.szrqRoportPeriodType;
	item.szrqRoportDataInterval=sysData.szrqRoportDataInterval;
	item.szrqtPeriodValue=sysData.szrqtPeriodValue;
	item.szrqcollectNextTm=szrqcollectNextTm;
	item.flow=hourAvgFlow[0];
	//-->>
	crc_append((uint8_t*)&item,sizeof(szrqVolLogItem_t)-2);
	qc_data_write_to_media(itemAddr,(uint8_t*)&item,sizeof(szrqVolLogItem_t));
	//
	if(partHeader.itemNum<SZRQ_VOLLOG_ITEM_NUM_LIMITS)partHeader.itemNum++;
	crc_append((uint8_t*)&partHeader,sizeof(szrqVolLogHeader_t)-2);
	qc_data_write_to_media(SZRQ_VOLLOG_HEADER_START_ADDR,(uint8_t*)&partHeader,sizeof(szrqVolLogHeader_t));
	return partHeader.itemNum;
}

uint16_t szrq_item_read(uint8_t* buf,uint16_t ssize,uint16_t itemOffset)
{
	//FREEZE
	//volItem_t
	//unreadNum
	uint32_t t32,itemAddr;
	uint16_t t16;
	szrqVolLogHeader_t partHeader;
	qc_data_read_from_media((uint8_t*)&partHeader,SZRQ_VOLLOG_HEADER_START_ADDR,sizeof(szrqVolLogHeader_t));	
	if(partHeader.itemNum==0)return 0;
	
	if(ssize<sizeof(szrqVolLogItem_t))return 0;
	//找到读起始位置
	if(partHeader.readLoc+itemOffset>=partHeader.itemNum)return 0;
	
	t32=partHeader.readLoc+itemOffset;
	itemAddr=t32*sizeof(szrqVolLogItem_t);
	itemAddr+=SZRQ_VOLLOG_BODY_START_ADDR;
	qc_data_read_from_media(buf,itemAddr,sizeof(szrqVolLogItem_t));
	t16=crc_verify(buf,sizeof(szrqVolLogItem_t));
	if(!t16){
		szrq_part_format();
		return 0;
	}
	return sizeof(szrqVolLogItem_t);
}

uint16_t szrq_item_set_read_loc(uint16_t readNum)
{
	uint16_t t16;
	szrqVolLogHeader_t partHeader;
	qc_data_read_from_media((uint8_t*)&partHeader,SZRQ_VOLLOG_HEADER_START_ADDR,sizeof(szrqVolLogHeader_t));	
	t16=partHeader.readLoc+readNum;
	if(t16>=partHeader.itemNum){
		szrq_part_format();
		return 1;
	}
	partHeader.readLoc=t16;
	crc_append((uint8_t*)&partHeader,sizeof(szrqVolLogHeader_t)-2);
	qc_data_write_to_media(SZRQ_VOLLOG_HEADER_START_ADDR,(uint8_t*)&partHeader,sizeof(szrqVolLogHeader_t));	
	return partHeader.readLoc;
}

uint16_t szrq_item_get_num(void)
{
	szrqVolLogHeader_t partHeader;
	qc_data_read_from_media((uint8_t*)&partHeader,SZRQ_VOLLOG_HEADER_START_ADDR,sizeof(szrqVolLogHeader_t));	
	return partHeader.itemNum;
}

uint16_t szrq_item_get_unread_num(void)
{
	uint16_t t16;
	szrqVolLogHeader_t partHeader;
	qc_data_read_from_media((uint8_t*)&partHeader,SZRQ_VOLLOG_HEADER_START_ADDR,sizeof(szrqVolLogHeader_t));	
	//return partHeader.itemNum-partHeader.readLoc;
	//t16=partHeader.itemNum;
	if(partHeader.itemNum>partHeader.readLoc){
		t16=partHeader.itemNum-partHeader.readLoc;
	}else{
		t16=0;
	}
	if(t16==0 && partHeader.itemNum!=0){
		szrq_part_format();
	}
	return t16;
}



//-->

//<<-- for send fifo 
uint16_t rf_send_fifo_format(void)
{
	//uint16_t t16;
	uint8_t buf[16];
	rfSendFifoHeader_t* sth;
	sth=(rfSendFifoHeader_t*)buf;

	sth->writeLoc=0x00;
	sth->unreadNum=0x00;
	sth->itemNum=0x00;
	crc_append(buf,sizeof(rfSendFifoHeader_t)-2);
	qc_data_write_to_media(RF_SEND_FIFO_HEAD_START_ADDR,buf,sizeof(rfSendFifoHeader_t));
	return 0;
}

uint16_t rf_send_fifo_init(void)
{
	//uint16_t len;
	uint16_t t16;
	uint8_t buf[16];
	rfSendFifoHeader_t* sth;
	sth=(rfSendFifoHeader_t*)buf;
	qc_data_read_from_media(buf,RF_SEND_FIFO_HEAD_START_ADDR,sizeof(rfSendFifoHeader_t));
	t16=crc_verify(buf,sizeof(rfSendFifoHeader_t));
	if(!t16){
		sth->writeLoc=0x00;
		sth->unreadNum=0x00;
		sth->itemNum=0x00;
		crc_append(buf,sizeof(rfSendFifoHeader_t)-2);
		qc_data_write_to_media(RF_SEND_FIFO_HEAD_START_ADDR,buf,sizeof(rfSendFifoHeader_t));
	}
	return t16;
}

uint16_t rf_send_fifo_push(uint8_t* buf,uint16_t len)
{
	uint16_t t16;
	uint32_t t32,itemAddr;
	sysDataTime_t dt;
	//volLogHeader_t
	//volItem_t
	rfSendFifoHeader_t partHeader;
	rfSendFifoBody_t item;

	qc_data_read_from_media((uint8_t*)&partHeader,RF_SEND_FIFO_HEAD_START_ADDR,sizeof(rfSendFifoHeader_t));
	
	t32=sizeof(rfSendFifoBody_t)*partHeader.writeLoc;
	itemAddr=RF_SEND_FIFO_BODY_START_ADDR+t32;

	m_rtc_get(&dt);
	t32=system_dt_to_time_stamp(&dt);
	item.sendedFlg=0;
	item.reverse=0xaa;
	item.len=len;
	item.ts=t32;
	m_mem_cpy_len(item.buf,buf,len);
	
	crc_append((uint8_t*)&item,sizeof(rfSendFifoBody_t)-2);
	qc_data_write_to_media(itemAddr,(uint8_t*)&item,sizeof(rfSendFifoBody_t));
	t16=crc_verify((uint8_t*)&item,sizeof(rfSendFifoBody_t));
	if(!t16){
		__nop();
		return 0;
	}
	//
	if(partHeader.itemNum<RF_SEND_FIFO_ITEM_NUM_LIMIT)partHeader.itemNum++;
	if(partHeader.unreadNum<RF_SEND_FIFO_ITEM_NUM_LIMIT)partHeader.unreadNum++;
	t16=partHeader.writeLoc;
	t16++;t16%=RF_SEND_FIFO_ITEM_NUM_LIMIT;
	partHeader.writeLoc=t16;
	crc_append((uint8_t*)&partHeader,sizeof(rfSendFifoHeader_t)-2);
	qc_data_write_to_media(RF_SEND_FIFO_HEAD_START_ADDR,(uint8_t*)&partHeader,sizeof(rfSendFifoHeader_t));
	rf_send_fifo_init();
	return partHeader.itemNum;
}


uint16_t rf_send_fifo_item_get_num(void)
{
	rfSendFifoHeader_t partHeader;
	qc_data_read_from_media((uint8_t*)&partHeader,RF_SEND_FIFO_HEAD_START_ADDR,sizeof(rfSendFifoHeader_t));	
	if(partHeader.itemNum==0)return 0;
	return partHeader.itemNum;
}

uint16_t rf_send_fifo_item_get_unread_num(void)
{
	rfSendFifoHeader_t partHeader;
	qc_data_read_from_media((uint8_t*)&partHeader,RF_SEND_FIFO_HEAD_START_ADDR,sizeof(rfSendFifoHeader_t));	
	if(partHeader.itemNum==0)return 0;
	//return partHeader.unreadNum==0?1:partHeader.unreadNum;
	return partHeader.unreadNum;
}

uint16_t rf_send_fifo_get_tail(uint8_t* buf,uint16_t* unreadNum)
{
	//volItem_t
	uint32_t t32,itemAddr;
	uint16_t t16;
	rfSendFifoHeader_t partHeader;
	rfSendFifoBody_t item;

	qc_data_read_from_media((uint8_t*)&partHeader,RF_SEND_FIFO_HEAD_START_ADDR,sizeof(rfSendFifoHeader_t));	
	if(partHeader.itemNum==0)return 0;
	if(partHeader.unreadNum==0)return 0;
	*unreadNum=partHeader.unreadNum;
	//找到读起始位置
	t16=partHeader.writeLoc;
	t16=t16+RF_SEND_FIFO_ITEM_NUM_LIMIT;
	t16=t16-partHeader.unreadNum;
	t16%=RF_SEND_FIFO_ITEM_NUM_LIMIT;


	//计算item地址
	t32=sizeof(rfSendFifoBody_t)*(uint32_t)t16;
	itemAddr=RF_SEND_FIFO_BODY_START_ADDR+t32;
	
	qc_data_read_from_media((uint8_t*)&item,itemAddr,sizeof(rfSendFifoBody_t));
	t16=crc_verify((uint8_t*)&item,sizeof(rfSendFifoBody_t));
	if(!t16){
		rf_send_fifo_format();
		*unreadNum=0;
		return 0;
	}
	t16=item.len;
	m_mem_cpy_len(buf,item.buf,t16);
	return t16;
}

uint16_t rf_send_fifo_delete_tail(void)
{

	rfSendFifoHeader_t partHeader;

	qc_data_read_from_media((uint8_t*)&partHeader,RF_SEND_FIFO_HEAD_START_ADDR,sizeof(rfSendFifoHeader_t));	
	if(partHeader.itemNum==0)return 0;
	if(partHeader.unreadNum==0)return 0;

	partHeader.unreadNum--;
	crc_append((uint8_t*)&partHeader,sizeof(rfSendFifoHeader_t)-2);
	qc_data_write_to_media(RF_SEND_FIFO_HEAD_START_ADDR,(uint8_t*)&partHeader,sizeof(rfSendFifoHeader_t));
	return partHeader.unreadNum;	
}
//-->>
//file end
