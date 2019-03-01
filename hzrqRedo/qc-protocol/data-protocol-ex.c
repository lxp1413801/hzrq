#include "./includes/includes.h"
#include "data-protocol-ex.h"
#include "szrq-comm-protocol.h"
//__rtVolume:采集到的脉冲数
//rtVolume:计算用到的脉冲数
void __hal_cal_get_flow_pulse(void)
{
	__disable_irq();
	rtVolume=__rtVolume;
	__enable_irq();		
}

void __hal_calc_total_volume(void)
{
	uint32_t t32;

	t32=rtVolume*QS0_01m3/sysData.QS;
	totalVolume=sysData.totalVolume+t32;
}

void __hal_cal_current_volume(void)
{
	if(totalVolume<sysData.periodVolume)sysData.periodVolume=totalVolume;
	curPeriodVolume=totalVolume-sysData.periodVolume;	
}


bool __hal_cal_fi_holiday(void)
{
	return false;
}

void __hal_cal_overage_v(void)
{
	overrageVolume=sysData.OVerageVM-rtVolume*QS0_01m3/sysData.QS;;
}

int32_t __hal_cal_price(bool* fichanged,int32_t* retPrice,uint8_t* step)
{
    int32_t t32=0L;
	uint16_t i;
	if(fichanged)*fichanged=false;
	if(sysData.DWM==DWM_STEP_MONEY_MODE ){
		for(i=0;i<(SETP_PRICE_NUM-1);i++){
			if(curPeriodVolume<sysData.stepVolume[i])break;
			else
				continue;
		}
		t32=sysData.stepPrice[i];
	}else{
		t32=sysData.curPrice;
	}
	
	if(sysData.curPrice!=t32){
		if(fichanged)*fichanged=true;
		if(step)*step=(uint8_t)i;
	}
	if(retPrice)*retPrice=t32;
	return t32;	
}

void __hal_cal_overage_m(void)
{	
    int32_t t32,rtVM;
	
    t32=rtPrice;
    t32*=(rtVolume*QS0_01m3/sysData.QS);
	int32_t x=10000UL;
	rtVM=t32/x;
	OVerageVMRemainder=t32%x;
	overageMoney=sysData.OVerageVM-rtVM;
}

void __hal_cal_overage_v_for_m(void)
{
	float f;
	f=(float)overageMoney;
	
	int32_t x=10000L;
	f=f*(float)x/(float)rtPrice;

	overrageVolume=(int32_t)f;	
}

overageVMState_t __hal_cal_warning_off(void)
{
	overageVMState_t ov=OV_NORMAL;	
	//int32_t warningOff,warning,off,limit;
	int32_t t32;
	if((sysData.DWM>DWM_COMMON_MODE) || (sysData.DWM==DWM_COMMON_MODE && sysData.DLCS<=DLC_STATUS_B)){
		t32=overrageVolume;
	}else{
		t32=sysData.szrqBalance;
	}

	if(__hal_cal_fi_holiday()){
		if(t32<=sysData.limitOffsetVM){
			ov=OV_OFF;
		}
		return ov;
	}
	//修正几个告警的范围
	if(sysData.warnSetOverageVM<sysData.offSetOverageVM){
		sysData.warnSetOverageVM=default_WARNING_VOLUMING;
		sysData.offSetOverageVM=0L;
	}
	if(sysData.warnOffSetOverageVM<sysData.offSetOverageVM || sysData.warnOffSetOverageVM>sysData.warnSetOverageVM){
		sysData.warnOffSetOverageVM=(sysData.offSetOverageVM + sysData.warnSetOverageVM)/2;
	}
	if(t32<=sysData.offSetOverageVM){
		ov=OV_OFF;
	}else if(t32<=sysData.warnOffSetOverageVM){
		ov=OV_WARNNING_OFF;
	}else if(t32<=sysData.warnSetOverageVM){
		ov=OV_WARNNING;
	}

	return ov;	
}

bool fi_szrq_balance_temp_off_vavle_close_en(void)
{
	//在报警和关阀值中间的提示关阀
	bool ret=false;
	__szrq_valveCtrlByte_t szrqVctrl;
	szrqVctrl.t8=sysData.szrqValveCtrlByte ;
	if(szrqVctrl.bits.bBalanceTempOffEn){
		ret=true;
	}
	if(sysData.DLCS<=DLC_STATUS_B){
		ret=true;
	}
	return ret;	
}

bool fi_szrq_balance_off_vavle_close_en(void)
{
	//低于关阀值的关阀
	bool ret=false;
	__szrq_valveCtrlByte_t szrqVctrl;
	szrqVctrl.t8=sysData.szrqValveCtrlByte ;
	if(szrqVctrl.bits.bBalanceOffEn){
		ret=true;
	}
	if(sysData.DLCS<=DLC_STATUS_B){
		ret=true;
	}
	return ret;		
}

