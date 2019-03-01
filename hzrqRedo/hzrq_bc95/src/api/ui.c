#include "./includes/includes.h"
#ifndef cfg_IRDA_EN
#define cfg_IRDA_EN 1
#endif
#define LCD_DIG_NUM 8

volatile uint8_t menu=MENU_HOME; 
volatile uint8_t subMenu=subMENU_HOME_MAIN; 
const uint8_t hexTable[]="0123456789abcdef";
uint8_t lcdBuf[16]={0};
//uint8_t menu=0x00
//<<--add for menu

const uint8_t menuTableSzrq[13]={
	MENU_HOME,
	MENU_ID,
	MENU_TOTALE_VOL,
	MENU_RSSI,
	MENU_DLCS,
	MENU_QN,
	MENU_VER,
	MENU_IMEI,
	MENU_CCID,
	MENU_RTC_YMD,
	MENU_RTC_HMS,
	MENU_SZRQ_BALANCE,
	MENU_SZRQ_OV_VOL,
};
const uint8_t menuTableComReader[13]={
	MENU_HOME,
	MENU_ID,
	MENU_TOTALE_VOL,
	MENU_RSSI,
	MENU_DLCS,
	MENU_QN,
	MENU_VER,
	MENU_IMEI,
	MENU_CCID,
	MENU_RTC_YMD,
	MENU_RTC_HMS,
	MENU_PRICE,
	MENU_BALANCE,
};
const uint8_t menuTableVolMode[11]={
	MENU_HOME,
	MENU_ID,
	MENU_TOTALE_VOL,
	MENU_RSSI,
	MENU_DLCS,
	MENU_QN,
	MENU_VER,
	MENU_IMEI,
	MENU_CCID,
	MENU_RTC_YMD,
	MENU_RTC_HMS,
};
const uint8_t menuTableMoneryMode[14]={
	MENU_HOME,
	MENU_ID,
	MENU_PRICE,
	MENU_CURRENT_VOL,
	MENU_BALANCE,
	MENU_TOTALE_VOL,
	MENU_RSSI,
	MENU_DLCS,
	MENU_QN,
	MENU_VER,
	MENU_IMEI,
	MENU_CCID,
	MENU_RTC_YMD,
	MENU_RTC_HMS,
};

//-->>
bool fi_id_writed_in_dlcs_a(void)
{
	return (sysData.ID>10);
} 

void ui_mf_2_str(uint8_t* str,uiFloat32_t* mf,uint8_t digNum,uint8_t dpScale)
{
	if(digNum>10)return;
	uint32_t x=mf->significand;
	uint32_t t;
	uint16_t i=0;
	str[digNum]='\0';
	while(digNum>0){
		t=x%10;
		str[digNum-1]=hexTable[t];
		i++;
		x/=10;
		digNum--;
		if(x==0)break;
	}
	while(i<=dpScale){
		str[digNum-1]='0';
		i++;
		digNum--;
		if(digNum==0)break;
	}
	if(mf->sign && digNum>0){
		str[digNum-1]='-';
		digNum--;
	}
	while(digNum>0){
		str[digNum-1]=' ';
		digNum--;		
	}

	//str[digNum]='\0';
}


int16_t __int32_2_mflot32(int32_t x,uiFloat32_t* mf,uint16_t digNum,uint16_t dpScale)
{
	uint32_t tt=0;
	uint32_t t32;
	uint32_t limit;
	
	limit=m_math_pow(10,digNum);
	limit-=1;
	
	mf->significand=0L;
	mf->exponent=0;
	mf->sign=0;
	
	t32=x;
	if(x<0){
		t32 = ((~x) +1);
		mf->sign=1;
		limit/=10;
	}
	while(t32>limit){
		mf->exponent++;
		tt=t32%10;
		t32/=10;
		if( mf->exponent>=dpScale)break;
	}
	if(tt>=5)t32++;
	if(t32>limit)t32=limit;
	mf->significand=t32;
	return 0;
}

