#ifndef __M_ASE_H__
#define __M_ASE_H__
#ifdef __cplusplus
	extern "C"{
#endif



// #define the macros below to 1/0 to enable/disable the mode of operation.
//
// CBC enables AES128 encryption in CBC-mode of operation and handles 0-padding.
// ECB enables the basic ECB 16-byte block algorithm. Both can be enabled simultaneously.

// The #ifndef-guard allows it to be configured before #include'ing or at compile time.
#ifndef CBC
  #define CBC 1
#endif

#ifndef ECB
  #define ECB 1
#endif



#if defined(ECB) && ECB

extern void AES128_ECB_encrypt(uint8_t* input, const uint8_t* key, uint8_t *output);
extern void AES128_ECB_decrypt(uint8_t* input, const uint8_t* key, uint8_t *output);

#endif // #if defined(ECB) && ECB


#if defined(CBC) && CBC

extern void AES128_CBC_encrypt_buffer(uint8_t* output, uint8_t* input, uint32_t length, const uint8_t* key, const uint8_t* iv);
extern void AES128_CBC_decrypt_buffer(uint8_t* output, uint8_t* input, uint32_t length, const uint8_t* key, const uint8_t* iv);

#endif // #if defined(CBC) && CBC

extern  void test_encrypt_ecb(void);
extern void test_decrypt_ecb(void);
extern void test_encrypt_ecb_verbose(void);
extern void test_encrypt_cbc(void);
extern void test_decrypt_cbc(void);
extern uint16_t m_ebc_encrypt(uint8_t* output, uint8_t* input, uint8_t* key,uint32_t length);
extern uint16_t m_ebc_decrypt(uint8_t* output, uint8_t* input, uint8_t* key,uint32_t length);
#endif //_AES_H_
	
#ifdef __cplusplus
	}
#endif

//file end
//
