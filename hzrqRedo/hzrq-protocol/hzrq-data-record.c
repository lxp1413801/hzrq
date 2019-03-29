#include "./includes/includes.h"
volatile uint32_t recordReadStartTs=0x00UL;
uint8_t batCapacity;


uint16_t ex_eeprom_read(uint8_t devaddr,uint16_t addr,uint8_t* buf,uint16_t len)
{
	//device must enable 
	if((void*)osMutexExEeprom)osMutexWait(osMutexExEeprom,osWaitForever);
	
	uint16_t i;
    iic_pins_init();
	HAL_Delay(2);
	//iic_stop();	
	HAL_Delay(2);

	iic_start();
	iic_send_byte(devaddr);
	iic_send_byte((uint8_t)((addr>>0x08)&0x00ff));
	iic_send_byte((uint8_t)(addr&0x00ff));
	//
	iic_start();
	iic_send_byte(devaddr+1);
	for(i=0;i<len-1;i++){
		*buf++=iic_received_byte_if_ack(IIC_ACK);
	}
	*buf=iic_received_byte_if_ack(IIC_NACK);
	iic_stop();	
    HAL_Delay(5);
    iic_pins_deinit();
	HAL_Delay(5);
	if((void*)osMutexExEeprom)osMutexRelease(osMutexExEeprom);	
	return 1;
}

uint16_t ex_eeprom_write(uint8_t devaddr,uint16_t addr,uint8_t* buf,uint16_t len)
{
	uint16_t i;
    uint16_t t16,pagAddr,subAddr;
	if((void*)osMutexExEeprom)osMutexWait(osMutexExEeprom,osWaitForever);
	
	iic_pins_init();
	HAL_Delay(5);
	pagAddr=addr / EX_EEPROM_PAGE_SIZE;
	subAddr=addr % EX_EEPROM_PAGE_SIZE;

	while(len>0){
		iic_start();
		iic_send_byte(devaddr);
		t16=(pagAddr * EX_EEPROM_PAGE_SIZE)+subAddr;
		iic_send_byte((uint8_t)((t16 >> 0x08)&0x00ff));
		iic_send_byte((uint8_t)(t16 & 0x00ff));		
		for(i=subAddr;i<EX_EEPROM_PAGE_SIZE;i++){
			if(len>0)len--;
			else
				break;
			iic_send_byte(*buf++);
		}
		iic_stop();
        HAL_Delay(5);	
		if(len){
			pagAddr++;
			subAddr=0;
		}else{
			break;
		}
	}

    HAL_Delay(5);
    iic_pins_deinit();   
	HAL_Delay(5);
	if((void*)osMutexExEeprom)osMutexRelease(osMutexExEeprom);	
	return 1;
}

uint16_t ex_eeprom_init_chip_desc(void)
{
	uint8_t buf[64];
	uint16_t ret=0;
	exEepromChipDesc_t* stp=(exEepromChipDesc_t*)buf;
	stp->chipSize=EX_EEPROM_SIZE;
	stp->pageSize=EX_EEPROM_PAGE_SIZE;
	stp->secSize=EX_EEPROM_SECTOR_SIZE;
	stp->partNum=EX_EEPROM_PART_NUMBER;
	
	crc_append(buf,sizeof(exEepromChipDesc_t)-2);
	ex_eeprom_write(SLV_ADDR,EXEEP_START_ADDR_CHIP_DESC,buf,sizeof(exEepromChipDesc_t));
	
	ex_eeprom_read(SLV_ADDR,EXEEP_START_ADDR_CHIP_DESC,buf,sizeof(exEepromChipDesc_t));	
	ret=crc_verify(buf,sizeof(exEepromChipDesc_t));	
	return ret;
}

