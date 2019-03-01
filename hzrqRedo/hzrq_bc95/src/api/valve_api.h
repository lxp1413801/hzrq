//file name:valve_api.h
#ifndef __valve_api_h__
#define __valve_api_h__

#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif
	#define config_VALVE_STACK_SIZE 64

	#define tskVALVE_PRIORITY			(( UBaseType_t ) 3U )

	#define evenFlags_VALVE_ON				(1ul<<0)
	#define evenFlags_VALVE_OFF				(1ul<<1)
	#define evenFlags_VALVE_STOP			(1ul<<2)

	#define evenFlags_VALVE_ALL_BITS				(0x07)	

	extern void v_task_valve_create(void);
	/*
	typedef enum
	{
		VALVE_OFF=0,
		VALVE_ON=1,
	}em_VavleState_t;
	
	typedef enum
	{
		VALVE_OPERATION_NONE=0,
		VALVE_OPERATION_OFF=1,
		VALVE_OPERATION_ON=2,
	}em_VavleOperation_t;
	*/
	extern void valve_process(void);
	extern void app_valve_on(void);
	extern void app_valve_off(void);
//	extern volatile em_VavleState_t em_VavleState;
//	extern volatile em_VavleOperation_t em_VavleOperation;
	extern void vavle_off_from_app(uint8_t reson);
	extern void vavle_on_from_app(void);
	
	extern void vavle_off_for_start(void);
	
#ifdef __cplusplus
}
#endif

#endif