int32_t	__mflot32_2_int32(uiFloat32_t* mf)
{
    uint8_t i;
    int32_t ret=0;
	//mf.t32=x;
    ret=mf->significand;
    for(i=0;i< mf->exponent;i++){
        ret*=10;
    }
    if(mf->sign)ret=((~ret)+1);
	return ret;
}


void __ui_disp_int32_to_float(int32_t x,uint8_t dpScale)
{
	//dpScale
	uint8_t str[10];
	//uint8_t dp=dpScale;
	uiFloat32_t mf;
	
	__int32_2_mflot32(x,&mf,LCD_DIG_NUM,dpScale);
	ui_mf_2_str(str,&mf,LCD_DIG_NUM,dpScale);
	m_lcd_disp_str(str);
	if(dpScale>=mf.exponent){
		dpScale-=mf.exponent;
	}else{
		dpScale=0;
	}
	m_lcd_disp_dp(dpScale);
}



void ui_disp_comm_rssi_segment(uint8_t csq)
{
	if(csq>=25){
		m_lcd_disp_seg_rssi_0();
		m_lcd_disp_seg_rssi_1();
		m_lcd_disp_seg_rssi_2();	
		m_lcd_disp_seg_rssi_3();	
	}else if(csq>=17){
		m_lcd_disp_seg_rssi_0();
		m_lcd_disp_seg_rssi_1();
		m_lcd_disp_seg_rssi_2();
	
	}else/* if(csq>=10)*/{
		m_lcd_disp_seg_rssi_0();
		m_lcd_disp_seg_rssi_1();	
	}/*else{
		m_lcd_disp_seg_rssi_0();

	}*/
}

void ui_disp_comm_battery_segment(void)
{
	#if(BATTARY_TYPE==BATTARY_TYPE_LI_SOCL)
	m_lcd_disp_bat_level0();
	if(voltBat>=2500)m_lcd_disp_bat_level1();
	if(voltBat>=2900)m_lcd_disp_bat_level2();
	if(voltBat>=3100)m_lcd_disp_bat_level3();
	if(voltBat>=3300)m_lcd_disp_bat_level4();
	if(voltBat>=3500)m_lcd_disp_bat_level5();
	#else
	m_lcd_disp_bat_level0();
	if(voltBat>=3300)m_lcd_disp_bat_level1();
	if(voltBat>=3600)m_lcd_disp_bat_level2();
	if(voltBat>=3900)m_lcd_disp_bat_level3();
	if(voltBat>=4200)m_lcd_disp_bat_level4();
	if(voltBat>=4400)m_lcd_disp_bat_level5();		
	#endif
}



void ui_hard_default_disp(void)
{
    if(hardDefault.t32==0)return;
    //lcd_enable();
    lcd_clear_all();    
    do{
        if(hardDefault.bits.ClockErr){
            m_lcd_disp_str(__xT("fau-01"));
            break;
        }
        if(hardDefault.bits.FlashErr){
            m_lcd_disp_str(__xT("fau-02"));
            break;
        }
        if(hardDefault.bits.EepromErr){
            m_lcd_disp_str(__xT("fau-03"));
            break;
        }
        if(hardDefault.bits.RtcErr){
            m_lcd_disp_str(__xT("fau-04"));
            break;
        }
        if(hardDefault.bits.ProxErr){
            m_lcd_disp_str(__xT("fau-05"));
            break;
        }

    }while(0);
}


#define ui_disp_strong_magnetic() \
	{if(sysData.devStatus.bits.bStrongMagnetic)m_lcd_disp_seg_ste();}
	
	
	


void ui_disp_buy_gas_prompt(void)
{
	if(sysData.devStatus.bits.bBalanceSta==CNM_BALANCE_OFF){
		m_lcd_disp_seg_owe();
	}	
	else if(sysData.devStatus.bits.bBalanceSta!=CNM_BALANCE_NORMAL){
		//m_lcd_disp_seg_QGQ_I();
		m_lcd_disp_seg_pls_recharge();
	}
}


