//file name :15e-data-protocol.h
//15e基础数据协议
#ifndef __15e_data_protocol_h__
#define __15e_data_protocol_h__

#ifdef __cplusplus
	extern "C"{
#endif
	#include <stdint.h>
	#include <stdbool.h>
	

	
	//定义一个宏，获取结构体成员的偏移地址
	//#define get_offset_addr_of_str(TYPE, MEMBER) ((uint32_t) (&((TYPE *)0)->MEMBER))
    //#define __OFFSET get_offset_addr_of_str

	#include "subversion.h"
	#include "qndef.h"	
	typedef enum __emQN
    {
        QN10=QNDEF_QN10,
        QN16=QNDEF_QN16,            
        QN25=QNDEF_QN25,
        QN40=QNDEF_QN40,            
        QN60=QNDEF_QN60,
        QN100=QNDEF_QN100,            
        QN160=QNDEF_QN160,
        QN250=QNDEF_QN250,            
        QN400=QNDEF_QN400,
        QN600=QNDEF_QN600,            
        QN_reverse=QNDEF_QN_reverse,
    }emQN_t;
	extern const emQN_t qnTable[];
    //Flow resolution
    typedef enum __emQS
    {
        QS0_001m3=1000,
        QS0_01m3=100,
        QS0_1m3=10,
        QS1_m3=1,
        QS1_reverse=0xffff,
    }emQS_t;
    //Maximum measurement range
    typedef enum __emMMR
    {
        MMR10K=4,      //9999.999
        MMR100K=5,      //99999.99
        MMR1000K=6,     //999999.9
        MMR10000K=7,    //9999999.
        MMRreverse=0xffff,
    }emMMR_t;
	
    typedef enum __emDLCS
    {
		//add for 17a,以前秦川的版本定义的东西
		DLC_STATUS_A='a',	//出厂前
		DLC_STATUS_B='b',	//出厂后	
		DLC_STATUS_C='c',	//开户后
		DLC_STATUS_D='d',	//超时
		DLC_STATUS_E='e',	//锁定
		DLC_STATUS_F='f',	//销户或者返厂	
    }emDLCS_t;
	
    //device work mode
    typedef enum __emDWM
    {
        DWM_COMMON_MODE=0x01,
        DWM_VOLUME_MODE=0x02,   //
        DWM_STEP_MONEY_MODE=0x03,    //
        //DWM_ORTHER_MODE,       
    }emDWM_t;
    //device disp mode 

    typedef enum __emCOMM_PRERIOD
    {
        RDPER_RT=0,
        RDPER_1S=1,
        RDPER_1MIN=2,
        RDPER_30MIN=3,
        RDPER_1H=4,
        RDPER_2H=5,
        RDPER_4H=6,
        RDPER_8H=7,
        RDPER_12H=8,
        RDPER_1D=9,
        RDPER_1W=10,
        RDPER_1MON=11,
        RDPER_2MON=12,
		RDPER_4MON=13,
		RDPER_6MON=14,
        RDPER_reverse=0xffff,
    }emCOMMPERIOD_t;

    //Price resolution
    typedef enum __emPS
    {
        PS0_0001=10000,
		PS0_001=1000,          //0.001RMB or 
        PS0_01=100,          //0.01RMB
        PS0_1=10,          //0.1RMB
        PS1=1,           //1RMB
        //PS10=10000,
        PS_reverse=0xffff,
    }emPS_t;

	typedef enum{
		OFF_REASON_NONE,
		OFF_REASON_HARD_DEFAULT,
		
		OFF_REASON_LO_VOLETAGE,
		OFF_REASON_HI_VOLETAGE,
		
		OFF_REASON_NO_GAS_FLOW,
		OFF_REASON_SAMLL_GAS_FLOW,
		OFF_REASON_HIGHT_GAS_FLOW,
		OFF_REASON_CONTINUOUS_FLOW,

		OFF_REASON_LOW_OV,
		OFF_REASON_NO_OV,
		OFF_REASON_LIMIT_OV,
		
		
		OFF_REASON_STRONG_MAGTIC,
		
		OFF_REASON_EXTERN_SIG,
		
		OFF_REASON_SEVERS,
		OFF_REASON_MANUAL,
		
		//OFF_REASON_DEFAULT=0xffff,
		OFF_REASON_LEAKAGE,
		OFF_REASON_WARN,
		OFF_REASON_FORCE,
		//for czsn
		OFF_REASON_CZSN_NONET,
		OFF_REASON_CZSN_WARNING,
		//for szrq
		OFF_REASON_TEMP,
		OFF_REASON_SHELL_OPEN,
	}emVavleOffReason_t;
	
	//even set
	
	
	#define POWER_STATUS_NORMAL 		0	
	#define POWER_STATUS_LOW 			1
	#define	POWER_STATUS_DOWN 			2
	
	#define POWER_STATUS_OVER 			3
	
	#define CNM_BALANCE_NORMAL 			0
	#define CNM_BALANCE_WARNING			1
	#define CNM_BALANCE_WARNING_OFF		2
	#define CNM_BALANCE_OFF				3
	
    typedef union __emEVEN
    {
        uint32_t t32;
        struct{		
			uint32_t bPwrStatus:2;
			uint32_t bPwrStatusEx:2;

			
            uint32_t bStrongMagnetic:1;       	//Strong magnetic field
            uint32_t bStrongElectric:1;       	//strong electric field
			uint32_t bLeakage:1;             	//gas leak 
			uint32_t bAlarmConn:1;
			
			uint32_t bShellOpenB:1;				//基础部分开盖
			uint32_t bNoNetWork:1;
			uint32_t bQcFlowHi:1;
			uint32_t bQcFlowTmout:1;
			
            uint32_t bStdFlowWarnHi:1;           
			uint32_t bStdFlowWarnSubHi:1;
            uint32_t bStdFlowWarnSubLo:1;            
			uint32_t bStdFlowWarnLo:1;

			uint32_t bPressureHi:1;
			uint32_t bPressureLo:1;
			uint32_t bTempHi:1;
			uint32_t bTempLo:1;			
			
			uint32_t bRealFlowHi:1;
			uint32_t bValveStatus:3;
			
			uint32_t bPoseErr:1;
			uint32_t bMove:1;
			uint32_t bBalanceSta:2;
			
			uint32_t bWarnOffTime:1;
			uint32_t bTempOffTime:1;
        }bits;
    }stDevStatus_t;	

	typedef union
	{
		uint32_t t32;
		struct {
            uint32_t bNoflow:1;
			uint32_t bStrongMagnetic:3;
			uint32_t bSeverOff:1;
			uint32_t bNoNetwork:1;
			uint32_t bBalance:1;
			uint32_t bShellOpen:1;
		}bits;
	}stlockReasonBits_t;	

	typedef union{
		uint32_t t32;
		struct{
			uint8_t dayInWeek;	//dont,t used
			uint8_t dayInMonth;
			uint8_t month;
			uint8_t year;	
		}stru;
	}st_normalDate,normalDate_t;

	typedef union{
		uint32_t time;
		struct{
			uint8_t reverse;
			uint8_t second;
			uint8_t minute;
			uint8_t hour;
		}stru;
	}st_normalTime,normalTime_t;
	

    #define SETP_PRICE_NUM 6 
	//#define AES_KEY_LEN 16 		
	#define PAY_RECORD_NUM 6
	#define VM_RECORD_NUM 6


	typedef struct{
		uint8_t 	sampMin;
		uint8_t		sampHour;
		uint16_t	intervalMin;
		uint16_t	repeat;
	}sampleSet_t,sendSet_t,sampSendParam_t;
	
	typedef struct{
		uint8_t 	ip[6];
		uint16_t	port;
	}severInf_t;
	//extern severInf_t szrqDefaultSever;
	//<--for szrq
	typedef union{
		uint32_t t32;
		struct{
			uint32_t bPressureHi:1;
			uint32_t bPressureLo:1;
			uint32_t bTempHi:1;
			uint32_t bTempLo:1;
			uint32_t bShellOpenC:1;//通讯部分开盖
			uint32_t bShellOpenB:1;//基础部分开盖
			uint32_t bSte:1;
			uint32_t bLeakage:1;
			uint32_t bRealFlowHi:1;
			uint32_t bACPwrDown:1;
			uint32_t bBattary0Lo:1;
			uint32_t bBattary1Lo:1;
			uint32_t bFaultOff:1;
			uint32_t bMeasureErr:1;
			uint32_t bBalanceLow:1;
			uint32_t bTemporaryOff:1;
			uint32_t bForceOff:1;
		}bits;
	}__szrqWarningDef_t;

	//-->
	typedef struct __DEVICE_INFORMATION_STR
	{
		uint16_t 	HWVer;
		uint16_t	SWVer;
		uint16_t	ProtocolVer;
		uint16_t	rverse01;
		uint8_t		PD[4];
		uint8_t		commSN[20];				
		uint8_t		DeviceName[20];			//设备名称或者型号	

		//配置信息，与设备本身参数有关
		//针对煤气表，qmax，qmin，qst(始动流量)等具有一一对应关系，见GB/T 6968
		uint16_t	vendorCode;
		uint8_t		deviceType;				//燃气表，水表其他
		uint8_t		deViceClass;			//设备的
		
		
		uint16_t	QN;						//标称流量，
		uint16_t	QMIN;
		uint16_t	QMAX;					//最大流量，
		uint16_t	QS;						//脉冲当量，
		
		uint8_t		VavleOnTime;			//开阀门需要的时间,s·			
		uint8_t		VavleOffTime;			//关阀门需要的时间,s·	
		uint16_t	PS;

		int32_t		initVolume;				//初始读数
		
		uint8_t		HightFlowProtectEn;
		//mode configure descriptor,工作模式配置信息
		uint8_t		DWM;					//工作模式，普通抄表模式，气量模式等
		uint8_t		t8_reverse00;
		uint8_t		qcNoTryTimes;
		
		uint16_t	qcNoFlowTimeOut;		//长时间不用气设置，
		uint16_t	qcNoFlowTimeOutEx;		//预留，对齐

		int32_t		maxOverageVM;			//最大屯气量
		
		int32_t		warnSetOverageVM;
		int32_t		warnOffSetOverageVM;
		int32_t		offSetOverageVM;
        int32_t		limitOffsetVM;//int32_t     xreverse0x[4];
		
		uint8_t 	superPayNums;				//超级充值次数
		uint8_t		resetPayNums;				//复位充值次数
		
		uint32_t	ID;							//设备id
        uint32_t	userID;						//用户id
		uint16_t	companyCode;		
		uint16_t	reverse02;
		
		severInf_t	severTest;
		severInf_t	severRelease;
		severInf_t	severUser;
		uint8_t		severIndex;
		uint8_t		reverse00;
		uint16_t	unNetTimeOut;
		
		uint8_t		DLCS;					//设备状态生命周期,可兼容S=ABCDE
		uint8_t		keyIndex;				//秘钥等级，密匙索引
		uint8_t		reverse081600;//uint8_t		priceAdjTimes;
		uint8_t		reverse081601;//uint8_t		curPriceStep;        

		uint8_t		mmKey[16];
		uint8_t		userKey[16];
		
		//sampSendParam_t	sampleParam[4];
		//sampSendParam_t	sendParam[4];	
		
		//<--for 团标阶梯气价
		uint32_t	stepSluStartTm;
		uint32_t	stepSluEndTm;
		uint16_t	stepSluSn;
		uint8_t		stepCount;
		uint8_t		stepSluCtrlByte;
		uint16_t	stepPeriod;
		uint16_t	stepReverse;
		int32_t		stepVolume[6];
		int32_t		stepPrice[6];
		
		uint32_t	preStepSluStartTm;
		uint32_t	preStepSluEndTm;
		uint16_t	preStepSluSn;
		uint8_t		preStepCount;
		uint8_t		preStepSluCtrlByte;
		uint16_t	preStepPeriod;
		uint16_t	preStepReverse;
		int32_t		preStepVolume[6];
		int32_t		preStepPrice[6];		
		//-->
		
		uint32_t	curPrice;
		uint32_t	periodVolume;

		int32_t		OVerageVM;						//余量
		int32_t		OVerageVMRemainder;	

		int32_t		totalVolume;					//总量		
		int32_t		totalPayMoney;					//总金额	

		uint8_t		curPriceStep;
		uint8_t		offReason;//uint8_t     xreverse0;	
		uint16_t	xreverse0;
		
		stDevStatus_t	devStatus;					//设备状态
		stlockReasonBits_t	lockReason;				//锁定记录

		//<--for szrq
		int32_t		szrqBalance;
		int32_t		szrqBalanceVol;
		int32_t		szrqSeverVolume;
		int32_t		szrqSeverSetpStartVol;
		uint8_t		szrqSeverSettleTime[6];
		uint8_t		szrqSeverSetpStartDt[6];
		
		uint8_t		szrqRoportPeriodType;			//上报周期
		uint8_t		szrqRoportDataInterval;			//数据间隔
		uint8_t		szrqtPeriodValue;				//周期值
		uint8_t		szrqValveCtrlByte;
		
		uint8_t		szrqtPeriodTime[2];				//上报时间
		uint8_t		szrqtFreezeDay;					//冻结日
		uint8_t		szrqtFreezeHour;				//冻结时
		//-->
		uint16_t	crc;
	}stDeviceInf_t;	
	
	extern uint16_t noNetworkTimer;
	typedef enum
	{
		VALVE_UNKNOW=0,
		VALVE_OPERATION_OFF,
		VALVE_OFF,
		VALVE_OPERATION_ON,
		VALVE_ON,
		VALVE_LOC,
		VALVE_FAULT,
	}em_vavleState,vavleState_t;
	
	extern vavleState_t vavleState;
	/*
	typedef enum
	{
		VALVE_OPERATION_NONE=0,
		VALVE_OPERATION_OFF=1,
		VALVE_OPERATION_ON=2,
	}em_vavleOperation,vavleOperation_t;
	*/
	
	typedef enum __OVERAGE_VM_STATE
    {
		OV_NORMAL=0x00,
        OV_WARNNING=0x01,
		OV_WARNNING_OFF=0x02,
		OV_OFF=0x03,
    }em_overageVMState,overageVMState_t;
	
	typedef union 
	{
		uint32_t t32;
		struct
		{
			uint32_t ClockErr:1;
			uint32_t FlashErr:1;
			uint32_t EepromErr:1;
			uint32_t RtcErr:1;
			uint32_t ProxErr:1;
			uint32_t:1;		
		}bits;
	}st_hardDefault,hardDefault_t;
	
	extern hardDefault_t  hardDefault;
	extern int8_t rssi;
	//extern uint8_t opStatus;
	//add by a.la.l. 2017.10.18
	extern volatile int32_t totalVolume;
	//extern volatile int32_t totalMoney;
	extern volatile int32_t curMonthVolume;
	extern volatile int32_t curPeriodVolume;
	//extern volatile int32_t curMoney;
	

	extern volatile int32_t rtPrice;
	extern volatile int32_t overageMoney;
	extern volatile int32_t overrageVolume;	
	
	extern volatile int32_t rtVolume;
	extern volatile int32_t	__rtVolume;
	extern volatile bool rtVolumeRefreshFlag;
	
	extern const stDeviceInf_t sysDataDefault;
	extern stDeviceInf_t sysData;
	//extern stDeviceInf_t* stpSysData;
	
	extern overageVMState_t ovmStatus;
	//extern overageVMState_t omStatus;
	
	//extern st_eventBits DeviceEvent;
	//#define VavleOffReason	(DeviceEvent.bits.bOffReason)
	//extern stDevStatus_t lastDevStatus,nowDevStatus;

	
	#define VavleOffReason sysData.offReason
	//extern stlockReasonBits_t lockReason;
	
	extern volatile uint32_t popTimesEveryday;
	extern uint8_t commonBuf[512+64];
	
	extern uint16_t qc_data_realtime_data_clear(void);
	extern uint16_t qc_data_device_even_lock_clear(void);
	
	//extern uint16_t qc_data_save_sys_data(bool reCalc);
	//extern void data_api_calc_all_ex(void);
	//extern void data_api_calc_all(void);
	
	//extern void data_api_day_change(void);
	extern uint32_t cal_second_in_day(void);
	//
	extern volatile int32_t realPayValue;
	
	//v104
	extern void qc_data_clear_sysdata_to_dcls_b(void);
	extern void qc_data_clear_sysdata_to_dcls_a(void);
	//add 20171221
	extern volatile int32_t OVerageVMRemainder;
	//红外改写id数据结构定义
	#define IR_HEAD_CODE	0x55
	#define IR_END_CODE		0xaa
	#define IR_CMD_SET_ID	0x01
	
	typedef struct{
		uint8_t headCode[2];
		uint8_t len;
		uint8_t	cmd;
		uint8_t	id[4];
		uint8_t	qn;
		uint8_t	sum;
		uint8_t	endCode;
	}irCmdDef_t;
	/*
	11:55 55 08 01 0b 00 00 00 01 15 aa
	01:55 55 08 01 01 00 00 00 01 0b aa	
	*/
	extern void data_api_clear_calc_period_volmue(void);
	extern void data_api_clear_calc_period_volmue_ex(void);
	extern bool flgValveErrSend;
	
	

#ifdef __cplusplus
	}
#endif
	
#endif
//file end
