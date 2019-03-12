#ifndef _CIPHER_SHA256_H
#define _CIPHER_SHA256_H

#include <stdint.h>
#ifdef  __cplusplus

extern "C" {
#endif /* __cplusplus */
    typedef unsigned int uint32;
    typedef struct {
        uint32 h[8];
        uint8_t block[64];
        int blkused;
        uint32 lenhi, lenlo;
    } SHA256_State;
    extern void SHA256_Init(SHA256_State * s);
    extern void SHA256_Bytes(SHA256_State * s, const void *p, int len);
    extern void SHA256_Final(SHA256_State * s, uint8_t *output);
    extern void SHA256_Simple(uint8_t *p, int len, uint8_t *output);
    //extern void sha256_do_hmac(uint8_t *hmac,uint8_t* key, int keylen,   uint8_t *blk, int len);
	//extern void sha256(uint8_t *p, int len, uint8_t *output) ;
	extern void sha256(uint8_t *out,uint8_t *in, int len) ;
	extern void hmac_sha256(uint8_t *hmac,uint8_t *in, int inLen,uint8_t *key, int keyLen);
#ifdef  __cplusplus
}
#endif /* __cplusplus */
#endif /* _CIPHER_SHA256_H */