void ui_disp_pay_value(void)
{

	uint32_t tm;
	int32_t t32;
	lcd_clear_all(); 
	if(sysData.DLCS>=DLC_STATUS_C){
		t32=realPayValue;
	}else{
		t32=sysData.OVerageVM;
	}
	if(tm & 0x01ul){
		__ui_disp_int32_to_float(t32,2);
		m_lcd_disp_seg_current_buy_m();
	}else{
	}
	
}

void ui_disp_overage_v(void)
{

	int32_t ov=overrageVolume;
	lcd_clear_all(); 
	__ui_disp_int32_to_float(ov,2);
	m_lcd_disp_seg_balance_vol();
	
}

void ui_disp_overage_m(void)
{

	//uint16_t i;
	int32_t om,t32;
	lcd_clear_all(); 
	t32= OVerageVMRemainder+sysData.OVerageVMRemainder;
	t32=t32 / 10000;
	om=overageMoney - t32;
	__ui_disp_int32_to_float(om,2);
	m_lcd_disp_seg_balance_m();	

}

void ui_disp_off_reason(void)
{
	uint8_t str[10];
	uint8_t t8=VavleOffReason;
	switch(t8){
		//case OFF_REASON_NONE:
		case OFF_REASON_HARD_DEFAULT:	m_lcd_disp_str((uint8_t*)"er-Off");break;
		case OFF_REASON_LO_VOLETAGE:	m_lcd_disp_str((uint8_t*)"lb-Off");break;
		case OFF_REASON_HI_VOLETAGE:	m_lcd_disp_str((uint8_t*)"hb-Off");break;
		case OFF_REASON_NO_GAS_FLOW:	m_lcd_disp_str((uint8_t*)"nf-Off");break;
		case OFF_REASON_SAMLL_GAS_FLOW:	m_lcd_disp_str((uint8_t*)"sf-Off");break;
		case OFF_REASON_HIGHT_GAS_FLOW:	m_lcd_disp_str((uint8_t*)"hf-Off");break;
		case OFF_REASON_CONTINUOUS_FLOW:m_lcd_disp_str((uint8_t*)"cf-Off");break;		
		case OFF_REASON_LOW_OV:		m_lcd_disp_str((uint8_t*)"lg-Off");break;
		case OFF_REASON_NO_OV:		m_lcd_disp_str((uint8_t*)"ng-Off");break;		
		case OFF_REASON_STRONG_MAGTIC:	
			m_str_cpy(str,(uint8_t*)" st-Off");
			str[0]=(uint8_t)(sysData.lockReason.bits.bStrongMagnetic)+'0';
			m_lcd_disp_str(str);
			break;	
		case OFF_REASON_EXTERN_SIG:		m_lcd_disp_str((uint8_t*)"et-Off");break;		
		case OFF_REASON_SEVERS:			m_lcd_disp_str((uint8_t*)"se-Off");break;
		case OFF_REASON_MANUAL:			m_lcd_disp_str((uint8_t*)"tstOff");break;
		case OFF_REASON_LEAKAGE:		m_lcd_disp_str((uint8_t*)"Gas-ala");break;
		case OFF_REASON_CZSN_NONET:		m_lcd_disp_str((uint8_t*)"nc-Off");break;
		case OFF_REASON_CZSN_WARNING:	m_lcd_disp_str((uint8_t*)"pr-Off");break;
		case OFF_REASON_SHELL_OPEN:		m_lcd_disp_str((uint8_t*)"sh-Off");break;
		default	:						m_lcd_disp_str((uint8_t*)" press");break;
	}
    
    //if(t8==OFF_REASON_HIGHT_GAS_FLOW)
        //m_lcd_disp_seg_GLBH_E();
}


#if SEND_TIMES_TEST
void ui_disp_send_times(void)
{
	uint32_t t32;
	lcd_clear_all(); 
	lcdBuf[7]='\0';
	//x>>=2;
	t32=nbPopTimes;
	ui_uint32_2_str((uint8_t*)(lcdBuf+sizeof(lcdBuf)-2),t32,7);	
	m_lcd_disp_str(lcdBuf);
}
#endif

