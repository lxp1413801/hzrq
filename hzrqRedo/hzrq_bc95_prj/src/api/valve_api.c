//file name:valve_api.c
#include "./includes/includes.h"


void app_valve_off(void)
{
	uint8_t valveRuntime;

	//if(vavleState==VALVE_OFF || vavleState==VALVE_OPERATION_OFF)return;
	vavleState=VALVE_OPERATION_OFF;

    //lcd_clear_all();
    //m_lcd_disp_str_refresh(__xT("Off--"));   

	api_sysdata_save();
	//m_flow_data_init();
	m_flow_all_data_init();
	
	m_gpio_config_vavle();
	valveRuntime=sysData.VavleOffTime;
	valveRuntime=sysData.VavleOffTime;
	m_vavle_off();
	while(valveRuntime){
		osDelay(1000);
		valveRuntime--;
	}
	m_vavle_stop();	
	__nop();
	vavleState=VALVE_OFF;
}

void app_valve_on(void)
{
	uint16_t t16;
	uint8_t btsta;
	uint16_t valveRuntime;
    if(vavleState==VALVE_ON)return;
    
	//lcd_clear_all();
	//m_lcd_disp_str_refresh(__xT("On--"));    

	m_flow_all_data_init();
	valveRuntime=sysData.VavleOnTime;
    if(valveRuntime<3)valveRuntime=3;
    if(valveRuntime>10)valveRuntime=10;
	vavleState=VALVE_OPERATION_ON;
	m_gpio_config_vavle();
	m_vavle_on();
	
	t16=valveRuntime*10;
	while(t16){
		get_battary_rtadc();
		btsta=get_power_status();
		
		if(btsta!=POWER_STATUS_NORMAL){
			if(btsta<=POWER_STATUS_DOWN){
				VavleOffReason=OFF_REASON_LO_VOLETAGE;
				pwr_status_set_down();
				pwrStatus=POWER_STATUS_DOWN;
				//if(sysData.DLCS>=DLC_STATUS_C)lockReason.bits.bPowerDown=1;						
			}else{
				VavleOffReason=OFF_REASON_HI_VOLETAGE;
				pwrStatus=POWER_STATUS_OVER; 
				//if(sysData.DLCS>=DLC_STATUS_C)lockReason.bits.bPowerHight=1;				
			}
		}
		if(btsta != POWER_STATUS_NORMAL){
			app_valve_off();
			return;		
		}
		osDelay(100);
		t16--;
	}
	m_vavle_stop();
    vavleState=VALVE_ON;
}


void vavle_off_for_start(void)
{
	m_gpio_config_vavle();
	m_vavle_off();
	osDelay(500);
	m_vavle_stop();	
	vavleState=VALVE_OFF;

}
