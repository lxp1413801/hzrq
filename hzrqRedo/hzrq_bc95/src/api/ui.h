#ifndef __UI_H__
#define __UI_H__
#ifdef __cplusplus
extern "C"{
#endif
	//extern uint8_t menu;
	#include <stdint.h>
	typedef struct{
		uint32_t 	significand;	
		uint8_t 	exponent;		
		uint8_t 	sign;			
	}uiFloat32_t;
	
	
	#include "stdint.h"

	//<<--mod by lxp
	#define MENU_HOME	0x00
	#define subMENU_HOME_MAIN		0x00
	#define subMENU_HOME_NB_CONN	0x01
	#define subMENU_HOME_NB_SEND	0x02
	#define subMENU_HOME_NB_REC		0x03
	#define subMENU_HOME_NB_END		0x04
	#define subMENU_HOME_VAVLE_OFF	0x05
	#define subMENU_HOME_VAVLE_ON	0x06
	//
	#define MENU_ID		0x01
	#define subMENU_ID_LO	0x00
	#define subMENU_ID_HI	0x01
	//
	#define MENU_PRICE 0x02
	#define subMENU_PRICE	0x00
	//
	#define MENU_CURRENT_VOL	0x03
	#define subMENU_CURRENT_VOL	0x00
	//
	#define MENU_BALANCE	0x04
	#define subMENU_BALANCE 0x00
	//
	#define MENU_TOTALE_VOL	0x05
	#define subMENU_TOTALE_VOL 0x00
	//rf信号
	#define MENU_RSSI	0x06
	#define subMENU_RSSI 0x00
	//dlcs
	#define MENU_DLCS 0x07
	#define subMENU_DLCS 0x00
	//qn 
	#define MENU_QN	0x08
	#define subMENU_QN 0x00
	//ver
	#define MENU_VER	0x09
	#define subMENU_VER	0x00s
	//imei
	#define MENU_IMEI	0x0a
	#define subMENU_IMEI 0x00
	//ccid
	#define MENU_CCID		0x0b
	#define subMENU_CCID	0x00
	//rtc YMD	
	#define MENU_RTC_YMD	0x0c
	#define subMENU_RTC_YMD 0x00
	//RTC HMS
	#define MENU_RTC_HMS	0x0d
	#define subMENU_RTC_HMS 0x00
	//szrq balance
	#define MENU_SZRQ_BALANCE	0x10
	#define subMENU_SZRQ_BALANCE 0x00
	//szrq overage vol
	#define MENU_SZRQ_OV_VOL	0x11
	#define subMENU_SZRQ_OV_VOL	0x00
	//ir
	#define MENU_IR 0x80
	#define subMENU_IR 0x00
	//-------------------------------------
	//-->>
	//extern uint8_t lcdLongStringHead;


	extern const uint8_t menuTableSzrq[13];
	extern const uint8_t menuTableComReader[13];
	extern const uint8_t menuTableVolMode[11];
	extern const uint8_t menuTableMoneryMode[14];
	
	extern volatile uint8_t menu; 
	extern volatile uint8_t subMenu;
	
	extern bool fi_id_writed_in_dlcs_a(void);

	extern void ui_disp_menu(void);
	extern void ui_disp_device_ver(void);
#ifdef __cplusplus
}
#endif
#endif
