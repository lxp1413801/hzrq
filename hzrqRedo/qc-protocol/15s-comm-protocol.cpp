//file name:15-comm-protocol.h
//#include "qc-protocol.h"
//#include "common.h"
#include "includes.h"

#define CRC_16_SIZE 2

#define MAX_PAY_CMD_COUNT 3
/*
#ifndef vavle_off_in_protocol
	#define vavle_off_in_protocol() __nop()
#endif

#ifndef vavle_on_in_protocol
	#define vavle_on_in_protocol() __nop()
#endif
*/
volatile uint8_t protocolCmd=0x00;
volatile uint8_t stepPriceQueryStep=0x00;
extern int8_t rssi;
em_15sOpRetCode opRetCode;
_15sSendStaMichine_t _15sSendStaMichine;
uint8_t rxFrameIndex=0x00;
uint8_t txFrameIndex=0x00;
uint8_t __15sKey[8];
uint8_t popSequNm=0x00;
//pop的第二包属于特殊情况
//volatile bool needPopSecondPackage=false;
volatile popPackageType_t popPackageType=POP_TYPE_FIRST;
const emQN_t qnTable[]={QN16,QN25,QN40,QN60,QN100,QN160,QN250,QN400,QN600};

uint8_t  __x_qn_convert(emQN_t qn)
{
	uint8_t i;
	for(i=0;i<sizeof(qnTable)/sizeof(emQN_t);i++){
		if(qn== qnTable[i])break;
	}
	return i;
}

void __x_load_int32_to_buf(uint8_t* buf,uint32_t t32,uint8_t num)
{
	if(num>sizeof(uint32_t))num=sizeof(uint32_t);
	m_mem_cpy_len(buf,(uint8_t*)(&t32),num);
}

uint32_t __x_get_int32_from_buf(uint8_t* buf,uint8_t num)
{
	uint32_t t32;
	if(num>sizeof(uint32_t))num=sizeof(uint32_t);
	m_mem_cpy_len((uint8_t*)(&t32),buf,num);
	return t32;
}

uint16_t __12e_crc_cal(uint8_t *s,uint8_t len)
{
    uint16_t  crctmp,i,j,flag;
    crctmp=0;
    for(i=0;i<len;i++) {
        crctmp^=(uint16_t)s[i]<<8;         
        for(j=0;j<8;j++)  {
          flag=crctmp & 0x8000;  
          crctmp<<=1;
          if (flag!=0)crctmp^=0x1021;  // 1 0000 0010 0001  // CRC16= X^16 + X^12 + X^5 + X^0
			//modbus used 0xa001
		}
    }
    return crctmp;
}

uint16_t __12e_crc_append(uint8_t *s,uint8_t len)
{
    uint16_t  crctmp;
	crctmp=__12e_crc_cal(s,len);
	s[len]=(uint8_t)(crctmp>>8);
	s[len+1]=(uint8_t)(crctmp&0x00ff);
    return len+2;
}
//verify
uint16_t __12e_crc_verify(uint8_t *s,uint8_t len)
{
	uint16_t t16,crctmp;
	crctmp=__12e_crc_cal(s,len-2);
	t16=s[len-2];
	t16<<=8;
	t16 |= s[len-1];
	if(t16==crctmp)return 1;
	return 0;
}



uint8_t __device_status_cross_map(void)
{
	st_devStatusBits dSta;
	dSta.b=0x00;
	//event
	if(vavleState==VALVE_ON){
		
		dSta.bits.bHightBatVolt=DeviceEvent.bits.bPower1OvWarning;
		dSta.bits.bHightFlow=DeviceEvent.bits.bHightFlow;
		dSta.bits.bNoFlowOverTime=DeviceEvent.bits.bNoFlowOverTime;		
	}else{
		dSta.bits.bValveOff=1;
		if(VavleOffReason==OFF_REASON_STRONG_MAGTIC || DeviceEvent.bits.bStrongMagnetic){
			dSta.bits.bStrongMagnetic=1;
		}else if(VavleOffReason==OFF_REASON_HI_VOLETAGE){
			dSta.bits.bHightBatVolt=1;
		}else if(VavleOffReason==OFF_REASON_NO_GAS_FLOW){
			dSta.bits.bNoFlowOverTime=1;
		}else if(VavleOffReason==OFF_REASON_HIGHT_GAS_FLOW){
			//DeviceEvent.bits.bHightFlow
			dSta.bits.bHightFlow=1;
		}
	}
	
	uint8_t tmtry=sysData.tmOutTryTimes;
	tmtry+=1;
	//if(tmtry==1)tmtry=2;
	
	if(lockReason.bits.bNoflow>=tmtry && tmtry>1)dSta.bits.bValveLock=1;		
	if(lockReason.bits.bStrongMagnetic>=MAX_STE_OFF_TIMES)dSta.bits.bValveLock=1;	
	if(lockReason.bits.bSeverOff)dSta.bits.bValveLock=1;
	if(lockReason.bits.bSmallFlow)dSta.bits.bValveLock=1;
	dSta.bits.bValveErr=DeviceEvent.bits.bValveErr;
	return (dSta.b);
}
// 0b00无锂电池或断开锂电池 0b01锂电池不足 0b1x锂电池电量正常 
uint8_t __battery_status_cross_map(void)
{
	st_batteryStatusBits bSta;
	bSta.b=0x00;
	#if(BATTARY_TYPE==BATTARY_TYPE_LI_SOCL)
	if(lockReason.data){
		//bSta.bits.bOpenLock=1;
	}

	if(pwrStatus < POWER_STATUS_NORMAL){
		bSta.bits.bBatLiSoCl2=0x00;
	}else if(pwrStatus > POWER_STATUS_NORMAL){
		bSta.bits.bBatLiSoCl2=0x01;
	}else{
		bSta.bits.bBatLiSoCl2=0x02;
	}
	#else

	if(lockReason.data){
		//bSta.bits.bOpenLock=1;
	}

	if(pwrStatus < POWER_STATUS_NORMAL){
		bSta.bits.bBatZnMn=0x00;
	}else if(pwrStatus > POWER_STATUS_NORMAL){
		bSta.bits.bBatZnMn=0x01;

	}else{
		bSta.bits.bBatZnMn=0x02;
	}	
	#endif
	
	if(DeviceEvent.bits.bAlarmConn){
		bSta.bits.bLeakAlarm=1;
	}else{
		bSta.bits.bLeakAlarm=0;
	}
	return (bSta.b);

}

uint8_t __rf_status_cross_map(void)
{
	st_rfStatusBits rfSta;
	rfSta.b=0x00;
	
	return (rfSta.b);
}

//未解何意randpass
uint8_t __randpass_scross_map(void)
{
	st_randPassBits rpSta;
	rpSta.b=0x00;
	if(DeviceEvent.bits.bConstUsedOverTime){
		rpSta.bits.bConstFlow=1;
	}
	if(DeviceEvent.bits.bHightFlow){
		//rpSta.bits.bLargeFlow=1;
	}
	if(DeviceEvent.bits.bSmallConstOverTime){
		rpSta.bits.bSmallFlow=1;
	}
	if(DeviceEvent.bits.bLeakage || lockReason.bits.bLeakage){
		rpSta.bits.bLeakage__=1;
	}
	return (rpSta.b);
}

