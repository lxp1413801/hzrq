#ifndef __qn_def__
#define __qn_def__

#ifdef __cplusplus
extern "C"{
#endif
	#include "subversion.h"
	
	#ifndef __SUB_VER_DEF_QN
		#error "must defined __SUB_VER_DEF_QN in header file subversion.h,e.g.:1.6 2.5 4 6 10 ..." 
	#endif
	
	#define QNDEF_QN10		10
	#define QNDEF_QN16		16             
	#define QNDEF_QN25		25 
	#define QNDEF_QN40		40             
	#define QNDEF_QN60		60
	#define QNDEF_QN100		100            
	#define QNDEF_QN160		160
	#define QNDEF_QN250		250            
	#define QNDEF_QN400		400
	#define QNDEF_QN600		600            
	#define QNDEF_QN_reverse 	0xffff	
	
	/*
	//定义公称流量
	#ifndef default_QN
		#define default_QN QNDEF_QN25
	#endif
	
	//最大流量，必须和公称流量对应
	#ifndef default_QMAX
		#define default_QMAX QNDEF_QN40
	#endif	
	*/
	//根据subversion.h文件中定义的qn 定义当前头文件中需要的default_QN
	#if __SUB_VER_DEF_QN == 10 
		#define default_QN		QNDEF_QN10
		#define default_QMAX	QNDEF_QN16
	#elif __SUB_VER_DEF_QN == 16
		#define default_QN		QNDEF_QN16
		#define default_QMAX	QNDEF_QN25
	#elif __SUB_VER_DEF_QN == 25
		#define default_QN		QNDEF_QN25
		#define default_QMAX	QNDEF_QN40
	#elif __SUB_VER_DEF_QN == 40
		#define default_QN		QNDEF_QN40
		#define default_QMAX	QNDEF_QN60
	#elif __SUB_VER_DEF_QN == 60
		#define default_QN		QNDEF_QN60
		#define default_QMAX	QNDEF_QN100		
	#elif __SUB_VER_DEF_QN == 100
		#define default_QN		QNDEF_QN100
		#define default_QMAX	QNDEF_QN160
	#elif __SUB_VER_DEF_QN == 160
		#define default_QN		QNDEF_QN160
		#define default_QMAX	QNDEF_QN250
	#elif __SUB_VER_DEF_QN == 250
		#define default_QN		QNDEF_QN250
		#define default_QMAX	QNDEF_QN400
	#elif __SUB_VER_DEF_QN == 400
		#define default_QN		QNDEF_QN400
		#define default_QMAX	QNDEF_QN600	
	#else 
		#error "__SUB_VER_DEF_QN defined error,in header file subversion.h,e.g.:1.6 2.5 4 6 10 ..."
	#endif
	//定义AB阶段默认存气量
	#ifndef	default_OVERAGE_VM
		#if default_QN < QNDEF_QN60
			#define default_OVERAGE_VM_A 25 
			#define default_OVERAGE_VM_B 50
		#elif default_QN == QNDEF_QN60
			#define default_OVERAGE_VM_A 100
			#define default_OVERAGE_VM_B 500
		#else 
			#define default_OVERAGE_VM_A 500
			#define default_OVERAGE_VM_B 500
		#endif
	#endif
	//定义报警气量
	#ifndef default_WARNING_VOLUMING
		#if default_QN < QNDEF_QN60
			#define default_WARNING_VOLUMING	500
		#elif default_QN == QNDEF_QN60
			#define default_WARNING_VOLUMING	4000
		#else
			#define default_WARNING_VOLUMING	400
		#endif
	#endif	
	
	//定义阀门打开和关闭的时间
	#if default_QN <= QNDEF_QN40
		#define __VAVLE_ON_TIME		3
		#define __VAVLE_OFF_TIME	1	
	#else
		#define __VAVLE_ON_TIME		6
		#define __VAVLE_OFF_TIME	2	
	#endif		

#ifdef __cplusplus
}
#endif

#endif
//file end
