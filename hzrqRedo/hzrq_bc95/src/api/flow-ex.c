#include "./includes/includes.h"

const tcTableEx_t tcTableEx={
	{QN16,	QN25,	QN40,	QN60,	QN100,	QN160,	QN250,	QN400	},
	{QN25,	QN40,	QN60,	QN100,	QN160,	QN250,	QN400,	QN600	},
	{3,		5,		5,		8,		13,		13,		20,		32		},
	{10,	16,		24,		40,		6,		10,		16,		24		},
};

uint32_t flowPulseTimerTable[FLOW_PULSE_TABLE_NUM];

bool flwPTTRefrashFlg=false;
uint32_t noFlowPuleRecordTimer=0x00UL;	
uint32_t totalNoPulseTimer=0x00UL;	

int32_t _1per6_flow[FLOW_1_PER_6_LEN]={0};
int32_t _6per60_flow[FLOW_6_PER_60_LEN]={0};

volatile uint32_t flowQn=160;	//dm3
volatile uint32_t maxFlow=0x400;

int16_t dayAvgFlow[32];						//日均流量
int16_t hourAvgFlow[24];					//小时均量
int16_t lastDayMaxFlow=0x00;				//上一日最大量

sysDataTime_t flowAgoDt,flowNowDt;
float rtFlow=0.0f;
/*
uint16_t m_flow_get_table_index(void)
{
	uint16_t i;
	uint16_t qn=sysData.QN;
	for(i=0;i<TC_TABLE_EX_NUM;i++){
		if(qn==tcTableEx.qn[i])break;
	}
	if(i>=TC_TABLE_EX_NUM)i=TC_TABLE_EX_NUM-1;
	return i;
}
*/
void m_flow_pulse_timer_table_init(void)
{
	uint16_t i;
	for(i=0;i<FLOW_PULSE_TABLE_NUM;i++){
		flowPulseTimerTable[i]= ~0x00UL;
	}
}

void m_flow_no_pulse_timer_init(void)
{
	noFlowPuleRecordTimer=0;
	totalNoPulseTimer=0;
}



void m_flow_record_init(void)
{
	uint16_t i;
	for(i=0;i<sizeof(dayAvgFlow)/sizeof(dayAvgFlow[0]);i++){
		dayAvgFlow[i]=0x00UL;
	}
	for(i=0;i<sizeof(hourAvgFlow)/sizeof(hourAvgFlow[0]);i++){
		dayAvgFlow[i]=0x00UL;
	}

}

void m_flow_data_init(void)
{
	lastDayMaxFlow=0x00UL;
	switch(sysData.QN){
		case QN16:	flowQn=160;		maxFlow=250*12/10;break;
		case QN25:	flowQn=250;		maxFlow=400*12/10;break;
		case QN40:	flowQn=400;		maxFlow=600*12/10;break;
		case QN60:	flowQn=600;		maxFlow=1000*12/10;break;
		case QN100:	flowQn=1000;	maxFlow=1600*12/10;break;
		case QN160:	flowQn=1600;	maxFlow=2500*12/10;break;
		case QN250:	flowQn=2500;	maxFlow=4000*12/10;break;
		case QN400:	flowQn=4000;	maxFlow=6000*12/10;break;
		default:	flowQn=250;		maxFlow=400*12/10;break;
	}
}

void m_flow_all_data_init(void)
{
	
	m_mem_cpy_len((uint8_t*)&flowAgoDt,(uint8_t*)&sysRtcDataTime,sizeof(sysDataTime_t));
	m_mem_cpy_len((uint8_t*)&flowNowDt,(uint8_t*)&sysRtcDataTime,sizeof(sysDataTime_t));
	m_flow_pulse_timer_table_init();
	//m_flow_record_init();
	m_flow_data_init();
	m_flow_no_pulse_timer_init();
}

