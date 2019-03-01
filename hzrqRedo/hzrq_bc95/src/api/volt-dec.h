#ifndef __VOLT_DEC_H__
#define __VOLT_DEC_H__

	#ifdef __cplusplus
	extern "C"{
	#endif
	#include <stdint.h>
	/*
    typedef enum
	{		
		POWER_STATUS_DOWN=0,
        POWER_STATUS_LOW=1,
        POWER_STATUS_NORMAL=2,		
		POWER_STATUS_OVER=3,
        POWER_STATUS_OVER_OFF=4,
	}em_PwrStatusDef,pwrStatus_t;
*/
	//extern pwrStatus_t em_PwrStatus;
	extern uint8_t pwrStatus;
    extern uint8_t pwrArr[4];//={POWER_STATUS_NORMAL};    
	
    #define pwr_status_shift() do{ \
        pwrArr[0]=pwrArr[1]; \
        pwrArr[1]=pwrArr[2]; \
        pwrArr[2]=pwrArr[3]; \
        pwrArr[3]=pwrStatus;  \
        }while(0);	
/*
	#define if_pwr_status_normal() ( \
		pwrArr[0]==POWER_STATUS_NORMAL && \
		pwrArr[1]==POWER_STATUS_NORMAL && \
		pwrArr[2]==POWER_STATUS_NORMAL && \
		pwrArr[3]==POWER_STATUS_NORMAL)
*/
	#define if_pwr_status_normal() (pwrStatus==POWER_STATUS_NORMAL)
	#define pwr_status_set_down() do{ \
		pwrArr[0]=POWER_STATUS_DOWN; \
		pwrArr[1]=POWER_STATUS_DOWN; \
		pwrArr[2]=POWER_STATUS_DOWN; \
		pwrArr[3]=POWER_STATUS_DOWN; \
		pwrStatus=POWER_STATUS_DOWN; \
		}while(0);

	#define voltage_vdd_status_nomal() (voltAvdd>1900)

	
    extern uint8_t em_PwrStatusPrevious;//=POWER_STATUS_NORMAL;

	extern uint8_t get_power_status(void);
	extern uint32_t get_battary_rtadc(void);
	extern uint32_t get_avdd_rtadc(void);
	extern uint32_t get_internal_vref_rtadc(void);
	
	#ifdef __cplusplus
	}
	#endif
#endif
