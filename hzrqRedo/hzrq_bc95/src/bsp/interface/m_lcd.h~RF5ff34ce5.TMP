#ifndef __M_LCD_H__
#define __M_LCD_H__


	#ifdef __cplusplus
	extern "C" {
	#endif
	/* Includes ------------------------------------------------------------------*/
	#include "stm32l0xx_hal.h"
	#define __mySfr_LCD_CR (*( ( volatile uint32_t *)0X40002400))
	#define __mySfr_LCD_SR (*( ( volatile uint32_t *)0X40002400 + 0x0008))
	/* Define for scrolling sentences*/
	#define SCROLL_SPEED  	200
	#define SCROLL_SPEED_L  400
	#define SCROLL_NUM    	10
		
	/* Define for character '.' */
	#define POINT_OFF      0
	#define POINT_ON       1

	/* Define for caracter ":" */
	#define COLUMN_OFF     0
	#define COLUMN_ON      1

	/* Element values correspond to LCD Glass BAR Level. */
	#define BARLEVEL_OFF    0
	#define BARLEVEL_1_4    1
	#define BARLEVEL_1_2    2
	#define BARLEVEL_3_4    3
	#define BARLEVEL_FULL   4		

	/*
	//for msp430f4152
	#define LCD_DSEG_A 0x01
	#define LCD_DSEG_B 0x02
	#define LCD_DSEG_C 0x04
	
	
	#define LCD_DSEG_F 0x10
	#define LCD_DSEG_G 0x20
	#define LCD_DSEG_E 0x40
	#define LCD_DSEG_D 0x80
  
	#define LCD_DSEG_DP 0x08
	*/
	//What the fuck are you doing, dickhead
	// for stm32l073xx
	#define LCD_DSEG_A 0x80
	#define LCD_DSEG_F 0x40
	#define LCD_DSEG_B 0x20
	#define LCD_DSEG_G 0x10
	
	
	#define LCD_DSEG_E 0x08
	#define LCD_DSEG_C 0x04
	#define LCD_DSEG_D 0x02
	//#define LCD_DSEG_DP 0x80
  
	//#define LCD_DSEG_DP 0x08	
	#define LCD_SSEG_YUE_A1 0x01
	#define LCD_SSEG_YQJ_A2 0x02
	#define LCD_SSEG_DANJIA_B 0x08
	#define LCD_SSEG_GYBH_C 0x04
	#define LCD_SSEG_DYBH_D 0x08
	#define LCD_SSEG_GLBH_E 0x08
	#define LCD_SSEG_CSBH_F 0x80
	#define LCD_SSEG_QCBH_G 0x40
	#define LCD_SSEG_KYC_H 0x20
	#define LCD_SSEG_QGQ_I 0x10

	#define LCD_CODE_0 (LCD_DSEG_F + LCD_DSEG_E + LCD_DSEG_D + LCD_DSEG_C + LCD_DSEG_B + LCD_DSEG_A)
	#define LCD_CODE_1 (LCD_DSEG_B + LCD_DSEG_C)
	#define LCD_CODE_2 (LCD_DSEG_A + LCD_DSEG_B + LCD_DSEG_G + LCD_DSEG_E + LCD_DSEG_D)      
	#define LCD_CODE_3 (LCD_DSEG_A + LCD_DSEG_B + LCD_DSEG_G + LCD_DSEG_C + LCD_DSEG_D)  
	#define LCD_CODE_4 (LCD_DSEG_F + LCD_DSEG_G + LCD_DSEG_B + LCD_DSEG_C) 
	#define LCD_CODE_5 (LCD_DSEG_F + LCD_DSEG_G + LCD_DSEG_C + LCD_DSEG_D + LCD_DSEG_A)
	#define LCD_CODE_6 (LCD_DSEG_A + LCD_DSEG_F + LCD_DSEG_E + LCD_DSEG_D + LCD_DSEG_C + LCD_DSEG_G)
	#define LCD_CODE_7 (LCD_DSEG_A + LCD_DSEG_B + LCD_DSEG_C)  
	#define LCD_CODE_8 (LCD_DSEG_A + LCD_DSEG_B + LCD_DSEG_C + LCD_DSEG_D + LCD_DSEG_E + LCD_DSEG_F + LCD_DSEG_G)
	#define LCD_CODE_9 (LCD_DSEG_A + LCD_DSEG_F + LCD_DSEG_G + LCD_DSEG_B + LCD_DSEG_C+ LCD_DSEG_D)

	#define LCD_CODE_A (LCD_DSEG_A + LCD_DSEG_F + LCD_DSEG_E + LCD_DSEG_B + LCD_DSEG_C + LCD_DSEG_G)
	#define LCD_CODE_B (LCD_DSEG_F + LCD_DSEG_E + LCD_DSEG_G + LCD_DSEG_C + LCD_DSEG_D)
	
	#define LCD_CODE_C_U (LCD_DSEG_A + LCD_DSEG_F + LCD_DSEG_E + LCD_DSEG_D)
	#define LCD_CODE_C (LCD_DSEG_G + LCD_DSEG_E + LCD_DSEG_D)
	
	#define LCD_CODE_D (LCD_DSEG_B + LCD_DSEG_E + LCD_DSEG_G + LCD_DSEG_C + LCD_DSEG_D)
	#define LCD_CODE_E (LCD_DSEG_A + LCD_DSEG_F + LCD_DSEG_E + LCD_DSEG_D + LCD_DSEG_G)
	#define LCD_CODE_F (LCD_DSEG_A + LCD_DSEG_F + LCD_DSEG_E + LCD_DSEG_G)
	#define LCD_CODE_G (LCD_DSEG_A + LCD_DSEG_F + LCD_DSEG_G + LCD_DSEG_B + LCD_DSEG_C+ LCD_DSEG_D)
	#define LCD_CODE_G_U (LCD_DSEG_A + LCD_DSEG_C + LCD_DSEG_D + LCD_DSEG_E + LCD_DSEG_F)
	
	#define LCD_CODE_H_U (LCD_DSEG_B + LCD_DSEG_C + LCD_DSEG_E + LCD_DSEG_F + LCD_DSEG_G)
	#define LCD_CODE_H (LCD_DSEG_C + LCD_DSEG_E + LCD_DSEG_F + LCD_DSEG_G)
	#define LCD_CODE_I LCD_CODE_1
	#define LCD_CODE_J (LCD_DSEG_B + LCD_DSEG_C + LCD_DSEG_D)
	#define LCD_CODE_K (LCD_DSEG_B+LCD_DSEG_D+LCD_DSEG_E+LCD_DSEG_F+LCD_DSEG_G)
	#define LCD_CODE_L (LCD_DSEG_F +LCD_DSEG_E +LCD_DSEG_D)
	#define LCD_CODE_M 0
	#define LCD_CODE_N_U (LCD_DSEG_F +LCD_DSEG_E +LCD_DSEG_A+LCD_DSEG_B+LCD_DSEG_C)
	#define LCD_CODE_N (LCD_DSEG_C +LCD_DSEG_E +LCD_DSEG_G)
	
	#define LCD_CODE_O_U LCD_CODE_0
    #define LCD_CODE_O (LCD_DSEG_C + LCD_DSEG_D + LCD_DSEG_E + LCD_DSEG_G)
	
	#define LCD_CODE_P (LCD_DSEG_A +LCD_DSEG_B +LCD_DSEG_E+LCD_DSEG_F+LCD_DSEG_G)
	#define LCD_CODE_Q (LCD_DSEG_A +LCD_DSEG_B +LCD_DSEG_C+LCD_DSEG_F+LCD_DSEG_G)

	#define LCD_CODE_R (LCD_DSEG_A +LCD_DSEG_F +LCD_DSEG_E)
    //#define LCD_CODE_R (LCD_DSEG_E + LCD_DSEG_G)
	#define LCD_CODE_S LCD_CODE_5
	#define LCD_CODE_T (LCD_DSEG_F +LCD_DSEG_E +LCD_DSEG_D+LCD_DSEG_G)
	
	#define LCD_CODE_U (LCD_DSEG_B +LCD_DSEG_C +LCD_DSEG_D+LCD_DSEG_E+LCD_DSEG_F)
	#define LCD_CODE_V 0
	#define LCD_CODE_W 0

	#define LCD_CODE_X 0
	#define LCD_CODE_Y (LCD_DSEG_B +LCD_DSEG_C +LCD_DSEG_D+LCD_DSEG_G+LCD_DSEG_F)
	#define LCD_CODE_Z LCD_CODE_2
	
	#define LCD_CODE_DP LCD_DSEG_DP
	#define LCD_CODE__ (LCD_DSEG_G)
	#define LCD_CODE_DOT	(LCD_DSEG_G)
	#define LCD_CODE_DDOT	(LCD_DSEG_G+LCD_DSEG_D)

	
	#define m_lcd_disp_seg_dp6() __nop();
	#define m_lcd_disp_seg_dp5() __nop();
	#define m_lcd_disp_seg_dp4() m_lcd_disp_seg(0,5);
	#define m_lcd_disp_seg_dp3() m_lcd_disp_seg(0,4);
	#define m_lcd_disp_seg_dp2() m_lcd_disp_seg(0,7);
	#define m_lcd_disp_seg_dp1() m_lcd_disp_seg(0,8);
	#define m_lcd_disp_seg_dp0() __nop();
	
	#define m_lcd_disp_seg_T21_SY() m_lcd_disp_seg(0,1)
	#define m_lcd_disp_seg_T17_YUAN() m_lcd_disp_seg(6,11)
	#define m_lcd_disp_seg_T18_M3() m_lcd_disp_seg(5,11)
	
	
	#define m_lcd_disp_seg_LJ_T20() m_lcd_disp_seg(0,0);
	#define m_lcd_disp_seg_QJ_T19() m_lcd_disp_seg(1,0);
	#define m_lcd_disp_seg_KG_T1()	m_lcd_disp_seg(2,0);
	#define m_lcd_disp_seg_QF_T2() m_lcd_disp_seg(3,0);
	#define m_lcd_disp_seg_GR_T3() m_lcd_disp_seg(4,0);
	#define m_lcd_disp_seg_QCZ_T4() m_lcd_disp_seg(5,0);
	#define m_lcd_disp_seg_ID_T5() m_lcd_disp_seg(6,0);

	#define m_lcd_disp_seg_YC_T6() m_lcd_disp_seg(7,0);
	#define m_lcd_disp_seg_CGR_T16() m_lcd_disp_seg(7,11);
	#define m_lcd_disp_seg_FG_T15() m_lcd_disp_seg(7,10);
	#define m_lcd_disp_seg_FK_T14() m_lcd_disp_seg(6,10);
	#define m_lcd_disp_seg_MLC_T13() m_lcd_disp_seg(5,10);
	#define m_lcd_disp_seg_DKZ_T12() m_lcd_disp_seg(4,10);
	
	#define m_lcd_disp_seg_OCLOCL_DOT_S5() m_lcd_disp_seg(0,3);
	#define m_lcd_disp_seg_OCLOCL_DOT_S6() m_lcd_disp_seg(0,6);
	
	#define m_lcd_disp_seg_oclock() do{m_lcd_disp_seg_OCLOCL_DOT_S5();m_lcd_disp_seg_OCLOCL_DOT_S6();}while(0);
	
	
	//T2สฃำเ+T17ิช
	#define m_lcd_disp_seg_yue() 			do{m_lcd_disp_seg_T21_SY();m_lcd_disp_seg_T17_YUAN();}while(0);
	#define m_lcd_disp_seg_balance_m()		m_lcd_disp_seg_yue();
	//T2สฃำเ+T18 m3
	#define m_lcd_disp_seg_yql() 			do{m_lcd_disp_seg_T21_SY();m_lcd_disp_seg_T18_M3();}while(0);
	#define m_lcd_disp_seg_balance_vol()	m_lcd_disp_seg_yql();
	#define m_lcd_disp_seg_total_vol()		do{m_lcd_disp_seg_LJ_T20();m_lcd_disp_seg_T18_M3();}while(0);
	#define m_lcd_disp_seg_total_m()	do{m_lcd_disp_seg_LJ_T20();m_lcd_disp_seg_T17_YUAN();}while(0);
	#define m_lcd_disp_seg_total_v()	do{m_lcd_disp_seg_LJ_T20();m_lcd_disp_seg_T18_M3();}while(0);
	
	#define m_lcd_disp_seg_price()			do{m_lcd_disp_seg_QJ_T19();m_lcd_disp_seg_T17_YUAN();}while(0);
	#define m_lcd_disp_seg_open_shell()		do{m_lcd_disp_seg_KG_T1();}while(0);
	#define m_lcd_disp_seg_owe() 		do{m_lcd_disp_seg_QF_T2();}while(0);
	#define m_lcd_disp_seg_current_buy_m()	do{m_lcd_disp_seg_GR_T3();m_lcd_disp_seg_T17_YUAN();}while(0);
	#define m_lcd_disp_seg_current_buy_V() 	do{m_lcd_disp_seg_GR_T3();m_lcd_disp_seg_T18_M3();}while(0);
	
	#define m_lcd_disp_seg_total_buy_m()	do{m_lcd_disp_seg_LJ_T20();m_lcd_disp_seg_GR_T3();m_lcd_disp_seg_T17_YUAN();}while(0);
	#define m_lcd_disp_seg_total_buy_V() 	do{m_lcd_disp_seg_LJ_T20();m_lcd_disp_seg_GR_T3();m_lcd_disp_seg_T18_M3();}while(0);	
	
	#define m_lcd_disp_seg_pls_recharge()	do{m_lcd_disp_seg_QCZ_T4();}while(0);
	#define m_lcd_disp_seg_device_id()		do{m_lcd_disp_seg_ID_T5();}while(0);
	
	#define m_lcd_disp_seg_fault()			do{m_lcd_disp_seg_YC_T6();}while(0);
	#define m_lcd_disp_seg_ste()			do{m_lcd_disp_seg_CGR_T16();}while(0);
	
	#define m_lcd_disp_seg_valve_open()		do{m_lcd_disp_seg_FK_T14();}while(0);
	#define m_lcd_disp_seg_valve_close()	do{m_lcd_disp_seg_FG_T15();}while(0);
	
	#define m_lcd_disp_seg_cmd_error()		do{m_lcd_disp_seg_MLC_T13();}while(0);
	#define m_lcd_disp_seg_reading()		do{m_lcd_disp_seg_DKZ_T12();}while(0);
	
	#define m_lcd_disp_seg_DJ_B() __nop();
	#define m_lcd_disp_seg_DYBH_D() __nop();
	#define m_lcd_disp_seg_GYBH_C() __nop();
	#define m_lcd_disp_seg_QCHB_G() m_lcd_disp_seg(6,10);
	#define m_lcd_disp_seg_CSBH_F() __nop();
	
	#define m_lcd_disp_seg_rssi_0() m_lcd_disp_seg(2,10);	
	#define m_lcd_disp_seg_rssi_1() m_lcd_disp_seg(1,10);	
	#define m_lcd_disp_seg_rssi_2() m_lcd_disp_seg(0,10);	
	#define m_lcd_disp_seg_rssi_3() m_lcd_disp_seg(0,2);	
	
	
	#define m_lcd_disp_bat_level0() m_lcd_disp_seg(7,9);
	#define m_lcd_disp_bat_level1() m_lcd_disp_seg(4,9);
	#define m_lcd_disp_bat_level2() m_lcd_disp_seg(5,9);
	#define m_lcd_disp_bat_level3() m_lcd_disp_seg(6,9);
	#define m_lcd_disp_bat_level4() m_lcd_disp_seg(3,9);
	#define m_lcd_disp_bat_level5() m_lcd_disp_seg(2,9);
	
	#define m_lcd_off() __HAL_LCD_DISABLE(&LCDHandle);
	#define m_lcd_on() __HAL_LCD_ENABLE(&LCDHandle);
	
	
	#define LCD_TABLE_LEN (sizeof(LCDDigitalIndexTable)/sizeof(uint8_t))
	#define m_lcd_disp_dig_dp(loc,dig) m_lcd_disp_dig(loc,dig+LCD_DSEG_DP);
	
	#define m_lcd_get_status() (__mySfr_LCD_CR & 0x01ul)
	#define m_lcd_enable() do{__mySfr_LCD_CR |= 0x01ul;}while(0);
	#define m_lcd_disable() do{__mySfr_LCD_CR &= ~0x01ul;}while(0);
	/*
	#define m_lcd_refresh() do{ \
		if(__mySfr_LCD_SR & 0x08ul)__mySfr_LCD_SR |= 0x04ul; \
		}while(0);
		
	#define m_lcd_refresh() do{ \
		__mySfr_LCD_SR &= ~LCD_SR_UDD;\
		__mySfr_LCD_SR |= LCD_SR_UDR; \
		}while(0);	
	*/	
	extern const uint8_t LCDDigitalTable[];
	extern const uint8_t  LCDDigitalIndexTable[];
	
	//
	extern LCD_HandleTypeDef LCDHandle;
	extern uint16_t LCDHandleStat;

	extern void m_lcd_disp_dig(uint8_t loc,uint8_t dig);
	extern void m_lcd_disp_chr(uint8_t loc,uint8_t chr);
	extern void m_lcd_disp_str(uint8_t* str);
	extern void m_lcd_disp_str_refresh(uint8_t* str);
	extern void m_lcd_disp_seg(uint8_t c,uint8_t s);
	extern void m_lcd_clear_seg(uint8_t c,uint8_t s);

	extern void m_lcd_ram_set(uint32_t t);
	extern void m_lcd_ram_clear(void);
	extern void m_lcd_all_on(void);
	extern void m_lcd_all_off(void);
	
	extern void m_lcd_sisp_dp(uint8_t loc);
	
	extern void m_lcd_init(void);	
	extern void m_lcd_reinit(void);
	extern void m_lcd_clear(void);
	extern void m_lcd_refresh(void);
	
	extern uint8_t day_of_week(int16_t y, uint16_t m, uint16_t d);
	#define lcd_clear_all() do{m_lcd_ram_set(~0x0000fffful);}while(0);
	#ifdef __cplusplus
	}
	#endif		
	
	
#endif
	
//file end
//@