uint16_t ex_eeprom_init_part_desc_clog_hour()
{
	uint8_t buf[64];
	uint16_t ret=0;
	exEepromPartDesc_t* stp=(exEepromPartDesc_t*)buf;
	
	stp->partSize=PART_CONSTLOG_HOUR_SIZE;
	stp->partStartAddr=EXEEP_START_ADDR_PART_BODY_CLOG_HOUR;
	stp->recordSize=sizeof(consumeLog_t);
	//stp->recordNum=0x00;
	stp->recordWriteLoc=0x00;
	stp->recordNum=0x00;
	stp->recordUnRead=0x00;
	
	crc_append(buf,sizeof(exEepromPartDesc_t)-2);
	ex_eeprom_write(SLV_ADDR,EXEEP_START_ADDR_PART_DESC_CLOG_HOUR,buf,sizeof(exEepromPartDesc_t));	
	
	ex_eeprom_read(SLV_ADDR,EXEEP_START_ADDR_PART_DESC_CLOG_HOUR,buf,sizeof(exEepromPartDesc_t));	
	ret=crc_verify(buf,sizeof(exEepromPartDesc_t));	
	return ret;
}

uint16_t ex_eeprom_init_part_desc_clog_day(void)
{
	uint8_t buf[64];
	uint16_t ret=0;
	exEepromPartDesc_t* stp=(exEepromPartDesc_t*)buf;
	
	stp->partSize=PART_CONSTLOG_DAY_SIZE;
	stp->partStartAddr=EXEEP_START_ADDR_PART_BODY_CLOG_DAY;
	stp->recordSize=sizeof(consumeLog_t);
	//stp->recordNum=0x00;
	stp->recordWriteLoc=0x00;
	stp->recordNum=0x00;
	stp->recordUnRead=0x00;
	
	crc_append(buf,sizeof(exEepromPartDesc_t)-2);
	ex_eeprom_write(SLV_ADDR,EXEEP_START_ADDR_PART_DESC_CLOG_DAY,buf,sizeof(exEepromPartDesc_t));	
	
	ex_eeprom_read(SLV_ADDR,EXEEP_START_ADDR_PART_DESC_CLOG_DAY,buf,sizeof(exEepromPartDesc_t));	
	ret=crc_verify(buf,sizeof(exEepromPartDesc_t));	
	return ret;
}

uint16_t ex_eeprom_init_part_desc_clog_month(void)
{
	uint8_t buf[64];
	uint16_t ret=0;
	exEepromPartDesc_t* stp=(exEepromPartDesc_t*)buf;
	
	stp->partSize=PART_CONSTLOG_MON_SIZE;
	stp->partStartAddr=EXEEP_START_ADDR_PART_BODY_CLOG_MON;
	stp->recordSize=sizeof(consumeLog_t);

	stp->recordWriteLoc=0x00;
	stp->recordNum=0x00;
	stp->recordUnRead=0x00;
	
	crc_append(buf,sizeof(exEepromPartDesc_t)-2);
	ex_eeprom_write(SLV_ADDR,EXEEP_START_ADDR_PART_DESC_CLOG_MON,buf,sizeof(exEepromPartDesc_t));	
	
	ex_eeprom_read(SLV_ADDR,EXEEP_START_ADDR_PART_DESC_CLOG_MON,buf,sizeof(exEepromPartDesc_t));	
	ret=crc_verify(buf,sizeof(exEepromPartDesc_t));	
	return ret;
}

uint16_t ex_eeprom_init_part_desc_eventlog(void)
{
	uint8_t buf[64];
	uint16_t ret=0;
	exEepromPartDesc_t* stp=(exEepromPartDesc_t*)buf;
	
	stp->partSize=PART_EVENTLOG_SIZE;
	stp->partStartAddr=EXEEP_START_ADDR_PART_BODY_EVENT_CLOG;
	stp->recordSize=sizeof(eventLog_t);
	//stp->recordNum=0x00;
	stp->recordWriteLoc=0x00;
	stp->recordNum=0x00;
	stp->recordUnRead=0x00;
	
	crc_append(buf,sizeof(exEepromPartDesc_t)-2);
	ex_eeprom_write(SLV_ADDR,EXEEP_START_ADDR_PART_DESC_EVENTLOG,buf,sizeof(exEepromPartDesc_t));	
	
	ex_eeprom_read(SLV_ADDR,EXEEP_START_ADDR_PART_DESC_EVENTLOG,buf,sizeof(exEepromPartDesc_t));	
	ret=crc_verify(buf,sizeof(exEepromPartDesc_t));	
	return ret;
}

