#ifndef __user_configs_h__
#define __user_configs_h__
#ifdef __cplusplus
extern "C"{
#endif
	#define __S(x) #x
	#define __def(x) x
	
	//<<--
	//运行设置，不可修改
	
	//使用hsi时钟还是使用msi
	#ifndef cfg_SYSTEM_CLOCK_HSI
		#define cfg_SYSTEM_CLOCK_HSI 0
	#endif 
	//RTC的唤醒时间间隔
	#ifndef cfg_RTC_INTERVAL
		#define cfg_RTC_INTERVAL	1
	#endif
	
	//主要的循环线程的执行周期，用于计时
	#ifndef MAIN_TASK_TIME_INTERVAL
		#define MAIN_TASK_TIME_INTERVAL 1
	#endif

	#ifndef default_CONST_ID
		#define default_CONST_ID (*( ( volatile uint32_t *)(0x0801FFF0)))
	#endif
	//-->>
	//----------------------------------------------------------------------------------
	//选项设置
	#define BC26 0
	#define L620 1
	#define NB_MODULE_VENDOR	BC26	
	//定义软件升级版本号
	#define sw_VER_NUM 	10
	#define hw_VER_NUM	10
	#define pro_VER_NUM	10
	
	//是否启用单独的掉电检测
	#ifndef PWR_DOWN_IO_STATUS_EN
		#define PWR_DOWN_IO_STATUS_EN 0
	#endif
	
	#ifndef cfg_IRDA_EN
		#define cfg_IRDA_EN 1
	#endif

	//平台定义
	#define UDP 	0
	#define COAP	1	
	#ifndef config_NB_PLAT
		#define config_NB_PLAT COAP
	#endif
	
	//定义coap正式平台还是测试平台
	//#define USED_NB_TEST_PLAT 0

	#define COAP_PLAT_CTCC_TEST		0
	#define COAP_PLAT_CTCC_RELEASE	1
	#define COAP_PLAT_HW_TEST		2
	#define COAP_PLAT_HW_RELEASE	3
	
	#ifndef COAP_PLAT
		//#define COAP_PLAT	COAP_PLAT_CTCC_TEST
		#define COAP_PLAT	COAP_PLAT_CTCC_RELEASE
		//#define COAP_PLAT	COAP_PLAT_HW_TEST
		//#define COAP_PLAT	COAP_PLAT_HW_RELEASE
	#endif
	//是否工作在psm模式
	#ifndef config_NB_PSM_EN
		#define config_NB_PSM_EN 1
	#endif
	
	#if (config_NB_PLAT == UDP &&  config_NB_PSM_EN==1)
		#error "where config_NB_PLAT = UDP,config_NB_PSM_EN must set be 0!!!"
	#endif 

	//定义公称流量
	//定义内部子版本
	#include "subversion.h"
	#include "qndef.h"
	
	//测试电池发射能力
	#ifndef SEND_TIMES_TEST
		#define SEND_TIMES_TEST 0
	#endif

	//定义超时测试
	#define NO_FLOW_PROTECT_EN_TEST 0
	//附加超时测试
	//#define NO_FLOW_PROTECT_EN_TEST_EX 0
	
	#if(NO_FLOW_PROTECT_EN_TEST ==0 && NO_FLOW_PROTECT_EN_TEST_EX!=0)
		#error "no flow test defined error!!!"
	#endif
	
	//----------------------------------------------------------------------------------
	//一下为用户配置
	//过流保护
	#ifndef HIGHT_FLOW_PROTECT_EN
		#define HIGHT_FLOW_PROTECT_EN 1
	#endif 
	
	//长时间无流量保护
	#ifndef NO_FLOW_PROTECT_EN
		#define NO_FLOW_PROTECT_EN 1
	#endif
	
	//异常小流量保护
	#ifndef SMALL_FLOW_PROTECT_EN
		#define SMALL_FLOW_PROTECT_EN 0
	#endif 
	
	//连续用气时间超时
	#ifndef CONST_FLOW_TIMEOUT_PROTECT_EN
		#define CONST_FLOW_TIMEOUT_PROTECT_EN 0
	#endif
	
	//附加的强磁保护
	#ifndef STRONGMAGNETIC_PROTECT_EX_EN
		#define STRONGMAGNETIC_PROTECT_EX_EN 0
	#endif
	
	//是否允许进入D、E阶段
	#if (NB20E11_SUB_VER >= NB20F11 && NB20E11_SUB_VER<=NB20F11FQ)
		#define USED_STATIC_D_E 0
		#define NO_FLOW_PROTECT_LOCK_EN 0
	#else
		#define USED_STATIC_D_E 1
		#define NO_FLOW_PROTECT_LOCK_EN 1
	#endif

	//超时时间
	#ifndef default_CONST_NO_FLOW_TIME_OUT
	
		#define default_CONST_NO_FLOW_TIME_OUT 24*30
		
		#if NO_FLOW_PROTECT_EN_TEST
			#warning "no flow time out test!!!"
		#endif	
	#endif
	
	#ifndef default_CONST_NO_FLOW_TIME_OUT_EX
	
		#define default_CONST_NO_FLOW_TIME_OUT_EX 24*4
		
		#if NO_FLOW_PROTECT_EN_TEST
			#warning "no flow time out test!!!"
		#endif
		
	#endif	
	
	#ifndef default_CONST_FLOW_TIME_OUT
		#define default_CONST_FLOW_TIME_OUT 0
	#endif
	
	#ifndef default_CONST_SMALL_FLOW_TIME_OUT
		#define default_CONST_SMALL_FLOW_TIME_OUT 0
	#endif
	//初始化时候0.25m3，跑完进入B阶段
	#ifndef DEVICE_DLCS_CONVERTED_USED_OFF
		#define  DEVICE_DLCS_CONVERTED_USED_OFF 1
	#endif
	//超级充值次数限制
	#ifndef SUPPER_PAY_TIMES_LIMITS
		#define SUPPER_PAY_TIMES_LIMITS 3
	#endif
	
	//ifndef PAY_prompt
	#ifndef PAY_PROMPT_TIME_LONG 
		#define PAY_PROMPT_TIME_LONG  24*3600
	#endif
	
	//强磁锁定次数
	#ifndef MAX_STE_OFF_TIMES
		#define MAX_STE_OFF_TIMES 3
	#endif
	//程序功能以及燃气表版号配置
	//默认 id
	#ifndef default_DEVICE_ID
		#define default_DEVICE_ID 1
	#endif
	
	//默认阶段
	#ifndef degault_DLC_STATUS
		#define degault_DLC_STATUS DLC_STATUS_A
	#endif
	
	//定义默认过流保护
	#ifndef default_HIGHT_FLOW_PROTECT_EN
		#define default_HIGHT_FLOW_PROTECT_EN 0
	#endif

	#if config_NB_PLAT==UDP
		//定义默认服务器配置
		#ifndef default_UDP_SEVER_SETTING_M 
			#define default_UDP_SEVER_SETTING_M "211.149.175.111,28883"
			
			//#define default_UDP_SEVER_SETTING_M "171.221.221.50,1895"
			//#define default_UDP_SEVER_SETTING_M "171.221.221.50,8123"
		#endif
		
		#ifndef default_UDP_SEVER_SETTING_S 
			//#define default_UDP_SEVER_SETTING_S "211.149.185.137,2200"
			#define default_UDP_SEVER_SETTING_M "211.149.175.111,28883"
			//#define default_UDP_SEVER_SETTING_M "171.221.221.50,1895"
		#endif
		#warning "nb iot used udp!!!"
	#elif config_NB_PLAT==COAP
		#ifndef COAP_PLAT
			#error "COAP_PLAT must be defined!"
		#endif
		
		#define CTCC_TEST_IOT_IP_BIN 	{180,101,147,115} 
		#define CTCC_REALEAS_IOT_IP_BIN	{117,60,157,137}
		#define HW_TEST_IOT_IP_BIN		{139,159,140,34}
		#define HW_RELEAS_IOT_IP_BIN	{139,159,140,34}
		

		//电信测试平台
		#if(COAP_PLAT == COAP_PLAT_CTCC_TEST)
			#define default_SEVER_INDEX 0
			#define IOT_IP "180.101.147.115"
			//#define default_UDP_SEVER_SETTING_M "AT+NCDP=180.101.147.115\r\n"
			//#define default_UDP_SEVER_SETTING_S "AT+NCDP=180.101.147.115\r\n"
			#warning "nb iot used coap ctcc test plat!!!"
		//电信正式平台
		#elif(COAP_PLAT == COAP_PLAT_CTCC_RELEASE)
			#define IOT_IP "117.60.157.137"
			#define default_SEVER_INDEX 1
			//#define default_UDP_SEVER_SETTING_M "AT+NCDP=117.60.157.137\r\n"
			//#define default_UDP_SEVER_SETTING_S "AT+NCDP=117.60.157.137\r\n"
		//华为测试平台
		#elif(COAP_PLAT == COAP_PLAT_HW_TEST)
			#define default_SEVER_INDEX 2
			#define IOT_IP "139.159.140.34"
			//#define default_UDP_SEVER_SETTING_M "AT+NCDP=139.159.140.34\r\n"
			//#define default_UDP_SEVER_SETTING_S "AT+NCDP=139.159.140.34\r\n"	
			#warning "nb iot used coap hw test plat!!!"
		//华为正式平台
		#elif(COAP_PLAT == COAP_PLAT_HW_RELEASE)
			#define default_SEVER_INDEX 2
			#define IOT_IP "139.159.140.34"
			//#define default_UDP_SEVER_SETTING_M "AT+NCDP=139.159.140.34\r\n"
			//#define default_UDP_SEVER_SETTING_S "AT+NCDP=139.159.140.34\r\n"
			#warning "nb iot used coap hw plat!!!"			
		#else
		#error "COAP_PLAT defined error!"
		#endif
	/*
	#if USED_NB_TEST_PLAT
		//测试平台

		#define IOT_IP "180.101.147.115"

		#ifndef default_UDP_SEVER_SETTING_M 
			#define default_UDP_SEVER_SETTING_M "AT+NCDP=180.101.147.115\r\n"
		#endif
		
		#ifndef default_UDP_SEVER_SETTING_S 
			#define default_UDP_SEVER_SETTING_S "AT+NCDP=180.101.147.115\r\n"
		#endif	
		#warning "nb iot used coap test plat!!!"
	#else 
		//正式平台
		#define IOT_IP "117.60.157.137"

		#ifndef default_UDP_SEVER_SETTING_M 
			#define default_UDP_SEVER_SETTING_M "AT+NCDP=117.60.157.137\r\n"
		#endif
		
		#ifndef default_UDP_SEVER_SETTING_S 
			#define default_UDP_SEVER_SETTING_S "AT+NCDP=117.60.157.137\r\n"
		#endif

	#endif	
	*/
	#endif

#ifdef __cplusplus
}
#endif
#endif
