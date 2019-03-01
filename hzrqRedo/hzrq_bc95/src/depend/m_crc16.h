#ifndef __M_CRC_16_H__
#define __M_CRC_16_H__


#ifdef __cplusplus
	extern "C"{
#endif
	
	
	extern uint16_t crc_sum(uint8_t *puchMsg, uint16_t usDataLen);
	extern uint16_t crc_verify(uint8_t* message, uint16_t n);
	extern void crc_append(uint8_t* message, uint16_t n);
		
#ifdef __cplusplus
	}
#endif

#endif

//file end
//
