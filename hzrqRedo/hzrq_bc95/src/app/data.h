//file name :data.h
#ifndef __m_DATA_H__
#define __m_DATA_H__
#ifdef __cplusplus
	extern "C"{
#endif
	#include <stdint.h>
	#include <stdbool.h>
	#include <string.h>
	
	#ifdef STM32L073xx
		#include "stm32l073xx.h"
	#elif defined(STM32L152xB)
		#ifndef DATA_EEPROM_BASE
			#define DATA_EEPROM_BASE (0x08080000ul)
		#endif
		#include "stm32l1xx_hal.h"

	#endif	
	
	
	#define SYS_DATA_MAIN_ADDR (DATA_EEPROM_BASE)

	
	extern uint32_t adcValue;
	extern uint32_t voltBat;
	extern uint32_t voltAvdd;	
	
	extern void qc_data_read_from_media(uint8_t* d,uint32_t mediaAddr,uint16_t len);
	extern void qc_data_write_to_media(uint32_t mediaAddr,uint8_t* s,uint16_t len);
	extern void qc_data_set_rtc(uint8_t* dt);
	extern uint16_t qc_data_sysdata_set_default(void);
	extern uint16_t qc_data_sysdata_init(void);
	
	extern uint16_t qc_data_sysdata_write_only(uint8_t* tbuf);
	extern osMutexId osMutexSysData;
	//extern osMutexId osMutexSysDataSave;

	extern void qc_data_mutex_create(void);
	
	//extern const uint32_t defaultID;
	
	
	//<<--for czsn
	
	extern uint8_t czsnFlowStatus;
	//??????????????
	
	
	
	typedef struct{
		uint32_t ts;
		uint32_t vol;
		uint16_t sta;
		uint16_t crc;
	}recordDef_t,volItem_t;
	
	
	typedef	struct{
		uint16_t	writeLoc;
		uint16_t	itemNum;
		uint16_t	unreadNum;
		uint16_t	crc;
	}volLogHeader_t;
	
	#define FREEZE_VOLLOG_HEADER_START_ADDR	(DATA_EEPROM_BANK2_BASE)
	#define FREEZE_VOLLOG_BODY_START_ADDR		(FREEZE_VOLLOG_HEADER_START_ADDR)+sizeof(volLogHeader_t)
	#define FREEZE_VOLLOG_ITEM_NUM_LIMITS	20
	
	typedef	struct{
		uint16_t	writeLoc;
		uint16_t	itemNum;
		uint16_t	readLoc;
		uint16_t	crc;
	}szrqVolLogHeader_t;
	
	typedef struct{
		uint32_t 	ts;
		uint8_t		szrqRoportPeriodType;			//????
		uint8_t		szrqRoportDataInterval;			//????
		uint8_t		szrqtPeriodValue;				//??? 
		uint8_t		reverse;
		uint32_t 	vol;
		uint32_t	szrqcollectNextTm;
		uint16_t 	flow;
		uint16_t 	crc;
	}szrqVolLogItem_t;	
	

	
	#define SZRQ_VOLLOG_HEADER_START_ADDR	(DATA_EEPROM_BANK2_BASE+256)
	#define SZRQ_VOLLOG_BODY_START_ADDR		(SZRQ_VOLLOG_HEADER_START_ADDR)+sizeof(szrqVolLogHeader_t)
	#define SZRQ_VOLLOG_ITEM_NUM_LIMITS	100
	
	
	extern uint16_t freeze_part_format(void);
	extern uint16_t freeze_part_init(void);
	extern uint16_t freeze_item_save(void);
	extern uint16_t freeze_item_read(uint8_t* buf,uint16_t ssize,uint16_t itemOffset);
	extern uint16_t freeze_item_unread_num_offset(uint16_t itemOffset);
	extern uint16_t freeze_item_get_unread_num(void);
	
	extern uint16_t freeze_item_get_num(void);
	
	extern void freeze_item_save_test(void);
	
	extern bool szrqRtcSync; 
	extern uint32_t szrqcollectNextTm;
	extern uint16_t szrqMultSendNum;
	extern uint8_t  szrqMulHasMore;
	extern uint8_t szrqRecHasMore;
	//<--for szrq
	extern uint16_t szrq_item_get_num(void);
	extern uint16_t szrq_part_format(void);
	extern uint16_t szrq_item_save(void);
	extern uint16_t szrq_item_read(uint8_t* buf,uint16_t ssize,uint16_t itemOffset);
	extern uint16_t	szrq_item_set_read_loc(uint16_t readLoc);
	extern uint16_t szrq_item_get_unread_num(void);
	//-->	
	
	//<<-- add for send fifo
	//2019.02.13
	#define RF_SEND_FIFO_HEAD_START_ADDR	(DATA_EEPROM_BASE + 1024)
	typedef struct{
		uint16_t	writeLoc;
		uint16_t	itemNum;
		uint16_t	unreadNum;
		uint16_t	crc;
	}rfSendFifoHeader_t;
	
	#define RF_SEND_FIFO_BODY_START_ADDR  (RF_SEND_FIFO_HEAD_START_ADDR + sizeof(rfSendFifoHeader_t))
	#define RF_SEND_FIFO_ITEM_NUM_LIMIT	8
	
	typedef struct{
		uint8_t  sendedFlg;
		uint8_t  reverse;
		uint16_t len;
		uint32_t ts;
		uint8_t  buf[200];
		uint16_t crc16;
	}rfSendFifoBody_t;

	extern uint16_t rf_send_fifo_format(void);
	extern uint16_t rf_send_fifo_init(void);
	extern uint16_t rf_send_fifo_push(uint8_t* buf,uint16_t len);
	extern uint16_t rf_send_fifo_delete_tail(void);
	extern uint16_t rf_send_fifo_get_tail(uint8_t* buf,uint16_t* unreadNum);
	
	extern uint16_t rf_send_fifo_init(void);

	//-->>
#ifdef __cplusplus
	}
#endif
#endif
//file end

