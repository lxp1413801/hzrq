#ifndef __m_buzzer_h__
#define __m_buzzer_h__

	#ifdef __cplusplus
		extern "C"{
	#endif

	#define BUZZER_PINS		GPIO_PIN_2
	#define BUZZER_PORT		GPIOB
	
	extern void m_buzzer_deinit(void);
	extern void m_buzzer_init(void);
	extern void m_buzzer_on(void);
	extern void m_buzzer_off(void);
	//apl
	extern void m_buzzer_start(void);

	#ifdef __cplusplus
		}
	#endif
#endif
//file end
