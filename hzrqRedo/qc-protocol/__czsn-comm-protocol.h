//file :czsn-comm-protocol.h
//潮州深能定制协议
#ifndef __czsn_comm_protocol_h__
#define __czsn_comm_protocol_h__

#ifdef __cplusplus
	extern "C"{
#endif
	//CZSN = 潮州深能
	#include <stdint.h>
	#include <stdbool.h>
	#include <float.h>
	#include <string.h>
	#include "./configs/user_configs.h"
	
	typedef enum{
		CZSN_NB_COMM_IDLE,
		CZSN_NB_COMM_POP,
		CZSN_NB_COMM_CMD,
		CZSN_NB_COMM_CMD_ERROR,
		CZSN_NB_COMM_POP_ACK,
		CZSN_NB_COMM_END,
	}__czsnNbCommStatus_t;
	extern __czsnNbCommStatus_t czsnNbCommStatus;
	#define CZSN_COMM_FRAME_BUF_LEN 200
	extern uint8_t frameBuf[CZSN_COMM_FRAME_BUF_LEN];
	extern int16_t czsnRssi;
	
	#define CZSN_FRAM_START_CHR		0x68
	
	
	//数据方向，主站呼叫0，从机应答1
	#define CZSN_DIR_MAS_CALL	0
	#define CZSN_DIR_SLV_APLAY	1
	#define CZSN_DIR_SLV_POP	1
	
	//密文标志
	#define CZSN_FI_ENCRYPT_PLAINT	0
	#define CZSN_FI_ENCRYPT_CHIPER	1
	
	#define CZSN_MULT_FRAME		0
	#define CZSN_SINGLE_FRAME	1
	//命令模式,占位符地位在前
	typedef union{
		uint8_t t8;
		struct{
			uint8_t bMeterType:3;
			uint8_t	:2;
			uint8_t	bEncrypt:1;
			uint8_t	nMultFrameFlg:1;
			uint8_t	dir:1;
		}bits;
	}__czsn_cmdModeByte_t;
	//帧头
	typedef struct{
		uint8_t sta;
		uint8_t	meterId[8];
		uint8_t	cmdMode;
		uint8_t	cmdCode;
		uint8_t	mid[4];
		uint8_t	frameSn;
		uint8_t	dfLen[2];
	}__czsn_frameHeader_t;
	
	//阀门状态
	/*
	#define CZSN_VALVE_OPEN		0b01
	#define CZSN_VALVE_CLOSE	0b10
	#define CZSN_VALVE_ERR		0b11
	*/
	#define CZSN_VALVE_OPEN		1
	#define CZSN_VALVE_CLOSE	2
	#define CZSN_VALVE_ERR		3	
	//阀门控制模式
	//VCM=valve ctrl mode
	/*
	#define CZSN_VCM_OPEN_ENABLE	0b01
	#define CZSN_VCM_FORCE_CLOSE	0b10
	#define CZSN_VCM_WARNING_CLOSE	0b11
	*/
	#define CZSN_VCM_OPEN_ENABLE	1
	#define CZSN_VCM_FORCE_CLOSE	2
	#define CZSN_VCM_WARNING_CLOSE	3	
	//流量状态
	/*
	#define CZSN_FLOW_NORMAL		0b00
	#define CZSN_FLOW_SMALL			0b01
	#define CZSN_FLOW_CONST			0b10
	#define CZSN_FLOW_OVER			0b11
	*/
	#define CZSN_FLOW_NORMAL		0
	#define CZSN_FLOW_SMALL			1
	#define CZSN_FLOW_CONST			2
	#define CZSN_FLOW_OVER			3	
	//脉冲状态
	/*
	#define CZSN_PULSE_NORMAL		0b00
	#define CZSN_PULSE_SINGLE_SENS	0b01
	#define CZSN_PULSE_STE			0b10
	*/
	#define CZSN_PULSE_NORMAL		0
	#define CZSN_PULSE_SINGLE_SENS	1
	#define CZSN_PULSE_STE			2	
	//表状态，
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
	
	
	
	//通讯数据域定义
	//命令定义
	#define CZSN_CMD_POP			0x00
	#define CZSN_CMD_READING		0x01
	#define CZSN_CMD_VALVE_CTRL		0x02
	
	
	//咱不用__packed关键字，typedef __packed struct{
	typedef struct{
		uint8_t	dt[5];
		uint8_t	vol[4];
		uint8_t	meterSta[2];
	}__czsn_ddfSampleRecord_t;
	
	typedef struct{
		uint8_t	rssi[2];
		uint8_t	batVolt;
		uint8_t	dt[6];
		uint8_t	meterStatus[2];
		uint8_t	ver[2];
		uint8_t	currVol[4];	
	}__czsn_ddfReading_t,__czsn_ddfPop_t;
	
	/*
	typedef struct{
		uint8_t	len[2];

		uint8_t	rssi[2];
		uint8_t	batVolt;
		uint8_t	dt[6];
		uint8_t	meterStatus[2];
		uint8_t	ver[2];
		uint8_t	currVol[4];

		//后面跟历史采集数据
		//__czsnddef_sampleRecord_t record[24];
	}__cszn_dfd_pop_t;

	typedef struct{
		uint8_t	rssi[2];
		uint8_t	batVolt;
		uint8_t	dt[6];
		uint8_t	meterStatus[2];
		uint8_t	ver[2];
		uint8_t	currVol[4];	
	}__czsn_ddef_reading_t;	
	*/
	//阀门控制
	#define CZSN_CMD_VALVE_CTRL			0x02
	
	#define CZSN_VALVE_CTRL_OPEN		0x01
	#define CZSN_VALVE_CTRL_FORCE_CLOSE	0x02
	#define CZSN_VALVE_CTRL_WARN_CLOSE	0x03
	typedef struct{
		uint8_t	valveCtrlByte;
		uint8_t	meterId[8];
		//uint8_t	meterStatus[2];
	}__czsn_ddfValveCtrl_t;		
	
	//校时
	#define  CZSN_CMD_SYNC_RTC			0x03
	typedef struct{
		uint8_t dt[6];
	}__czsn_ddfSyncRtc_t;	
	
	//设置网络
	#define CZSN_CMD_SET_APN		0x04
	#define CZSN_CMD_GET_APN		0x05
	

	//基本参数
	#define CZSN_CMD_SET_BASE_PARAM		0x06
	#define CZSN_CMD_GET_BASE_PARAM		0x07
	typedef __packed struct{
		uint8_t	time[2];
		uint8_t	interval[2];
		uint8_t	repeat[2];
	}__czsn_ddfSendTime_t,__czsn_ddfSampleTime_t;	
	typedef __packed struct{
		//__czsn_ddfSendTime_t 	sendParam[4];
		//__czsn_ddfSampleTime_t	sampleParam[4];
		uint8_t sendParam[24];
		uint8_t sampleParam[24];
		uint8_t	settleDate;
	}__czsn_ddfbaseParam_t;
	
	//结算信息
	#define CZSN_CMD_GET_SETTLE_INF		0x08
	typedef struct{
		uint8_t	mode;
		uint8_t	dt[5];
		uint8_t	readVol[4];
		uint8_t	meterSta[2];
	}__czsn_ddfSettleInf_t;
	
	typedef struct{
		uint8_t	mode;
		uint8_t	dt[3];
	}__czsn_ddfSettleInfDown_t;	
	//预警关阀模式,00普通模式，关阀后按键可打开并不重复，1-255，打开后等待1-255小时再次关阀
	#define CZSN_CMD_SET_WARN_CLOSE			0x0a
	#define CZSN_CMD_GET_WARN_CLOSE			0x0b
	//长时间无计量关阀
	#define CZSN_CMD_SET_NOFLOW_TIMEOUT		0x0c
	#define CZSN_CMD_GET_NOFLOW_TIMEOUT		0x0d
	//失联关阀检测时长
	#define CZSN_CMD_SET_NONET_TIMEOUT		0x0e
	#define CZSN_CMD_GET_NONET_TIMEOUT		0x0f
	//余额同步
	#define CZSN_CMD_SYNC_BALANCE			0x10
	typedef struct{
		uint8_t	preSettleDt[3];
		uint8_t	preSettleM[4];
		uint8_t	lastChargeDt[3];
		uint8_t	lastChargeM[4];
		//uint8_t	syncBalanceStatus;
		uint8_t valveCtrlMode;
		uint8_t syncShowSta;
		//uint8_t	meterSta[2];
	}__czsn_ddfSyncBalance_t;	
	//获取模组信息
	#define CZSN_CMD_GET_MODULE_INF		0x11
	//IMEI-模组ID号（ASCII）+0x09+IMSI-卡CCID（ASCII）
	#define CZSN_CMD_UPDATE_KEY			0x12
	typedef struct{
		uint8_t key[16];
	}__czsn_ddfUpdateKey_t;	
	//写表号
	#define CZSN_CMD_SET_METER_ID		0x13
	typedef struct{
		uint8_t id[8];
	}__czsn_ddfSetId_t;	
	//写表底数
	#define CZSN_CMD_MOD_VOL			0x14
	typedef struct{
		uint8_t vol[4];
	}__czsn_ddfModVol_t;	
	
	//common api
	extern uint16_t czsnLastSendSampleRecordNum;
	extern uint32_t noNetworkTimer;
	extern uint32_t warningOffTimer;
	extern uint16_t __czsn_meter_status_cross_map(void);
	extern uint16_t __czsn_load_frame_pop(uint8_t* buf,uint16_t ssize);
	extern uint16_t czsn_comm_recevied_process(uint8_t*rbuf,uint16_t rlen);
#ifdef __cplusplus
	}
#endif
#endif