void m_flow_pulse_timer_table_push(uint32_t tick)
{
	uint16_t i;
	if(vavleState!=VALVE_ON){noFlowPuleRecordTimer=0;return;}
	for(i=FLOW_PULSE_TABLE_NUM-1;i>0;i--){
		flowPulseTimerTable[i]=flowPulseTimerTable[i-1];
	}
	flowPulseTimerTable[0]=tick;
	flwPTTRefrashFlg=true;
	//m_flow_no_pulse_timer_init();
	noFlowPuleRecordTimer=0UL;
}

uint16_t m_flow_hight_flow_protect(uint32_t pulse)
{
	uint32_t t32;
	uint32_t flow;
	uint16_t i;
	
	if(vavleState!=VALVE_ON){return 0;}
	
	
	if(!(sysData.HightFlowProtectEn)){
		sysData.devStatus.bits.bQcFlowHi=0;
		return 0;
	}	
	
	if(pulse>20)pulse=20;

	for(i=0;i<pulse;i++){
		if(flowPulseTimerTable[i]==~0x00UL)return 0;
	}
	if(flowPulseTimerTable[0]<flowPulseTimerTable[pulse])return 0;
	t32=flowPulseTimerTable[0]-flowPulseTimerTable[pulse];
	flow=pulse*3600UL/t32;
	flow=flow*100UL/sysData.QS;
	
	
	//maxFlow=600*12/10;
	//if(flow>=720){
	if(flow>=maxFlow){
		sysData.devStatus.bits.bQcFlowHi=1;
		vavle_off_from_app(OFF_REASON_HIGHT_GAS_FLOW);
		//lockReason.bits.bHifhtFlow=1;		
		if(!overFlowFlgforSendWarning){
			overFlowFlgforSendWarning=true;
			osSignalSet( vTheadEvenID, flg_EVENT_HI_FLOW);
		}
	}else{
		sysData.devStatus.bits.bQcFlowHi=0;
	}
	return 1;
	
	//t32=
}

uint16_t m_flow_no_flow_protect(void)
{
	uint32_t tm,tmEx;
	uint8_t tmtry;

	if(vavleState!=VALVE_ON){totalNoPulseTimer=0x00L;return 0;}
	tm=(uint32_t)(sysData.qcNoFlowTimeOut);
#if NO_FLOW_PROTECT_EN_TEST
	tm=120;
#else
	tm*=3600UL;
#endif

	tmEx=(uint32_t)(sysData.qcNoFlowTimeOutEx);
#if NO_FLOW_PROTECT_EN_TEST
	tmEx=120;
#else
	tmEx*=3600UL;
#endif
	tmtry=sysData.qcNoTryTimes;
	if(tmtry==1)tmtry=2;	
	
    if(tm ==0 )return 1;
	if(sysData.DLCS<DLC_STATUS_C)return 1;

	if(sysData.lockReason.bits.bNoflow ==0 || tmEx==0){
		if(totalNoPulseTimer>=tm){
			totalNoPulseTimer=0;
			sysData.devStatus.bits.bQcFlowTmout=1;
			sysData.lockReason.bits.bNoflow=1;

            #if USED_STATIC_D_E
            if(sysData.DLCS==DLC_STATUS_C)sysData.DLCS=DLC_STATUS_D;
            #endif
			vavle_off_from_app(OFF_REASON_NO_GAS_FLOW);
		}
	}else {
		if(totalNoPulseTimer>=tmEx){
			totalNoPulseTimer=0;
			sysData.devStatus.bits.bQcFlowTmout=1;
			if(tmtry!=0){
				if(sysData.lockReason.bits.bNoflow<tmtry)sysData.lockReason.bits.bNoflow++;
			}
            #if USED_STATIC_D_E
			if(sysData.lockReason.bits.bNoflow>=tmtry && tmtry!=0){
				if(sysData.DLCS==DLC_STATUS_D)sysData.DLCS=DLC_STATUS_E;
			}
            #endif            
			vavle_off_from_app(OFF_REASON_NO_GAS_FLOW);
		}
	}
	return 1;	
}

