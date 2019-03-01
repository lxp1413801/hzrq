//file 15s-comm-protocol.h
//15e基础通讯协议
#ifndef __15s_comm_protocl_h__
#define __15s_comm_protocl_h__

#ifdef __cplusplus
	extern "C"{
#endif
	#include <stdint.h>
	#include <stdbool.h>
	#include <float.h>
	#include <string.h>
	#include "./configs/user_configs.h"
	/*
	协议概要：
	1,帧格式：长度字节(1B)+明文区(12B)+密文区(n)+CRC16(2B)+RSSI(1B);
	2,长度字节=明文区(12B)+密文区(n)+CRC16(2B);
	3,CRC16=crc16(长度字节(1B)+明文区(12B)+密文区(n));
	*/
	#if(NB20E11_SUB_VER>=NB20F11 && NB20E11_SUB_VER<=NB20F11FQ)
		#define NB20E1_DEVICE_VER 0x1f
	#else
		#define NB20E1_DEVICE_VER 0x20
	#endif
	
	//定义协议头=长度字节(1B)+明文区(12B)
	
	//控制字
	#define __15E_CB_CTRL_CODE_LOCAL					0
	#define __15E_CB_CTRL_CODE_SETTING					1
	#define __15E_CB_CTRL_CODE_REG						2
	#define __15E_CB_CTRL_CODE_DATA						3
	#define __15E_CB_CTRL_CODE_TRANS_FAIL				4
	#define __15E_CB_CTRL_CODE_RET_SYNC					5
	#define __15E_CB_CTRL_CODE_RET_DATA					6
	#define __15E_CB_CTRL_CODE_RET_CLEAR				7
	//密块类型
	#define __15E_ENCRYPT_TYPE_NO_KEY					0
	#define __15E_ENCRYPT_TYPE_USER_TRNAS_KEY			1
	#define __15E_ENCRYPT_TYPE_USER_SERVICE_KEY			2
	#define __15E_ENCRYPT_TYPE_COMPANY_TRNAS_KEY		3
	#define __15E_ENCRYPT_TYPE_COMPANY_SERVICE_KEY		4
	//指令定义
	/*
	#define	__15E_CMD_SET_ID				0x51
	#define __15E_CMD_SET_PARAM				0x52
	#define	__15E_CMD_CLEAT_TO_ZERO			0x53
	#define	__15E_CMD_SUPPER_PAY			0x54
	#define	__15E_CMD_OPEN_ACCOUNT			0x57
	#define	__15E_CMD_PAY					0x58
	#define	__15E_CMD_VALVE_CTRL_LOCK		0x59
	#define	__15E_CMD_VALVE_CTRL_UNLOCK		0x60
	#define	__15E_CMD_SYNC_STEP_PRICE_SLU	0x61
	
	#define	__15E_CMD_CONST_FLOW_TIME_OUT	0x69
	#define	__15E_CMD_SET_FLOW_MODE			0x19
	#define	__15E_CMD_ACK					0x00
	#define	__15E_CMD_STATUS_QUERY			0x63
	#define	__15E_CMD_STATUS_FB				0x63
	#define __15E_CMD_POP					0x63
	#define	__15E_CMD_STEP_PRICE_QUERY		0x64
	#define	__15E_CMD_LOG_QUERY				0x65
	*/
	
	#define	__15E_CMD_SET_ID						51
	#define __15E_CMD_SET_PARAM						52
	#define	__15E_CMD_CLEAT_TO_ZERO					53
	#define	__15E_CMD_SUPPER_PAY					54
	#define __15E_CMD_KEY_UPDATE					56
	#define	__15E_CMD_OPEN_ACCOUNT					57
	#define	__15E_CMD_PAY							58
	#define	__15E_CMD_VALVE_CTRL_LOCK				59
	#define	__15E_CMD_VALVE_CTRL_UNLOCK				60
	#define	__15E_CMD_SYNC_STEP_PRICE_SLU			61
	
	#define	__15E_CMD_CONST_FLOW_TIME_OUT			69
	#define	__15E_CMD_SET_FLOW_MODE					19
	//#define __15E_CMD_SET_NO_FLOW_TIME_OUT			69
	#define	__15E_CMD_ACK							00
	#define __15E_CMD_PAY_QUERY						62
	#define	__15E_CMD_STATUS_QUERY					63
	#define	__15E_CMD_STATUS_FB						63
	#define __15E_CMD_POP							63
	#define	__15E_CMD_STEP_PRICE_QUERY				64
	#define __15E_CMD_STEP_PRICE_QUERY_EX			94
	#define	__15E_CMD_LOG_QUERY						65
	
	#define __15E_CMD_SYNC_RTC_SHORT				37
	
	//
	#define __20E_CMD_GET_MODULE_INF				74
	#define __15E_CMD_POP_EX						75
	
	//设置超时参数，命令76
	#define __15E_CMD_SET_TIME_OUT					76

	typedef struct{
		uint8_t id[4];	
		uint8_t	cmd;
		uint8_t tmOut[2];		//C阶段超时时间(h),设置为0表示禁止超时,小端
		uint8_t	tmOutEx[2];		//D阶段超时时间(h),设置为0表示禁止超时，
		uint8_t	tryLimit;		//D阶段重试次数,设置为0表示不限制，只有在tmOutEx设置为非零时有效
		uint8_t	reverse[6];
		uint8_t	checkM[4];		
	}__15e_tmOutSettingdef_t;	

	
	typedef enum{
		__15S_OP_NOTHING=0,				//不关注
		__15S_OP_OK,					//操作成功
		__15S_OP_PAY_OK,				//充值成功
		__15S_OP_PAY_OVER,				//充值金额超限
		__15S_OP_PAY_VERIFY,			//充值校验错误
		__15S_OP_STEP_PRICE_ERR,		//阶梯价格不符，
		__15S_OP_DCLS_ERR,				//表状态不符
		__15S_OP_KEY_UPDATE_ERR,		//密匙升级不符
		__15S_OP_BATTERY_LOW,			//低电，flash禁用
		__15S_OP_BATTERY_LOWx,			//低电，eeprom禁用
		__15S_OP_ONLY_RECEIVED,			//紧紧接收，接收但不执行
		__15S_OP_ERR=11,				//操作错误
		__15S_OP_NOT_MINE=12,			//身份不符
		__15S_OP_VER_ERR=13,			//版本错误，不支持该版本
		__15S_OP_NEED_ORTHER_PACKAGE=14,//需要其余的数据包，分包指令
		__15S_OP_FLASH_ERR=91,			//flash错误
		__15S_OP_EEPROM_ERR=92,			//eeprom错误
		__15S_OP_OSC_ERR=93,			//晶振失效
		__15S_OP_REC_DUPLICATE,
		__15S_OP_SEND_ACK,
		//add by lxp
		__15S_OP_RTC_SYNC,
		
	}em_15sOpRetCode; 
	
	
	extern em_15sOpRetCode opRetCode;
	#define __15S_OP_EXECUTED	__15S_OP_PAY_OK
	
	
	typedef enum{
		__15S_SEND_SM_POP,
		__15S_SEND_SM_CLEAR,
		__15S_SEND_SM_ACK,
	}em_15sSendStaMichine,_15sSendStaMichine_t;		
	extern _15sSendStaMichine_t _15sSendStaMichine;
	
	typedef union{
		uint8_t b;
		struct{
			uint8_t	ctrlCode:3;
			uint8_t	reverse:1;
			uint8_t	fp:1;
			uint8_t	dataDir:1;
			uint8_t	:2;
		}bits;
	}st_15eCtrlByteBits,st_15eCtrlByteFormatDef,ctrlByte_t;

	typedef union{
		uint8_t b;
		struct{
			uint8_t blockNum:4;
			uint8_t keyType:4;
		}bits;
	}st_encryptTypeBits,st_encryptTypeDef,encryptTypeDef_t;
	
	typedef struct{
		uint8_t 			len;
		ctrlByte_t			ctrlByte;
		uint8_t				id[4];
		uint8_t				ver;
		union{
			uint8_t 		cmd;
			uint8_t 		DeviceType;
		};

		uint8_t				serNum;
		encryptTypeDef_t	encryptType;
		uint8_t				CompanyCode[2];
		uint8_t				keyIndex;
	}st_frameHeaderDef;
	
	//密文区：具体业务指令
	//设置id,cmd=0x51
	typedef struct{
		uint8_t id[4];
		uint8_t	cmd;
		uint8_t reverse[3];
	}st_comCmdHeaderDef,comCmdHeaderDef_t;
	
	typedef union{
		uint8_t b;
		struct{
			uint8_t valveMode:1;
			uint8_t	flowMode:1;
			uint8_t	meterType:4;
			uint8_t	changFlg:1;
			uint8_t:1;
		}bits;
	}st_setIdModeBits,st_setIdModeDef,setIdModeDef_t;
	/*
	typedef struct{
		uint8_t id[4];
		uint8_t	cmd;
		uint8_t mode;
		uint8_t	reverse0[2];
		uint8_t	newid[4];
		uint8_t	year[2];
		uint8_t	month;
		uint8_t	date;
		uint8_t	hour;
		uint8_t	minute;
		uint8_t	second;
		uint8_t	day;
		uint8_t	gateRssi;
		uint8_t startVol[3];
		uint8_t	checkM[4];
	}st_setIdDef;
	*/
	typedef struct{
		uint8_t id[4];
		uint8_t	cmd;
		uint8_t mode;
		uint8_t	reverse0[2];
		uint8_t	newid[4];
		uint8_t	year[2];
		uint8_t	month;
		uint8_t	date;
		uint8_t	hour;
		uint8_t	minute;
		uint8_t	second;
		uint8_t	day;
		uint8_t	gateRssi;
		uint8_t startVol[3];
		uint8_t	ipSetting0[24];
		uint8_t ipSetting1[24];
		uint8_t	checkM[4];
	}st_setIdDef;
	
	//参数设置,cmd=0x52
	typedef union{
		uint8_t b;
		struct{
			uint8_t flwOverType:1;	//1:启用过流,0:关闭过流；
			uint8_t	modVolume:1;		//1:修改字轮值;
			uint8_t modMaxPay:1;	//1:修改最大储量
			uint8_t	valveCtrl:1;	//1:锁定阀门
			uint8_t	ulockModDT:1;	//1:解除调时锁定
			uint8_t flowEna:3;		//
		}bits;
	}st_setParFuncCodeBits,setParFuncCodeBits_t;
	
	typedef struct{
		uint8_t id[4];
		uint8_t	cmd;
		uint8_t funcCode;
		uint8_t	period_s;			//秒数
		uint8_t	times;				//次数
		uint8_t	volume[4];			//字轮值
		uint8_t	maxStoreVol[4];
		uint8_t	checkM[4];		
	}st_paramSettingdef;
	
	//清零,cmd=0x53
	typedef struct{
		uint8_t id[4];
		uint8_t	cmd;
		uint8_t	reverse1[3];
		uint8_t	checkM[4];	
	}st_setToZeroDef;
	
	//超级充值,cmd=0x54
	typedef struct{
		uint8_t id[4];
		uint8_t	cmd;
		uint8_t warningSet;
		uint8_t	payValue[2];
		uint8_t	checkM[4];	
	}st_supperPayDef;
	
	//升级密匙:cmd=0x56
	typedef struct{
		uint8_t id[4];
		uint8_t	cmd;
		uint8_t level;
		uint8_t	transKey[8];
		uint8_t	serviceKey[8];
		uint8_t	keyIndex;
		uint8_t reverse;
		uint8_t	checkM[4];	
	}st_keyUpdataDef,keyUpdataDef_t;
	//开户,cmd=0x57
	typedef struct{
		uint8_t id[4];
		uint8_t	cmd;
		uint8_t	stepAndPriceAdjTimes;
		uint8_t	consumeGasType;
		uint8_t	startMonth;
		uint8_t	CompanyCode[2];
		uint8_t	priceStep_00[2];
		uint8_t	userSerNum[4];
		uint8_t	payValue[4];
		uint8_t	warningSet[2];
		uint8_t	stepPricePeriod;
		uint8_t	reverse;
		uint8_t	checkM[4];	
	}st_openAccountDef;
	//同步阶梯价格61,0x3d
	typedef struct{
		uint8_t id[4];
		uint8_t	cmd;
		uint8_t stepYear;		//bit7=0,从0阶开始，第一包；bit7=1，从3阶开始，第一包
		uint8_t	month;			//bit7-4,计价起始月，bit3-0；调价执行日期，月
		uint8_t	day;			//bit7,unused;bit6=0：不清楚累计用气量；bit6=0：清楚
		uint8_t	priceAdjedTimes;
		uint8_t	period;
		uint8_t gasGate0[2];
		uint8_t basePrice0[2];
		uint8_t gasGate1[2];
		uint8_t basePrice1[2];
		uint8_t gasGate2[2];
		uint8_t basePrice2[2];	
		uint8_t	nPriceKind;
		uint8_t	oPriceKind;
		uint8_t	checkM[4];	
	}st_syncStepPricdSln,syncStepPricdSln_t;
	
	//充值,cmd=0x58
	typedef struct{
		uint8_t id[4];
		uint8_t	cmd;
		uint8_t reverse;
		uint8_t payTimes[2];
		uint8_t	preprePayValue[4];
		uint8_t	prePayValue[4];
		uint8_t	PayValue[4];
		uint8_t	warningSet[2];
		uint8_t reverse0[2];
		uint8_t	checkM[4];	
	}st_payDef;
	//阀控，cmd=0x59(LOCK)	cmd=
	typedef struct{
		uint8_t id[4];
		uint8_t	cmd;
		uint8_t reverse[3];
		uint8_t	checkM[4];	
	}st_valveControllerDef;
	//应答，cmd=0;

	
	
	typedef union{
		uint8_t b;
		struct{
			uint8_t bHightFlow:1;
			uint8_t	bHightBatVolt:1;
			uint8_t	bStrongMagnetic:1;	//Strong magnetic field
			uint8_t	bValveErr:1;
			uint8_t	bValveLock:1;
			uint8_t	bValveOff:1;
			uint8_t bReverse:1;
			uint8_t	bNoFlowOverTime:1;
		}bits;
	}st_devStatusBits,devStatusBits_t;
	
	typedef union{
		uint8_t b;
		struct{
			uint8_t bSyncRtc:1;			//randPass.0=1 时间同步请求
			uint8_t bSyncStepPrice:1;	//randPass.1=1五阶梯价格同步请求
			uint8_t bEnterProtect:1;	//randPass.2=1 进入保护状态
			uint8_t bLeakage__:1;		//randPass.3=1燃气泄漏切断报警/干簧管异常，启用单干簧管计数
			uint8_t	bLargeFlow:1;		//randPass.4=1异常大流量切断报警
			uint8_t bPopCycleEn:1;		//randPass.5=1 定时上传功能禁止B
			uint8_t bSmallFlow:1;		//randPass.6=1异常微小流量切断报警
			uint8_t	bConstFlow:1;		//brandPass.7=1持续流量超时报警
		}bits;
	}st_randPassBits;
	
	typedef union{
		uint8_t b;
		struct{
			uint8_t bRfSendFalt:1;		//Rfstate.0=1 上次RF发送数据失败
			uint8_t bRfReceivedTo:1;	//Rfstate.1=1 上次RF接收数据超时
			uint8_t bRfCrcError:1;		//Rfstate.2=1 上次RF接收到的数据CRC校验错误
			uint8_t bRfProtocolErr;		//Rfstate.3=1 上次RF接收到的指令头文件协议不符
			uint8_t bRfMacError;		//Rfstate.4=1 上次RF接收到的指令MAC校验错
			uint8_t bRfEncryBlkErr:1;	//Rfstate.5=1 上次RF接收到的指令加密区协议不符
			uint8_t bRfNotMine:1;		//Rfstate.6=1 上次RF接收到的指令用户身份/表类型不匹配
			uint8_t bRfCompCodeErr:1;	//Rfstate.7=1 上次RF接收到的指令的公司不匹配
		}bits;
	}st_rfStatusBits,rfStatusBits_t;
	
	typedef union{
		uint8_t b;
		struct{
			uint8_t bBatLiSoCl2:2;		//battery.1-0: 00断开锂电池,01锂电池不足,1x锂电池电量正常 
			uint8_t bBatZnMn:2;			//battery.3-2: 00无干电池,01干电池不足,1x干电池电量正常
			uint8_t bLeakAlarm:1;		//battery.4=1报警器已连接
			uint8_t bPopEventEn:1;		//battery.5=1当天主动上传锁定
			uint8_t	bOpenLock:1;		//battery.6=1表端发生拆盖并锁定。			
			uint8_t bDischargeFull:1;	//battery.7=1表端发生完全放电并锁定

		}bits;
	}st_batteryStatusBits,batteryStatusBits_t;
	
	typedef struct{
		uint8_t id[4];
		uint8_t	cmd;
		uint8_t	opStatus;			//执行结果
		uint8_t	deviceStatus;		//表状态
		uint8_t	batteryAndX;		//电池状态
		uint8_t	balance[3];			//
		uint8_t balanceAttach;		//
		uint8_t	randPass;			//事务信息
		uint8_t	rfStatus;			//射频状态
		uint8_t	rssi;				//
		uint8_t	CompanyKeyIndex;	//公司密匙等级，所指是表厂还是燃气公司？
		uint8_t	totalPayValue[4];	//累计充值量
		uint8_t	payTimes[2];		//充值次数
		uint8_t	userKeyIndex;		//用户密匙等级
		uint8_t	ver;				//版号
		uint8_t	checkM[4];			
	}st_AckDef;
	//充值信息查询 cmd=0x62
	typedef struct{
		uint8_t id[4];
		uint8_t	cmd;
		uint8_t	deviceType;
		uint8_t	warningSet[2];
		uint8_t	cacheTimes[2];	//缓存次数，未解何意
		uint8_t	payTimes[2];
		uint8_t	preprePayValue[4];
		uint8_t	prePayValue[4];
		uint8_t	payValue[4];
		uint8_t	checkM[4];			
	}st_payInfQueryDef,payInfQuery_t;
	/*
	//状态信息查询,冒泡
	typedef struct{
		uint8_t id[4];
		uint8_t	cmd;	
		uint8_t reverse00[3];
		uint8_t packageNum;
		uint8_t	dcls;		//状态阶段
		uint8_t	deviceStatus;
		uint8_t	balance[3];
		uint8_t balanceAttach;
		uint8_t	totalVolume[4];
		uint8_t batterySta;
		uint8_t	affairInf;//表事务
		uint8_t	radioStatus;
		uint8_t	deviceVer;
		uint8_t	deviceVerSub;
		uint8_t	priceAdjTimes;
		uint8_t	consumeGasType;
		
		uint8_t	currentPriceStep;
		uint8_t	qn;
		uint8_t	companyKeyIndex;
		uint8_t	userKeyIndex;
		uint8_t	rssi;
		uint8_t	payTimes[2];
		uint8_t	totalPayValue[4];
		uint8_t	currentPrice[2];
		uint8_t	reverse01;
		uint8_t	checkM[4];
	}st_statusInfdef,statusInfdef_t,popDef_t,st_popDef;
	*/
	//冒泡帧第一包格式
	typedef struct{
		uint8_t id[4];
		uint8_t	cmd;	
		uint8_t seqNo;
		
		uint8_t	dcls;		//状态阶段
		uint8_t	deviceStatus;
		uint8_t	balance[3];
		uint8_t balanceAttach;
		uint8_t	curTotalVolume[4];
		uint8_t	totalVolume[4];
		uint8_t batterySta;
		uint8_t	randPass;//表事务
		uint8_t	radioStatus;
		uint8_t	deviceType;	
		uint8_t	checkM[4];
	}st_staInfdef_Pkg0,statusInfdef_Pkg0_t,popDef_Pkg0_t,st_popDef_Pkg0;
	//冒泡帧第二包格式
	typedef struct{
		uint8_t id[4];
		uint8_t	cmd;
		uint8_t seqNo;
		
		uint8_t version;
		uint8_t	priceAdjTimes;	//priceNum
		uint8_t	consumeGasType;
		uint8_t	currentPriceStep;
		uint8_t	qn;
		uint8_t	companyKeyIndex;
		uint8_t	userKeyIndex;
		uint8_t	rssi;
		uint8_t	payTimes[2];
		uint8_t	totalPayValue[4];
		uint8_t	currentPrice[2];
		uint8_t	deviceStatus2;
		uint8_t deviceType2;
		uint8_t	checkM[4];
	}st_staInfdef_Pkg1,statusInfdef_Pkg1_t,popDef_Pkg1_t,st_popDef_Pkg1;
	
	typedef struct{
		uint8_t id[4];
		
		uint8_t	cmd;	
		uint8_t	dcls;		//状态阶段
		uint8_t	deviceStatus;
		uint8_t	deviceType;	
		
		uint8_t	balance[4];
		uint8_t	curTotalVolume[4];
		uint8_t	totalVolume[4];
		uint8_t	totalPayValue[4];
		
		uint8_t	currentPrice[4];	
		
		uint8_t batterySta;
		uint8_t	randPass;//表事务
		uint8_t	radioStatus;
		uint8_t	qn;
		
		uint8_t	payTimes[2];
		uint8_t	priceAdjTimes;	//priceNum
		uint8_t	consumeGasType;

		uint8_t	currentPriceStep;
		uint8_t	companyKeyIndex;
		uint8_t	userKeyIndex;
		uint8_t	rssi;
		uint8_t	dt[8];
		
		uint8_t	checkM[4];
	}popPkgDefEx_t;
	
	typedef struct{
		uint8_t id[4];
		
		uint8_t	cmd;	
		uint8_t	dcls;		//状态阶段
		uint8_t	deviceStatus;
		uint8_t	deviceType;	
		
		//uint8_t	balance[4];
		//uint8_t	curTotalVolume[4];
		uint8_t	totalVolume[4];
		//uint8_t	totalPayValue[4];
		
		//uint8_t	currentPrice[4];	
		
		uint8_t batterySta;
		uint8_t	randPass;//表事务
		uint8_t	radioStatus;
		uint8_t	qn;
		
		//uint8_t	payTimes[2];
		//uint8_t	priceAdjTimes;	//priceNum
		//uint8_t	consumeGasType;

		//uint8_t	currentPriceStep;
		uint8_t	softVer;
		uint8_t	companyKeyIndex;
		uint8_t	userKeyIndex;
		uint8_t	rssi;
		uint8_t	reverse0[4];
		uint8_t	dt[8];
		
		uint8_t	checkM[4];
	}popPkgDefExShort_t;	
	
	typedef struct{
		uint8_t id[4];
		uint8_t	cmd;	
		uint8_t	seqNo;
		uint8_t priceNum;
		uint8_t priceKind;
		uint8_t AdjperiodAdjDay;
		uint8_t	startMonth;
		uint8_t gasGate0[2];
		uint8_t basePrice0[2];
		uint8_t gasGate1[2];
		uint8_t basePrice1[2];
		uint8_t gasGate2[2];
		uint8_t basePrice2[2];	
		uint8_t reverse;
		uint8_t deviceType;
		uint8_t	checkM[4];		
	}st_stepPriceInfQuery,stepPriceInfQuery_t;
	
	typedef struct{
		uint8_t id[4];
		
		uint8_t	cmd;	
		uint8_t	seqNo;
		uint8_t priceNum;
		uint8_t priceKind;
		
		uint8_t AdjperiodAdjDay;
		uint8_t	startMonth;
		
		uint8_t gasGate[12];
		uint8_t basePrice[12];

		uint8_t reverse;
		uint8_t deviceType;
		uint8_t reversex[6];
		uint8_t	checkM[4];		
	}st_stepPriceInfQueryEx,stepPriceInfQueryEx_t;	
	//特殊指令
	//DownstreamTask:11 25 01 000000110A140A180D080624004634	
	typedef struct{
		uint8_t len;
		uint8_t cmd;
		uint8_t id[4];
		uint8_t year;
		uint8_t month;
		uint8_t dayInMonth;
		uint8_t hour;
		uint8_t minute;
		uint8_t second;
		uint8_t timeChip;
		uint8_t mormalRate;
		uint8_t sendSerialNum[2];
		uint8_t crc[2];
		uint8_t rssi;
	}st_syncRtcDef,syncRtcDef_t;
	/*
	typedef struct{
		uint8_t id[4];
		uint8_t	cmd;	
		uint8_t reverse0;
		uint8_t	noFlowTimeOut[2];
		uint8_t	reverse1;
		uint8_t	noFlowTimeOutEx[2];
		uint8_t	reverse[5];
		uint8_t	checkM[4];	
	}noFlowTimeOutSetDef_t;
	*/
	//模组信息
	typedef struct{
		uint8_t id[4];
		uint8_t	cmd;
		uint8_t reverse00[3];
		uint8_t CIMI[16];
		uint8_t	CGSN[16];
		uint8_t	ICCID[21];
		uint8_t reverse01[3];
		uint8_t	checkM[4];	
	}nbModuleInf_t;
	
	extern uint8_t rxFrameIndex;
	extern uint8_t txFrameIndex;	
	
	//add 
	extern volatile uint8_t protocolCmd;
	extern volatile uint8_t stepPriceQueryStep;
	
	//extern uint16_t qc_comm_send_pop_ready(uint8_t* sbuf,uint8_t seqno,uint8_t ctrlCode);
	extern uint16_t qc_comm_send_pop_ready(uint8_t* sbuf,uint8_t ctrlCode);
	extern uint16_t qc_comm_ins_reply_ready(uint8_t* sbuf,uint8_t ctrlCode);
	extern uint16_t qc_comm_received_process(uint8_t* buf,uint16_t len);
	
	//
	//extern volatile bool needPopSecondPackage;
	typedef enum{
		POP_TYPE_FIRST,
		POP_TYPE_SECOND,
		POP_TYPE_FREEZE_SHORT,
	}popPackageType_t;
	extern volatile popPackageType_t popPackageType;
	//
	extern uint8_t __device_status_cross_map(void);
	extern uint8_t __battery_status_cross_map(void);
	extern uint8_t __randpass_scross_map(void);
	extern uint8_t __rf_status_cross_map(void);

#ifdef __cplusplus
	}
#endif	
#endif
//file end