uint16_t __qc_comm_load_header(uint8_t* buf,uint8_t cmd,uint8_t ploylen,uint8_t ctrlCode)
{
	uint16_t t16;
	st_frameHeaderDef* stp;
	stp=(st_frameHeaderDef*)buf;
	m_mem_set(buf,0,sizeof(st_frameHeaderDef));

	t16=sizeof(st_frameHeaderDef)+ploylen+CRC_16_SIZE-1;

	if(t16>255)return 0;
	stp->len=(uint8_t)t16;
	
	stp->ctrlByte.b=0x00;

	stp->ctrlByte.bits.ctrlCode=ctrlCode;
	//stpSysData->ID=0x3123f2;
	m_mem_cpy_len(stp->id,(uint8_t*)(&(stpSysData->ID)),sizeof(uint32_t));
	
	stp->ver=NB20E1_DEVICE_VER;
	stp->cmd=cmd;
	
	stp->serNum=0x00;
	
	stp->encryptType.bits.keyType=__15E_ENCRYPT_TYPE_USER_TRNAS_KEY;
	t16=ploylen-4;
	if(t16 & 0x07)t16=(t16 & ~0x07)+8;
	t16>>=3;
	stp->encryptType.bits.blockNum=(uint8_t)t16;

	//stp->CompanyCode
	m_mem_cpy_len(stp->CompanyCode,(uint8_t*)(&(sysData.CompanyCode)),sizeof(uint16_t));
	stp->keyIndex=0x00;
	if(cmd==__15E_CMD_ACK || ctrlCode==__15E_CB_CTRL_CODE_RET_CLEAR){
		//serNum
		stp->serNum = rxFrameIndex;
	}else{
		stp->serNum = txFrameIndex;
		txFrameIndex++;
	}
	return sizeof(st_frameHeaderDef);
}
/*
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

*/
uint16_t __qc_comm_load_ack_body(uint8_t* buf)
{
	int32_t t32;
	st_AckDef* stp;
	stp=(st_AckDef*)buf;
	m_mem_cpy_len(stp->id,(uint8_t*)(&(stpSysData->ID)),sizeof(uint32_t));
	stp->cmd=__15E_CMD_ACK;
	stp->opStatus=opRetCode;
	stp->deviceStatus=__device_status_cross_map();
	stp->batteryAndX=__battery_status_cross_map();
	stp->randPass=__randpass_scross_map();
	stp->rfStatus=__rf_status_cross_map();
	stp->rssi=rssi;
	stp->CompanyKeyIndex=stpSysData->compServiceKeyIndex;
	
	m_mem_cpy_len(stp->totalPayValue,(uint8_t*)(&(sysData.totalPayMoney)),sizeof(uint32_t));
	
	
	t32=sysData.OVerageVM;
	t32*=100;
	
	if(t32<0l)t32=0l;

	m_mem_cpy_len(stp->balance,(uint8_t*)(&t32)+1,3);
	stp->balanceAttach=(uint8_t)(t32 & 0x0fful);
	//
	
	uint16_t t16=(uint16_t)(stpSysData->LastBSNVM);
	m_mem_cpy_len(stp->payTimes,(uint8_t*)(&t16),sizeof(uint16_t));	
	
	stp->userKeyIndex=stpSysData->userServiceKeyIndex;
	//stp->userKeyIndex=0;
	stp->ver=NB20E1_DEVICE_VER;
	return sizeof(st_AckDef);
}

//cmd=63 冒泡,状态查询，第一包
/*
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
*/
uint16_t __qc_comm_load_pop_ex(uint8_t* buf)
{

	int32_t t32;
	freezeInf_t frInf={0};
	popPkgDefExShort_t* stp;
	stp=(popPkgDefExShort_t*)buf;
	m_mem_set(buf,0,sizeof(popPkgDefExShort_t));	
	qc_data_read_from_media((uint8_t*)&frInf,SYS_DATA_FREEZE_ADDR,sizeof(freezeInf_t));
	//osMutexWait(osMutexSysData,osWaitForever);
	m_mem_cpy_len(stp->id,(uint8_t*)(&(sysData.ID)),sizeof(uint32_t));
	stp->cmd=__15E_CMD_POP_EX;
	
	stp->dcls=(frInf.dlcs)-DLC_STATUS_A+0x0a;
	
	stp->deviceStatus=frInf.deviceStatus;
	stp->deviceType=NB20E1_DEVICE_VER;
	
	//m_mem_cpy_len((uint8_t*)stp->balance,(uint8_t*)frInf.balance,sizeof(int32_t));
	//m_mem_cpy_len((uint8_t*)stp->curTotalVolume,(uint8_t*)frInf.curTotalVolume,sizeof(int32_t));
	#if FREEZE_DATA_TEST==0
		t32=frInf.totalVolume;
	#else
		t32=totalVolume;
	#endif
	t32=(t32*100/sysData.QS);	
	m_mem_cpy_len((uint8_t*)stp->totalVolume,(uint8_t*)&t32,sizeof(int32_t));
	//m_mem_cpy_len((uint8_t*)stp->totalPayValue,(uint8_t*)frInf.totalPayValue,sizeof(int32_t));	
	//m_mem_cpy_len((uint8_t*)stp->currentPrice,(uint8_t*)frInf.price,sizeof(int32_t));	

	stp->batterySta=frInf.batterySta;
	stp->randPass=frInf.randPass;//表事务
	stp->radioStatus=frInf.randPass;
	stp->qn=(__x_qn_convert(sysData.QN))<<2;

	//m_mem_cpy_len(stp->payTimes,(uint8_t*)&frInf.payTimes,sizeof(uint16_t));
	//stp->priceAdjTimes=frInf.priceAdjTimes;	//priceNum
	//stp->consumeGasType=frInf.priceAdjTimes;	
	stp->softVer=soft_VER_NUM;
	stp->rssi=rssi;
	m_mem_cpy_len((uint8_t*)stp->dt,(uint8_t*)frInf.dt,8);
	//osMutexRelease(osMutexSysData);
	return sizeof(popPkgDefExShort_t);
}
#if(NB20E11_SUB_VER >= NB20F11 && NB20E11_SUB_VER <= NB20F11FQ)
uint16_t __qc_comm_load_pop_query_body_pkg0(uint8_t* buf)
{
	int32_t t32;
	st_popDef_Pkg0* stp;
	stp=(st_popDef_Pkg0*)buf;
	m_mem_set(buf,0,sizeof(st_popDef_Pkg0));
	
	osMutexWait(osMutexSysData,osWaitForever);
	m_mem_cpy_len(stp->id,(uint8_t*)(&(stpSysData->ID)),sizeof(uint32_t));
	stp->cmd=__15E_CMD_POP;
	stp->seqNo=0;
	stp->dcls=(stpSysData->DLCS)-DLC_STATUS_A+0x0a;
	stp->deviceStatus=__device_status_cross_map();
	/*
	t32=sysData.OVerageVM;
	t32*=100;
	if(t32<0l)t32=0l;
	m_mem_cpy_len(stp->balance,(uint8_t*)(&t32)+1,3);
	stp->balanceAttach=(uint8_t)(t32 & 0x0fful);
	*/
	/*
	t32=stpSysData->totalVolume-sysData.periodVolume;
	t32=(t32*100/sysData.QS);
	if(t32<0)t32=0;
	m_mem_cpy_len(stp->curTotalVolume,(uint8_t*)(&t32),sizeof(uint32_t));
	*/
	
	t32=sysData.totalVolume;
	t32=(t32*100/sysData.QS);
	m_mem_cpy_len(stp->totalVolume,(uint8_t*)(&t32),sizeof(uint32_t));
	
	stp->batterySta=__battery_status_cross_map();
	stp->randPass=__randpass_scross_map();
	stp->radioStatus=__rf_status_cross_map();
	stp->deviceType=NB20E1_DEVICE_VER;
	osMutexRelease(osMutexSysData);
	return sizeof(st_popDef_Pkg0);
}
#else
uint16_t __qc_comm_load_pop_query_body_pkg0(uint8_t* buf)
{
	int32_t t32;
	st_popDef_Pkg0* stp;
	stp=(st_popDef_Pkg0*)buf;
	m_mem_set(buf,0,sizeof(st_popDef_Pkg0));
	
	osMutexWait(osMutexSysData,osWaitForever);
	m_mem_cpy_len(stp->id,(uint8_t*)(&(stpSysData->ID)),sizeof(uint32_t));
	stp->cmd=__15E_CMD_POP;
	stp->seqNo=0;
	stp->dcls=(stpSysData->DLCS)-DLC_STATUS_A+0x0a;
	stp->deviceStatus=__device_status_cross_map();
	
	t32=sysData.OVerageVM;
	t32*=100;

	if(t32<0l)t32=0l;
	m_mem_cpy_len(stp->balance,(uint8_t*)(&t32)+1,3);
	stp->balanceAttach=(uint8_t)(t32 & 0x0fful);
	
	t32=stpSysData->totalVolume-sysData.periodVolume;
	t32=(t32*100/sysData.QS);
	if(t32<0)t32=0;
	//m_mem_cpy_len(stp->curTotalVolume,(uint8_t*)(&curPeriodVolume),sizeof(uint32_t));
	m_mem_cpy_len(stp->curTotalVolume,(uint8_t*)(&t32),sizeof(uint32_t));
	
	t32=sysData.totalVolume;
	t32=(t32*100/sysData.QS);
	//m_mem_cpy_len(stp->totalVolume,(uint8_t*)(&(stpSysData->totalVolume)),sizeof(uint32_t));
	m_mem_cpy_len(stp->totalVolume,(uint8_t*)(&t32),sizeof(uint32_t));
	
	stp->batterySta=__battery_status_cross_map();
	stp->randPass=__randpass_scross_map();
	stp->radioStatus=__rf_status_cross_map();
	stp->deviceType=NB20E1_DEVICE_VER;
	osMutexRelease(osMutexSysData);
	return sizeof(st_popDef_Pkg0);
}
#endif
/*
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
*/
//cmd=63 冒泡,状态查询，第二包
uint16_t __qc_comm_load_pop_query_body_pkg1(uint8_t* buf)
{
	//uint32_t t32;
	st_popDef_Pkg1* stp;
	stp=(st_popDef_Pkg1*)buf;
	m_mem_set(buf,0,sizeof(st_popDef_Pkg1));
	
	osMutexWait(osMutexSysData,osWaitForever);
	m_mem_cpy_len(stp->id,(uint8_t*)(&(stpSysData->ID)),sizeof(uint32_t));
	stp->cmd=__15E_CMD_POP;
	stp->seqNo=1;
	
	stp->version=0x01;
	stp->priceAdjTimes=(uint8_t)stpSysData->priceAdjTimes;
	stp->consumeGasType=stpSysData->consumeGasType;
	stp->currentPriceStep=sysData.curPriceStep;
	//stp->qn=stpSysData->QN;
	stp->qn=(__x_qn_convert(stpSysData->QN))<<2;
	stp->companyKeyIndex=stpSysData->compTransKeyIndex;
	stp->userKeyIndex=stpSysData->userTransKeyIndex;
	stp->rssi=rssi;

	uint16_t t16=(uint16_t)(stpSysData->LastBSNVM);
	m_mem_cpy_len(stp->payTimes,(uint8_t*)(&t16),sizeof(uint16_t));

	m_mem_cpy_len(stp->totalPayValue,(uint8_t*)(&(stpSysData->totalPayMoney)),sizeof(uint32_t));
	//t16=100;//price;
	t16=(uint16_t)(sysData.curPrice);
	m_mem_cpy_len(stp->currentPrice,(uint8_t*)(&t16),sizeof(uint16_t));
	
	stp->deviceStatus2=__device_status_cross_map();;
	stp->deviceType2=NB20E1_DEVICE_VER;
	osMutexRelease(osMutexSysData);
	return sizeof(st_popDef_Pkg1);
}