void ui_disp_device_ver(void)
{

	uint8_t* p;
	p=(uint8_t*)(&sysData);
	p+=get_offset_addr_of_str(stDeviceInf_t,DeviceName);
	m_mem_cpy(lcdBuf,p);
	lcd_clear_all(); 
	m_lcd_disp_str(lcdBuf);	
	//m_lcd_refresh();
}

void ui_disp_off(void)
{
    lcd_clear_all(); 
	uint8_t tmtry=sysData.qcNoTryTimes;
	if(tmtry==1)tmtry=2;
	if((sysData.lockReason.bits.bNoflow>=tmtry && tmtry!=0) || (sysData.lockReason.bits.bStrongMagnetic>=MAX_STE_OFF_TIMES) \
	|| sysData.lockReason.bits.bSeverOff || sysData.lockReason.bits.bNoNetwork || sysData.lockReason.bits.bBalance \
	|| sysData.lockReason.bits.bShellOpen ){
		//m_lcd_disp_str((uint8_t*)"   hold");
		if(sysData.lockReason.bits.bNoflow>=tmtry && tmtry!=0){m_lcd_disp_str((uint8_t*)"nf-loc ");}
		else if(sysData.lockReason.bits.bStrongMagnetic>=MAX_STE_OFF_TIMES){
			m_lcd_disp_str((uint8_t*)"st-loc ");
		}
        else if(sysData.lockReason.bits.bSeverOff){m_lcd_disp_str((uint8_t*)"se-loc ");}
		else if(sysData.lockReason.bits.bNoNetwork){m_lcd_disp_str((uint8_t*)"nc-loc ");}
		else if(sysData.lockReason.bits.bShellOpen){m_lcd_disp_str((uint8_t*)"sh-loc ");}
		else if(sysData.lockReason.bits.bBalance){m_lcd_disp_str((uint8_t*)"ng-loc ");}
		
	}else{
		ui_disp_off_reason();
	}	
	//m_lcd_refresh();	
}


void ui_disp_seg_valve_status(void)
{
	uint32_t t32;
	t32=globleTickerSec;
	uint8_t t8=vavleState;
	switch(t8){
		case VALVE_OFF:
		case VALVE_LOC:
			m_lcd_disp_seg_valve_close();break;
		case VALVE_ON:
			m_lcd_disp_seg_valve_open();break;
		case VALVE_OPERATION_OFF:
			if(t32&0x01L){m_lcd_disp_seg_valve_close();}
			break;
		case VALVE_OPERATION_ON:
			if(t32&0x01L){m_lcd_disp_seg_valve_open();}
			break;	
		case VALVE_FAULT:
			m_lcd_disp_seg_valve_close();
			m_lcd_disp_seg_valve_open();
			break;
		default:break;
	}
}

void ui_disp_seg_shell_open(void)
{
	if(sysData.devStatus.bits.bShellOpenB)m_lcd_disp_seg_open_shell();
}

void ui_disp_menu_id(void);
void ui_disp_menu_rssi(void);
void ui_disp_menu_totale_vol(void);
void __ui_disp_menu_home_main(void)
{
	uint32_t t32;
	if(sysData.DLCS==DLC_STATUS_A){

		t32=globleTickerSec;
		t32>>=1;
		t32%=3;
		switch(t32){
			case 0x00:ui_disp_menu_id();break;
			case 0x01:ui_disp_overage_v();break;
			case 0x02:ui_disp_menu_rssi();break;
		}
	}else{
		if(vavleState==VALVE_ON){
			if(sysData.DWM==DWM_VOLUME_MODE){
				ui_disp_overage_v();
			}else if(sysData.DWM==DWM_COMMON_MODE){
				ui_disp_menu_totale_vol();
			}else{
				ui_disp_overage_m();
			}
		}
		else if(vavleState==VALVE_OFF)
		{
			ui_disp_off();
		}	
	}
}

void __ui_disp_menu_home_rf_conn(void)
{
	uint16_t t16;
	uint32_t tm=globleTickerSec;
	m_mem_set(lcdBuf,'\0',sizeof(lcdBuf));
	
	lcd_clear_all();
	t16=(uint16_t)(tm % 0x04ul);
	m_mem_cpy_len(lcdBuf,(uint8_t*)" conn---",5+t16);
	lcdBuf[t16+5]='\0';
	m_lcd_disp_str(lcdBuf); 	
}

