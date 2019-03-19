//file sy-data-record.h
#ifndef __sy_data_record_h__
#define __sy_data_record_h__

#ifdef __cplusplus
	extern "C"{
#endif
	#include "stdint.h"
	/*
	typedef struct{
		uint32_t ts;
		uint32_t volume;
		uint16_t crc16;
	}consumeLog_t;
	*/
	
	typedef __packed struct{
		uint32_t ts;
		uint32_t volume;
		uint16_t crc16;
	}consumeLog_t;	
	
	/*
	typedef struct{
		uint8_t ts[4];
		uint8_t volume[4];
		uint8_t crc16[2];
	}consumeLog_t;		
	*/
	typedef __packed struct{
		uint32_t 	ts;
		uint16_t	eventCode;
		uint8_t		dcls;
		uint8_t		valveSta;
		uint32_t 	volume;
		//uint32_t 	periodVolum;
		//uint32_t	price;
		uint32_t 	balanceVol;
		uint32_t	balance;
		uint32_t	devSta;
		uint32_t	lock;	
		uint8_t		readFlg;
		uint8_t		reverse;
		uint16_t	crc16;
	}eventLog_t;
	
	typedef __packed struct{
		uint16_t partSize;
		uint16_t partStartAddr;
		
		uint16_t recordSize;
		uint16_t recordNum;
		
		uint16_t recordWriteLoc;
		//uint16_t recordReadLoc;	
		uint16_t recordUnRead;
		uint16_t crc16;
	}exEepromPartDesc_t;
	
	#define EX_EEPROM_PART_NUMBER	4
	typedef struct{
		uint32_t chipSize;
		uint16_t secSize;
		uint16_t pageSize;
		uint16_t partNum;
		uint16_t partBodySize[EX_EEPROM_PART_NUMBER];
		uint16_t crc16;
	}exEepromChipDesc_t;
	#define EX_EEPROM_SLV_ADDR	0xa0
	#define SLV_ADDR			EX_EEPROM_SLV_ADDR	
	
	#define EX_EEPROM_SIZE 			0x10000
	#define EX_EEPROM_SECTOR_SIZE	128
	#define EX_EEPROM_PAGE_SIZE		128
	//#define EX_EEPROM_PART_NUMBER	4
	
	#define PART_SN_CONSTLOG_HOUR	0
	#define PART_SN_CONSTLOG_DAY	1
	#define PART_SN_CONSTLOG_MON	2
	#define PART_SN_EVENTLOG		3
	
	#define CONSTLOG_RECORD_ITEM_SIZE		sizeof(consumeLog_t)
	
	#define CONSTLOG_RECORD_HOUR_ITEM_LIMITS		180*24
	#define CONSTLOG_RECORD_DAY_ITEM_LIMITS			400
	#define CONSTLOG_RECORD_MON_ITEM_LIMITS			12
	
	#define PART_CONSTLOG_HOUR_SIZE				((CONSTLOG_RECORD_ITEM_SIZE)*(CONSTLOG_RECORD_HOUR_ITEM_LIMITS))
	#define PART_CONSTLOG_DAY_SIZE				((CONSTLOG_RECORD_ITEM_SIZE)*(CONSTLOG_RECORD_DAY_ITEM_LIMITS))
	#define PART_CONSTLOG_MON_SIZE				((CONSTLOG_RECORD_ITEM_SIZE)*(CONSTLOG_RECORD_MON_ITEM_LIMITS))
	
	#define EVENT_RECORD_ITEM_SIZE		sizeof(eventLog_t)
	#define EVENT_RECORD_ITEM_LIMITS	100
	#define PART_EVENTLOG_SIZE			((EVENT_RECORD_ITEM_SIZE)*(EVENT_RECORD_ITEM_LIMITS))
	
	#define EXEEP_START_ADDR_CHIP_DESC	0
	
	#define EXEEP_START_ADDR_PART_DESC_CLOG_HOUR 	(EXEEP_START_ADDR_CHIP_DESC+sizeof(exEepromChipDesc_t))
	#define EXEEP_START_ADDR_PART_DESC_CLOG_DAY		(EXEEP_START_ADDR_PART_DESC_CLOG_HOUR+sizeof(exEepromPartDesc_t))
	#define EXEEP_START_ADDR_PART_DESC_CLOG_MON		(EXEEP_START_ADDR_PART_DESC_CLOG_DAY+sizeof(exEepromPartDesc_t))
	#define EXEEP_START_ADDR_PART_DESC_EVENTLOG		(EXEEP_START_ADDR_PART_DESC_CLOG_MON+sizeof(exEepromPartDesc_t))
	
	#define EXEEP_START_ADDR_PART_BODY_CLOG_HOUR	(EXEEP_START_ADDR_PART_DESC_EVENTLOG+sizeof(exEepromPartDesc_t))
	#define EXEEP_START_ADDR_PART_BODY_CLOG_DAY		(EXEEP_START_ADDR_PART_BODY_CLOG_HOUR+PART_CONSTLOG_HOUR_SIZE)
	#define EXEEP_START_ADDR_PART_BODY_CLOG_MON		(EXEEP_START_ADDR_PART_BODY_CLOG_DAY+PART_CONSTLOG_DAY_SIZE)
	#define EXEEP_START_ADDR_PART_BODY_EVENT_CLOG	(EXEEP_START_ADDR_PART_BODY_CLOG_MON+PART_CONSTLOG_MON_SIZE)

	extern  int8_t eventlogReadedTab[EVENT_RECORD_ITEM_LIMITS];
	
	extern uint16_t ex_eeprom_write(uint8_t devaddr,uint16_t addr,uint8_t* buf,uint16_t len);
	extern uint16_t ex_eeprom_read(uint8_t devaddr,uint16_t addr,uint8_t* buf,uint16_t len);
	
	extern uint16_t ex_eeprom_verify(void);
	extern uint16_t ex_eeprom_init(void);
	extern void ex_eeprom_format(void);
	
	extern uint16_t ex_data_get_part_inf(uint8_t partNb,uint8_t* buf,uint16_t ssize);
		
	extern uint16_t ex_data_write_record(uint8_t partNb,uint8_t* buf,uint16_t len);
	extern uint16_t ex_data_read_record_loc(uint8_t partNb,uint16_t recordLoc,uint8_t* buf,uint16_t len);
	
	extern uint16_t ex_data_read_record(uint8_t partNb,uint8_t* buf,uint16_t len,uint16_t* unRead);
	
	extern uint16_t record_api_save_hour_log(void);
	extern uint16_t record_api_save_day_log(void);
	extern uint16_t record_api_save_month_log(void);
	
	//搜索记录中时间大于指定时间ts的第一条记录位置;返回值非负表示位置，返回值负表示搜索失败
	extern int16_t record_search_at_ts(uint8_t partNb,uint32_t ts);
	extern uint16_t record_read_start_to_end(uint8_t partNb,uint32_t tsStart,uint32_t tsEnd,uint8_t* buf,uint16_t ssize);
	extern uint16_t record_save_event_log(uint16_t eventCode);
	extern uint16_t record_read_eventlog_start_to_end(uint8_t* buf,uint16_t ssize,uint32_t startTs,uint32_t endTs,uint16_t rdEventCode,uint16_t rdItems);
	extern uint16_t record_read_eventlog_new(uint8_t* buf,uint16_t ssize,uint16_t rdEventCode,uint16_t rdItems);
	extern void ex_data_test(void);
	
#ifdef __cplusplus
	}
#endif	

#endif