uint16_t __qc_comm_load_pay_query_pkg(uint8_t* buf)
{
	int32_t t32;
	uint16_t t16;
	payInfQuery_t* stp;
	stp=(payInfQuery_t*)buf;
	m_mem_set(buf,0,sizeof(payInfQuery_t));
	osMutexWait(osMutexSysData,osWaitForever);
	m_mem_cpy_len(stp->id,(uint8_t*)(&(stpSysData->ID)),sizeof(uint32_t));
	stp->cmd=__15E_CMD_PAY_QUERY;
	stp->deviceType=NB20E1_DEVICE_VER;
	t16=(uint16_t)sysData.LastBSNVM;
	m_mem_cpy_len((stp->payTimes),(uint8_t*)&t16,sizeof(uint16_t));
	
	t16=(uint16_t)(sysData.WarnSetOverageVM);
	m_mem_cpy_len(stp->warningSet,(uint8_t*)&t16,sizeof(uint16_t));
	
	t32=(int32_t)(sysData.PaymentVM[0]);
	m_mem_cpy_len(stp->payValue,(uint8_t*)&t32,sizeof(int32_t));
	
	t32=(int32_t)(sysData.PaymentVM[1]);
	m_mem_cpy_len(stp->prePayValue,(uint8_t*)&t32,sizeof(int32_t));	

	t32=(int32_t)(sysData.PaymentVM[2]);
	m_mem_cpy_len(stp->preprePayValue,(uint8_t*)&t32,sizeof(int32_t));	
	osMutexRelease(osMutexSysData);
	return sizeof(payInfQuery_t);
}
/*
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
*/
uint16_t  __qc_comm_load_module_inf_pkg(uint8_t* buf)
{
	//getModuleInf_t
	nbModuleInf_t* stp=(nbModuleInf_t*)buf; 
	m_mem_set(buf,0,sizeof(nbModuleInf_t));
	osMutexWait(osMutexSysData,osWaitForever);
	
	m_mem_cpy_len(stp->id,(uint8_t*)(&(stpSysData->ID)),sizeof(uint32_t));
	stp->cmd=__20E_CMD_GET_MODULE_INF;	
	m_mem_cpy_len(stp->CIMI,CIMI,sizeof(CIMI));
	m_mem_cpy_len(stp->CGSN,CGSN,sizeof(CGSN));
	m_mem_cpy_len(stp->ICCID,ICCID,sizeof(ICCID));
	osMutexRelease(osMutexSysData);
	
	return sizeof(nbModuleInf_t);
}
uint16_t __qc_comm_load_step_price_query_pkg_ex(uint8_t* buf)
{
	uint8_t t8,i;
	uint16_t t16,buf16[6];
	stepPriceInfQueryEx_t* stp=(stepPriceInfQueryEx_t*)buf; 
	m_mem_set(buf,0,sizeof(stepPriceInfQueryEx_t));
	osMutexWait(osMutexSysData,osWaitForever);
	
	m_mem_cpy_len(stp->id,(uint8_t*)(&(stpSysData->ID)),sizeof(uint32_t));
	stp->cmd=__15E_CMD_STEP_PRICE_QUERY_EX;	
	
	stp->seqNo=0;
	
	t8=sysData.calcDateTime.calcPeriod;
	if(t8>6)t8=7;
	t8<<=5;
	//i=sysData.adjDateTime.dayInMonth;
	i=sysData.calcDateTime.dayInMonth;
	i&=0x1f;
	if(i)i--;
	t8|=i;
	stp->AdjperiodAdjDay=t8;
	//
	t8=sysData.adjDateTime.month;
	if(t8)t8--;
	stp->startMonth=t8;
	//
	sysData.priceAdjTimes=stp->priceNum;
	sysData.consumeGasType=stp->priceKind;
	
	for(i=0;i<6;i++){
		t16=(uint16_t)(sysData.SetpVolume[i]/100);
		buf16[i]=t16;
	}	
	m_mem_cpy_len(stp->gasGate,(uint8_t*)&(buf16[0]),12);
	
	for(i=0;i<6;i++){
		t16=(uint16_t)sysData.SetpPrice[i];
		buf16[i]=t16;
	}
	m_mem_cpy_len(stp->basePrice,(uint8_t*)&(buf16[0]),12);
	
	stp->deviceType=NB20E1_DEVICE_VER;
	osMutexRelease(osMutexSysData);	
	return sizeof(stepPriceInfQueryEx_t);
}