uint16_t ex_eeprom_verify(void)
{
	uint8_t buf[64];
	uint16_t ret=0,t16;
	ex_eeprom_read(SLV_ADDR,EXEEP_START_ADDR_CHIP_DESC,buf,sizeof(exEepromChipDesc_t));	
	t16=crc_verify(buf,sizeof(exEepromChipDesc_t));
	if(!t16){
		ret+=1;
		ex_eeprom_init_chip_desc();
	}
	//
	ex_eeprom_read(SLV_ADDR,EXEEP_START_ADDR_PART_DESC_CLOG_HOUR,buf,sizeof(exEepromPartDesc_t));	
	t16=crc_verify(buf,sizeof(exEepromPartDesc_t));
	if(!t16){
		ret+=2;
		ex_eeprom_init_part_desc_clog_hour();
	}
	//
	ex_eeprom_read(SLV_ADDR,EXEEP_START_ADDR_PART_DESC_CLOG_DAY,buf,sizeof(exEepromPartDesc_t));
	t16=crc_verify(buf,sizeof(exEepromPartDesc_t));
	if(!t16){
		ret+=4;
		ex_eeprom_init_part_desc_clog_day();
	}
	//
	ex_eeprom_read(SLV_ADDR,EXEEP_START_ADDR_PART_DESC_CLOG_MON,buf,sizeof(exEepromPartDesc_t));	
	t16=crc_verify(buf,sizeof(exEepromPartDesc_t));	
	if(!t16){
		ret+=8;
		ex_eeprom_init_part_desc_clog_month();	
	}
	//
	ex_eeprom_read(SLV_ADDR,EXEEP_START_ADDR_PART_DESC_EVENTLOG,buf,sizeof(exEepromPartDesc_t));	
	t16=crc_verify(buf,sizeof(exEepromPartDesc_t));	
	if(!t16){
		ret+=16;
		ex_eeprom_init_part_desc_eventlog();	
	}	
	return ret;
}

void ex_eeprom_format(void)
{
	ex_eeprom_init_chip_desc();
	ex_eeprom_init_part_desc_clog_hour();
	ex_eeprom_init_part_desc_clog_day();
	ex_eeprom_init_part_desc_clog_month();	
	ex_eeprom_init_part_desc_eventlog();	
}

uint16_t ex_data_get_part_inf(uint8_t partNb,uint8_t* buf,uint16_t ssize)
{
	uint16_t partInfAddr;
	if(ssize<sizeof(exEepromPartDesc_t))return 0;
	partInfAddr=sizeof(exEepromChipDesc_t)+ ((uint16_t)partNb)*sizeof(exEepromPartDesc_t);
	ex_eeprom_read(SLV_ADDR,partInfAddr,buf,sizeof(exEepromPartDesc_t));
	return sizeof(exEepromPartDesc_t);
}

uint16_t ex_data_write_record(uint8_t partNb,uint8_t* buf,uint16_t len)
{
	uint16_t t16;
	uint16_t partInfAddr,limitsItem,bodyItemAddr,ItemSize;
	
	exEepromPartDesc_t partInf;
	partInfAddr=sizeof(exEepromChipDesc_t)+ ((uint16_t)partNb)*sizeof(exEepromPartDesc_t);
	ex_eeprom_read(SLV_ADDR,partInfAddr,(uint8_t*)&partInf,sizeof(exEepromPartDesc_t));	
	
	t16=crc_verify((uint8_t*)&partInf,sizeof(exEepromPartDesc_t));	
	if(!t16)return 0;
	
	ItemSize=partInf.recordSize;
	if(len!=ItemSize)return 0;
	limitsItem=partInf.partSize/ItemSize;

	//t16=partInf.recordReadLoc * ;
	bodyItemAddr=partInf.partStartAddr + ItemSize * partInf.recordWriteLoc;
	
	crc_append(buf,len-2);
	ex_eeprom_write(SLV_ADDR,bodyItemAddr,buf,len);
	
	ex_eeprom_read(SLV_ADDR,bodyItemAddr,buf,len);
	t16=crc_verify(buf,len);	
	if(!t16)return 0;
	
	t16=partInf.recordWriteLoc;
	t16++;
	t16=(t16%limitsItem);
	partInf.recordWriteLoc=t16;
	
	if(partInf.recordNum < limitsItem)partInf.recordNum++;
	if(partInf.recordUnRead < limitsItem)partInf.recordUnRead++;

	//if(partInf.recordNum<limitsItem)partInf.recordNum++;	
	crc_append((uint8_t*)&partInf,sizeof(exEepromPartDesc_t)-2);
	ex_eeprom_write(SLV_ADDR,partInfAddr,(uint8_t*)&partInf,sizeof(exEepromPartDesc_t));	
	
	t16=crc_verify((uint8_t*)&partInf,sizeof(exEepromPartDesc_t));	
	return t16;
}

