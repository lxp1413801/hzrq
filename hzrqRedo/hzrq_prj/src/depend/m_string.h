#ifndef __M_STRING_H__
#define __M_STRING_H__

	#ifdef __cplusplus
	extern "C"{
	#endif
	extern void m_mem_set(uint8_t* buf,uint8_t x,uint16_t len);
	extern void m_mem_cpy(uint8_t* d,uint8_t* s);
	extern void m_mem_cpy_len(uint8_t* d,uint8_t* s,uint16_t len);
	extern uint16_t m_str_match(uint8_t* b,uint8_t* c);
	extern int32_t m_math_pow(int32_t x,int32_t y);
	extern uint16_t m_str_cmp(uint8_t* d,uint8_t* s);
	extern uint16_t m_str_b2h(uint8_t* h,uint8_t* b,uint16_t len);
	extern uint16_t m_str_h2b(uint8_t* b,uint8_t* h,uint16_t len);

	#ifdef __cplusplus
	}
	#endif
#endif
//file end
