#ifndef __sub_version_h__
#define __sub_version_h__

#ifdef __cplusplus
extern "C"{
#endif
	//-----------------基础选项定义--------------------------------------------	
	//电池
	#define BATTARY_TYPE_LI_SOCL 0
	#define BATTARY_TYPE_ZN_MN	1
	//	
	//-----------------sub版本定义--------------------------------------------
	//钢壳表+锂电池
	#define NB20E11		1
	//钢壳表+锂电池+报警器
	#define NB20E11Q	2
	//钢壳表+干电池
	#define NB20E11G	3
	//钢壳表+干电池+报警器
	#define NB20E11GQ	4
	//铝壳表+锂电池
	#define NBL20E11	5
	//铝壳表+锂电池+报警器
	#define NBL20E11Q	6

	//铝壳表+干电池
	#define NBL20E11G	7
	//铝壳表+干电池+报警器
	#define NBL20E11GQ	8	
	
	//现行工业基表
	//现行工业基表6方
	#define NB20E11A	9
	//现行工业基表10/16/25方
	#define NB20E11B	10
	//现行工业基表40方
	#define NB20E11C	11
	
	//现行工业基表6方+报警器
	#define NB20E11AQ	12
	//现行工业基表10/16/25方+报警器
	#define NB20E11BQ	13
	//现行工业基表40方+报警器
	#define NB20E11CQ	14

	//小体积工业基表
	//小体积工业基表6方
	#define NB20E11D	15
	//小体积工业基表10/16/25方
	#define NB20E11E	16
	//小体积工业基表40方
	#define NB20E11F	17
	//小体积工业基表6方+报警器
	
	#define NB20E11DQ	18
	//小体积工业基表10/16/25方+报警器
	#define NB20E11EQ	19
	//小体积工业基表40方+报警器
	#define NB20E11FQ	20
	
	//阀控加抄表
	//钢壳表+锂电池+
	#define NB20F11		21
	//钢壳表+锂电池+报警器
	#define NB20F11Q	22
	//钢壳表+干电池
	#define NB20F11G	23
	//钢壳表+干电池+报警器
	#define NB20F11GQ	24
	//铝壳表+锂电池
	#define NBL20F11	25
	//铝壳表+锂电池+报警器
	#define NBL20F11Q	26
	//铝壳表+锂电池
	#define NBL20F11G	27
	//铝壳表+锂电池+报警器
	#define NBL20F11GQ	28	
	
	//G6	现行工业基表
	#define NB20F11A 	29
	//G10/16/25	现行工业基表
	#define NB20F11B	30
	//G40	现行工业基表
	#define NB20F11C	31
	//--------------------
	//G6	现行工业基表+报警器
	#define NB20F11AQ	32
	//G10/16/25	现行工业基表+报警器
	#define NB20F11BQ	33
	//G40	现行工业基表+报警器
	#define NB20F11CQ	34
	//--------------------
	//G6	小体积工业基表
	#define NB20F11D	35
	//G10/16/25	小体积工业基表
	#define NB20F11E	36
	//G40	小体积工业基表
	#define NB20F11F	37
	//--------------------
	//G6	小体积工业基表+报警器
	#define NB20F11DQ	38
	//G10/16/25	小体积工业基表+报警器
	#define NB20F11EQ	39
	//G40	小体积工业基表+报警器
	#define NB20F11FQ	40
	
	
	//-----------------设置软件升级代号------------------------------------------------	
	//#define soft_VER_NUM	11
	//-----------------选择sub版本------------------------------------------------
	#ifndef NB20E11_SUB_VER
		// #define NB20E11_SUB_VER		NB20E11
		// #define NB20E11_SUB_VER		NB20E11Q
		// #define NB20E11_SUB_VER		NB20E11G		
		// #define NB20E11_SUB_VER		NB20E11GQ
		// #define NB20E11_SUB_VER		NBL20E11
		// #define NB20E11_SUB_VER		NBL20E11Q
		// #define NB20E11_SUB_VER		NBL20E11G
		// #define NB20E11_SUB_VER		NBL20E11GQ
		// #define NB20E11_SUB_VER		NB20E11A
		// #define NB20E11_SUB_VER		NB20E11B
		// #define NB20E11_SUB_VER		NB20E11C
		// #define NB20E11_SUB_VER		NB20E11AQ
		// #define NB20E11_SUB_VER		NB20E11BQ
		// #define NB20E11_SUB_VER		NB20E11CQ
		// #define NB20E11_SUB_VER		NB20E11D
		// #define NB20E11_SUB_VER		NB20E11E
		// #define NB20E11_SUB_VER		NB20E11F
		// #define NB20E11_SUB_VER		NB20E11DQ
		// #define NB20E11_SUB_VER		NB20E11EQ
		// #define NB20E11_SUB_VER		NB20E11FQ

		 #define NB20E11_SUB_VER		NB20F11
		// #define NB20E11_SUB_VER		NB20F11Q
		// #define NB20E11_SUB_VER		NB20F11G
		// #define NB20E11_SUB_VER		NB20F11GQ
		// #define NB20E11_SUB_VER		NBL20F11
		// #define NB20E11_SUB_VER		NBL20F11Q
		// #define NB20E11_SUB_VER		NBL20F11G
		// #define NB20E11_SUB_VER		NBL20F11GQ
		// #define NB20E11_SUB_VER		NB20F11A
		// #define NB20E11_SUB_VER		NB20F11B
		// #define NB20E11_SUB_VER		NB20F11C
		// #define NB20E11_SUB_VER		NB20F11AQ
		// #define NB20E11_SUB_VER		NB20F11BQ
		// #define NB20E11_SUB_VER		NB20F11CQ
		// #define NB20E11_SUB_VER		NB20F11D
		// #define NB20E11_SUB_VER		NB20F11E
		// #define NB20E11_SUB_VER		NB20F11F
		// #define NB20E11_SUB_VER		NB20F11DQ
		// #define NB20E11_SUB_VER		NB20F11EQ
		// #define NB20E11_SUB_VER		NB20F11FQ

	#else
		#error "NB20E11_SUB_VER redefined!!!"
	#endif
	
	//定义液晶类型
	#if(NB20E11_SUB_VER == NB20E11Q || NB20E11_SUB_VER == NB20E11GQ || NB20E11_SUB_VER == NBL20E11Q \
	|| NB20E11_SUB_VER == NBL20E11GQ || NB20E11_SUB_VER == NB20E11AQ || NB20E11_SUB_VER == NB20E11BQ \
	|| NB20E11_SUB_VER == NB20E11CQ || NB20E11_SUB_VER == NB20E11DQ || NB20E11_SUB_VER == NB20E11EQ \
	|| NB20E11_SUB_VER == NB20E11FQ || NB20E11_SUB_VER == NB20F11Q || NB20E11_SUB_VER == NB20F11GQ \
	|| NB20E11_SUB_VER == NBL20F11Q || NB20E11_SUB_VER == NBL20F11GQ || NB20E11_SUB_VER == NB20F11AQ \
	|| NB20E11_SUB_VER == NB20F11BQ || NB20E11_SUB_VER == NB20F11CQ || NB20E11_SUB_VER == NB20F11DQ \
	|| NB20E11_SUB_VER == NB20F11EQ || NB20E11_SUB_VER == NB20F11FQ)
		#define config_USED_ALARM_LCD	1
	#elif (NB20E11_SUB_VER == NB20E11 || NB20E11_SUB_VER == NB20E11G || NB20E11_SUB_VER == NBL20E11 \
	|| NB20E11_SUB_VER == NBL20E11G || NB20E11_SUB_VER == NB20E11A || NB20E11_SUB_VER == NB20E11B \
	|| NB20E11_SUB_VER == NB20E11C || NB20E11_SUB_VER == NB20E11D || NB20E11_SUB_VER == NB20E11E \
	|| NB20E11_SUB_VER == NB20E11F || NB20E11_SUB_VER == NB20F11 || NB20E11_SUB_VER == NB20F11G \
	|| NB20E11_SUB_VER == NBL20F11 || NB20E11_SUB_VER == NBL20F11G || NB20E11_SUB_VER == NB20F11A \
	|| NB20E11_SUB_VER == NB20F11B || NB20E11_SUB_VER == NB20F11C || NB20E11_SUB_VER == NB20F11D \
	|| NB20E11_SUB_VER == NB20F11E || NB20E11_SUB_VER == NB20F11F)
		#define config_USED_ALARM_LCD	0
	#endif
	#ifndef config_USED_ALARM_LCD
		#error "config_USED_ALARM_LCD must be defined!!!"
	#endif
	//定义电池类型
	#if(NB20E11_SUB_VER == NB20E11G || NB20E11_SUB_VER == NB20E11GQ || NB20E11_SUB_VER == NBL20E11G \
	|| NB20E11_SUB_VER == NBL20E11GQ || NB20E11_SUB_VER == NB20F11G || NB20E11_SUB_VER == NB20F11GQ \
	|| NB20E11_SUB_VER == NBL20F11G || NB20E11_SUB_VER == NBL20F11GQ)
		#define BATTARY_TYPE		BATTARY_TYPE_ZN_MN
	 
	#elif(NB20E11_SUB_VER == NB20E11 || NB20E11_SUB_VER == NB20E11Q || NB20E11_SUB_VER == NBL20E11 \
	|| NB20E11_SUB_VER == NBL20E11Q || NB20E11_SUB_VER == NB20F11 || NB20E11_SUB_VER == NB20F11Q \
	|| NB20E11_SUB_VER == NBL20F11 || NB20E11_SUB_VER == NBL20F11Q)
	
		#define BATTARY_TYPE		BATTARY_TYPE_LI_SOCL
	#elif(NB20E11_SUB_VER == NB20E11A || NB20E11_SUB_VER == NB20E11B	|| NB20E11_SUB_VER == NB20E11C \
	|| NB20E11_SUB_VER == NB20E11AQ || NB20E11_SUB_VER == NB20E11BQ		|| NB20E11_SUB_VER == NB20E11CQ \
	|| NB20E11_SUB_VER == NB20E11D 	|| NB20E11_SUB_VER == NB20E11E		|| NB20E11_SUB_VER == NB20E11F \
	|| NB20E11_SUB_VER == NB20E11DQ || NB20E11_SUB_VER == NB20E11EQ		|| NB20E11_SUB_VER == NB20E11FQ \
	|| NB20E11_SUB_VER == NB20F11A 	|| NB20E11_SUB_VER == NB20F11B		|| NB20E11_SUB_VER == NB20F11C \
	|| NB20E11_SUB_VER == NB20F11AQ || NB20E11_SUB_VER == NB20F11BQ		|| NB20E11_SUB_VER == NB20F11CQ \
	|| NB20E11_SUB_VER == NB20F11D 	|| NB20E11_SUB_VER == NB20F11E		|| NB20E11_SUB_VER == NB20F11F \
	|| NB20E11_SUB_VER == NB20F11DQ || NB20E11_SUB_VER == NB20F11EQ		|| NB20E11_SUB_VER == NB20F11FQ)
	
		#define BATTARY_TYPE		BATTARY_TYPE_LI_SOCL
	#endif
	#ifndef BATTARY_TYPE
		#error "BATTARY_TYPE must be defined!!!"
	#endif 
	
	//定义公称流量
	#if(NB20E11_SUB_VER == NB20E11 || NB20E11_SUB_VER == NB20E11Q || NB20E11_SUB_VER == NB20E11G \
	|| NB20E11_SUB_VER == NB20E11GQ || NB20E11_SUB_VER == NBL20E11 || NB20E11_SUB_VER == NBL20E11Q \
	|| NB20E11_SUB_VER == NBL20E11G || NB20E11_SUB_VER == NBL20E11GQ || NB20E11_SUB_VER == NB20F11 \
	|| NB20E11_SUB_VER == NB20F11Q || NB20E11_SUB_VER == NB20F11G || NB20E11_SUB_VER == NB20F11GQ \
	|| NB20E11_SUB_VER == NBL20F11 || NB20E11_SUB_VER == NBL20F11Q || NB20E11_SUB_VER == NBL20F11G \
	|| NB20E11_SUB_VER == NBL20F11GQ)
		//默认流量2.5
		#define __SUB_VER_DEF_QN	25
	#elif(NB20E11_SUB_VER == NB20E11A || NB20E11_SUB_VER == NB20E11AQ || NB20E11_SUB_VER == NB20E11D \
	|| NB20E11_SUB_VER == NB20E11DQ || NB20E11_SUB_VER == NB20F11A || NB20E11_SUB_VER == NB20F11AQ \
	|| NB20E11_SUB_VER == NB20F11D || NB20E11_SUB_VER == NB20F11DQ)
		#define __SUB_VER_DEF_QN	60
	#elif(NB20E11_SUB_VER == NB20E11B || NB20E11_SUB_VER == NB20E11BQ || NB20E11_SUB_VER == NB20E11E \
	|| NB20E11_SUB_VER == NB20E11EQ || NB20E11_SUB_VER == NB20F11B || NB20E11_SUB_VER == NB20F11BQ  \
	|| NB20E11_SUB_VER == NB20F11E || NB20E11_SUB_VER == NB20F11EQ)
		// #define __SUB_VER_DEF_QN	100
		// #define __SUB_VER_DEF_QN	160
		#define __SUB_VER_DEF_QN	250		
	#elif(NB20E11_SUB_VER == NB20E11C || NB20E11_SUB_VER == NB20E11CQ || NB20E11_SUB_VER == NB20E11F \
	|| NB20E11_SUB_VER == NB20E11FQ || NB20E11_SUB_VER == NB20F11C || NB20E11_SUB_VER == NB20F11CQ \
	|| NB20E11_SUB_VER == NB20F11F || NB20E11_SUB_VER == NB20F11FQ)
		#define __SUB_VER_DEF_QN	400
	#endif
	#ifndef __SUB_VER_DEF_QN
		#error "__SUB_VER_DEF_QN must be defined!!!"
	#endif 
	//定义设备名称
	#if(NB20E11_SUB_VER >= NB20E11 && NB20E11_SUB_VER <= NB20E11FQ)
	
		#if NB_MODULE_VENDOR==L620
			#define device_SN "nb20e31"
			#define Device_NAME "nb20e31"
		#elif NB_MODULE_VENDOR==BC26
			#define device_SN "nb20e41"
			#define Device_NAME "nb20e41"		
		#else
			#error "nb module define error"
		#endif 
		
	#elif(NB20E11_SUB_VER >= NB20F11 && NB20E11_SUB_VER <= NB20F11FQ)
	
		#if NB_MODULE_VENDOR==L620
			#define device_SN "nb20f31"
			#define Device_NAME "nb20f31"	
		#elif NB_MODULE_VENDOR==BC26
			#define device_SN "nb20f41"
			#define Device_NAME "nb20f41"
		#else
			#error "nb module define error"
		#endif 
	
	#else
		#error "NB20E11_SUB_VER define error !!!!"
	#endif
	//#warning
	#define soft_VER "nb10210"
	
	#if BATTARY_TYPE==BATTARY_TYPE_LI_SOCL
		#define BAT_VOLT_DOWN 	2500
		#define BAT_VOLT_LOW	2800
		#define BAT_VOLT_HIGHT	4500
	#else
		#define BAT_VOLT_DOWN 	3300
		#define BAT_VOLT_LOW	3900
		#define BAT_VOLT_HIGHT	5500		
	#endif
	
#ifdef __cplusplus
}
#endif

#endif
//file end