uint16_t ex_data_read_record_loc(uint8_t partNb,uint16_t recordLoc,uint8_t* buf,uint16_t len)
{
	return 1;
}
/*
uint16_t ex_data_read_record(uint8_t partNb,uint8_t* buf,uint16_t len,uint16_t* unRead)
{
	uint16_t t1,t2,ItemSize,bodyItemAddr,limitsItem;
	exEepromPartDesc_t partInf;
	ex_data_get_part_inf(partNb,(uint8_t*)&partInf,sizeof(exEepromPartDesc_t));
	
	*unRead = partInf.recordUnRead;
	

	//计算位置
	ItemSize=partInf.recordSize;
	if(len!=ItemSize)return 0;
	limitsItem=partInf.partSize/ItemSize;

	t1=partInf.recordUnRead;
	if(t1==0)t1=1;
	t2 = partInf.recordWriteLoc + limitsItem - t1;
	t2 %= limitsItem;
	//t16=partInf.recordReadLoc * ;
	bodyItemAddr=partInf.partStartAddr + ItemSize * t2;	
	ex_eeprom_write(SLV_ADDR,bodyItemAddr,buf,ItemSize);
	
	return ItemSize;
}
*/
uint16_t record_save_gas_log(uint8_t partNb)
{
	uint32_t t32;
	consumeLog_t log;
	sysDataTime_t dt;
	uint16_t ret;
	m_mem_cpy_len((uint8_t*)&dt,(uint8_t*)&sysRtcDataTime,sizeof(sysDataTime_t));
	t32=system_dt_to_time_stamp(&dt);
	//m_mem_cpy_len(log.ts,(uint8_t*)&t32,sizeof(uint32_t));
	log.ts=t32;
	
	t32=totalVolume;
	//m_mem_cpy_len(log.volume,(uint8_t*)&t32,sizeof(uint32_t));
	log.volume=t32;
	ret=ex_data_write_record(partNb,(uint8_t*)&log,sizeof(consumeLog_t));
	return ret;
}

uint16_t record_api_save_hour_log(void)
{
	return record_save_gas_log(PART_SN_CONSTLOG_HOUR);
}

uint16_t record_api_save_day_log(void)
{
	return record_save_gas_log(PART_SN_CONSTLOG_DAY);
}

uint16_t record_api_save_month_log(void)
{
	return record_save_gas_log(PART_SN_CONSTLOG_MON);
}

