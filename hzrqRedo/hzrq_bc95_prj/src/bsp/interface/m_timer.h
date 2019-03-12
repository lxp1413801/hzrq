//file:m_timer.h
#ifndef __m_timer_h__
#define __m_timer_h__

#ifdef __cplusplus
extern "C"{
#endif
	#define TIMx                           TIM2
	#define TIMx_CLK_ENABLE()              __HAL_RCC_TIM2_CLK_ENABLE()


	/* Definition for TIMx's NVIC */
	#define TIMx_IRQn                      TIM2_IRQn
	#define TIMx_IRQHandler                TIM2_IRQHandler

	extern TIM_HandleTypeDef    TimHandle;
	extern void m_timer_init(void);
	extern void m_timer_deinit(void);

	#define IR_BUFFER_LEN 32
	extern uint8_t irRecevedBuf[IR_BUFFER_LEN];
	extern uint16_t irRecevedByte;
	extern uint16_t irSampleTimes;	
	extern void ir_data_init(void);
	extern void ir_received_ready(void);
	extern void ir_disable(void);
	extern uint32_t irTimeOut;
	extern void ir_idle_time_hook(void);
#ifdef __cplusplus
}
#endif
#endif
//file end