uint16_t __qc_comm_load_step_price_query_pkg(uint8_t* buf)
{

	uint8_t t8,i;
	uint16_t t16,buf16[6];
	stepPriceInfQuery_t* stp=(stepPriceInfQuery_t*)buf; 
	m_mem_set(buf,0,sizeof(stepPriceInfQuery_t));
	if(stepPriceQueryStep>6)stepPriceQueryStep=0;
	
	osMutexWait(osMutexSysData,osWaitForever);
	m_mem_cpy_len(stp->id,(uint8_t*)(&(stpSysData->ID)),sizeof(uint32_t));
	stp->cmd=__15E_CMD_STEP_PRICE_QUERY;
	
	if(stepPriceQueryStep!=0)stepPriceQueryStep=3;
	if(stepPriceQueryStep){
		stp->seqNo=1;
	}else{
		stp->seqNo=0;
	}
	//
	t8=sysData.calcDateTime.calcPeriod;
	if(t8>6)t8=7;
	t8<<=5;
	//i=sysData.adjDateTime.dayInMonth;
	i=sysData.calcDateTime.dayInMonth;
	i&=0x1f;
	if(i)i--;
	t8|=i;
	stp->AdjperiodAdjDay=t8;
	//
	t8=sysData.adjDateTime.month;
	if(t8)t8--;
	stp->startMonth=t8;
	//
	sysData.priceAdjTimes=stp->priceNum;
	sysData.consumeGasType=stp->priceKind;
	//
	if(stepPriceQueryStep==0){
		t8=0;
		stepPriceQueryStep=3;
	}else{
		stepPriceQueryStep=0;
		t8=3;
	}
	
	for(i=0;i<6;i++){
		t16=(uint16_t)(sysData.SetpVolume[i]/100);
		buf16[i]=t16;
	}
	m_mem_cpy_len(stp->gasGate0,(uint8_t*)&(buf16[t8]),2);
	m_mem_cpy_len(stp->gasGate1,(uint8_t*)&(buf16[t8+1]),2);
	m_mem_cpy_len(stp->gasGate2,(uint8_t*)&(buf16[t8+2]),2);
	
	for(i=0;i<6;i++){
		t16=(uint16_t)sysData.SetpPrice[i];
		buf16[i]=t16;
	}
	m_mem_cpy_len(stp->basePrice0,(uint8_t*)&(buf16[t8]),2);
	m_mem_cpy_len(stp->basePrice1,(uint8_t*)&(buf16[t8+1]),2);
	m_mem_cpy_len(stp->basePrice2,(uint8_t*)&(buf16[t8+2]),2);	
	
	stp->deviceType=NB20E1_DEVICE_VER;
	osMutexRelease(osMutexSysData);
	return sizeof(stepPriceInfQuery_t);
}
/*
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
	uint8_t startVol;
	uint8_t	checkM[4];
*/



// uint16_t __qc_ins_comm_reply(uint8_t* rbuf,uint16_t len)
// {
	// uint16_t ret;
	
	// return ret;
// }
// uint16_t __qc_ins_comm_reply(void)
// {
	// uint16_t ret;
	// uint
	
	
	// return ret;	
// }
uint16_t __qc_ins_set_id(uint8_t* rbuf,uint16_t len)
{
	uint32_t t32;
	uint16_t t16;
	uint8_t t8;
	
	st_setIdDef* stp;
	setIdModeDef_t mode;
	qc_data_save_sys_data(true);
	stp=(st_setIdDef*)(rbuf+sizeof(st_frameHeaderDef));
	m_mem_cpy_len((uint8_t*)&t32,(uint8_t*)(stp->newid),sizeof(uint32_t));
	if(stpSysData->DLCS!=DLC_STATUS_A){
		opRetCode=__15S_OP_DCLS_ERR;
		return 0;
	}
	stpSysData->ID=t32;
	mode.b=stp->mode;
	
	if(mode.bits.changFlg){

		t8=mode.bits.meterType;
		if(t8>sizeof(qnTable)/sizeof(qnTable[0])-2)return 0;
		stpSysData->QN=qnTable[t8];
		stpSysData->QMAX=qnTable[t8+1];
	}
	
	t32=0x00ul;
	m_mem_cpy_len((uint8_t*)&t32,(uint8_t*)(&(stp->startVol[0])),sizeof(uint32_t)-1);
	if(t32<0xffffff){
		stpSysData->totalVolume=t32;
		stpSysData->periodVolume=t32;
	}
	//add 
	if(stp->ipSetting0[0]!='\0')m_mem_cpy(sysData.venderUdpSetting,stp->ipSetting0);
	
	if(stp->ipSetting1[0]!='\0')m_mem_cpy(sysData.userUdpSetting,stp->ipSetting1);
	//
	
	qc_data_save_sys_data(true);
	//同步时间
	stRtcTypeDef_t dt;
	m_mem_cpy_len((uint8_t*)&t16,(uint8_t*)(stp->year),sizeof(uint16_t));
	
	dt.year=(uint8_t)(t16 -2000);
	dt.month=stp->month+1;
	dt.dayInMonth=stp->day+1;
	dt.hour=stp->hour;
	dt.minute=stp->minute;
	dt.second=stp->second;
	//m_bcd_2_hex
	t8=day_of_week(t16,m_bcd_2_hex(dt.month),m_bcd_2_hex(dt.dayInMonth));
	dt.dayInWeek=t8;
	qc_data_set_rtc((uint8_t*)&dt);
	opRetCode=__15S_OP_OK;
	return 1;
}
/*
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
*/
uint16_t __qc_ins_set_param(uint8_t* rbuf,uint16_t len)
{
	bool flgSave=false;
	//uint8_t t8;
	uint32_t t32;
	
	qc_data_save_sys_data(true);
	st_paramSettingdef* stp;
	stp=(st_paramSettingdef*)(rbuf+sizeof(st_frameHeaderDef));
	st_setParFuncCodeBits fcode;
	fcode.b=stp->funcCode;
	if(fcode.bits.valveCtrl){
		lockReason.bits.bSeverOff=1;
		flgSave=true;
	}
	if(fcode.bits.modVolume){
		m_mem_cpy_len((uint8_t*)&t32,(uint8_t*)&(stp->volume),sizeof(uint32_t));
		sysData.totalVolume=t32;
		sysData.periodVolume=sysData.totalVolume;
		sysData.VolumePerMon[0]=sysData.periodVolume;
		flgSave=true;
	}
	if(fcode.bits.modMaxPay){
		m_mem_cpy_len((uint8_t*)(&stpSysData->MaxOverageVM),(uint8_t*)&(stp->maxStoreVol),sizeof(uint32_t));
		flgSave=true;
	}

	if(fcode.bits.flwOverType){
		if(stp->period_s ==0 && stp->times==0){
			sysData.HightFlowProtectEn=0;
			DeviceEvent.bits.bHightFlow=0;
			lockReason.bits.bHifhtFlow=0;
		}else{
			sysData.HightFlowProtectEn=1;
		}
		flgSave=true;
	}

	if(flgSave)qc_data_save_sys_data(true);
	opRetCode=__15S_OP_OK;
	return 1;
}

uint16_t __qc_ins_clear_to_zero(uint8_t* rbuf,uint16_t len)
{
	//uint32_t t32,i;
	//st_setToZeroDef* stp;
	//stp=(st_setToZeroDef*)(rbuf+sizeof(st_frameHeaderDef));
	qc_data_save_sys_data(true);
	if(sysData.DLCS<=DLC_STATUS_B){
		qc_data_clear_sysdata_to_dcls_a();
	}
	else{
		qc_data_clear_sysdata_to_dcls_b();
	}
	qc_data_save_sys_data(true);
	rtVolumeRefreshFlag=true;
	data_api_calc_all();
	opRetCode=__15S_OP_OK;
	return 1;
}