//搜索记录中时间大于指定时间ts的第一条记录位置;返回值非负表示位置，返回值负表示搜索失败
int16_t record_search_at_ts(uint8_t partNb,uint32_t ts)
{
	
	uint16_t loc,t16,partStartAddr,limitsItem,bodyItemAddr,ItemSize,newestLoc;
	uint32_t t32,newestTs;
	uint8_t buf[32];
	exEepromPartDesc_t* pDesc;
	
	int16_t ret=-1;
	if(partNb>3)return -1;
	ex_data_get_part_inf(partNb,buf,sizeof(buf));
	pDesc=(exEepromPartDesc_t*)buf;
	t16=crc_verify(buf,sizeof(exEepromPartDesc_t));	
	if(!t16)return ret;
	
	ItemSize=pDesc->recordSize;
	limitsItem=pDesc->partSize/ItemSize;
	
	partStartAddr=pDesc->partStartAddr;
	
	//获取最新记录
	loc=pDesc->recordWriteLoc;
	loc+=limitsItem;loc--;loc%=limitsItem;
	ret=loc;
	newestLoc=loc;
	
	bodyItemAddr=partStartAddr+loc*ItemSize;
	ex_eeprom_read(SLV_ADDR,bodyItemAddr,buf,ItemSize);
	t16=crc_verify(buf,ItemSize);	
	if(!t16)return -1;
	
	//获取最新记录时间
	m_mem_cpy_len((uint8_t*)&newestTs,buf,sizeof(uint32_t));
	if(ts>=newestTs)return ret;
	
	//估算大概位置
	uint32_t stepTs=3600;
	if(partNb==1)stepTs=3600*24;
	if(partNb==2)stepTs=3600*24*30;
	
	t32=((newestTs-ts)/stepTs)+1;
	if(t32 >= limitsItem)return -1;
	loc+=limitsItem;loc-=(uint16_t)t32;loc%=limitsItem;

	//向前向后搜索
	bodyItemAddr=partStartAddr+loc*ItemSize;
	ex_eeprom_read(SLV_ADDR,bodyItemAddr,buf,ItemSize);	
	
	m_mem_cpy_len((uint8_t*)&t32,buf,sizeof(uint32_t));
	t16=crc_verify(buf,ItemSize);
	if(t32< ts || t16==0){
		while(1){
			//向后搜索
			loc++;loc%=limitsItem;
			
			bodyItemAddr=partStartAddr+loc*ItemSize;
			ex_eeprom_read(SLV_ADDR,bodyItemAddr,buf,ItemSize);	
			m_mem_cpy_len((uint8_t*)&t32,buf,sizeof(uint32_t));
			t16=crc_verify(buf,ItemSize);	

			if(t32>=ts && t16!=0){ret=loc;break;}
			if(loc==newestLoc){ret=-1;break;}
		}
	}else{
		while(1){
			//向前搜索更早的记录
			loc+=limitsItem;loc--;loc%=limitsItem;

			bodyItemAddr=partStartAddr+loc*ItemSize;
			ex_eeprom_read(SLV_ADDR,bodyItemAddr,buf,ItemSize);	
			m_mem_cpy_len((uint8_t*)&t32,buf,sizeof(uint32_t));
			t16=crc_verify(buf,ItemSize);
			
			if(t32<ts || t16==0){
				loc++;loc%=limitsItem;
				ret=loc;break;
			}
			if(t32>=newestTs){
				ret=-1;break;
			}
			newestTs=t32;
		}
	}
	return ret;
}

sysDataTime_t tempDt;
//读取一个时间段的记录
uint16_t record_read_start_to_end(uint8_t partNb,uint8_t* buf,uint16_t ssize,uint32_t tsStart,uint32_t tsEnd)
{
	uint32_t t32,lastReadTs=0x00Ul;
	int16_t loc,endloc;
	uint16_t t16,limitsItem;
	uint16_t len=0;
	
	uint8_t tbuf[32];
	if(partNb>3)return 0;
	loc=record_search_at_ts(partNb,tsStart);
	if(loc<0)return 0;
	
	exEepromPartDesc_t partDesc;
	ex_data_get_part_inf(partNb,(uint8_t*)&partDesc,sizeof(exEepromPartDesc_t));
	
	limitsItem=partDesc.partSize/partDesc.recordSize;
	endloc=partDesc.recordWriteLoc;
	
	endloc+=limitsItem;endloc--;endloc%=limitsItem;
	
	if(sizeof(tbuf)<partDesc.recordSize)while(1);
	do{
		t16=partDesc.partStartAddr+loc*(partDesc.recordSize);
		ex_eeprom_read(SLV_ADDR,t16,tbuf,partDesc.recordSize);
		m_mem_cpy_len((uint8_t*)&t32,tbuf,sizeof(uint32_t));
		if(t32>tsEnd)break;
		do{
			if(t32<tsStart)break;
			t16=crc_verify(tbuf,partDesc.recordSize);
			if(t16==0)break;
			if(len+sizeof(__hzrq_gasLog_t)>ssize-128)break;
			//<<--排重，这个重要
			if(partNb==PART_SN_CONSTLOG_HOUR){
				t32-=(t32%3600UL);
				if(t32==lastReadTs)break;
				
			}else if(partNb==PART_SN_CONSTLOG_DAY){
				t32-=(t32%86400UL);
				if(t32==lastReadTs)break;
			}else{
				time_stamp_to_system_dt(t32,&tempDt);
				t32=tempDt.MM;
				if(t32==lastReadTs)break;
			}
			lastReadTs=t32;
			//-->>
			if(recordReadStartTs==0x00UL)recordReadStartTs=((consumeLog_t*)tbuf)->ts;
			t16=__hzrq_gas_log_format((__hzrq_gasLog_t*)(buf+len),(consumeLog_t*)(tbuf));
			len+=t16;
		}while(0);

		if(loc==endloc)break;
		loc+=limitsItem;loc++;loc%=limitsItem;
	}while(1);
	return len;
}	


