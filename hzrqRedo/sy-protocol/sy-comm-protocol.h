//file sy-comm-protocol.h
#ifndef __sy_comm_protocl_h__
#define __sy_comm_protocl_h__

#ifdef __cplusplus
	extern "C"{
#endif
	#include "stdint.h"
	
	extern uint8_t batCapacity;
	
	#define __SY_PROTOCOL_VER	0x10
	#define __SY_PROTOCOL_CODE	0x01
	//事件
	
	//用气记录
	
	//由16位数字组成，包括3位表厂代码，4位生产年月，7位流水号，2位远传表嵌入式软件版本号。
	typedef union{
		uint8_t snBuf[16];
		struct{
			uint8_t factoryCode[3];
			uint8_t	productionDate[4];
			uint8_t	sn[7];
			uint8_t	ver[2];
		}stru;
	}__sy_snStrFormat_t;
	

	
	//帧控制字
	#define __bSY_CBCC_READ_REGISTER_FRAME		1
	#define __bSY_CBCC_COMMOM_DATA				2
	#define __bSY_CBCC_FIRST_READ_RECODE		3
	#define __bSY_CBCC_FLLOW_READ_RECODE		4
	#define __bSY_CBCC_FILE_DOWNLOAD			5
	
	#define __bSY_CBDIR_UP		1
	#define __bSY_CBDIR_DOWN	0

	#define __bSY_CBOP_WRITE	1
	#define __bSY_CBOP_READ		0
	

	typedef union{
		uint8_t b;
		struct{
			uint8_t bCode:6;
			uint8_t	bOp:1;
			uint8_t	bDir:1;
		}bits;
	}__sy_ctrlByteDef_t;
	
	//定义帧格式
	#define __nSY_FRAME_START_CHAR	0x68
	#define __nSY_FRAME_END_CHAR	0x16	
	typedef struct{
		uint8_t 	startChar;
		uint8_t		len[2];
		uint8_t		syID[8];
		uint8_t		protocolCode;
		uint8_t		protocolVer;
		uint8_t		cmdSn;
		uint8_t		ctrlByte;
		//uint8_t		iden[2];
	}__sy_frameHerder_t;
	
	typedef struct{
		uint8_t	crc[2];
		uint8_t	endChar;
	}__sy_frameTail_t;

	typedef struct{
		uint16_t len;
		uint16_t iden;
		uint8_t* data;
	}__sy_dataFieldRedef_t;
	/*
	typedef struct{
		
		
	}__sy_frameFormat_t;
	*/
	//
	//定义注册帧
	//fdf frame data field
	//dfid	data field data identify
	#define __SY_OP_RET_SUCCESS		1
	#define __SY_OP_RET_ERROR		0
	
	#define __fdfSY_DFID_REGISTER	0x8001
	typedef struct{
		uint8_t iden[2];
		uint8_t	esamId[8];
		uint8_t	keyCrypt;
		uint8_t	keyMac;
		uint8_t	dt[6];
		uint8_t	random[8];
		uint8_t	mac[4];
	}__sy_dfRegisterUp_t;
	#define __fdfSY_RET_REG_SUCCESS					0
	#define __fdfSY_RET_REG_REDO					1
	#define __fdfSY_RET_REG_ILLEGAL_ID				2
	#define __fdfSY_RET_REG_PROTOCOL_ERROR			3
	#define __fdfSY_RET_REG_PROTOCOL_VER_ERROR		4
	#define __fdfSY_RET_REG_FACTORRY_CODE_ERROR		5
	#define __fdfSY_RET_REG_NO_ACCONTED				6
	#define __fdfSY_RET_REG_DT_ERROR				7
	#define __fdfSY_RET_REG_MAC_ERROR				8
	typedef struct{
		uint8_t iden[2];
		uint8_t	ret;
		uint8_t	keyCrypt;
		uint8_t	keyMac;
		uint8_t	dt[6];
		uint8_t	mac[4];
	}__sy_fdfRegisterDown_t;	
	//全貌信息
	typedef struct{
		uint8_t dt[5];
		uint8_t	volume[4];
		uint8_t	volumeEx;
	}__sy_gasLog_t;
	
	typedef struct{
		uint8_t eventType[2];
		uint8_t	dt[6];
		uint8_t times[2];
		uint8_t	xx;
	}__sy_eventLog_t;
	
	#define __fdfSY_DFID_WHOLE_INF						0x0001
	typedef struct{
		uint8_t iden[2];
		uint8_t	sta;
		uint8_t rssi;
		uint8_t battaryCap;
		uint8_t sendGasLogNm[2];
		uint8_t usendGasLogNm[2];
		__sy_gasLog_t gasLog_1st;
	}__sy_fdfWholeInf_t;	

	typedef struct{
		uint8_t sendEventLogNm;
		uint8_t usendEventLogNm;
		__sy_eventLog_t eventLog_1st;
	}__sy_fdfWholeInfEx_t;		
	//运行状态
	#define __fdfSY_DFID_RUN_STATUS				0x1001
	typedef union{
		uint8_t b;
		struct{
			uint8_t 	bMstBatStatus:2;
			uint8_t		bSlvBatStatus:2;
			uint8_t		bValveStatus:1;
			uint8_t		bValveLocked:1;
			uint8_t		bMeterAbnormal:1;	//计量异常
			uint8_t		:1;
		}bits;
	}__sy_runStatusByteDef_t;
	
	typedef struct{
		uint8_t iden[2];
		uint8_t	sta;
	}__sy_fdfRunStatus_t;	
	//无下行数据
	
	//结算状态
	#define __fdfSY_DFID_SETLLE_STATUS		0x1002
	typedef union{
		uint8_t b;
		struct{
			uint8_t	bOverdraw:1;
			uint8_t	bBalance:1;
			uint8_t:6;
		}bits;
	}__sy_stelleStatusByteDef_t;
	
	typedef struct{
		uint8_t iden[2];
		uint8_t setlleSta;
	}__sy_fdfSetlleSta_t;	

	//
	
	//表当前电压2字节BCD码。整数1字节，小数1字节，整数在前。如0x05,0x77，表示电压值为5.77V。
	#define __fdfSY_DFID_VOLTAGE					0x2001
	typedef struct{
		uint8_t iden[2];
		uint8_t voltage[2];
	}__sy_fdfVoltage_t;		
	
	//表内时钟,6字节BCD码，如{0x14,0x02,0x22,0x12,0x01,0x02}表示14年2月22日12时1分2秒。
	#define __fdfSY_DFID_RTC						0x2003
	typedef struct{
		uint8_t iden[2];
		uint8_t	dt[6];
	}__sy_fdfRtc_t;
	
	//本次通信信号强度,1字节HEX格式，数值范围0-100，表示信号强度百分比
	#define __fdfSY_DFID_RSSI						0x2004
	typedef struct{
		uint8_t iden[2];
		uint8_t	rssi;
	}__sy_fdfRssi_t;
	
	//累积气量,6字节BCD码，4字节整数+2字节小数。如{0x00.0x00.0x01.0x23.0x45.0x60}表示123.456方。
	#define __fdfSY_DFID_TOTAL_VOL				0x2005
	typedef struct{
		uint8_t iden[2];
		uint8_t	tov[6];
	}__sy_fdfTotaleVlume_t;	
	
	//SIM卡信息,BCD码格式，15字节，高位在前，位数不足高位补0。
	#define __fdfSY_DFID_CCID						0x2006
	typedef struct{
		uint8_t iden[2];
		uint8_t	ccid[15];
	}__sy_fdfCcid_t;		
	
	//开阀,强制关阀,普通关阀
	#define __fdfSY_DFID_OPEN_VALVE					0x3001
	#define __fdfSY_DFID_FORCE_CLOSE				0x3002
	#define __fdfSY_DFID_CLOSE_VALVE				0x3003
	typedef struct{
		uint8_t iden[2];
		uint8_t	ret;
	}__sy_fdfValveCtrl_t;	
	
	
	//通信结束帧
	#define __fdfSY_DFID_END_FRAME					0x3004
	typedef struct{
		uint8_t iden[2];
		uint8_t	dt[6];
	}__sy_fdfEnd_t;
	
	//主服务器IP地址及端口号
	#define __fdfSY_DFID_MSTSEVER_IP			0x5031
	#define __fdfSY_DFID_SLVSEVER_IP			0x5032
	typedef struct{
		uint8_t iden[2];
		uint8_t	ip[4];
		uint8_t	port[2];
	}__sy_fdfServerSetting_t;	
	//
	
	//定时上传周期,5字节
	//1字节表示周期类型，HEX格式。0表示天（起始周期从每月1日开始）。1表示月（起始周期从每年1月开始）。
	//1字节表示周期值，HEX格式。
	//3字节表示上传时间，BCD码，表示日时分（周期类型为月时，日、时、分均有效，周期类型为天时，时、分有效）。 
	#define __fdfSY_DFID_POP_PERIOD					0x5033
	typedef struct{
		uint8_t iden[2];
		uint8_t	periodType;
		uint8_t	periodValue;
		uint8_t	dd;
		uint8_t	hh;
		uint8_t	mm;
	}__sy_fdfPopPeriod_t;
	
	//APN
	#define __fdfSY_DFID_APN						0x5034
	typedef struct{
		uint8_t iden[2];
		uint8_t apn[30];
	}__sy_fdfAPN_t;	
	
	//设置多天不用气关阀,1字节使能位（0表示关闭，1表示开启），1字节表示不用气天数
	#define __fdfSY_DFID_NOFLOW_PROTECT 			0x5035
	typedef struct{
		uint8_t iden[2];
		uint8_t noFlowProtectEn;
		uint8_t	timeOut;
	}__sy_fdfNoFlowProtect_t;		
	
	//设置多天不通信关阀
	#define __fdfSY_DFID_COMMFAIL_PROTECT			0x5036
	typedef struct{
		uint8_t iden[2];
		uint8_t commFailProtectEn;
		uint8_t	timeOut;
	}__sy_fdfCommFailProtect_t;	
	
	//事件记录
	#define __fdfSY_DFID_EVENT_LOG				0x6001
	typedef struct{
		uint8_t iden[2];
		uint8_t startDt[5];
		uint8_t	endDt[5];
	}__sy_fdfEventLog_t;	
	
	typedef struct{
		uint8_t iden[2];
		uint8_t startDt[5];
		uint8_t	endDt[5];
		
	}__sy_fdfEventLogUp_t;	
	//用气记录
	#define __fdfSY_DFID_GAS_LOG				0x6002
	typedef struct{
		uint8_t iden[2];
		uint8_t startDt[5];
		uint8_t	endDt[5];
	}__sy_fdfGasLog_t;		
	
	
	//一般应答
	typedef struct{
		uint8_t iden[2];
		uint8_t ret;
	}__sy_fdfComRet_t;
	
	typedef enum{
		__sy_COMMUNICATION_STATUS_IDLE,
		__sy_COMMUNICATION_STATUS_SENDREG,
		__sy_COMMUNICATION_STATUS_SENDWHOLEINF,
		__sy_COMMUNICATION_STATUS_CMD,
		__sy_COMMUNICATION_STATUS_END,
	} __sy_CommStatus_t;
	extern __sy_CommStatus_t syCommStatus;
	
	//error code
	typedef enum{
		__SY_ERRCODE_NO_ERR,
		//__SY_ERRCODE_VALVE_ERR=21,		//阀门故障
		
		
		__SY_ERRCODE_TEST_PAY=01,		//
		//__SY_ERRCODE_MAC_ERR=02,
		__SY_ERRCODE_KEY_AUTH_ERR=03,
		//__SY_ERRCODE_READ_CORD_ERR=04,
		//__SY_ERRCODE_WRITE_CORD_ERR=05,
		//__SY_ERRCODE_NO_TEST=14,
		//__SY_ERRCODE_TEST_ERR=24,
		__SY_ERRCODE_SYNC_RTC_ERR=50,	//校时超过允许差
		__SY_ERRCODE_FRAME_CRC_ERR=51,
		__SY_ERRCODE_FRAME_MAC_ERR=52,
		__SY_ERRCODE_ESAM_RET_ERR=56,
		__SY_ERRCODE_METER_ERR=60,		//计量处理单元故障
		
		__SY_ERRCODE_NB_CONN_ERR=101,
		__SY_ERRCODE_NB_SEND_ERR=102,
		__SY_ERRCODE_NB_REC_ERR=103
		
	}__sy_errCode_t;
	extern __sy_errCode_t __syErrorCode;
	extern void __sy_err_code_set(__sy_errCode_t ec);
	extern uint8_t __sy_err_code_get(void);
	//variable
	extern uint8_t	syCmdSn;
	extern uint8_t __syID[8];
	extern uint8_t keyCrypt;
	extern uint8_t keyMac;
	#define SY_COMM_FRAME_BUF_LEN 256
	extern uint8_t frameBuf[SY_COMM_FRAME_BUF_LEN];
	//api
	extern uint16_t __sy_gas_log_format(__sy_gasLog_t* syGasLog,consumeLog_t* commGasLog);
	extern uint16_t __sy_event_log_format(__sy_eventLog_t* syEventLog,eventLog_t* commEventLog);
	extern uint16_t __sy_load_meter_id(uint32_t id,uint16_t fc,uint16_t softVer,uint16_t pd,uint8_t* syid);
	extern uint16_t __sy_load_frame_register(uint8_t* sbuf,uint16_t ssize,uint8_t keyCrypt,uint8_t keyMac);
	extern uint16_t __sy_load_frame_whole_inf(uint8_t* sbuf,uint16_t ssize,uint8_t keyCrypt,uint8_t keyMac);
	
	extern uint16_t __sy_comm_received_process(uint8_t* rbuf,uint16_t rlen);
	extern uint16_t __sy_test(void);
	
#ifdef __cplusplus
	}
#endif	

#endif