uint16_t __qc_ins_supper_pay(uint8_t* rbuf,uint16_t len)
{
	
	int16_t t16;
	int32_t t32;
	st_supperPayDef* stp;
	stp=(st_supperPayDef*)(rbuf+sizeof(st_frameHeaderDef));
	if(stpSysData->DLCS>=DLC_STATUS_C){
		//todo
		opRetCode=__15S_OP_DCLS_ERR;
	}else {
		if(sysData.superPayNums<SUPPER_PAY_TIMES_LIMITS){
			if(sysData.DLCS!=DLC_STATUS_A){
				sysData.superPayNums++;
			}
			m_mem_cpy_len((uint8_t*)&t16,(uint8_t*)stp->payValue,sizeof(uint16_t));
			stpSysData->OVerageVM=t16;
			//m_mem_cpy_len((uint8_t*)&t16,(uint8_t*)stp->warningSet,sizeof(uint32_t));
			t16=stp->warningSet;
			t32=(int32_t)t16;
			//t32*=100;
			t32=t32*sysData.QS;
			stpSysData->WarnSetOverageVM=t32;
			qc_data_save_sys_data(true);
			//qc_data_device_even_lock_clear();
			opRetCode=__15S_OP_OK;
		}else{
			opRetCode=__15S_OP_PAY_VERIFY;
		}
		payPromptTimeOut=PAY_PROMPT_TIME_LONG;	
		menu=0x00;
		subMenu=0x00;		
	} 
	return 1;
}
//stDeviceInf_t* stpSysData=&sysData;
uint16_t __qc_ins_open_account(uint8_t* rbuf,uint16_t len)
{
	//used commonBuf
	uint8_t ret=0,t8;
	uint16_t t16;
	int32_t t32;
	st_openAccountDef* stp;
	stp=(st_openAccountDef*)(rbuf+sizeof(st_frameHeaderDef));
	qc_data_save_sys_data(true);
	m_mem_cpy_len(commonBuf,(uint8_t*)&sysData,sizeof(sysData));
	if(sysData.DLCS!=DLC_STATUS_B){
		opRetCode=__15S_OP_DCLS_ERR;
		ret=1;		
	}else{
		m_flow_data_init();
		//
		data_api_calc_all_ex();
		m_mem_cpy_len(commonBuf,(uint8_t*)&sysData,sizeof(stDeviceInf_t));
		//
		qc_data_realtime_data_clear();
		qc_data_device_even_lock_clear();
		//调价次数
		t8=stp->stepAndPriceAdjTimes;
		//sysData.priceAdjTimes=t8>>1;
		//sysData.prePriceAdjTimes=sysData.priceAdjTimes;
		sysData.priceAdjTimes=0;
		sysData.prePriceAdjTimes=0;
		
		//用气类型(原型，分组)
		//sysData.consumeGasType=stp->consumeGasType;
		//sysData.preConsumeGasType=sysData.consumeGasType;
		sysData.consumeGasType=0;
		sysData.preConsumeGasType=0;
		//公司编码
		m_mem_cpy_len((uint8_t*)&t16,(uint8_t*)stp->CompanyCode,sizeof(uint16_t));
		sysData.CompanyCode=t16;
		if(t16==0){
			m_mem_cpy_len((uint8_t*)&sysData,commonBuf,sizeof(sysData));
			opRetCode=__15S_OP_NOT_MINE;
			return 1;
		}
		//充值
		m_mem_cpy_len((uint8_t*)&t32,(uint8_t*)stp->payValue,sizeof(uint32_t));
		sysData.PaymentVM[0]=t32;
		sysData.LastBSNVM=1;
		
		//t32+=stpSysData->OVerageVM;
		sysData.OVerageVM=t32;
		sysData.totalPayMoney=t32;
		//单价
		m_mem_cpy_len((uint8_t*)&t16,(uint8_t*)stp->priceStep_00,sizeof(uint16_t));
		sysData.SetpPrice[0]=t16;
		sysData.SetpVolume[0]=INT32_MAX;
		
		sysData.preSetpPrice[0]=t16;
		sysData.preSetpVolume[0]=INT32_MAX;		
		
		sysData.curPrice=sysData.SetpPrice[0];
		//报警气量
		m_mem_cpy_len((uint8_t*)&t16,(uint8_t*)stp->warningSet,sizeof(uint16_t));
		//t16=swap_uint16((uint16_t)t16);
		//sysData.WarnSetOverageVM=(int32_t)t16*100;
		sysData.WarnSetOverageVM=(int32_t)t16*sysData.QS;
		//计价周期调价日
		t8=stp-> stepPricePeriod;
		t8 &= 0x1f;
		t8++;
		sysData.calcDateTime.dayInMonth=t8;
		
		t8=stp-> stepPricePeriod;
		t8>>=5;
		sysData.calcDateTime.calcPeriod=t8;
		
		t8=stp->startMonth;
		//sysData.adjDateTime.month=t8+1;
		sysData.calcDateTime.month=t8+1;
		sysData.calcDateTime.year=m_bcd_2_hex(stRtcDataTime.year);
		//
		sysData.periodVolume=sysData.totalVolume;

		stpSysData->DLCS=DLC_STATUS_C;
		
		qc_data_save_sys_data(true);
		ret=1;
		opRetCode=__15S_OP_OK;
	}
	return ret;
}
/*
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
*/
uint16_t __qc_ins_update_key(uint8_t* rbuf,uint16_t len)
{
	qc_data_save_sys_data(true);
	keyUpdataDef_t* stp=(keyUpdataDef_t*)(rbuf+sizeof(st_frameHeaderDef));
	
	if(sysData.DLCS < DLC_STATUS_C){
		opRetCode=__15S_OP_DCLS_ERR;
		return 1;
	}	
	
	if(stp->level == sysData.keyIndex){
		//return 0;
		opRetCode=__15S_OP_KEY_UPDATE_ERR;	
		return 1;		
	}
	
	osMutexWait(osMutexSysData,osWaitForever);
	sysData.userTransKeyIndex=stp->keyIndex;
	sysData.userServiceKeyIndex=stp->keyIndex;
	m_mem_cpy_len((uint8_t*)(sysData.userTransKey),stp->transKey,8);	
	m_mem_cpy_len((uint8_t*)(sysData.userServiceKey),stp->serviceKey,8);	
	if(stp->level==0xfe){
		//sysData.keyIndex=stp->level;
		sysData.compTransKeyIndex=stp->keyIndex;
		sysData.compServiceKeyIndex=stp->keyIndex;
		m_mem_cpy_len((uint8_t*)(sysData.compTransKey),stp->transKey,8);
		m_mem_cpy_len((uint8_t*)(sysData.compSeviceKey),stp->serviceKey,8);
	}
	
	osMutexRelease(osMutexSysData);
	qc_data_save_sys_data(true);
	opRetCode=__15S_OP_OK;	
	return 1;
}
/*
typedef struct{
	uint8_t id[4];
	uint8_t	cmd;
	uint8_t reverse;
	uint8_t payTimes;
	uint8_t	preprePayValue[4];
	uint8_t	prePayValue[4];
	uint8_t	PayValue[4];
	uint8_t	warningSet[2];
	uint8_t reverse0[2];
	uint8_t	checkM[4];	
}st_payDef;
*/
//stpSysData
uint16_t __qc_ins_pay(uint8_t* rbuf,uint16_t len)
{
	uint32_t pvl[3];
	int32_t t32,ov; 
	uint16_t t1,t2,ret;
	uint16_t i=0;
	qc_data_save_sys_data(true);
	stDeviceInf_t* sysTmp=(stDeviceInf_t*)commonBuf;
	
	osMutexWait(osMutexSysData,osWaitForever);
	do{
		payPromptTimeOut=0;
		m_mem_cpy_len((uint8_t*)sysTmp,(uint8_t*)&sysData,sizeof(stDeviceInf_t));
		st_payDef* stp;
		stp=(st_payDef*)(rbuf+sizeof(st_frameHeaderDef));
		if(sysTmp->DLCS<=DLC_STATUS_B){
			opRetCode=__15S_OP_DCLS_ERR;
			ret=1;
			break;
		}
		if(sysTmp->DWM== DWM_COMMON_MODE){
			opRetCode=__15S_OP_VER_ERR;
			ret=1;
			break;			
		}
		t1=(uint16_t)sysTmp->LastBSNVM;
		//t2=stp->payTimes;
		m_mem_cpy_len((uint8_t*)&t2,stp->payTimes,sizeof(int16_t));
		
		if(t2<=t1){ 
			opRetCode=__15S_OP_PAY_OK;ret=1;break;
		}else{
			if(t2-t1>3){opRetCode=__15S_OP_PAY_VERIFY;ret=1;break;}
		}
		//data_api_calc_all_ex();
		t2=t2-t1;
		
		m_mem_cpy_len((uint8_t*)&t32,(uint8_t*)stp->PayValue,sizeof(uint32_t));
		//t32>>=8;
		pvl[0]=t32;
		m_mem_cpy_len((uint8_t*)&t32,(uint8_t*)stp->prePayValue,sizeof(uint32_t));
		//t32>>=8;
		pvl[1]=t32;
		m_mem_cpy_len((uint8_t*)&t32,(uint8_t*)stp->preprePayValue,sizeof(uint32_t));
		//t32>>=8;
		pvl[2]=t32;	
		i=0;
		for(t1=t2;t1<3;t1++){
			if(pvl[t1]!=sysTmp->PaymentVM[i]){
				opRetCode=__15S_OP_PAY_VERIFY;ret=1;break;}
			i++;    
		} 
		if(opRetCode==__15S_OP_PAY_VERIFY)break;
		realPayValue=0;
		ov=sysTmp->OVerageVM;
		//for(t1=0;t1<t2;t1++){
		for(t1=t2;t1>0;t1--){
			t32=pvl[t1-1];//__SWAP32(stCmd->value[t1]);
			ov+=t32;
			
			realPayValue+=t32;
			
			int32_array_shift_right(sysTmp->PaymentVM,5);
			sysTmp->PaymentVM[0]=t32;
		}
		//add 
		//sysTmp->MaxOverageVM=0x7fffffff;
		sysTmp->totalPayMoney += realPayValue;
		
		
		//if(ov>sysTmp->MaxOverageVM){
		if(ov>20000000l){	
			realPayValue=0;
			opRetCode=__15S_OP_PAY_OVER;
			ret=1;//return 1;	
			break;
		}
		
		sysTmp->OVerageVM=ov;
		
		m_mem_cpy_len((uint8_t*)&t2,stp->payTimes,sizeof(int16_t));
		sysTmp->LastBSNVM=t2;
		//sysTmp->LastBSNVM=stp->payTimes;
		
		//修改报警量
		m_mem_cpy_len((uint8_t*)&t1,stp->warningSet,sizeof(int16_t));
		t32=t1;
		//t32*=100;
		t32=t32*sysData.QS;
		sysTmp->WarnSetOverageVM=t32;
		//
		opRetCode=__15S_OP_OK;
		m_mem_cpy_len((uint8_t*)&sysData,(uint8_t*)sysTmp,sizeof(stDeviceInf_t));
		
		osMutexRelease(osMutexSysData);
		qc_data_save_sys_data(true);
		osMutexWait(osMutexSysData,osWaitForever);
		payPromptTimeOut=PAY_PROMPT_TIME_LONG;
	}while(0);
	
	osMutexRelease(osMutexSysData);
	return ret;
}