uint16_t record_read_vol_log_hour_start_end(uint8_t* buf,uint16_t ssize,uint32_t startTm,uint32_t endTm)
{
	//24*4=96
	uint16_t t16;
	t16=record_read_start_to_end(PART_SN_CONSTLOG_HOUR,buf,ssize,startTm,endTm);
	while((t16%96) !=0){
		m_mem_cpy_len(buf+t16,buf+t16-4,4);
		t16+=4;
	}
	return t16;
	
}

uint16_t record_read_vol_log_day_start_end(uint8_t* buf,uint16_t ssize,uint32_t startTm,uint32_t endTm)
{
	//24*4=96
	uint16_t t16;
	t16=record_read_start_to_end(PART_SN_CONSTLOG_DAY,buf,ssize,startTm,endTm);
//	while((t16%96) !=0){
//		m_mem_cpy_len(buf+t16,buf+t16-4,4);
//		t16+=4;
//	}
	return t16;
}

uint16_t record_read_vol_log_month_start_end(uint8_t* buf,uint16_t ssize,uint32_t startTm,uint32_t endTm)
{
	//24*4=96
	int32_t t32=0;
	uint16_t t16;
	t16=record_read_start_to_end(PART_SN_CONSTLOG_MON,buf,ssize,startTm,endTm);
	//<<--协议问题，不支持无内容回复
	if(t16==0){
		t32=swap_uint32(totalVolume*10);
		m_mem_cpy_len(buf,(uint8_t*)&t32,4);
		t16=4;
	}
	//-->>
	while(((t16%48) !=0) || t16==0){
		m_mem_cpy_len(buf+t16,buf+t16-4,4);
		t16+=4;
	}
	return t16>48?48:t16;
}

//const uint8_t ExEepromTest[]={0xaa,0xaa,0xaa,0xaa};
//事件纪录
uint16_t record_save_event_log(uint16_t eventCode)
{
	eventLog_t eventlog={0};
	//uint8_t buf[32];
	uint16_t t16;
	uint16_t len,limitsItem,bodyItemAddr,ItemSize;
	exEepromPartDesc_t partInf;
	ex_eeprom_read(SLV_ADDR,EXEEP_START_ADDR_PART_DESC_EVENTLOG,(uint8_t*)&partInf,sizeof(exEepromPartDesc_t));	
	t16=crc_verify((uint8_t*)&partInf,sizeof(exEepromPartDesc_t));	
	if(!t16){
		ex_eeprom_init_part_desc_eventlog();	
		return 0;
	}	
	
	ItemSize=partInf.recordSize;
	//if(len!=ItemSize)return 0;
	limitsItem=partInf.partSize/ItemSize;

	bodyItemAddr=partInf.partStartAddr + ItemSize * partInf.recordWriteLoc;
	
	//load buffer
	sysDataTime_t dt;
	m_mem_cpy_len((uint8_t*)&dt,(uint8_t*)&sysRtcDataTime,sizeof(sysDataTime_t));

	eventlog.ts=system_dt_to_time_stamp(&dt);;
	eventlog.eventCode=eventCode;
	eventlog.dcls=sysData.DLCS;
	eventlog.valveSta=vavleState;
	eventlog.volume=totalVolume;
	//eventlog.periodVolum;
	//eventlog.price;
	eventlog.balanceVol=overageMoney;
	eventlog.balance=overageMoney;
	eventlog.devSta=sysData.devStatus.t32;
	eventlog.lock=sysData.lockReason.t32;	
	eventlog.readFlg=0;
	//eventlog.crc16;
	//endload
	
	crc_append((uint8_t*)&eventlog,sizeof(eventLog_t)-2);
	ex_eeprom_write(SLV_ADDR,bodyItemAddr,(uint8_t*)&eventlog,sizeof(eventLog_t));
	
	ex_eeprom_read(SLV_ADDR,bodyItemAddr,(uint8_t*)&eventlog,sizeof(eventLog_t));
	t16=crc_verify((uint8_t*)&eventlog,sizeof(eventLog_t));	
	if(!t16)return 0;
	
	
	t16=partInf.recordWriteLoc;
	t16++;
	t16=(t16%limitsItem);
	partInf.recordWriteLoc=t16;
	
	if(partInf.recordNum < limitsItem)partInf.recordNum++;
	
	
	if(partInf.recordUnRead < limitsItem)partInf.recordUnRead++;


	crc_append((uint8_t*)&partInf,sizeof(exEepromPartDesc_t)-2);
	ex_eeprom_write(SLV_ADDR,EXEEP_START_ADDR_PART_DESC_EVENTLOG,(uint8_t*)&partInf,sizeof(exEepromPartDesc_t));	
	
	t16=crc_verify((uint8_t*)&partInf,sizeof(exEepromPartDesc_t));	
	return t16;	
}

