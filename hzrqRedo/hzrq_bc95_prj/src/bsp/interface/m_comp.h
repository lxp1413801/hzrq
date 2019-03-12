#ifndef __M_ADC_H__
#define __M_ADC_H__
	#ifdef __cplusplus
		extern "C" {
	#endif
	//#include "stm32l0xx_hal.h"
	#include "./inc/stm32l0xx_hal.h"
	/* Exported types ------------------------------------------------------------*/
	/* Exported constants --------------------------------------------------------*/
	/* User can use this section to tailor COMPx instance used and associated 
	   resources */
	/* Definition for COMPx clock resources */
//	#define COMPx_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOC_CLK_ENABLE()
//	#define COMPx_GPIO_CLK_DISABLE()       __HAL_RCC_GPIOC_CLK_DISABLE()

	/* Definition for COMPx Channel Pin */
//	#define COMPx_PIN                      GPIO_PIN_0
//	#define COMPx_GPIO_PORT                GPIOC 

	/* Definition for COMPx's NVIC */
	#define COMP2_IRQn                   ADC1_COMP_IRQn
	#define COMP2_IRQHandler            ADC1_COMP_IRQHandler
	#define COMP2_IRQHandler			ADC1_COMP_IRQHandler
	/* Exported macro ------------------------------------------------------------*/
	/* Exported functions ------------------------------------------------------- */	
	extern void m_comp_config(void);
	extern void m_comp_init(void);
	extern void m_comp_start(COMP_HandleTypeDef *hcomp);
	extern void m_comp_stop(COMP_HandleTypeDef *hcomp);			
	#ifdef __cplusplus
		}
	#endif	
#endif
//file end
//@