uint32_t noPuleFunCalltimer=0UL;

void m_flow_calc_second(void)
{
	//uint32_t t32;
	noFlowPuleRecordTimer++;
	totalNoPulseTimer++;
	noPuleFunCalltimer++;
	
	if(noFlowPuleRecordTimer>=FLOW_RECAORD_MAX_TIME){
		//t32=get_sys_ticker_sec();
		m_flow_pulse_timer_table_push(~0x00UL);
	}
#if NO_FLOW_PROTECT_EN_TEST
	noPuleFunCalltimer=0;
	m_flow_no_flow_protect();
#else
	if(noPuleFunCalltimer>=60){
		noPuleFunCalltimer=0;
		m_flow_no_flow_protect();
	}
#endif
	if(flwPTTRefrashFlg){
		flwPTTRefrashFlg=0;
		m_flow_hight_flow_protect(20);
	}
}

uint16_t flowMinueProcessCallTime=0x00;
void m_flow_calc_mm(void)
{
	uint16_t i;
	int32_t t32;
	//1/6数据，
	for(i=FLOW_1_PER_6_LEN-1;i>0;i--){
		_1per6_flow[i]=_1per6_flow[i-1];
	}
	_1per6_flow[0]=totalVolume;
	flowMinueProcessCallTime++;
	
	//10/60数据
	if(flowMinueProcessCallTime>=6){
		flowMinueProcessCallTime=0;
		for(i=FLOW_6_PER_60_LEN-1;i>0;i--){
			_6per60_flow[i]=_6per60_flow[i-1];
		}
		
		t32=_1per6_flow[0]-_1per6_flow[FLOW_1_PER_6_LEN-1];
		if(t32<0)t32=0;

		rtFlow=(float)t32*60/(FLOW_1_PER_6_LEN-1);
		t32=(int32_t)rtFlow;
		_6per60_flow[0]=t32;
		
	}
}

void m_flow_calc_hour(void)
{
	uint16_t i;
	int32_t t32;
	for(i=sizeof(hourAvgFlow)/sizeof(hourAvgFlow[0])-1;i>0;i--){
		hourAvgFlow[i]=hourAvgFlow[i-1];
	}
	t32=0;
	for(i=0;i<FLOW_6_PER_60_LEN-1;i++){
		t32+=_6per60_flow[i];
	}
	t32/=(FLOW_6_PER_60_LEN-1);
	//
	//t32=t32*60/1000;
	hourAvgFlow[0]=(int16_t)t32;
}

void m_flow_calc_day(void)
{
	uint16_t i;
	int32_t t32,len,max=0;
	
	for(i=sizeof(dayAvgFlow)/sizeof(dayAvgFlow[0])-1;i>0;i--){
		dayAvgFlow[i]=dayAvgFlow[i-1];
	}
	
	t32=0;
	len=sizeof(hourAvgFlow)/sizeof(hourAvgFlow[0]);
	for(i=0;i<len;i++){
		t32+=hourAvgFlow[i];
		if(max<hourAvgFlow[i])max=hourAvgFlow[i];
	}
	t32/=len;
	
	dayAvgFlow[0]=t32;//此处单位为立方每小时
	lastDayMaxFlow=max;
}

uint32_t flowMinuteTimer=0UL;
uint32_t flowHoureTimer=0UL;
uint32_t flowDayTimer=0UL;
//秒事件发生，调用此函数
void m_flow_process_ex(void)
{
	flowMinuteTimer++;
	flowHoureTimer++;
	flowDayTimer++;
	
	m_flow_calc_second();
	if(flowMinuteTimer>=60){
		flowMinuteTimer=0;
		m_flow_calc_mm();
	}
	
	if(flowHoureTimer>3600){
		flowHoureTimer=0;
		m_flow_calc_hour();
	}
	
	if(flowDayTimer>3600UL*24UL){
		flowDayTimer=0;
		m_flow_calc_day();
	}
}
//file end