void __ui_disp_menu_home_rf_send(void)
{
	uint16_t t16;
	uint32_t tm=globleTickerSec;
	m_mem_set(lcdBuf,'\0',sizeof(lcdBuf));

	lcd_clear_all();
	t16=(uint16_t)(tm % 0x04ul);
	m_mem_cpy_len(lcdBuf,(uint8_t*)" send---",5+t16);
	lcdBuf[t16+5]='\0';
	m_lcd_disp_str(lcdBuf);
	//m_lcd_refresh();   
}

void __ui_disp_menu_home_rf_rec(void)
{
	uint16_t t16;
	uint32_t tm=globleTickerSec;
	m_mem_set(lcdBuf,'\0',sizeof(lcdBuf));
	lcd_clear_all();
	t16=(uint16_t)(tm % 0x04ul);
	m_mem_cpy_len(lcdBuf,(uint8_t*)"  rec---",5+t16);
	lcdBuf[t16+5]='\0';
	m_lcd_disp_str(lcdBuf);
}

void __ui_disp_menu_home_rf_end(void)
{
	uint16_t t16;
	uint32_t tm=globleTickerSec;
	m_mem_set(lcdBuf,'\0',sizeof(lcdBuf));

	lcd_clear_all();
	t16=(uint16_t)(tm % 0x04ul);
	m_mem_cpy_len(lcdBuf,(uint8_t*)"  end---",5+t16);
	lcdBuf[t16+5]='\0';
	m_lcd_disp_str(lcdBuf);
}

void __ui_disp_menu_home_vavle_on(void)
{
	uint16_t t16;
	uint32_t tm=globleTickerSec;
	m_mem_set(lcdBuf,'\0',sizeof(lcdBuf));

	lcd_clear_all();
	t16=(uint16_t)(tm % 0x04ul);
	m_mem_cpy_len(lcdBuf,(uint8_t*)" open---",5+t16);
	lcdBuf[t16+5]='\0';
	m_lcd_disp_str(lcdBuf);  
}

void __ui_disp_menu_home_vavle_off(void)
{
	uint16_t t16;
	uint32_t tm=globleTickerSec;
	m_mem_set(lcdBuf,'\0',sizeof(lcdBuf));

	lcd_clear_all();
	t16=(uint16_t)(tm % 0x04ul);
	m_mem_cpy_len(lcdBuf,(uint8_t*)"close---",5+t16);
	lcdBuf[t16+5]='\0';
	m_lcd_disp_str(lcdBuf); 
}

void ui_disp_menu_home(void)
{
	lcd_clear_all(); 
	if(payPromptTimeOut){
		ui_disp_pay_value();
		return ;
	}
	
	switch(subMenu){
		case subMENU_HOME_MAIN:			__ui_disp_menu_home_main();			break;
		case subMENU_HOME_NB_CONN:		__ui_disp_menu_home_rf_conn();		break;
		case subMENU_HOME_NB_SEND:		__ui_disp_menu_home_rf_send();		break;
		case subMENU_HOME_NB_REC:		__ui_disp_menu_home_rf_rec();		break;
		case subMENU_HOME_NB_END:		__ui_disp_menu_home_rf_end();		break;
		case subMENU_HOME_VAVLE_OFF:	__ui_disp_menu_home_vavle_off();	break;
		case subMENU_HOME_VAVLE_ON:		__ui_disp_menu_home_vavle_on();		break;
	}	
}

void ui_disp_menu_id(void)
{
	uint32_t t32;
	lcd_clear_all(); 
	lcdBuf[8]='\0';
	m_str_cpy(lcdBuf,sysData.commSN+8);
	m_lcd_disp_str(lcdBuf);
	m_lcd_disp_seg_device_id();
}

void ui_disp_menu_price(void)
{
	int32_t pc;
	lcd_clear_all(); 
	pc=sysData.curPrice;
	 
	__ui_disp_int32_to_float(pc,4);
	m_lcd_disp_seg_price();
}