uint16_t __qc_ins_valve_lock(uint8_t* rbuf,uint16_t len)
{	
	stpSysData->lockReason.bits.bSeverOff=1;
	lockReason.bits.bSeverOff=1;
	vavle_off_from_app(OFF_REASON_SEVERS);
	//qc_data_save_sys_data(true);
	opRetCode=__15S_OP_OK;	
	return 1;
}

uint16_t __qc_ins_valve_unlock(uint8_t* rbuf,uint16_t len)
{
	qc_data_device_even_lock_clear();
	if(sysData.DLCS>=DLC_STATUS_C)sysData.DLCS=DLC_STATUS_C;
	qc_data_save_sys_data(true);
	opRetCode=__15S_OP_OK;	
	return 1;
}
/*

*/
uint16_t __qc_ins_set_flow_tmo(uint8_t* rbuf,uint16_t len)
{
	opRetCode=__15S_OP_VER_ERR;	
	return 1;
}


uint16_t __qc_ins_set_flow_mode(uint8_t* rbuf,uint16_t len)
{
	opRetCode=__15S_OP_VER_ERR;	
	return 1;
}
extern void data_api_calc_clear_nf_flag(void);
extern void data_api_calc_clear_nf_force(void);
uint16_t __qc_ins_set_no_flow_time_out(uint8_t* rbuf,uint16_t len)
{
	uint16_t t16;
	__15e_tmOutSettingdef_t* stp;
	stp=(__15e_tmOutSettingdef_t*)(rbuf+sizeof(st_frameHeaderDef));
	qc_data_save_sys_data(true);
	m_mem_cpy_len((uint8_t*)&t16,stp->tmOut,sizeof(int16_t));
	if(t16>720*6)t16=720*6;
	sysData.ConstNoFlowTimeOut=t16;
	
	m_mem_cpy_len((uint8_t*)&t16,stp->tmOutEx,sizeof(int16_t));
	if(t16>720*6)t16=720*6;
	sysData.ConstNoFlowTimeOutEx=t16;
	//uint8_t t8=stp->tryLimit;

	sysData.tmOutTryTimes=stp->tryLimit;
	//清除超时
	//data_api_calc_clear_nf_flag();
	data_api_calc_clear_nf_force();
	qc_data_save_sys_data(true);
	opRetCode=__15S_OP_OK;	
	return 1;
}

uint8_t __qc_ins_sync_step_slu_calc_fi(void)
{
	//normalDate_t today,calcday,adjday;
	normalDate_t today,adjday;
	today.date=0x00ul;
	today.stru.year=m_bcd_2_hex(stRtcDataTime.year);
	today.stru.month=m_bcd_2_hex(stRtcDataTime.month);
	today.stru.dayInMonth=m_bcd_2_hex(stRtcDataTime.dayInMonth);
	/*
	calcday.date=0x00ul;
	calcday.stru.year=sysData.calcDateTime.year;
	calcday.stru.month=sysData.calcDateTime.month;
	calcday.stru.dayInMonth=sysData.calcDateTime.dayInMonth;	
	*/
	// adjday.date=0x00ul;
	// adjday.stru.year=sysData.adjDateTime.year;
	// adjday.stru.month=sysData.adjDateTime.month;
	// adjday.stru.dayInMonth=sysData.adjDateTime.dayInMonth;
	return (today.date>=adjday.date);
}

uint8_t __qc_ins_sync_step_slu_adj_fi(void)
{
	//normalDate_t today,calcday,adjday;
	normalDate_t today,adjday;
	today.date=0x00ul;
	today.stru.year=m_bcd_2_hex(stRtcDataTime.year);
	today.stru.month=m_bcd_2_hex(stRtcDataTime.month);
	today.stru.dayInMonth=m_bcd_2_hex(stRtcDataTime.dayInMonth);
	
	// calcday.date=0x00ul;
	// calcday.stru.year=sysData.calcDateTime.year;
	// calcday.stru.month=sysData.calcDateTime.month;
	// calcday.stru.dayInMonth=sysData.calcDateTime.dayInMonth;	
	
	adjday.date=0x00ul;
	adjday.stru.year=sysData.adjDateTime.year;
	adjday.stru.month=sysData.adjDateTime.month;
	adjday.stru.dayInMonth=sysData.adjDateTime.dayInMonth;	
	return (today.date>=adjday.date);
}

