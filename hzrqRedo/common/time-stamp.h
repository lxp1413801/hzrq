//file name:time-stamp.h
#ifndef __time_stamp_h__
#define __time_stamp_h__

#include "stdint.h"
#ifdef __cplusplus
extern "C"{
#endif

	typedef struct{
		uint8_t ss;
		uint8_t mm;
		uint8_t hh;
		uint8_t DD;
		uint8_t MM;
		uint8_t YY;
		uint8_t YYhi;
		uint8_t day;
	}sysDataTime_t;
	
	extern uint8_t is_leap_year(uint16_t year);

	extern uint8_t day_of_week(int16_t y, uint16_t m, uint16_t d);
	extern uint32_t __HHMMDD_to_int32day(uint16_t year,uint8_t month,uint8_t dayInMonth);
	extern uint8_t __int32day_to_YYMMDD(uint32_t dayInYear,uint16_t* year, uint8_t* month,uint8_t*day);
	
	extern uint32_t system_dt_to_time_stamp(sysDataTime_t* dt);
	extern uint8_t time_stamp_to_system_dt(uint32_t ts,sysDataTime_t* dt);
	
	extern uint16_t time_stamp_test(void);

#ifdef __cplusplus
}
#endif

#endif