void ui_disp_menu_current_vol(void)
{

	uint32_t cv;
	lcd_clear_all();
	cv=totalVolume-sysData.periodVolume;
	__ui_disp_int32_to_float(cv,2);
	m_lcd_disp_seg_T18_M3();
}

void ui_disp_menu_balance(void)
{
	int32_t ovm,t32;
	lcd_clear_all(); 
	if(sysData.DWM==DWM_VOLUME_MODE){
		ovm=overrageVolume;
		//lcd_clear_all(); 
		__ui_disp_int32_to_float(ovm,2);
		m_lcd_disp_seg_balance_vol();		
	}else{
		//lcd_clear_all(); 
		t32= OVerageVMRemainder+sysData.OVerageVMRemainder;
		t32=t32 / 10000;
		ovm=overageMoney - t32;
		__ui_disp_int32_to_float(ovm,2);
		m_lcd_disp_seg_balance_m();			
	}
}

void ui_disp_menu_totale_vol(void)
{
	uint32_t tov;
	lcd_clear_all();
	tov=totalVolume;
	__ui_disp_int32_to_float(tov,2);
	m_lcd_disp_seg_total_v();	
}

void ui_disp_menu_rssi(void)
{
	int8_t t8;

	uint32_t t32;
	t32=globleTickerSec>>1;
	t32 &= 0x01ul;	
	lcd_clear_all();
	if(t32==0){
		t8=rssi;	
		if(t8>0)t8=-1;
		t8=0-t8;
		
		m_mem_set(lcdBuf,0,sizeof(lcdBuf));
		if(t8>99){
			m_mem_cpy_len(lcdBuf,__xT(" co:-"),5);
			lcdBuf[5]='0'+t8/100;
			t8%=100;
			lcdBuf[6]='0'+t8/10;
			lcdBuf[7]='0'+t8%10;
			lcdBuf[8]='\0';
		}else{
			m_mem_cpy_len(lcdBuf,__xT(" co: -"),6);
			lcdBuf[6]='0'+t8/10;
			lcdBuf[7]='0'+t8%10;
			lcdBuf[8]='\0';
		}
	}else{
		if(EARFCN[0]=='\0'){
			m_mem_cpy(lcdBuf,(uint8_t*)" unread ");
		}else{
			lcdBuf[0]='f';
			m_mem_cpy_len(lcdBuf+1,(uint8_t*)EARFCN,7);
			lcdBuf[8]='\0';
		}
	}
	m_lcd_disp_str(lcdBuf);
}

void ui_disp_menu_dlcs(void)
{
    lcd_clear_all(); 
    switch(sysData.DLCS){
        case DLC_STATUS_A:m_lcd_disp_str(__xT("   s:a"));break;
        case DLC_STATUS_B:m_lcd_disp_str(__xT("   s:b"));break;
        case DLC_STATUS_C:m_lcd_disp_str(__xT("   s:c"));break;
        case DLC_STATUS_D:m_lcd_disp_str(__xT("   s:d"));break;
        case DLC_STATUS_E:m_lcd_disp_str(__xT("   s:e"));break;
    }
}

void ui_disp_menu_qn(void)
{	
	uint16_t _qn;
	_qn=sysData.QN;
	lcd_clear_all(); 
	switch(_qn){
        case QN10: 		m_lcd_disp_str(__xT("   qn:10"));m_lcd_disp_seg_dp1();	break;
        case QN16: 		m_lcd_disp_str(__xT("   qn:16"));m_lcd_disp_seg_dp1();	break;           
        case QN25: 		m_lcd_disp_str(__xT("   qn:25"));m_lcd_disp_seg_dp1();	break;
        case QN40: 		m_lcd_disp_str(__xT("   qn:4 "));						break;            
        case QN60: 		m_lcd_disp_str(__xT("   qn:6 "));						break;
        case QN100: 	m_lcd_disp_str(__xT("  qn:10 "));						break;            
        case QN160: 	m_lcd_disp_str(__xT("  qn:16 "));						break;
        case QN250: 	m_lcd_disp_str(__xT("  qn:25 "));						break;            
        case QN400: 	m_lcd_disp_str(__xT("  qn:40 "));						break;
		default :		m_lcd_disp_str(__xT("  qn:err"));						break;
	}
}