int8_t eventlogReadedTab[EVENT_RECORD_ITEM_LIMITS]={0};
uint16_t  record_read_eventlog_init(void)
{
	eventLog_t eventlog={0};
	uint16_t bodyItemAddr,t16=1,i=0;
	//uint16_t itemAddr=EXEEP_START_ADDR_PART_BODY_EVENT_CLOG;
	exEepromPartDesc_t partInf;
	ex_eeprom_read(SLV_ADDR,EXEEP_START_ADDR_PART_DESC_EVENTLOG,(uint8_t*)&partInf,sizeof(exEepromPartDesc_t));	
	t16=crc_verify((uint8_t*)&partInf,sizeof(exEepromPartDesc_t));	
	if(!t16){
		ex_eeprom_init_part_desc_eventlog();	
		return 0;
	}	
	
	for(i=0;i<EVENT_RECORD_ITEM_LIMITS;i++){
		
		if(eventlogReadedTab[i]){
			bodyItemAddr=partInf.partStartAddr+i*partInf.recordSize;
			ex_eeprom_read(SLV_ADDR,bodyItemAddr,(uint8_t*)&eventlog,sizeof(eventLog_t));
			t16=crc_verify((uint8_t*)&eventlog,sizeof(eventLog_t));	
			if(!t16)break;		
			if(eventlog.readFlg==0){
				eventlog.readFlg=1;
				crc_append((uint8_t*)&eventlog,sizeof(eventLog_t)-2);
				ex_eeprom_write(SLV_ADDR,bodyItemAddr,(uint8_t*)&eventlog,sizeof(eventLog_t));				
			}
		}

	}
	return t16;
}

uint16_t record_read_eventlog_new(uint8_t* buf,uint16_t ssize,uint16_t rdEventCode,uint16_t rdItems)
{
	eventLog_t eventlog={0};
	uint16_t i,len,t16,ergodicLoc,limitsItem,bodyItemAddr;
	uint16_t retLen=0;
	exEepromPartDesc_t partInf;
	
	record_read_eventlog_init();
	m_mem_set((uint8_t*)eventlogReadedTab,0,sizeof(eventlogReadedTab));
	
	ex_eeprom_read(SLV_ADDR,EXEEP_START_ADDR_PART_DESC_EVENTLOG,(uint8_t*)&partInf,sizeof(exEepromPartDesc_t));	
	t16=crc_verify((uint8_t*)&partInf,sizeof(exEepromPartDesc_t));	
	if(!t16){
		ex_eeprom_init_part_desc_eventlog();	
		return 0;
	}
	//找到起始位置
	limitsItem=partInf.partSize/partInf.recordSize;
	
	/*
	t16=partInf.recordWriteLoc+limitsItem;
	t16-=partInf.recordUnRead;
	t16%=limitsItem;
	ergodicLoc=t16;	
	*/
	
	ergodicLoc=0;
	for(i=0;i<rdItems && i<limitsItem;i++){

		if(i>=partInf.recordNum)break;
		if(retLen+sizeof(__hzrq_eventLog_t)>ssize){
			//has more;
			break;
		}
		do{
			t16=0;
			bodyItemAddr=partInf.partStartAddr+ergodicLoc*partInf.recordSize;
			ex_eeprom_read(SLV_ADDR,bodyItemAddr,(uint8_t*)&eventlog,sizeof(eventLog_t));
			t16=crc_verify((uint8_t*)&eventlog,sizeof(eventLog_t));	
			if(!t16)break;	
			//if(eventlog.ts<startTs || eventlog.ts>endTs)break;
			if(eventlog.readFlg)break;
			if(rdEventCode!=0xffff){
				if(eventlog.eventCode!=rdEventCode)break;
			}
			if(recordReadStartTs==0x00UL)recordReadStartTs=eventlog.ts;
			retLen+=__hzrq_event_log_format((__hzrq_eventLog_t*)(buf+retLen),&eventlog);
			eventlogReadedTab[ergodicLoc]=1;
		//
		}while(0);
		ergodicLoc++;
		
	}
//	if(t16==0){
//	}
	return retLen;	
}