uint16_t __qc_ins_sync_step_slu(uint8_t* rbuf,uint16_t len)
{
	uint8_t* p;
	uint8_t t8,i;
	uint16_t t16,pr;
	priceAdjDT_t adjDateTime={0};	// 调价日期
	priceCalcDT_t calcDataTime={0};	//计价日期
	uint8_t calcPeriod;
	
	if(sysData.DLCS < DLC_STATUS_C){
		opRetCode=__15S_OP_DCLS_ERR;
		return 1;	
	}		
	if(sysData.DWM== DWM_COMMON_MODE){
		opRetCode=__15S_OP_VER_ERR;
		return 1;		
	}
	qc_data_save_sys_data(true);
	osMutexWait(osMutexSysData,osWaitForever);
	syncStepPricdSln_t* stp=(syncStepPricdSln_t*)(rbuf+sizeof(st_frameHeaderDef));
	//2018.04.11 楼上的决定了要验证方案号和次数，gaoqiubudong
	t8=stp->stepYear & 0x80;
	if(sysData.prePriceAdjTimes==stp->priceAdjedTimes && sysData.preConsumeGasType==stp->nPriceKind && t8==0x00){
		osMutexRelease(osMutexSysData);
		//opRetCode=__15S_OP_EXECUTED;	
		opRetCode=__15S_OP_OK;
		return 1;		
	}
	t8=stp->stepYear;
	i=0;
	//第7位为1，从第三节开始调价
	if(t8 & 0x80)i=3;
	t8=i+3;
	p=(uint8_t*)(stp->gasGate0);
	for(;i<t8;i++){
		m_mem_cpy_len((uint8_t*)(&t16),p,sizeof(uint16_t));
		if(t16==0xffff){adjDateTime.preAdjSlu |= (0x01<<2);}
		//sysData.preSetpVolume[i]=(uint32_t)t16*100;
		sysData.preSetpVolume[i]=(uint32_t)t16*(sysData.QS);
		p+=sizeof(uint16_t);
		m_mem_cpy_len((uint8_t*)(&t16),p,sizeof(uint16_t));
		pr=t16;
		sysData.preSetpPrice[i]=(uint32_t)t16;
		p+=sizeof(uint16_t);		
	}
	for(;i<6;i++){
		sysData.preSetpVolume[i]=INT32_MAX;
		sysData.preSetpPrice[i]=pr;
	}
	adjDateTime.year=((stp->stepYear) & 0x7f);
	calcDataTime.year=((stp->stepYear) & 0x7f);
	
	t8=stp->month;
	adjDateTime.month=(t8&0x0f)+1;
	calcDataTime.month=(t8>>0x04)+1;
	
	t8=stp->day;
	adjDateTime.dayInMonth=(t8 & 0x1f)+1;
	//
	t8=stp->period;
	calcDataTime.dayInMonth=(t8 & 0x1f)+1;
	t8>>=5;
	if(t8>=1 && t8<=6){calcPeriod=t8;}
	else{
		calcPeriod=12;
	}
	calcDataTime.calcPeriod=calcPeriod;

	adjDateTime.preAdjSlu |= (0x01<<0);
	
	t8=stp->day;
	//第4位是否清除累计用气量
	if(t8 & 0x40){
		adjDateTime.preAdjSlu |= (0x01<<1);
		//mod at 2018.03.30
		//adjDateTime.preAdjSlu &= ~(0x01<<1);
	}
	t8=stp->stepYear & 0x80;
	if(t8==0x00){
		m_mem_cpy_len((uint8_t*)(&sysData.adjDateTime),(uint8_t*)(&adjDateTime),sizeof(adjDateTime));
		m_mem_cpy_len((uint8_t*)(&sysData.preCalcDateTime),(uint8_t*)(&calcDataTime),sizeof(calcDataTime));

		__nop();
		sysData.prePriceAdjTimes=stp->priceAdjedTimes;
		sysData.preConsumeGasType=stp->nPriceKind;
		if(adjDateTime.preAdjSlu & (0x01<<2)){
			opRetCode=__15S_OP_OK;	
		}else{
			//opRetCode=__15S_OP_NEED_ORTHER_PACKAGE;	
			opRetCode=__15S_OP_OK;	
		}
	}else{
		if(adjDateTime.preAdjSlu & (0x01<<2)){
			sysData.adjDateTime.preAdjSlu |= (0x01<<2);
		}
		opRetCode=__15S_OP_OK;	
	}
	osMutexRelease(osMutexSysData);
	qc_data_save_sys_data(true);

	//opRetCode=__15S_OP_OK;	
	return 1;
}
uint16_t __qc_ins_step_price_query(uint8_t* rbuf,uint16_t len)
{
	qc_data_save_sys_data(true);
	opRetCode=__15S_OP_OK;	
	return 1;
}
uint16_t __qc_ins_step_price_query_ex(uint8_t* rbuf,uint16_t len)
{
	qc_data_save_sys_data(true);
	opRetCode=__15S_OP_OK;	
	return 1;
}
uint16_t  __qc_ins_get_module_inf(uint8_t* rbuf,uint16_t len)
{
	qc_data_save_sys_data(true);
	opRetCode=__15S_OP_OK;	
	return 1;	
}
uint16_t __qc_ins_pay_query(uint8_t* rbuf,uint16_t len)
{
	qc_data_save_sys_data(true);
	opRetCode=__15S_OP_OK;	
	return 1;
}

uint16_t __qc_ins_sta_query(uint8_t* rbuf,uint16_t len)
{
	qc_data_save_sys_data(true);
	opRetCode=__15S_OP_OK;	
	return 1;	
}

uint16_t __qc_ins_log_query(uint8_t* rbuf,uint16_t len)
{
	opRetCode=__15S_OP_VER_ERR;	
	return 1;
}

uint16_t __qc_ins_ack(uint8_t* rbuf,uint16_t len)
{
	opRetCode=__15S_OP_VER_ERR;	
	return 1;
}

uint16_t __qc_ins_sync_rtc_short(uint8_t* rbuf,uint16_t len)
{
	uint8_t t8;
	uint16_t t16;
	stRtcTypeDef_t dt={0};
	syncRtcDef_t* stp;
	
	stp=(syncRtcDef_t*)rbuf;
	if(stp->len !=0x11)return 0;
	__nop();
	
	dt.year=stp->year;
	dt.month=stp->month;
	dt.dayInMonth=stp->dayInMonth;
	dt.hour=stp->hour;
	dt.minute=stp->minute;
	dt.second=stp->second;
	t16=2000+dt.year;
	t8=day_of_week(t16,(uint16_t)dt.month,(uint16_t)dt.dayInMonth);
	dt.dayInWeek=t8;
	qc_data_set_rtc((uint8_t*)&dt);	
	//opRetCode=__15S_OP_OK;
	opRetCode=__15S_OP_RTC_SYNC;
	return 1;
}

uint16_t qc_comm_down_package_is_mine(uint8_t* rbuf,uint16_t len)
{
	uint32_t t32;
	uint16_t t16;
	st_frameHeaderDef* stph=(st_frameHeaderDef*)rbuf;
	
	opRetCode=__15S_OP_NOT_MINE;
	m_mem_cpy_len((uint8_t*)&t32,(uint8_t*)stph->id,sizeof(uint32_t));
	
	//if(!(t32<=10ul || t32==sysData.ID))return 0;
	if(!(!fi_id_writed_in_dlcs_a() || t32==sysData.ID))return 0;
	m_mem_cpy_len((uint8_t*)&t16,(uint8_t*)stph->CompanyCode,sizeof(uint16_t));
	if(stpSysData->DLCS >=DLC_STATUS_C){
		if(t32!=stpSysData->ID)return 0;
		if(t16!=stpSysData->CompanyCode && t16!=0)return 0;
	}
	/*
	if(sysData.DLCS >= DLC_STATUS_C && stph->keyIndex==0){
		opRetCode=__15S_OP_KEY_UPDATE_ERR;
		return 0;
	}	
	*/
	opRetCode=__15S_OP_NOTHING;
	return 1;
}

uint16_t qc_comm_down_package_analysis_short(uint8_t* rbuf,uint16_t len)
{
	uint16_t ret=0;
	uint8_t cmd;	
	//st_frameHeaderDef* stpfh=(st_frameHeaderDef*)rbuf;
	cmd=rbuf[1];
	switch(cmd){
		case __15E_CMD_SYNC_RTC_SHORT:ret=__qc_ins_sync_rtc_short(rbuf,len);break;
		default:break;
	}
	return ret;
}

uint16_t qc_comm_down_package_analysis(uint8_t* rbuf,uint16_t len)
{
	uint16_t ret=0;
	uint8_t cmd;
	st_comCmdHeaderDef* stph=(st_comCmdHeaderDef*)(rbuf+sizeof(st_frameHeaderDef));

	cmd=stph->cmd;
	protocolCmd=cmd;
	switch(cmd){
		case __15E_CMD_SET_ID:				ret=__qc_ins_set_id(rbuf,len);			break;
		case __15E_CMD_SET_PARAM:			ret=__qc_ins_set_param(rbuf,len);		break;
		case __15E_CMD_CLEAT_TO_ZERO:		ret=__qc_ins_clear_to_zero(rbuf,len);	break;
		case __15E_CMD_SUPPER_PAY:			ret=__qc_ins_supper_pay(rbuf,len);		break;			
		case __15E_CMD_OPEN_ACCOUNT:		ret=__qc_ins_open_account(rbuf,len);	break;	
		case __15E_CMD_PAY:					ret=__qc_ins_pay(rbuf,len);				break;		
		case __15E_CMD_VALVE_CTRL_LOCK:		ret=__qc_ins_valve_lock(rbuf,len);		break;	
		case __15E_CMD_VALVE_CTRL_UNLOCK:	ret=__qc_ins_valve_unlock(rbuf,len);	break;	
		case __15E_CMD_SYNC_STEP_PRICE_SLU:	ret=__qc_ins_sync_step_slu(rbuf,len);	break;
		
		//case __15E_CMD_CONST_FLOW_TIME_OUT:	ret=__qc_ins_set_flow_tmo(rbuf,len);	break;
		//case __15E_CMD_SET_FLOW_MODE:		ret=__qc_ins_set_flow_mode(rbuf,len);	break;
		case __15E_CMD_SET_TIME_OUT:		ret=__qc_ins_set_no_flow_time_out(rbuf,len);	break;
		case __15E_CMD_ACK:					ret=__qc_ins_ack(rbuf,len);				break;
		case __15E_CMD_STATUS_QUERY:		ret=__qc_ins_sta_query(rbuf,len);		break;
		//case __15E_CMD_STATUS_FB
		//case __15E_CMD_POP
		case __15E_CMD_STEP_PRICE_QUERY:	ret=__qc_ins_step_price_query(rbuf,len);break;
		case __15E_CMD_STEP_PRICE_QUERY_EX:	ret=__qc_ins_step_price_query_ex(rbuf,len);break;
		case __20E_CMD_GET_MODULE_INF:		ret=__qc_ins_get_module_inf(rbuf,len);break;
		case __15E_CMD_PAY_QUERY:			ret=__qc_ins_pay_query(rbuf,len);		break;
		case __15E_CMD_LOG_QUERY:			ret=__qc_ins_log_query(rbuf,len);		break;
		//
		case __15E_CMD_KEY_UPDATE:			ret=__qc_ins_update_key(rbuf,len);		break;
		default:
		opRetCode=__15S_OP_VER_ERR;
		break;
	}
	return ret;
}

