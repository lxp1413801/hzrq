#ifndef __m_rng_h__
#define __m_rng_h__
#ifdef __cplusplus
extern "C"{
#endif

extern RNG_HandleTypeDef RngHandle;
extern uint8_t m_rng_init(void);
extern uint8_t m_rng_generation(float *rng);
extern uint8_t m_rng_reinit(void);


#ifdef __cplusplus
}
#endif
#endif
//file end
