//file hzrq-comm-protocol.h
#ifndef __hzrq_comm_protocl_h__
#define __hzrq_comm_protocl_h__
#include <stdint.h>
#ifdef __cplusplus
	extern "C"{
#endif
	
	#define __bHZRQ_CBDIR_UP				0
	#define __bHZRQ_CBDIR_DOWN				1
	
	#define __bHZRQ_CBHASMORE_HASMORE 		1
	#define __bHZRQ_CBHASMORE_OVER			0	
	
	#define __bHZRQ_CBFC_DATA_UP_REPORT		1
	#define __bHZRQ_CBFC_DATA_DOWN_SEND		2
	#define __bHZRQ_CBFC_DATA_CONTINUE		3
	#define __bHZRQ_CBFC_DATA_READ			4
	#define __bHZRQ_CBFC_DATA_WRITE			5
	#define __bHZRQ_CBFC_RECORD_READ		7	
	#define __bHZRQ_CBFC_WRITE_FEEDBACK		8
	typedef union{
		uint8_t b;
		struct{
			uint8_t bFuncCode:5;
			uint8_t:1;
			uint8_t	bhasMore:1;
			uint8_t	bDir:1;
		}bits;
	}__hzrq_ctrlByteDef_t;
	
	//定义帧格式
	#define __hzrq_FRAME_START_CHAR			0x68
	#define __hzrq_FRAME_END_CHAR			0x16	
	
	#define __hzrq_PROTOCOL_TYPE_DEFAULT			0
	#define __hzrq_PROTOCOL_FRAME_VER_DEFAULT		1
	typedef struct{
		uint8_t 	startChar;
		uint8_t 	protocolType;
		uint8_t 	protocolFrameVer;
		uint8_t		frameLen[2];
		uint8_t		msgIndex;
		uint8_t		ctrlByte;
	}__hzrq_frameHerder_t;	

	typedef struct{
		uint8_t	crc[2];
		uint8_t	endChar;
	}__hzrq_frameTail_t;
	
	//A.4数据采集
	#define __hzrq_DFID_REGISTER	0x3001
	typedef struct{
		uint8_t iden[2];					
		uint8_t dt[6];						//6	时钟
		uint8_t venderCode[2];				//2	厂商ID
		uint8_t	deviceType[2];				//2	表型号
		uint8_t hazqMeterID[33];			//33	表号参数
		uint8_t accountSta;					//1	开户状态
		uint8_t mobleSeverInf;				//1	运营商信息
		uint8_t commWay;					//1	通信模式
		uint8_t appSoftVer[4];				//4	软件版本
		uint8_t appProtocolVer[2];			//2	应用协议版本		
		uint8_t randomCode[16];				//16	通信随机码
		
		//uint8_t rssi[4];					//4	NB网络信号强度
		uint8_t rsrp[2];
		uint8_t	snr[2];
		uint8_t eclLevel;					//1	ECL覆盖等级
		uint8_t cellID[6];					//6	CellId
		uint8_t realNearfcn[2];				//2	REAL_NEARFCN
		uint8_t imei[15];					//15	IMEI
		uint8_t nbModuleType[10];			//10	模组型号
		uint8_t nbModuleSoftVer[20];			//20	模组固件版本
		uint8_t keyVer;						//1	密钥版本	
	}__hzrq_dfdRegisterReq_t;	
	
	typedef struct{
		uint8_t iden[2];
		uint8_t retCode[2];					//2	错误码
		uint8_t dt[6];						//6	时钟
	}__hzrq_dfdRegisteReply;
	//3002H	通信结束
	#define __hzrq_DFID_END	0x3002
	typedef struct{
		uint8_t iden[2];
		uint8_t retCode[2];					//2	错误码
		uint8_t dt[6];						//6	时钟
		uint8_t balanceVol[4];				//4	剩余气量
		uint8_t overdraftStat;				//1	透支状态
		uint8_t balanceVolSta;				//1	余量状态
		uint8_t price[4];					//4	单价
		uint8_t balanceMoney[4];			//4	剩余金额
	}__hzrq_dfdCommEnd_t;
	
	//3003H 主动上报数据集
	#define __hzrq_POP_TYPE_TIMER	0
	#define __hzrq_POP_TYPE_MANUAL	1
	#define __hzrq_POP_TYPE_EVENT	2
	#define __hzrq_DFID_POP	0x3003
	
	#define __hzrq_EVENT_FLG_ING 	1
	#define __hzrq_EVENT_FLG_PASS 	0
	typedef struct{
		uint8_t iden[2];					
		uint8_t dt[6];						//6	时钟	
		uint8_t popType;					//1	上报方式:0 定时上报；1 手动上报；2 事件上报
		uint8_t	curTotalVol[4];				//4	当前累计气量
		uint8_t	deviceSta[2];				//2	表状态
		//uint8_t customDeviceSta[4];			//4	表厂自定义表状态
		uint8_t	eventCode[2];
		uint8_t eventFlg;
		uint8_t reverse;
		uint8_t	powerType;					//1	供电类型
		uint8_t batVoltage[2];				//2	主电电池电压
		uint8_t	batPercent;					//1	主电电量百分比
		uint8_t hourVolLogBuf[100];			//100	昨天每小时用气日志
		uint8_t dayVolLogBuf[24];			//24	前5天日用气记录
	}__hzrq_dfPop_t;
	
	
	typedef struct{
		uint8_t iden[2];
		uint8_t retCode[2];	
	}__hzrq_dfdCommonReply_t;
	//A.1　状态数据
	//0001H	阀门状态 RW
	typedef struct{
		uint8_t iden[2];
		uint8_t	data[4];
	}__hzrq_dfdStatusDataCommonn_t32_t;
	
	#define __hzrq_DFID_VALVE_CTRL	0x0001
	typedef struct{
		uint8_t iden[2];
		union{
		uint8_t valveCtrl;		//阀门状态(0——开阀门；1——关阀门；2——关阀门并锁定）
		uint8_t valveSta;
		};
	}__hzrq_dfdValveCtrl_t;
	
	//0002H	时钟	RW
	#define __hzrq_DFID_DATETIME	0x0002
	typedef struct{
		uint8_t iden[2];
		uint8_t dt[6];
	}__hzrq_dfdDateTime_t;
	
	//0003H	当前累计气量
	#define __hzrq_DFID_CUR_TOLTAL_VOL 0x0003
	typedef struct{
		uint8_t iden[2];
		uint8_t	curTotalVol[4];	//4	当前累计气量,无符号整数（HEX），数值扩大1000倍用于保留3位小数。
	}__hzrq_dfdCurToltalVol_t;
	
	//0004H	主电电压	R	2
	#define __hzrq_DFID_BAT_VOLTAGE 0x0004
	#define __hzrq_DFID_BAT_VOLTAGE_EX 0x0006
	typedef struct{
		uint8_t iden[2];	
		uint8_t batVoltage[2];	//2	主电电池电压无符号整数（HEX），电压值扩大1000倍，以保留3位小数，单位V。
	}_hzrq_dfdBtVoltage_t;
	
	//0005H	主电电量百分比 R	1
	#define __hzrq_DFID_BAT_PERCENT 	0x0005
	#define __hzrq_DFID_BAT_PERCENT_EX	0x0007
	typedef struct{
		uint8_t iden[2];	
		uint8_t	batPercent;	//无符号整数（HEX）0~100
	}_hzrq_dfdBtPercent_t;	
	
	//0008H	预留量,不懂干设么用
	#define __hzrq_DFID_REVERSE_UKN 0x0008
	typedef struct{
		uint8_t iden[2];
		uint8_t	reverseVol[4];	//4	当前累计气量,无符号整数（HEX），数值扩大1000倍用于保留3位小数。
	}__hzrq_dfdReverseVolUnknow_t;	
	
	//0009H	剩余气量	RW	4
	#define __hzrq_DFID_BALANCE_VOL 0x0009
	typedef struct{
		uint8_t iden[2];
		uint8_t balanceVol[4];	//4剩余气量有符号整数，
								//扩大1000倍以保留3位小数。仅当是气量表时有意义。
	}__hzrq_dfdBalanceVol_t;
	
	//000AH	透支状态	RW	1,uint8_t overdraftStat;				//1	透支状态
	#define __hzrq_DFID_OVERDRAFT_STA 0x000a
	typedef struct{
		uint8_t iden[2];
		uint8_t overdraftStat;//0——非透支；1——透支
	}__hzrq_dfdOverdraftStat_t;
	
	//000BH	表状态	R	2
	#define __hzrq_DFID_DEVICE_STA 0x000b
	/*
	BYTE0	BIT0	阀门状态（0：关；1：开）
		BIT1	表具被强制命令关阀（0：否；1：是）
		BIT2	主电电量不足（0：否；1：是）
		BIT3	备电电量不足（0：否；1：是）
		BIT4	无备电，系统不能正常工作（0：否；1：是）
		BIT5	过流（0：否；1：是）
		BIT6	阀门直通（0：否；1：是）
		BIT7	外部报警触发（0：否；1：是）
	BYTE1	BIT0	计量模块异常（0：否；1：是）
		BIT1	多少天不用气导致阀门关闭（0：否；1：是）
		BIT2	曾出现多天没有远传数据上发成功而导致阀门关闭（0：否；1：是）
		BIT3	电磁干扰（0：否；1：是）
		BIT4	（未定义）
		BIT5	（未定义）
		BIT6	（未定义）
		BIT7	（未定义）	
	*/
	typedef union{
		uint16_t t16;
		struct{
			uint16_t bVavleStaFiOpen:1;
			uint16_t bVavleStaFiLock:1;
			uint16_t bBatPowerLow:1;
			uint16_t bBatPowerLowEx:1;
			uint16_t bHaseExPower:1;
			uint16_t bFlowOver:1;
			uint16_t bVavleErr:1;
			uint16_t bAlarm:1;
			//
			uint16_t bNoFlowTimeOut:1;
			uint16_t bNoConnectTimeOut:1;
			uint16_t bSte:1;
		}bits;
	}__hzrq_deviceStaByteDef_t;
	typedef struct{
		uint8_t iden[2];
		uint8_t	deviceSta[2];//
	}__hzrq_dfdDeviceStat_t;
	
	//000CH	通信随机码	R	16
	#define __hzrq_DFID_RANDOM_CODE 0x000c
	typedef struct{
		uint8_t iden[2];
		uint8_t randomCode[16];				//16	通信随机码
	}__hzrq_dfdRandonCode_t;
	
	//000DH	单价	RW	4
	#define __hzrq_DFID_PRICE 0x000d
	typedef struct{
		uint8_t iden[2];
		uint8_t price[4];//无符号整数（HEX），扩大10000倍以保留4位小数。
	}__hzrq_dfdPrice_t;
	
	//000EH	开户状态	RW	1
	#define __hzrq_DFID_ACCOUNT_STA 0x000e
	#define __hzrq_ACCOUNT_STA_OPENED	1
	#define __hzrq_ACCOUNT_STA_UNOPEN	0
	typedef struct{
		uint8_t iden[2];
		uint8_t accountSta;//无符号整数（HEX）。0-未开户；1-已经开户
	}__hzrq_dfdAccountSTa_t;
	
	//000FH	剩余金额	RW	4
	#define __hzrq_DFID_BALANCE_M 0x000f
	typedef struct{
		uint8_t iden[2];
		uint8_t balanceMoney[4];	//有符号整数（HEX），扩大100倍以保留2位小数。仅当时金额表时有效。
	}__hzrq_dfdBalanceMoney_t;	
	
	//0010H	余量状态	RW	1
	#define __hzrq_DFID_BALANCE_STA 0x0010
	#define __hzrq_BALANCE_VOL_STA_NORMAL	0
	#define __hzrq_BALANCE_VOL_STA_LOW		1
	typedef struct{
		uint8_t iden[2];
		uint8_t balanceVolSta;
	}__hzrq_dfdBalanceVolSta_t;	
	
	//0011H	NB网络信号强度	R	2
	#define __hzrq_DFID_RSSI 0x0011
	//uint8_t rssi[4];
	typedef struct{
		uint8_t iden[2];
		uint8_t rsrp[2];	
		uint8_t snr[2];
	}__hzrq_dfdRssi_t;	
	
	//0012H	通信失败计数	R	2
	#define __hzrq_DFID_COMM_FAIL_TIMES 0x0012
	typedef struct{
		uint8_t iden[2];
		uint8_t commFailTimes[2];
	}__hzrq_dfdCommFailTimes_t;	
	
	//0013H	ECL覆盖等级	R	2	有符号整数 (HEX)	ECL覆盖等级	R	2
	#define __hzrq_DFID_ECL_LEVEL 0x0013
	typedef struct{
		uint8_t iden[2];
		uint8_t eclLevel[2]; //有符号整数 (HEX)
	}__hzrq_dfdEclLevel_t;	
	
	//0014H	CellId	R	6
	#define __hzrq_DFID_CELL_ID 0x0014
	typedef struct{
		uint8_t iden[2];
		uint8_t cellId[6]; //BCD码，最多12位，不足高位补0
	}__hzrq_dfdCellId_t;

	//0015H	REAL_NEARFCN	R	2
	#define __hzrq_DFID_REAL_NEARFCN 0x0015
	typedef struct{
		uint8_t iden[2];
		uint8_t realNearfcn[2];	 //实际接入频点,无符号整数（HEX）
	}__hzrq_dfdRealNerafcn_t;
	
	//0016H	IMEI	R	15
	#define __hzrq_DFID_IMEI 0x0016
	typedef struct{
		uint8_t iden[2];
		uint8_t imei[15];					//15	IMEI
	}__hzrq_dfdImei_t;	
	
	//0017H	模组固件版本	R	20
	#define __hzrq_DFID_NBMODULE_SWVER 0x0017
	typedef struct{
		uint8_t iden[2];
		uint8_t nbModuleSoftVer[20];			//20	模组固件版本
	}__hzrq_dfdNbModuleSwVer_t;

	//0018H	模组型号	R	10
	#define __hzrq_DFID_NBMODULE_TYPE 0x0018
	typedef struct{
		uint8_t iden[2];
		uint8_t nbModuleType[10];			//10	模组型号
	}__hzrq_dfdNbModuleType_t;	
	
	//0100H	厂商自定义表状态	R	4
	#define __hzrq_DFID_CUSTOM_DEVICE_STA 0x0100
	typedef struct{
		uint8_t iden[2];
		uint8_t customDeviceSta[4];			//4	表厂自定义表状态
	}__hzrq_dfdCustomDeviceSta_t;		
	
	//A.2　记录数据
	//1000H	读时间段事件记录
	/*
	2	事件代码, FFFFH表示所有类型，其他类型参考表A.7
	6	起始时间，YYMMDDhhmmss, FFFFFFFFFFFFH表示结束时间之前
	6	结束时间，YYMMDDhhmmss，FFFFFFFFFFFFH表示起始时间之后
	1	记录条数，1字节，应答不超过该条数。FFH表示不限定	
			==================
	1	本帧记录条数（1字节）,记为N
	N*11	事件记录数据（事件记录格式参考附录A.6）		
	*/
	#define __hzrq_DFID_READ_EVENTLOG 0x1000
	typedef struct{
		uint8_t iden[2];
		uint8_t eventCode[2];
		uint8_t startDt[6];
		uint8_t endDt[6];
		uint8_t readNum;
	}__hzrq_dfdReadEventLogReq_t;
	typedef struct{
		uint8_t iden[2];	
		uint8_t eventItemsNum;
		uint8_t	logBuf[4];
	}__hzrq_dfdReadEventLogReply_t;

	//1001H	读最新事件记录	
	/*
	2	事件代码, FFFFH表示所有类型，其他类型参考表A.7
	1	记录条数，应答不超过该条数。FFH表示不限定	
	*/
	#define __hzrq_DFID_READ_EVENTLOG_NEW 0x1001
	typedef struct{
		uint8_t iden[2];
		uint8_t eventCode[2];
		uint8_t eventItemsNum;
	}__hzrq_dfdReadEventLogNewReq_t;	

	//1002H	读每小时用气日志
	//1003H	每小时用气上报
	/*
	3	起始日期，BCD码， YYMMDD
	1	天数
	-----------------------------------------------------
	3	日期，BCD码， YYMMDD
	1	天数
	4*24*天数	每小时用气数据,无符号整数值扩大1000倍以保留3位小数。
	*/	
	#define __hzrq_DFID_READ_VOLLOG_HOUR 0x1002
	#define __hzrq_DFID_POP_VOLLOG_HOUR 0x1003
	//1004H	读日用气记录
	//1005H	日用气记录上报
	#define __hzrq_DFID_READ_VOLLOG_DAY 0x1004
	#define __hzrq_DFID_POP_VOLLOG_DAY 0x1005	
	
	typedef struct{
		uint8_t iden[2];
		uint8_t startDate[3];
		uint8_t days;
	}__hzrq_dfdReadVolLogDayReq_t,__hzrq_dfdReadVolLogHourReq_t;
	
	typedef struct{
		uint8_t iden[2];
		uint8_t startDate[3];
		uint8_t days;
	}__hzrq_dfdReadVolLogDayReply_t,__hzrq_dfdReadVolLogHourReply_t;	
	
	//1006H	读月用气记录
	//1007H	月用气记录上报
	/*
	1	年 BCD码 YY
	1	年 BCD码 YY
	12*4
	=48	月用气累计量，整数值扩大1000倍以保留3位小数	
	*/
	#define __hzrq_DFID_READ_VOLLOG_MONTH 0x1006
	#define __hzrq_DFID_POP_VOLLOG_MONTH 	0x1007
	typedef struct{
		uint8_t iden[2];
		uint8_t year;
	}__hzrq_dfdReadVolLogMonthReq_t;
	typedef struct{
		uint8_t iden[2];
		uint8_t year;
	}__hzrq_dfdReadVolLogMonthReply_t;	

	//A.3　设置数据
	//2000H	厂商ID	R	2
	#define __hzrq_DFID_VENDER_ID 0x2000
	typedef struct{
		uint8_t iden[2];
		uint8_t venderCode[2];	
	}__hzrq_dfdVenderId_t;
	//2001H	软件版本	R	4	BCD码
	#define __hzrq_DFID_APPSW_VER 0x2001
	typedef struct{
		uint8_t iden[2];
		uint8_t appSoftVer[4];	
	}__hzrq_dfdAppSwVer_t;
	
	//2002H	表型号	R	2	无符号整数（HEX）
	#define __hzrq_DFID_DEVICE_TYPE 0x2002
	typedef struct{
		uint8_t iden[2];
		uint8_t	deviceType[2];				//2	表型号
	}__hzrq_dfdDeviceType_t;	
	//2003H	表号参数	R
	#define __hzrq_DFID_DEVICE_ID 0x2003
	typedef struct{
		uint8_t iden[2];
		uint8_t	deviceIdLen;
		uint8_t hazqMeterID[33];			//33	表号参数
	}__hzrq_dfdDeviceId_t;
	
	//2004H	带阀门	R	1
	#define __hzrq_DFID_VALVE_HAS 0x2004
	#define __hzrq_HAS_VALVE	1
	#define __hzrq_HASNO_VALVE		0
	typedef struct{
		uint8_t iden[2];
		uint8_t	fiHasValve;				//0——不带阀门；1——带阀门
	}__hzrq_dfdHasValve_t;	
	
	//2005H	通信模式	R	1
	#define __hzrq_DFID_COMM_WAY 0x2005
	#define __hzrq_DFID_COMM_WAY_NB			0
	#define __hzrq_DFID_COMM_WAY_GPRS		1
	#define __hzrq_DFID_COMM_WAY_LORAWAN	2
	#define __hzrq_DFID_COMM_WAY_IRDA		3
	typedef struct{
		uint8_t iden[2];
		uint8_t	commWay;				//0，NB-IoT；1，GPRS；2，LoRaWAN；3，红外
	}__hzrq_dfdCommWay_t;	
	
	//2006H	定时上传参数
	/*
	1	天数/月份区分位，无符号整数（HEX）。
		0：表示按天来发送，例如2天一次，3天一次
		1：表示按月来发送，例如每月的2号，每月的3号
	
	1	无符号整数（HEX）。周期值，如果配置为按天传，则该字段表示几天。如果配置为按月传，则该字段表示每月的几号
	1	上传时间，BCD码，表示时。
	1	上传时间，BCD码，表示分。	
	*/
	#define __hzrq_DFID_REPORT_PARAM 0x2006
	typedef struct{
		uint8_t iden[2];
		uint8_t	periodType;				//0：表示按天来发送，1：表示按月来发送
		uint8_t periodValue;
		uint8_t hour;
		uint8_t minute;
	}__hzrq_dfdReportParam_t;		
	
	//2007H	采集服务参数	RW
	#define __hzrq_DFID_SEVERS_ADDR_PARAM 0x2007
	typedef struct{
		uint8_t iden[2];
		uint8_t	addr[16];				
		uint8_t port[2];
	}__hzrq_dfdSeversParam_t;
	
	//2008H	结算方式	R	1
	#define __hzrq_DFID_SETTLE_MODE 0x2008
	#define __hzrq_SETTLE_MODE_VOL 1
	#define __hzrq_SETTLE_MODE_M 0
	typedef struct{
		uint8_t iden[2];
		uint8_t	settleMode;//无符号整数（HEX）0. 金额表；1. 气量表；
	}__hzrq_dfdSettleMode_t;	
	
	//2009H	密钥参数	W
	#define __hzrq_DFID_KEY_PARAM 0x2009
	typedef struct{
		uint8_t iden[2];
		uint8_t	keyLen;
		uint8_t	keyVer;
		uint8_t key[32];
	}__hzrq_dfdKeyParam_t;	
	
	//200AH	SIM卡信息	R
	#define __hzrq_DFID_ICCID 0x200a
	typedef struct{
		uint8_t iden[2];
		uint8_t	iccid[20];
	}__hzrq_dfdIccid_t;	
	
	//200BH	运营商信息
	#define __hzrq_DFID_MOBLE_SEVERS_INF 0x200b
	#define __hzrq_MOBLE_SEVERS_CTCC 0
	#define __hzrq_MOBLE_SEVERS_CMCC 1
	#define __hzrq_MOBLE_SEVERS_CUCC 2
	typedef struct{
		uint8_t iden[2];
		uint8_t mobleSeverInf;				//1	运营商信息 1	0--电信；1--移动；2--联通
	}__hzrq_dfdMobleSeverInf_t;		
	
	//200CH	应用协议版本	R	2	BCD码
	//uint8_t appProtocolVer[2];			//2	应用协议版本
	#define __hzrq_DFID_APP_PROTOCOL_VER 0x200c
	typedef struct{
		uint8_t iden[2];
		uint8_t appProtocolVer[2];			//2	应用协议版本
	}__hzrq_dfdAppProtocolVer_t;	

	//200DH	供电类型	R	1	0--碱电；1--锂电
	#define __hzrq_DFID_POWER_TYPE 0x200d
	#define __hzrq_POWER_TYPE_ZNMN		0
	#define __hzrq_POWER_TYPE_LISOCL	1
	typedef struct{
		uint8_t iden[2];
		uint8_t powerType;			//0--碱电；1--锂电
	}__hzrq_dfdPowerType_t;		
	
	//200EH	错峰间隔时间.refuses
	//说明：定时上传错峰方案：定时上传时间 + 表号后3位 ×错峰时间间隔 + 60秒。
	#define __hzrq_DFID_REFUSESPEEK_INTERVAL 0x200e
	typedef struct{
		uint8_t iden[2];
		uint8_t refusesPeekInterval[2];			//无符号整数（HEX）。单位秒，范围15~43。
	}__hzrq_dfdRefusesPeekInterval_t;			
	
	//200FH	多天不用气关阀控制参数	RW	1
	//NoFlowTimeOut
	#define __hzrq_DFID_NOFLOW_TIMEOUT 0x200f
	typedef struct{
		uint8_t iden[2];
		uint8_t noFlowTimeOut;			//无符号整数（HEX）。1字节：0--禁止；1~255 表示天数。
	}__hzrq_dfdNoFlowTimeOut_t;		
	
	
	//2010H	多天不上传关阀控制参数	RW	1
	//NoConnectTimeOut
	#define __hzrq_DFID_NOCONN_TIMEOUT 0x2010
	typedef struct{
		uint8_t iden[2];
		uint8_t noConnectTimeOut;			//无符号整数（HEX）。1字节：0--禁止；1~255表示天数。
	}__hzrq_dfdNoConnectTimeOut_t;	

	//2011H	过流报警使能	RW	1
	#define __hzrq_DFID_HIFLOW_WARNING_EN 0x2011
	typedef struct{
		uint8_t iden[2];
		uint8_t hiFlowWarningEn;			//无符号整数。0：禁止；1：使能
	}__hzrq_dfdhiFlowWarningEn_t;		
	//2012H	APN	RW	32
	#define __hzrq_DFID_APN 0x2012
	typedef struct{
		uint8_t iden[2];
		uint8_t apn[32];			//RW	32	字符串。
	}__hzrq_dfdApn_t;		
	
	
	//A.7　远传表事件记录类型
	/*
	事件代码	事件名称
	0001H	开阀：执行开阀动作
	0002H	关阀：执行关阀动作
	0003H	重新启动
	0004H	电量低
	0005H	电量不足
	0006H	磁干扰
	0007H	电源断电
	0008H	异常流量
	0009H	计量处理单元异常
	A000H~AFFFH	用户自定义事件码	
	*/
	#define __hzrq_EVENT_NONE 			0
	#define __hzrq_EVENT_VALVE_OPEN		1
	#define __hzrq_EVENT_VALVE_CLOSE 	2
	#define __hzrq_EVENT_RST		3
	#define __hzrq_EVENT_POWER_LOW		4
	#define __hzrq_EVENT_POWER_LOWER	5
	#define __hzrq_EVENT_STE			6
	#define __hzrq_EVENT_POWER_DOWN		7
	#define __hzrq_EVENT_FLOW_ABNORMAL	8
	#define __hzrq_EVENT_METER_FAULT	9
	
	//A.8　错误码
	/*
	错误码	说明
	0	无错误
	1	数据对象ID不正确
	2	日期非法
	3	协议代码不支持
	4	协议框架版本不支持
	5	MAC认证错误
	6	应用协议版本不支持
	7	写参数值非法
	8	表号非法
	*/
	#define __hzrq_ERR_NONE				0
	#define __hzrq_ERR_DFID				1
	#define __hzrq_ERR_DT				2
	#define __hzrq_ERR_PROTOCOL			3
	#define __hzrq_ERR_PROTOCOL_FRAME	4
	#define __hzrq_ERR_MAC				5
	#define __hzrq_ERR_APP_PROTOCOL		6
	#define __hzrq_ERR_WRITE_PARAM		7
	#define __hzrq_ERR_METER_ID			8
	
	//B.1　校验算法
	extern uint16_t __hzrq_crc16(uint8_t* puchMsg,uint16_t usDataLen);
	
	//与数据记录有关的数据结构定义
	typedef struct{
		uint8_t dt[5];
		uint8_t	volume[4];
		uint8_t	volumeEx;
	}__hzrq_gasLog_t;
	
	typedef struct{
		uint8_t eventType[2];
		uint8_t	dt[6];
		//uint8_t times[2];
		uint8_t	xx;
	}__hzrq_eventLog_t;
	extern uint16_t __hzrq_event_log_format(__hzrq_eventLog_t* hzrqEventLog,eventLog_t* commEventLog);
	extern uint16_t __hzrq_gas_log_format(__hzrq_gasLog_t* syGasLog,consumeLog_t* commGasLog);
	
	//为nb数据交互定义的状态机
	typedef enum{
		__hzrq_SEND_SM_NONE,
		__hzrq_SEND_SM_REGISTER,
		__hzrq_SEND_SM_POP,
		__hzrq_SEND_SM_CMD,
		__hzrq_SEND_SM_END,
		__hzrq_SEND_SM_FOTA,

	}__hzrq_sendStaMichine_t;
	extern __hzrq_sendStaMichine_t sendStaMichine;	
	//api
	extern volatile int16_t hzrqRsrp;
	extern volatile int16_t hzrqSnr;
	extern volatile uint16_t __hzrqUnSendNum;
	extern volatile int8_t hzrqEclLevel;
	extern uint8_t hzrqCellId[6];
	extern volatile uint16_t hzrqRealNearFcn;
	extern uint8_t hzrqNbModuleType[10];
	extern uint8_t hzrqNbModuleSoftVer[20];
	
	extern uint16_t __hzrq_load_frame_header(uint8_t* sbuf,uint16_t ssize,uint16_t len,uint8_t cb);
	extern uint16_t hzrq_load_register_frame(uint8_t* sbuf,uint16_t ssize);
	extern uint16_t hzrq_load_pop_frame(uint8_t* sbuf,uint16_t ssize,uint8_t popType,uint16_t eventCode,uint8_t eventFlg);
	extern int16_t hzrq_comm_received_process(uint8_t* rbuf,uint16_t rlen,uint8_t* sbuf,uint16_t ssize,uint8_t popType);
#ifdef __cplusplus
	}
#endif
#endif
//file end