uint8_t* qc_comm_get_key(uint8_t* buf,uint8_t* key)
{
	st_frameHeaderDef* sth=(st_frameHeaderDef*)buf;
	uint8_t t8;
	uint8_t* ptmp;
	t8=sth->encryptType.bits.keyType;
	if(sth->keyIndex==0){
		switch(t8){
			case 0:ptmp=NULL;break;
			case 1:ptmp=(uint8_t*)userTransKeyDefault;break;
			case 2:ptmp=(uint8_t*)userServiceKeyDefault;break;
			case 3:ptmp=(uint8_t*)compTransKeyDefault;break;
			case 4:ptmp=(uint8_t*)compServiceKeyDefault;break;
			default:ptmp=(uint8_t*)compTransKeyDefault;break;
		}
	}else{
		switch(t8){
			case 0:ptmp=NULL;break;
			case 1:ptmp=(uint8_t*)(sysData.userTransKey);break;
			case 2:ptmp=(uint8_t*)(sysData.userServiceKey);break;
			case 3:ptmp=(uint8_t*)(sysData.compTransKey);break;
			case 4:ptmp=(uint8_t*)(sysData.compSeviceKey);break;
			default:ptmp=(uint8_t*)(sysData.compTransKey);break;
		}		
	}
	if(ptmp){
		m_mem_cpy_len(key,ptmp,8);
		ptmp=(uint8_t*)key;
	}
	return ptmp;
}


uint16_t qc_comm_received_process(uint8_t* buf,uint16_t len)
{
	//uint8_t t8;
	st_frameHeaderDef* sth=(st_frameHeaderDef*)buf;
	uint8_t* key;
	uint8_t keyBuf[8];
	uint16_t ret=0;//
	//uint16_t ret,t16;
	opRetCode=__15S_OP_NOTHING;
	protocolCmd=0xff;
	do{
		rxFrameIndex=sth->serNum;
		if(len<sizeof(st_comCmdHeaderDef)+sizeof(st_frameHeaderDef)){
			//特殊指令
			ret=qc_comm_down_package_analysis_short(buf,len);
			if(ret)break;
			ret=0;opRetCode=__15S_OP_NOTHING;
			break;
		}
		
		if(!qc_comm_down_package_is_mine(buf,len)){ret=0;break;}

		key=qc_comm_get_key(buf,keyBuf);
		ret=encrypt_decrypt_verification(false,buf,(uint8_t*)key,len);
		if(!ret){opRetCode=__15S_OP_KEY_UPDATE_ERR;ret=0;break;}

		//rxFrameIndex=sth->serNum;
		ret=qc_comm_down_package_analysis(buf,len);
		__nop();
		break;
	}while(0);
	if(ret>0 || opRetCode>__15S_OP_NOTHING){
		return ret>0?ret:1;
	}else{
		return 0;
	}
}

uint16_t qc_comm_send_pop_ready(uint8_t* sbuf,uint8_t ctrlCode)
{
	uint8_t* buf=sbuf;
	uint16_t t16;
	//if(seqno>1)seqno=1;
	//t16=__qc_comm_load_header(buf,__15E_CMD_POP,sizeof(st_popDef_Pkg0),ctrlCode);
	//buf+=t16;
	if(popPackageType==POP_TYPE_FIRST){
		t16=__qc_comm_load_header(buf,__15E_CMD_POP,sizeof(st_popDef_Pkg0),ctrlCode);
		buf+=t16;
		t16=t16+__qc_comm_load_pop_query_body_pkg0(buf);
	}else if(popPackageType==POP_TYPE_SECOND){
		t16=__qc_comm_load_header(buf,__15E_CMD_POP,sizeof(st_popDef_Pkg1),ctrlCode);
		buf+=t16;	
		t16=t16+__qc_comm_load_pop_query_body_pkg1(buf);
	}else{
		t16=__qc_comm_load_header(buf,__15E_CMD_POP_EX,sizeof(popPkgDefExShort_t),ctrlCode);
		buf+=t16;	
		t16=t16+__qc_comm_load_pop_ex(buf);
	}
	popPackageType=POP_TYPE_FIRST;
	encrypt_decrypt_verification(true,sbuf,(uint8_t*)toolTransportionKey,t16);
	__12e_crc_append(sbuf,t16);
	t16+=2;
	sbuf[t16]=rssi;//rssi
	t16++;	
	return t16;
}

uint16_t qc_comm_send_pop_ready_ex(uint8_t* sbuf,uint8_t ctrlCode)
{
	uint8_t* buf=sbuf;
	uint16_t t16;

	t16=__qc_comm_load_header(buf,__15E_CMD_POP,sizeof(st_popDef_Pkg0),ctrlCode);
	buf+=t16;
	
	if(popSequNm==0){
		t16=t16+__qc_comm_load_pop_query_body_pkg0(buf);
	}else{
		t16=t16+__qc_comm_load_pop_query_body_pkg1(buf);
	}
	
	encrypt_decrypt_verification(true,sbuf,(uint8_t*)toolTransportionKey,t16);
	__12e_crc_append(sbuf,t16);
	t16+=2;
	sbuf[t16]=rssi;//rssi
	t16++;	
	
	if(popSequNm==0){
		popSequNm=1;
	}else{
		popSequNm=0;
	}
	return t16;
}

uint16_t qc_comm_ins_reply_ready(uint8_t* sbuf,uint8_t ctrlCode)
{
	uint8_t* buf=sbuf;
	uint16_t t16;
	switch(protocolCmd){
		case __15E_CMD_STEP_PRICE_QUERY:
		case __15E_CMD_PAY_QUERY:
		case __15E_CMD_STATUS_QUERY:
			//t16=__qc_comm_load_header(buf,protocolCmd,sizeof(st_AckDef),ctrlCode);break;
		default:
			t16=__qc_comm_load_header(buf,__15E_CMD_ACK,sizeof(st_AckDef),ctrlCode);break;
	}
	
	buf+=t16;
	if(sysData.DWM== DWM_COMMON_MODE){
		t16=t16+__qc_comm_load_ack_body(buf);
	}else{
		switch(protocolCmd){
			case __20E_CMD_GET_MODULE_INF:		t16=t16+__qc_comm_load_module_inf_pkg(buf);	break;
			case __15E_CMD_STEP_PRICE_QUERY:	t16=t16+__qc_comm_load_step_price_query_pkg(buf);	break;
			case __15E_CMD_STEP_PRICE_QUERY_EX:	t16=t16+__qc_comm_load_step_price_query_pkg_ex(buf);	break;
			case __15E_CMD_PAY_QUERY:			t16=t16+__qc_comm_load_pay_query_pkg(buf);			break;
			case __15E_CMD_STATUS_QUERY:		t16=t16+__qc_comm_load_pop_query_body_pkg1(buf);	break;	
			default:t16=t16+__qc_comm_load_ack_body(buf);break;
			
		}
	}
	encrypt_decrypt_verification(true,sbuf,(uint8_t*)toolTransportionKey,t16);
	__12e_crc_append(sbuf,t16);
	t16+=2;
	sbuf[t16]=rssi;//rssi
	t16++;	
	
	return t16;	
}

//http://www.afwing.com/aircraft/an225-mriya-photos.html
//file end