bool fi_szrq_balance_warn_off_vavle_close_en(void)
{
	//低于报警值时的关阀
	bool ret=false;
	__szrq_valveCtrlByte_t szrqVctrl;
	szrqVctrl.t8=sysData.szrqValveCtrlByte ;
	if(szrqVctrl.bits.bBalanceWarnOffEn){
		ret=true;
	}
	if(sysData.DLCS<=DLC_STATUS_B){
		ret=false;
	}
	return ret;		
}
bool __hal_fi_off(overageVMState_t ov)
{
	bool ret=false;
	//nowOvSta=ov;
	if(ov==OV_OFF){
		sysData.devStatus.bits.bBalanceSta=CNM_BALANCE_OFF;
		sysData.lockReason.bits.bBalance=1;
		//ret=true;
		ret=fi_szrq_balance_off_vavle_close_en();
	}else if(ov==OV_WARNNING_OFF){
		sysData.lockReason.bits.bBalance=0;
		//sysData.lockReason.bits.bBalance=0;
		if(sysData.devStatus.bits.bTempOffTime==0){
			sysData.devStatus.bits.bBalanceSta=OV_WARNNING_OFF;
			sysData.devStatus.bits.bTempOffTime=1;
			//ret=true;
			ret=fi_szrq_balance_temp_off_vavle_close_en();
		}
		
	}else if(  ov==OV_WARNNING){
		sysData.lockReason.bits.bBalance=0;
		sysData.devStatus.bits.bBalanceSta=CNM_BALANCE_WARNING;
		sysData.devStatus.bits.bTempOffTime=0;
		if(sysData.devStatus.bits.bWarnOffTime==0){
			sysData.devStatus.bits.bWarnOffTime=1;
			ret=fi_szrq_balance_warn_off_vavle_close_en();
		}
		
	}else{
		sysData.lockReason.bits.bBalance=0;
		sysData.devStatus.bits.bBalanceSta=CNM_BALANCE_NORMAL;
		sysData.devStatus.bits.bWarnOffTime=0;
	}

	if(ov!=OV_OFF || !ret){
		sysData.lockReason.bits.bBalance=0;
	}
	//后台结算以及抄表模式下不不关阀
//	if(sysData.DWM==DWM_COMMON_MODE && sysData.DLCS>DLC_STATUS_B){
//		ret=false;
//	}
	return ret;
}

void __hal_fi_off_redo(overageVMState_t ov)
{

	bool ret=false;
	if(ov==OV_OFF){
		sysData.devStatus.bits.bBalanceSta=CNM_BALANCE_OFF;
		sysData.lockReason.bits.bBalance=1;
		ret=fi_szrq_balance_off_vavle_close_en();
	}else if(ov==OV_WARNNING_OFF){
		sysData.lockReason.bits.bBalance=0;
		if(sysData.devStatus.bits.bTempOffTime==0){
			sysData.devStatus.bits.bBalanceSta=OV_WARNNING_OFF;
			//ret=fi_szrq_balance_temp_off_vavle_close_en();
		}
		
		
	}else if(  ov==OV_WARNNING){
		sysData.lockReason.bits.bBalance=0;
		sysData.devStatus.bits.bBalanceSta=CNM_BALANCE_WARNING;
		sysData.devStatus.bits.bTempOffTime=0;
		if(sysData.devStatus.bits.bWarnOffTime==0){
			//sysData.devStatus.bits.bWarnOffTime=1;
			//ret=fi_szrq_balance_warn_off_vavle_close_en();
		}
		
	}else{
		sysData.lockReason.bits.bBalance=0;
		sysData.devStatus.bits.bBalanceSta=CNM_BALANCE_NORMAL;
		sysData.devStatus.bits.bWarnOffTime=0;
	}	
	if(ov!=OV_OFF || !ret){
		sysData.lockReason.bits.bBalance=0;
	}
}


int16_t __hal_sysdata_save(void)
{
	int16_t ret;
	int32_t t32,remainder;
	
	if(!voltage_vdd_status_nomal())return 0;
	//api_calc_all_redo();
	osMutexWait(osMutexSysData,osWaitForever);
	__hal_calc_all_redo();
	sysData.totalVolume=totalVolume;
    if(sysData.DWM==DWM_STEP_MONEY_MODE){
		remainder= OVerageVMRemainder+sysData.OVerageVMRemainder;
		t32 =  remainder / 10000;
		sysData.OVerageVM=overageMoney-t32;
		sysData.OVerageVMRemainder = remainder % 10000;
    }else{
		sysData.OVerageVM=overrageVolume;        
    }	

	//save even flag+
	//sysData.devStatus.t32=nowDevStatus.t32;
	//save lock flag
	//sysData.lockReason.t32=lockReason.t32;
    //save dlcs
	crc_append((uint8_t*)(&sysData),sizeof(stDeviceInf_t)-2);
	m_mem_cpy_len(commonBuf,(uint8_t*)(&sysData),sizeof(stDeviceInf_t));
	
	qc_data_write_to_media(SYS_DATA_MAIN_ADDR,(uint8_t*)(&sysData),sizeof(stDeviceInf_t));
	qc_data_read_from_media((uint8_t*)(&sysData),SYS_DATA_MAIN_ADDR,sizeof(stDeviceInf_t));
	ret=crc_verify((uint8_t*)(&sysData),sizeof(stDeviceInf_t));
	if(!ret){
		m_mem_cpy_len((uint8_t*)(&sysData),commonBuf,sizeof(stDeviceInf_t));
	}else{
		__disable_irq();
		__rtVolume-=rtVolume;
		__enable_irq();
	}	
	osMutexRelease(osMutexSysData);
	return ret;	
}

