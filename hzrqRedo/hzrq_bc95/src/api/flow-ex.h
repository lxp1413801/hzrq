#ifndef __flow_ex_h__
#define __flow_ex_h__
#ifdef __cplusplus
	extern "C"{
#endif
	#include <stdint.h>
	//#include <bool.h>
	#define TC_TABLE_EX_NUM (8)
	typedef struct
	{
		emQN_t qn[TC_TABLE_EX_NUM];
		emQN_t qmax[TC_TABLE_EX_NUM];
		uint16_t qStartWork[TC_TABLE_EX_NUM];
		uint16_t tcQrPlusNumPer120s[TC_TABLE_EX_NUM];
	}tcTableEx_t;	
	
	extern const tcTableEx_t tcTableEx;
	
	//脉冲时刻表,记录每个脉冲发生的时刻
	#define FLOW_PULSE_TABLE_NUM	32
	extern uint32_t flowPulseTimerTable[FLOW_PULSE_TABLE_NUM];
	extern bool flwPTTRefrashFlg;
	#define FLOW_RECAORD_MAX_TIME	3600
	
	/*
	无脉冲记录时间，超过FLOW_RECAORD_MAX_TIME没有脉冲，需要在时刻表里面标记
	用于更新实时流量
	*/
	
	extern	uint32_t noFlowPuleRecordTimer;		//无脉冲记录时间
	extern	uint32_t totalNoPulseTimer;			//无脉冲时间，用于不计量关阀
	
	extern  int16_t dayAvgFlow[32];			//日均流量
	extern 	int16_t hourAvgFlow[24];			//小时均量
	#define FLOW_1_PER_6_LEN	7
	#define FLOW_6_PER_60_LEN	11
	extern 	int32_t _1per6_flow[FLOW_1_PER_6_LEN];
	extern  int32_t _6per60_flow[FLOW_6_PER_60_LEN];
	extern  int16_t lastDayMaxFlow;			//上一日最大量
	
	extern void m_flow_pulse_timer_table_push(uint32_t tick);
	extern void m_flow_all_data_init(void);
	extern void m_flow_calc_second(void);
	//extern void m_flow_process_ex(void);
	extern void m_flow_process_ex(void);
#ifdef __cplusplus
	}
#endif
#endif
//file end