void ui_disp_menu_ver(void)
{
	uint32_t t32;
	uint8_t* p;
	uint16_t t16;
	t32=globleTickerSec;//>>2;
	t32>>=2;
	t32%=2;
	switch(t32){
		case 0x00:
			lcdBuf[0]='b';
			lcdBuf[1]='n';
			lcdBuf[2]='b';
			
			t16=NB20E11_SUB_VER;
			t16%=100;
			lcdBuf[3]=(uint8_t)((t16/10)+'0');
			lcdBuf[4]=(uint8_t)((t16%10)+'0');
			
			lcdBuf[5]=(uint8_t)(config_NB_PLAT+'1');
			
			t16=sw_VER_NUM;
			t16%=100;
			lcdBuf[6]=(uint8_t)((t16/10)+'0');
			lcdBuf[7]=(uint8_t)((t16%10)+'0');			
			
			lcdBuf[8]='\0';
			
			break;
		case 0x01:
			p=SPVER;
			if(*p=='\0'){
				m_mem_cpy(lcdBuf,(uint8_t*)"  unread");
			}else{
				m_mem_cpy_len(lcdBuf,p,6);
				lcdBuf[6]='\0';
			}
			break;
			/*
		case 0x02:
			p=CGSN+8;
			if(*p=='\0'){
				m_mem_cpy(lcdBuf,(uint8_t*)"unread");
			}else{
				m_mem_cpy_len(lcdBuf,p,7);
				lcdBuf[7]='\0';
			}			
			break;
			*/
	}
	
	lcd_clear_all(); 
	m_lcd_disp_str(lcdBuf);	
	if(t32==0x00ul){
		//m_lcd_disp_seg_dp2();
		m_lcd_disp_seg_dp2();
		m_lcd_disp_seg_dp3();
	}
	//m_lcd_refresh();
}

void ui_disp_menu_imei(void)
{
	uint32_t t32;
	t32=globleTickerSec;//>>2;	
	t32>>=2;
	m_mem_set(lcdBuf,'\0',sizeof(lcdBuf));
	if(CGSN[0]=='\0'){
		//m_mem_cpy_len(lcdBuf,CGSN,8);
		m_mem_cpy(lcdBuf,(uint8_t*)"  unread");
	}else{
		if(t32 & 0x01UL){
			m_mem_cpy_len(lcdBuf,CGSN,8);
		}else{
			m_mem_cpy_len(lcdBuf+1,CGSN+8,7);
			lcdBuf[0]='-';
		}			
	}
	lcd_clear_all(); 
	m_lcd_disp_str(lcdBuf);		
}

void ui_disp_menu_ccid(void)
{
	uint32_t t32;
	t32=globleTickerSec;//>>2;	
	t32>>=2;
	m_mem_set(lcdBuf,'\0',sizeof(lcdBuf));
	if(ICCID[0]=='\0'){
		//m_mem_cpy_len(lcdBuf,CGSN,8);
		m_mem_cpy(lcdBuf,(uint8_t*)"  unread");
	}else{
		t32%=3;
		if(t32 == 0x00ul){
			m_mem_cpy_len(lcdBuf,ICCID,7);
			lcdBuf[7]='-';
		}else if(t32==1){
			m_mem_cpy_len(lcdBuf+1,ICCID+7,6);
			lcdBuf[0]='-';
			lcdBuf[7]='-';
		}else{
			m_mem_cpy_len(lcdBuf+1,ICCID+13,7);
			lcdBuf[0]='-';
			//lcdBuf[7]='-';			
		}			
	}
	lcd_clear_all(); 
	m_lcd_disp_str(lcdBuf);		
}