void __hal_calc_all_redo(void)
{
	//osMutexWait(osMutexSysData,osWaitForever);
	__hal_cal_get_flow_pulse();
	__hal_calc_total_volume();
	if(sysData.DWM==DWM_VOLUME_MODE){
		__hal_cal_overage_v();
	}else if(sysData.DWM==DWM_COMMON_MODE){
		if(sysData.DLCS<=DLC_STATUS_B){
			__hal_cal_overage_v();
		}
	}else{
		__hal_cal_overage_m();
		__hal_cal_overage_v_for_m();
	}
	ovmStatus=__hal_cal_warning_off();
	//__hal_fi_off(ovmStatus);
	__hal_fi_off_redo(ovmStatus);
	//osMutexRelease(osMutexSysData);
}

void api_calc_all_redo(void){
	osMutexWait(osMutexSysData,osWaitForever);
	__hal_calc_all_redo();
	osMutexRelease(osMutexSysData);
}
extern uint32_t szrqRoportNextTm;
void api_calc_all(void)
{
	int32_t nextPrice=0L;
	uint8_t step=0;
	bool change=false;
	
	osMutexWait(osMutexSysData,osWaitForever);
	__hal_cal_get_flow_pulse();
	__hal_calc_total_volume();
	if(sysData.DWM==DWM_VOLUME_MODE){
		__hal_cal_overage_v();
	}else if(sysData.DWM==DWM_COMMON_MODE){
		if(sysData.DLCS<=DLC_STATUS_B){
			__hal_cal_overage_v();
		}
	}else{
		//cal price;
		__hal_cal_price(&change,&nextPrice,&step);
		if(change){
			sysData.curPrice=nextPrice;
			sysData.curPriceStep=step;	
			//必须释放osMutexSysData
			osMutexRelease(osMutexSysData);
			api_sysdata_save();
			//send adjust price info
			rtPrice=nextPrice;
			osMutexWait(osMutexSysData,osWaitForever);
		}
		
		__hal_cal_overage_m();
		__hal_cal_overage_v_for_m();
	}
	ovmStatus=__hal_cal_warning_off();
	bool fiOff=__hal_fi_off(ovmStatus);
	osMutexRelease(osMutexSysData);
	if(fiOff){
		if(ovmStatus==OV_WARNNING_OFF){
			vavle_off_from_app(OFF_REASON_LOW_OV);
		}else{
			vavle_off_from_app(OFF_REASON_NO_OV);
		}
	}
	
	//<<--更改上报频次
	/*
	if(sysData.szrqBalanceVol<=2000){
		if(sysData.szrqRoportPeriodType!=6 || sysData.szrqRoportDataInterval!=4){
			sysData.szrqRoportPeriodType=6;
			sysData.szrqRoportDataInterval=4;			
			szrqRoportNextTm=0;
			szrqcollectNextTm=0;
		}
	}
	*/
	//-->
	
}
osMessageQDef(reqMsgQ, 1,sizeof(TaskHandle_t));
osMessageQId reqMsgQ;

osMessageQDef(respMsgQ, 1,sizeof(int32_t));
osMessageQId respMsgQ;

int16_t api_sysdata_save(void)
{
	int16_t ret=0;
	osEvent event;
	TaskHandle_t threadID;
	threadID=osThreadGetId();
	if(!reqMsgQ)return ret;
	osMessagePut(reqMsgQ,(uint32_t)threadID,osWaitForever);
	
	if(!respMsgQ)return ret;
	event = osMessageGet(respMsgQ, 5*configTICK_RATE_HZ);
	if(event.status==osEventMessage){
		ret=(int16_t)(event.value.v);
	}
	return ret;
}

void vThreadDataSave(void * pvParameters)
{
	osEvent event;
	//uint16_t len;
	int16_t ret;
	//(void)pvParameters;	
	while(1){
		event=osMessageGet(reqMsgQ,osWaitForever);
		if(event.status==osEventMessage){
			ret=__hal_sysdata_save();
			osMessagePut(respMsgQ,(uint32_t)ret,osWaitForever);
		}
	}
}

TaskHandle_t idThreadDataSave;

void m_thread_create_sysdata(void)
{
	reqMsgQ=osMessageCreate(osMessageQ(reqMsgQ),NULL);
	respMsgQ=osMessageCreate(osMessageQ(respMsgQ),NULL);
	osThreadDef(ThreadDataSave, vThreadDataSave, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
	idThreadDataSave=osThreadCreate(osThread(ThreadDataSave), NULL);		
}

