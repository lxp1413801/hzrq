#include "./includes/includes.h"
uint8_t batCapacity;

uint16_t ex_eeprom_read(uint8_t devaddr,uint16_t addr,uint8_t* buf,uint16_t len)
{
	//device must enable 
	uint16_t i;
    iic_pins_init();
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
	return 1;
}
/*
uint16_t ex_eeprom_write(uint8_t devaddr,uint16_t addr,uint8_t* buf,uint16_t len)
{
	uint16_t i;
    uint16_t t16;
	uint8_t tmpBuf[64];
	ex_eeprom_read(devaddr,addr,tmpBuf,len);
	iic_pins_init();
	
	for(i=0;i<len;i++){
		if(tmpBuf[i]==buf[i])continue;
		
		iic_start();
		iic_send_byte(devaddr);
		t16=addr+i;
		iic_send_byte((uint8_t)((t16 >> 0x08)&0x00ff));
		iic_send_byte((uint8_t)(t16 & 0x00ff));
		
		iic_send_byte(buf[i]);
		iic_stop();
        HAL_Delay(5);
	}
    HAL_Delay(2);
    iic_pins_deinit();    
	return 1;
}
*/
uint16_t ex_eeprom_write(uint8_t devaddr,uint16_t addr,uint8_t* buf,uint16_t len)
{
	uint16_t i;
    uint16_t t16,pagAddr,subAddr;

	iic_pins_init();
	HAL_Delay(2);
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

    HAL_Delay(2);
    iic_pins_deinit();    
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
	stp->recordReadLoc=0x00;
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
	stp->recordReadLoc=0x00;
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
	//stp->recordNum=0x00;
	stp->recordWriteLoc=0x00;
	stp->recordReadLoc=0x00;
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
	stp->recordReadLoc=0x00;
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
	
	t16=partInf.recordUnRead;
	if(partInf.recordUnRead < limitsItem)partInf.recordUnRead++;
	else{
		t16=partInf.recordReadLoc;
		t16++;
		t16=(t16%limitsItem);
		partInf.recordReadLoc=t16;
	}

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

//读取一个时间段的记录
uint16_t record_read_start_to_end(uint8_t partNb,uint32_t tsStart,uint32_t tsEnd,uint8_t* buf,uint16_t ssize)
{
	uint32_t t32;
	int16_t loc,endloc;
	uint16_t t16,limitsItem;
	uint16_t len=0;
	
	uint8_t tbuf[32];
	if(partNb>3)return 0;
	loc=record_search_at_ts(partNb,tsStart);
	if(loc<0)return 0;
	
	exEepromPartDesc_t partDesc;
	ex_data_get_part_inf(partNb,(uint8_t*)&limitsItem,sizeof(exEepromPartDesc_t));
	
	limitsItem=partDesc.partSize/partDesc.recordSize;
	endloc=partDesc.recordWriteLoc;
	
	endloc+=limitsItem;endloc--;endloc%=limitsItem;
	
	if(sizeof(tbuf)<partDesc.recordSize)while(1);

	do{
		
		t16=partDesc.partStartAddr+loc*(partDesc.recordSize);
		ex_eeprom_read(SLV_ADDR,t16,buf,partDesc.recordSize);
		m_mem_cpy_len((uint8_t*)&t32,buf,sizeof(uint32_t));
		if(t32>tsEnd)break;
		if(partNb<3){
			if(len+sizeof(__sy_gasLog_t)>ssize)break;
			t16=__sy_gas_log_format((__sy_gasLog_t*)(buf+len),(consumeLog_t*)(tbuf));
			len+=t16;
		}else{
			if(len+sizeof(__sy_eventLog_t)>ssize)break;
			t16=__sy_event_log_format((__sy_eventLog_t*)(buf+len),(eventLog_t*)(tbuf));
			len+=t16;
		}
		if(loc==endloc)break;
		loc+=limitsItem;loc++;loc%=limitsItem;
	}while(1);
	return len;
}	

//const uint8_t ExEepromTest[]={0xaa,0xaa,0xaa,0xaa};
void ex_data_test(void)
{
	// uint8_t buf[4];
	// ex_eeprom_write(SLV_ADDR,255,(uint8_t*)ExEepromTest,4);
	// __nop();
	// ex_eeprom_read(SLV_ADDR,255,(uint8_t*)buf,4);
	//ex_eeprom_format();
	ex_eeprom_verify();
	__nop();
}

