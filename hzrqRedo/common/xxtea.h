#ifndef __XXTEA_H__
#define __XXTEA_H__
#include <stdint.h>
	#ifdef __cplusplus
	extern "C"	{
	#endif

        extern const uint32_t XxteaKey[4];
        extern void btea(uint32_t* v, int16_t n, uint32_t* key);
		//extern void xxtea_encrypt(void);
		//extern void xxtea_dencrypt(void);
		extern void xxtea_encrypt(void);
        extern void xxtea_dencrypt(void);
    #ifdef __cplusplus
    }
    #endif
#endif
//file end
