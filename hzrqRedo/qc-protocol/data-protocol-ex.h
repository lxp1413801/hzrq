#ifndef __data_protocol_ex_h__
#define __data_protocol_ex_h__


#ifdef __cplusplus
	extern "C"{
#endif 
	#include <stdint.h>

	extern void api_calc_all(void);
	extern void __hal_calc_all_redo(void);
	extern void api_calc_all_redo(void);
	extern int16_t api_sysdata_save(void);
	extern void m_thread_create_sysdata(void);
	
#ifdef __cplusplus
	}
#endif 
#endif
//file end
