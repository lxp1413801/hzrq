//#include "stdint.h"
//#include "time-stamp.h"
#include "common.h"
//2000-2100年时间戳转换
const uint8_t leapDayInMonth[]={31,29,31,30,31,30,31,31,30,31,30,31};
const uint8_t noleapDayInMonth[]={31,28,31,30,31,30,31,31,30,31,30,31};

/* 1 <= m <= 12,  y > 1752 (in the U.K.) */
uint8_t day_of_week(int16_t y, uint16_t m, uint16_t d)  
{
	static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
	y -= m < 3;
	return (uint8_t)((y + y/4 - y/100 + y/400 + t[m-1] + d) % 7);
}

uint8_t is_leap_year(uint16_t year)
{
	if(year % 400 ==0)return 1;
	else if(((year % 4)==0) && ((year % 100) != 0))return 1;
	return 0;
}

uint32_t __YYMMDD_to_int32day(uint16_t year,uint8_t month,uint8_t dayInMonth)
{
	uint32_t day;
	uint8_t* pdayInMon;
	uint16_t t16=2000;
	uint8_t t8;
	if(year<2000 || year>2100)return 0;
	if(month<1 || month>12)return 0;
	if(dayInMonth<1 || dayInMonth>31)return 0;
	day=0;
	for(t16=2000;t16<year;t16++){
		if(is_leap_year(t16))day+=366;
		else
			day+=365;
	}
	if(is_leap_year(year))pdayInMon=(uint8_t*)leapDayInMonth;
	else
		pdayInMon=(uint8_t*)noleapDayInMonth;
	
	for(t8=1;t8<month;t8++){
		day+=pdayInMon[t8-1];
	}
	day+=(dayInMonth-1);
	
	return day;
}

uint8_t __int32day_to_YYMMDD(uint32_t dayInYear,uint16_t* year, uint8_t* month,uint8_t* day)
{
	uint16_t t16;
	uint8_t* pdayInMon;
	uint16_t y=2000;
	uint8_t mon=1;
	uint8_t	d=1;
	while(dayInYear){
		if(is_leap_year(y))t16=366;
		else
			t16=365;
		
		if(dayInYear>=t16){
			y++;
			dayInYear-=t16;
		}else{
			break;
		}
		if(y>2100)return 0;
	}
	
	if(is_leap_year(y))pdayInMon=(uint8_t*)leapDayInMonth;
	else
		pdayInMon=(uint8_t*)noleapDayInMonth;	
	
	while(dayInYear){
		t16=pdayInMon[mon-1];
		if(dayInYear>=t16){
			mon++;
			dayInYear-=t16;
		}
		else{
			break;
		}
		if(mon>12)return 0;
	}
	
	if(dayInYear>31)return 0;
	d+=(uint8_t)dayInYear;
	
	*year=y;
	*month=mon;
	*day=d;
	return 1;
}

uint32_t __hhmmss_to_int32sec(uint8_t h,uint8_t m,uint8_t s)
{
	uint32_t t32=0;
	if(h>23 || m>59 || s>59)return 0;
	t32=h*3600+m*60+s;
	return t32;
}

uint8_t __int32sec_to_hhmmss(uint32_t int32sec,uint8_t* h, uint8_t* m,uint8_t* s)
{

	if(int32sec>24*3600)return  0;
	*h=(uint8_t)(int32sec/3600);

	
	int32sec%=3600;
	*m=(uint8_t)(int32sec/60);

	
	*s=(uint8_t)(int32sec%60);
	return 1;
}

void dt_format_hex_to_bcd(sysDataTime_t* dt)
{
	uint8_t* pdt=(uint8_t*)dt;
	uint8_t t8;
	uint16_t i;
	for(i=0;i<sizeof(sysDataTime_t);i++){
		t8=*pdt;
		t8=m_hex_2_bcd(t8);
		*pdt=t8;
		pdt++;
	}
}

void dt_format_bcd_to_hex(sysDataTime_t* dt)
{
	uint8_t* pdt=(uint8_t*)dt;
	uint8_t t8;
	uint16_t i;
	for(i=0;i<sizeof(sysDataTime_t);i++){
		t8=*pdt;
		t8=m_bcd_2_hex(t8);
		*pdt=t8;
		pdt++;
	}
}

uint32_t system_dt_to_time_stamp(sysDataTime_t* dt)
{
	uint32_t t32;
	dt_format_bcd_to_hex(dt);
	
	t32=__YYMMDD_to_int32day((dt->YY)+2000,dt->MM,dt->DD);
	t32*=(3600*24);
	t32+=__hhmmss_to_int32sec(dt->hh,dt->mm,dt->ss);
	return t32;
}

uint8_t time_stamp_to_system_dt(uint32_t ts,sysDataTime_t* dt)
{
	uint32_t t32;
	uint16_t YY;
	uint8_t MM,DD,hh,mm,ss;
	
	t32=ts/(3600*24);
	__int32day_to_YYMMDD(t32,&YY,&MM,&DD);
	t32=ts%(3600*24);
	__int32sec_to_hhmmss(t32,&hh,&mm,&ss);
	dt->YY=YY-2000;
	dt->MM=MM;
	dt->DD=DD;
	dt->hh=hh;
	dt->mm=mm;
	dt->ss=ss;
	
	dt_format_hex_to_bcd(dt);
	return 1;
	
}

uint16_t time_stamp_test(void)
{
	uint32_t t32;
	sysDataTime_t testDt={0x30,0x30,0x15,0x15,0x11,0x99,0,0};
	t32=system_dt_to_time_stamp(&testDt);
	__nop();
	time_stamp_to_system_dt(t32,&testDt);
	__nop();
	return 1;
}