void ui_disp_menu_YMD(void)
{
	uint8_t t8;
	m_mem_set(lcdBuf,'\0',sizeof(lcdBuf));

	t8=sysRtcDataTime.YY;

	lcdBuf[0]=hexTable[t8/16];
	lcdBuf[1]=hexTable[t8%16];
	lcdBuf[2]='-';

	t8=sysRtcDataTime.MM;
	lcdBuf[3]=hexTable[t8/16];
	lcdBuf[4]=hexTable[t8%16];	
	lcdBuf[5]='-';
	t8=sysRtcDataTime.DD;
	lcdBuf[6]=hexTable[t8/16];
	lcdBuf[7]=hexTable[t8%16];	
	lcd_clear_all(); 
	m_lcd_disp_str(lcdBuf);		
}

void ui_disp_menu_hms(void)
{
	uint8_t t8;
	m_mem_set(lcdBuf,'\0',sizeof(lcdBuf));

	t8=sysRtcDataTime.hh;

	lcdBuf[0]=hexTable[t8/16];
	lcdBuf[1]=hexTable[t8%16];
	lcdBuf[2]=' ';
	m_lcd_disp_seg_dp5();
	t8=sysRtcDataTime.mm;
	lcdBuf[3]=hexTable[t8/16];
	lcdBuf[4]=hexTable[t8%16];	
	lcdBuf[5]=' ';
	t8=sysRtcDataTime.ss;
	lcdBuf[6]=hexTable[t8/16];
	lcdBuf[7]=hexTable[t8%16];	
	lcd_clear_all(); 
	m_lcd_disp_str(lcdBuf);		
	m_lcd_disp_seg_oclock();
}

void ui_disp_menu_szrq_balance(void)
{

	int32_t om;
	lcd_clear_all(); 
	om=sysData.szrqBalance;
	__ui_disp_int32_to_float(om,2);
	m_lcd_disp_seg_balance_m();

}

void ui_disp_menu_szrq_ov_vol(void)
{
	int32_t ov;
	lcd_clear_all(); 
	ov=sysData.szrqBalanceVol;
	__ui_disp_int32_to_float(ov,2);
	m_lcd_disp_seg_balance_vol();
}


void ui_disp_menu_ir(void)
{
	m_mem_cpy(lcdBuf,(uint8_t*)"ir----");
	lcd_clear_all(); 
	m_lcd_disp_str(lcdBuf);		
}

extern bool nbRun;
void ui_disp_menu(void)
{
	//显示充值量
	m_lcd_enable();
	switch(menu){
		case MENU_HOME:				ui_disp_menu_home();		break;
		case MENU_ID:				ui_disp_menu_id();			break;
		case MENU_PRICE:			ui_disp_menu_price();		break;
		case MENU_CURRENT_VOL:		ui_disp_menu_current_vol();	break;
		case MENU_BALANCE:			ui_disp_menu_balance();		break;
		case MENU_TOTALE_VOL:		ui_disp_menu_totale_vol();	break;
		case MENU_RSSI:				ui_disp_menu_rssi();		break;
		case MENU_DLCS:				ui_disp_menu_dlcs();		break;
		case MENU_QN:				ui_disp_menu_qn();			break;
		case MENU_VER:				ui_disp_menu_ver();			break;
		case MENU_IMEI:				ui_disp_menu_imei();		break;
		case MENU_CCID:				ui_disp_menu_ccid();		break;
		case MENU_RTC_YMD:			ui_disp_menu_YMD();			break;
		case MENU_RTC_HMS:			ui_disp_menu_hms();			break;
		case MENU_SZRQ_BALANCE:		ui_disp_menu_szrq_balance();break;
		case MENU_SZRQ_OV_VOL:		ui_disp_menu_szrq_ov_vol();	break;
		//
		case MENU_IR:				ui_disp_menu_ir();			break;
		default:break;
	}
	ui_disp_seg_shell_open();
	ui_disp_seg_valve_status();
    //ui_disp_voltage_promit();
	ui_disp_comm_battery_segment();
	
	if((nbRun && (globleTickerSec & 1UL)) || !nbRun){
		ui_disp_comm_rssi_segment(rssi);
		
	}
    ui_disp_buy_gas_prompt();
    ui_disp_strong_magnetic();
    //ui_disp_nf_prompt();
	//ui_disp_alarm();
	m_lcd_refresh();	
}