uint16_t record_read_eventlog_start_to_end(uint8_t* buf,uint16_t ssize,uint32_t startTs,uint32_t endTs,uint16_t rdEventCode,uint16_t rdItems)
{
	//
	eventLog_t eventlog={0};
	uint16_t i,len,t16,ergodicLoc,limitsItem,bodyItemAddr;
	uint16_t retLen=0;
	exEepromPartDesc_t partInf;
	
	record_read_eventlog_init();
	m_mem_set((uint8_t*)eventlogReadedTab,0,sizeof(eventlogReadedTab));	
	
	ex_eeprom_read(SLV_ADDR,EXEEP_START_ADDR_PART_DESC_EVENTLOG,(uint8_t*)&partInf,sizeof(exEepromPartDesc_t));	
	t16=crc_verify((uint8_t*)&partInf,sizeof(exEepromPartDesc_t));	
	if(!t16){
		ex_eeprom_init_part_desc_eventlog();	
		return 0;
	}
	//找到起始位置
	limitsItem=partInf.partSize/partInf.recordSize;
	//ergodicLoc=partInf.recordWriteLoc;
	t16=partInf.recordWriteLoc+limitsItem;
	t16-=partInf.recordNum;
	t16%=limitsItem;
	ergodicLoc=t16;

	len=0;
	if(startTs==~0x00UL)startTs=0UL;
	
	for(i=0;i<rdItems;i++){

		if(i>=partInf.recordNum)break;
		if(retLen+sizeof(__hzrq_eventLog_t)>ssize){
			//has more;
			break;
		}
		do{
			t16=0;
			bodyItemAddr=partInf.partStartAddr+ergodicLoc*partInf.recordSize;
			ex_eeprom_read(SLV_ADDR,bodyItemAddr,(uint8_t*)&eventlog,sizeof(eventLog_t));
			t16=crc_verify((uint8_t*)&eventlog,sizeof(eventLog_t));	
			if(!t16)break;	
			if(eventlog.ts<startTs || eventlog.ts>endTs)break;
			if(rdEventCode!=0xffff){
				if(eventlog.eventCode!=rdEventCode)break;
			}			
			if(recordReadStartTs==0x00UL)recordReadStartTs=eventlog.ts;
			retLen+=__hzrq_event_log_format((__hzrq_eventLog_t*)(buf+retLen),&eventlog);
			eventlogReadedTab[ergodicLoc]=1;
		//
		}while(0);
		ergodicLoc++;
	}
//	if(t16==0){
//	}
	return retLen;
}

void ex_data_test(void)
{
	uint8_t ExEepromTest[8]={0x55,0xaa,0x55,0xaa,0xaa,0x11,0x33,0x33};
	 uint8_t buf[8]={0};
	 ex_eeprom_write(SLV_ADDR,255,(uint8_t*)ExEepromTest,8);
	// __nop();
	 ex_eeprom_read(SLV_ADDR,255,(uint8_t*)buf,8);
	//ex_eeprom_format();
	//ex_eeprom_verify();
	__nop();
}
//file end
