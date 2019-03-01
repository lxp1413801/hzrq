//file:szrq-comm-protocol.h
//深圳燃气定制协议
#ifndef __szrq_comm_protocol_h__
#define __szrq_comm_protocol_h__

#ifdef __cplusplus
	extern "C"{
#endif

	#include <stdint.h>
	/*
	typedef enum{
		POP_TYPE_FIRST,
		POP_TYPE_SECOND,
		POP_TYPE_FREEZE_SHORT,
	}popPackageType_t;	
	extern volatile popPackageType_t popPackageType;
	*/
	typedef enum{
		__15S_SEND_SM_POP,
		__15S_SEND_SM_CMD,
		__15S_SEND_SM_END_AFTER_SEND,
		__15S_SEND_SM_END,
	}szrqSendStaMichine_t;
	extern szrqSendStaMichine_t szrqSendStaMichine;
	//协议版本
	#define SZRQ_PROTOCOL_VER	1
	
	#define SZRQ_FRAME_START_CHR	0x68
	#define SZRQ_FRAME_END_CHR	0x16	
	#define SZRQ_VENDOR_CODE	0x04

	//plaintext
	#define SZRQ_CIPHER_FLG		0x01
	#define SZRQ_PLAIN_FLG		0x00
	//附加信息第一字节？？,数据来源
	#define EXINF_DS_deviceReq 	0
	#define EXINF_DS_deviceRsp	1
	#define EXINF_DS_cloudReq	2
	#define EXINF_DS_cloudRsp	3
	//附加信息第二字节？？hasMore
	#define EXINF_HAS_MORE	1
	#define EXINT_HAS_NOT	0
	//附加信息第3、4字节
	
	typedef struct{
		uint8_t startChr;
		uint8_t	frameSource;
		uint8_t	hasMore;
		uint8_t	mid[2];
		
		uint8_t	protocolVer;
		uint8_t	vendorCode;
		
		uint8_t	cmd;
		uint8_t	encryptFlg;
		uint8_t	iccid[20];
		//uint8_t	dataLen[2];
	}__szrq_framHeader_t;
	

	//命令定义
	//无此项数据-20000，抄表错误-10000
	#define SZRQ_DATA_UNREAD		-30000
	#define SZRQ_DATA_INVILID		-20000
	#define SZRQ_DATA_READEING_FAIL	-10000
	
	#define SZRQ_DEVTYPE_EPI	1	
	#define SZRQ_DEVTYPE_ROOTS	2
	#define SZRQ_DEVTYPE_ULTRA	3
	#define SZRQ_DEVTYPE_TURBO	4
	#define SZRQ_DEVTYPE_OTHER	5
	
	//民用0，非民用1
	#define SZRQ_DEVCLASS_CIVIL		0
	#define SZRQ_DEVCLASS_NONCIVIL	1
	
	
	//if 
	#if __SUB_VER_DEF_QN>=60
		#define __my_SZRQ_DEVCLASS SZRQ_DEVCLASS_NONCIVIL
	#else 
		#define __my_SZRQ_DEVCLASS SZRQ_DEVCLASS_CIVIL
	#endif
	//平台回复
	#define SZRQ_CMD_CLOUDRSP	0x00
	typedef struct{
		uint8_t	dataLen[2];
		uint8_t	commSn[16];	
		
		uint8_t	rsp;
		uint8_t	dt[6];
		
		uint8_t crc16[2];
		uint8_t endChr;
		
	}__szrq_ddfCloudRsp_t;
	
	/*
	typedef struct{
		uint8_t	dataLen[2];
		uint8_t	rsp;		
	}
	__szrq_ddfCommonBody_t;
	*/
	//单条上报
	//standard temperature and pressure 标准状况( standard temperature and pressure, STP, 标准温度与标准压力),
	#define SZRQ_CMD_POP_SINGLE	0x07
	#define SZRQ_CMD_POP_SINGLE_RSP	0x08
	//单条上报的上报周期
	#define POP_SINGLE_PERIOD_TEMP		0x00
	#define POP_SINGLE_PERIOD_MINUTE	0x01
	#define POP_SINGLE_PERIOD_DAY		0x02
	#define POP_SINGLE_PERIOD_WEEK		0x03
	#define POP_SINGLE_PERIOD_10DAY		0x04
	//上报类型
	#define POP_TYPE_SINGLE				0x07
	#define POP_TYPE_MULT				0x09
	#define POP_TYPE_WARNING			0x0d
	//定义UPDx消息
	typedef union{
		uint32_t t32;
		struct{
			uint8_t	eventMsg;
			uint8_t popType;
			uint8_t warnFlg;	
			union{
				uint8_t	popPeriod;
				uint8_t warnValue;
			};
			
		}str;
	}__szrq_udpSendmsg_t;
	
	extern uint32_t udpSendmsg;
	typedef	struct{
		uint8_t	dataLen[2];
		uint8_t	commSn[16];			//表具通讯号；
		uint8_t	reportPeriod;		//通讯周期
		uint8_t	reportPeriodValue;
		uint8_t	reportTime[2];
		uint8_t	readingTime[6];
		
		uint8_t	stpTotaleVol[4];
		uint8_t	totalVol[4];
		
		uint8_t	rtFlow[4];
		uint8_t	rtTemp[2];
		uint8_t	rtPressure[2];
		
		int8_t	rssi;
		uint8_t	deviceType;
		uint8_t	deviceClass;
		
		uint8_t	valveStatus;
		uint8_t	pwrType;
		uint8_t	batPercent0;
		uint8_t	batPercent1;
		
		uint8_t	simCodeLen;
		uint8_t	simCode[8];
		
		uint8_t	balance[4];
		
		uint8_t	settleVol[4];
		uint8_t	settleDt[6];
		
		uint8_t	reverse[8];
		
		uint8_t crc16[2];
		uint8_t endChr;		
	}__szrq_ddfPopS_t;
	
	//打包数据上报

	#define SZRQ_CMD_POP_MULT	0x09
	#define SZRQ_CMD_POP_MULT_RSP	0x0a
	//typedef 
	typedef	struct{
		uint8_t	dataLen[2];
		uint8_t	commSn[16];			//表具通讯号；
		
		uint8_t	reportPeriod;
		uint8_t	reportDataInterval;
		uint8_t	reportPeriodValue;
		uint8_t	reportTime[2];
		uint8_t	readingTime[6];		
			
	}__szrq_ddfPopMult_h_t;	
	
	typedef	struct{
		uint8_t	stpTotaleVol[4];
		uint8_t	totalVol[4];
		
		uint8_t	rtFlow[4];
		uint8_t	rtTemp[2];
		uint8_t	rtPressure[2];	
			
	}__szrq_ddfPopMult_b_t;		
	
	typedef	struct{
		int8_t	rssi;
		uint8_t	deviceType;
		uint8_t	deviceClass;
		
		uint8_t	valveStatus;
		uint8_t	pwrType;
		uint8_t	batPercent0;
		uint8_t	batPercent1;
		
		uint8_t	simCodeLen;
		uint8_t	simCode[8];
		
		uint8_t	balance[4];
		
		uint8_t	settleVol[4];
		uint8_t	settleDt[6];
		
		uint8_t	reverse[8];
		
		uint8_t crc16[2];
		uint8_t endChr;		
			
	}__szrq_ddfPopMult_tail_t;		
	//tail

	//读冻结数据
	#define SZRQ_CMD_FREEZE_DATA	0x0b
	#define SZRQ_CMD_FREEZE_DATA_RSP	0x0c
	typedef	struct{
		uint8_t	dataLen[2];
		uint8_t	commSn[16];			//表具通讯号；
		
		uint8_t	freezeDate[2];

		uint8_t crc16[2];
		uint8_t endChr;			
	}__szrq_ddfFreezeReq_t;	
	
	typedef	struct{
		uint8_t	dataLen[2];
		uint8_t	commSn[16];			//表具通讯号；
		int8_t rsp;	
		uint8_t	readingTime[6];
		
		uint8_t	stpTotaleVol[4];
		uint8_t	totalVol[4];
		
		//uint8_t	rtFlow[4];
		uint8_t	rtTemp[2];
		uint8_t	rtPressure[2];		
		
		uint8_t	deviceClass;
		
		uint8_t crc16[2];
		uint8_t endChr;				
	}__szrq_ddfFreezeRsp_t;	
	
	//报警事件上报
	#define SZRQ_CMD_WARNING_REPORT 0x0d
	#define SZRQ_CMD_WARNING_REPORT_RSP 0x0e
	//报警1，恢复报警0
	#define SZRQ_WARNING
	#define SZRQ_WARNING_RESUME
	//报警类型
	#define SZRQ_WARNVAL_PRHI			1	/*压力高*/	
	#define SZRQ_WARNVAL_PRLO			2	/*压力低*/
	#define SZRQ_WARNVAL_TEMPHI			3	/*温度高*/
	#define SZRQ_WARNVAL_TEMPLO			4	/*温度低*/
	
	#define SZRQ_WARNVAL_SHELL_OPENC	5	/*通讯部分壳体打开*/
	#define SZRQ_WARNVAL_SHELL_OPENB			6		/*基础部分壳体打开*/
	#define	SZRQ_WARNVAL_STE			7
	#define	SZRQ_WARNVAL_LEAKAGE		8
	#define	SZRQ_WARNVAL_HF				9
	
	#define	SZRQ_WARNVAL_PD_AC			10
	#define	SZRQ_WARNVAL_CUTLINE		11
	
	#define	SZRQ_WARNVAL_PDBAT0			12
	#define	SZRQ_WARNVAL_PDBAT1			13
	#define	SZRQ_WARNVAL_ABMORMAL_OFF	14
	#define	SZRQ_WARNVAL_FAULT			15
	#define	SZRQ_WARNVAL_BALANCE_WARN	16
	#define	SZRQ_WARNVAL_BALANCE_LOW	17
	#define	SZRQ_WARNVAL_BALANCE_NO		18
	//阀门状态
	#define SZRQ_VALVE_NONE				0
	#define SZRQ_VALVE_ON				1
	#define SZRQ_VALVE_OFF				2
	#define SZRQ_VALVE_OFF_FORCE		3
	#define SZRQ_VALVE_UNKNOW			4
	#define SZRQ_VALVE_FAULT			5
	//关阀原因
	#define SZRQ_OFF_REASON_BLAN_LOW	1
	#define SZRQ_OFF_REASON_BLAN_ZERO	2
	#define SZRQ_OFF_REASON_BLAN_OFF	3
	#define SZRQ_OFF_REASON_REMOTE		4
	#define SZRQ_OFF_REASON_PWR_LOW		5
	#define SZRQ_OFF_REASON_SHELL_OPEN	6
	#define SZRQ_OFF_REASON_CUT_LINE	7
	#define SZRQ_OFF_REASON_STE			8
	#define SZRQ_OFF_REASON_OTHER		9		
	//abnormal fault
	typedef struct{
		uint8_t	dataLen[2];
		uint8_t	commSn[16];			//表具通讯号；
		uint8_t	warningDt[6];
		uint8_t	warningFlg;
		uint8_t	warnType;
		//
		uint8_t	rtPressure[2];
		uint8_t	rtTemp[2];
		//uint8_t	rtPressure[2];		
		
		uint8_t	speed[4];			//声速值
		uint8_t	strength[4];		//浓度值
		
		uint8_t	rtFlow[4];
		
		int8_t	batPercent0;
		int8_t	batPercent1;

		uint8_t	balance[4];
		uint8_t	valveStatus;
		
		uint8_t	offReason;
		
		uint8_t	stpTotaleVol[4];
		uint8_t	totalVol[4];
		
		uint8_t	deviceClass;
		
		uint8_t crc16[2];
		uint8_t endChr;				
	}__szrq_ddfWarningReport_t;
	
	
	//阀门控制
	#define SZRQ_CMD_VALVE_CTRL	0x0f
	#define SZRQ_CMD_VALVE_CTRL_RSP	0x10
	
	#define SZRQ_VALVE_CTRL_OPEN	0x33cc
	#define SZRQ_VALVE_CTRL_CLOSE	0x34cb
	#define SZRQ_VALVE_CTRL_CLOSE_F	0x35ca
	#define SZRQ_VALVE_CTRL_LOCK	0x35ca
	
	typedef struct{
		uint8_t	dataLen[2];
		uint8_t	commSn[16];			//表具通讯号；
		
		uint8_t	valveCtrl[2];
		
		uint8_t crc16[2];
		uint8_t endChr;			
	}__szrq_ddfValveCtrl_t;		
	
	typedef struct{
		uint8_t	dataLen[2];
		uint8_t	commSn[16];			//表具通讯号；
		int8_t	rsp;
		uint8_t valveStatus;
		uint8_t crc16[2];
		uint8_t endChr;			
	}__szrq_ddfValveCtrlRsp_t;	

	//基表参数
	#define SZRQ_CMD_SET_BASE_PRAM		0x11
	#define SZRQ_CMD_SET_BASE_PRAM_RSP	0x12
	typedef struct{
		uint8_t		dataLen[2];
		uint8_t		commSn[16];			//表具通讯号；
		
		int8_t		qsSetFlg;
		uint8_t		qs[4];
		int8_t		volSetFlg;
		uint8_t		volume[4];
		int8_t		commSnSetFlg;
		uint8_t		commSnNew[16];
		uint8_t 	crc16[2];
		uint8_t 	endChr;				
	}__szrq_ddfSetBaseParm_t,__szrq_ddfSetBaseParmRsp_t;
	
	//上报周期参数设置
	#define SZRQ_CMD_SET_REPORT_PRAM		0x13
	#define SZRQ_CMD_SET_REPORT_PRAM_RSP	0x14
	typedef struct{
		uint8_t		dataLen[2];
		uint8_t		commSn[16];			//表具通讯号；
		
		uint8_t		periodFlg;
		uint8_t		period;
		
		uint8_t		intervalFlg;
		uint8_t		interval;
		
		uint8_t		periodValueFlg;
		uint8_t		periodValue;
		
		uint8_t		reportTimeFlg;
		uint8_t		reportTime[2];
		
		uint8_t		freezeDayFlg;
		uint8_t		freezeDay;
		
		uint8_t		freezeHourFlg;
		uint8_t		freezeHour;	

		uint8_t 	crc16[2];
		uint8_t 	endChr;			
	}__szrq_ddfSetReportParm_t,__szrq_ddfSetReportParmRsp_t;
	
	//通讯参数
	#define SZRQ_CMD_SET_IOT_PRAM		0x15
	#define SZRQ_CMD_SET_IOT_PRAM_RSP	0x16
	typedef struct{
		uint8_t		dataLen[2];
		uint8_t		commSn[16];			//表具通讯号；
		
		uint8_t		iotIp0Flg;
		uint8_t		iotIp0[4];
		
		uint8_t		iotPort0Flg;
		uint8_t		iotPort0[2];

		uint8_t		iotIp1Flg;
		uint8_t		iotIp1[4];
		
		uint8_t		iotPort1Flg;
		uint8_t		iotPort1[2];

		uint8_t 	crc16[2];
		uint8_t 	endChr;				
	}__szrq_ddfSetIotParm_t,__szrq_ddfSetIotParmRsp_t;	
	
	//报警参数
	#define SZRQ_CMD_SET_WARN_PRAM		0x17
	#define SZRQ_CMD_SET_WARN_PRAM_RSP	0x18
	//自动关阀条件
	typedef union{
		uint8_t	t8;
		struct{
			uint8_t bBalanceWarnOffEn:1;
			uint8_t	bBalanceTempOffEn:1;
			uint8_t	bBalanceOffEn:1;
			uint8_t	bShellOpenEn:1;
			uint8_t	bCutLineEn:1;
			uint8_t	bPwrLowEn:1;
			uint8_t	bSteEn:1;
		}bits;
	}__szrq_valveCtrlByte_t;
	typedef struct{
		uint8_t		dataLen[2];
		uint8_t		commSn[16];			//表具通讯号；
		
		int8_t		pressHiFlg;
		uint8_t		pressHi[4];
		
		int8_t		pressLoFlg;
		uint8_t		pressLo[4];	

		int8_t		tempHiFlg;
		uint8_t		tempHi[4];		

		int8_t		tempLoFlg;
		uint8_t		tempLo[4];	

		int8_t		balanceWarnFlg;
		uint8_t		balanceWarn[4];	

		int8_t		balanceWarnOffFlg;
		uint8_t		balanceWarnOff[4];	

		int8_t		balanceOffFlg;
		uint8_t		balanceOff[4];			
		
		int8_t		valveCtrlByteFlg;
		uint8_t		valveCtrlByte;	

		uint8_t 	crc16[2];
		uint8_t 	endChr;			
	}__szrq_ddfSetWarnParm_t,__szrq_ddfSetWarnParmRsp_t;		

	//读参数
	#define SZRQ_CMD_GET_ALL_PRAM		0x19
	#define SZRQ_CMD_GET_ALL_PRAM_RSP		0x1a
	typedef struct{
		uint8_t		dataLen[2];
		uint8_t		commSn[16];			//表具通讯号；
		
		uint8_t 	crc16[2];
		uint8_t 	endChr;			
	}__szrq_ddfGetAllParm_t;
	
	typedef struct{
		uint8_t		dataLen[2];
		uint8_t		commSn[16];			//表具通讯号；
		
		int8_t		qsSetFlg;
		uint8_t		qs[4];
		int8_t		volSetFlg;
		uint8_t		volume[4];	
		
		int8_t		periodFlg;
		uint8_t		period;
		
		int8_t		intervalFlg;
		uint8_t		interval;
		
		int8_t		periodValueFlg;
		uint8_t		periodValue;
		
		int8_t		reportTimeFlg;
		uint8_t		reportTime[2];
		
		int8_t		freezeDayFlg;
		uint8_t		freezeDay;
		
		int8_t		freezeHourFlg;
		uint8_t		freezeHour;			
		
		int8_t		iotIp0Flg;
		uint8_t		iotIp0[4];
		
		int8_t		iotPort0Flg;
		uint8_t		iotPort0[2];

		int8_t		iotIp1Flg;
		uint8_t		iotIp1[4];
		
		int8_t		iotPort1Flg;
		uint8_t		iotPort1[2];	
		
		int8_t		pressHiFlg;
		uint8_t		pressHi[4];
		
		int8_t		pressLoFlg;
		uint8_t		pressLo[4];	

		int8_t		tempHiFlg;
		uint8_t		tempHi[4];		

		int8_t		tempLoFlg;
		uint8_t		tempLo[4];	

		int8_t		balanceWarnFlg;
		uint8_t		balanceWarn[4];	

		int8_t		balanceOffFlg;
		uint8_t		balanceOff[4];	

		int8_t		balanceLimitOffFlg;
		uint8_t		balanceLimitOff[4];			
		
		int8_t		valveCtrlByteFlg;
		uint8_t		valveCtrlByte;	

		uint8_t 	crc16[2];
		uint8_t 	endChr;			
	}__szrq_ddfGetAllParmRsp_t;	
	
	//设置阶梯气价
	#define SZRQ_CMD_SET_STEPPRICE			0x1b
	#define SZRQ_CMD_SET_STEPPRICE_RSP		0x1c
	#define SZRQ_CMD_GET_STEPPRICE			0x1d
	#define SZRQ_CMD_GET_STEPPRICE_RSP		0x1f	
	typedef struct{
		uint8_t		dataLen[2];
		uint8_t		commSn[16];			//表具通讯号；
		
		uint8_t 	crc16[2];
		uint8_t 	endChr;				
	}__szrq_ddfGetSetpPrice_t;	
	
	typedef struct{
		uint8_t		dataLen[2];
		uint8_t		commSn[16];			//表具通讯号；
		
		uint8_t		billingModeFlg;
		uint8_t		billingMode;
		
		uint8_t		StepFlg;
		uint8_t		Steps;
		uint8_t		stepVol[16];
		uint8_t		stepPrice[20];
		uint8_t		startDt[6];
		uint8_t		billingPeriod;
		/*
		uint8_t		stepFlg;
		uint8_t		steps;
		uint8_t		stepVol[16];
		uint8_t		stepPrice[20];
		uint8_t		startDt[6];
		uint8_t		billingPeriod;
		*/
		uint8_t		preStepFlg;
		uint8_t		preSteps;
		uint8_t		preStepVol[16];
		uint8_t		preStepPrice[20];
		uint8_t		preStartDt[6];
		uint8_t		preBillingPeriod;		
		
		uint8_t		rentFlg;
		uint8_t		rent[4];
		uint8_t		rentMode;
		uint8_t		rentStopValue[4];
		uint8_t		rentStopDt[6];
		
		uint8_t		balanceDispFlg;
		uint8_t		balanceDisp;
		
		uint8_t 	crc16[2];
		uint8_t 	endChr;				
	}__szrq_ddfSetpPrice_t,__szrq_ddfSetSetpPrice_t,__szrq_ddfSetSetpPriceRsp_t,__szrq_ddfGetSetpPriceRsp_t;

	//充值退费
	#define SZRQ_CMD_PAY	0x1f
	#define SZRQ_CMD_PAY_RSP	0x20
	
	typedef struct{
		uint8_t		commSn[16];			//表具通讯号；

		uint8_t		payType;
		uint8_t		payValue[4];
		uint8_t		balanceNew[4];
		uint8_t		cis[20];
		uint8_t		payDt[6];
		uint8_t		severVolume[4];
		uint8_t		severSettleTime[6];
	}__szrq_ddfPay_t,__szrq_ddfPayRsp_t;
	//余额同步
	#define SZRQ_CMD_BALANCE_SYNC	0x21
	#define SZRQ_CMD_BALANCE_SYN_RSP	0x22	
	
	typedef struct{
		uint8_t		dataLen[2];
		uint8_t		commSn[16];			//表具通讯号；
		
		uint8_t		balance[4];
		uint8_t		balanceVol[4];
		uint8_t		severVolume[4];
		uint8_t		severSettleTime[6];
		
		uint8_t		severSetpStartVol[4];
		uint8_t		severSetpStartDt[6];
		
		uint8_t 	crc16[2];
		uint8_t 	endChr;				
	}__szrq_ddfBalanceSync_t;
	
	typedef struct{
		uint8_t		dataLen[2];
		uint8_t		commSn[16];			//表具通讯号；
		
		uint8_t		rsp;
		
		uint8_t		balance[4];
		uint8_t		balanceVol[4];
		uint8_t		severVolume[4];
		uint8_t		severSettleTime[6];
		
		uint8_t		severSetpStartVol[4];
		uint8_t		severSetpStartDt[6];
		
		uint8_t 	crc16[2];
		uint8_t 	endChr;		
	}__szrq_ddfBalanceSyncRsp_t;
	
	//-------------------------------------------------------------------------------------------------
	//api
	extern int16_t __szrq_modify_hase_more(uint8_t* buf,uint16_t len,uint8_t haseMore);
	
	extern int16_t __szrq_load_frame_header(uint8_t* buf,uint16_t ssize,uint8_t dsr,uint8_t cmd);
	extern int16_t __szrq_load_frame_pop_s(uint8_t* buf,uint16_t ssize,uint8_t popPeriodType);
	extern int16_t __szrq_load_frame_pop_m(uint8_t* buf,uint16_t ssize);
	extern int16_t __szrq_load_frame_warn_report(uint8_t* buf,uint16_t ssize,uint8_t warnFlg,uint8_t value);
	extern int16_t __szrq_load_frame_warn_report_ex(uint8_t* buf,uint16_t ssize,uint8_t warnFlg,uint8_t value);
	
	extern int16_t __szrq_load_frame_freeze(uint8_t* buf,uint16_t ssize);
	
	extern int16_t __szrq_received_process(uint8_t* rbuf, uint16_t rlen, uint8_t* sbuf,uint16_t ssize);
	
	extern uint8_t szrqRssi;
	
	extern uint8_t szrqWarnType;
	
	extern uint8_t szrqWarnReportSendOld;
	extern uint8_t szrqWarnReportbuf[128];	

#ifdef __cplusplus
	}
#endif
#endif
//@file end
