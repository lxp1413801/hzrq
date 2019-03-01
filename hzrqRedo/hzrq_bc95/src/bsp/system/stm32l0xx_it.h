/**
  ******************************************************************************
  * @file    FreeRTOS\FreeRTOS_LowPower\Inc\stm32l0xx_it.h
  * @author  MCD Application Team
  * @version V1.5.0
  * @date    8-January-2016
  * @brief   This file contains the headers of the interrupt handlers.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32L0xx_IT_H
#define __STM32L0xx_IT_H
//#include "core_cm0plus.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void DebugMon_Handler(void);
void SysTick_Handler(void);
void RTC_IRQHandler(void);
void EXTI0_1_IRQHandler(void);
void EXTI2_3_IRQHandler(void);
void EXTI4_15_IRQHandler(void);	
	
void ADC1_COMP_IRQHandler(void);

void USART1_IRQHandler(void);
void USART4_5_IRQHandler(void);
void LPUSART1_IRQHandler(void);
//add by lxp
extern uint32_t HAL_GetTick(void);
extern void HAL_SetTick(uint32_t x);
extern void HAL_IncTick(void);

extern void vPortEnterCritical( void );
extern void vPortExitCritical(void);

extern void osSystickHandler(void);
//extern void m_timer_2_irqhandler(void);
extern void TIM2_IRQHandler(void);
extern __IO uint32_t uwTick;
#ifdef __cplusplus
}
#endif

#endif /* __STM32L0xx_IT_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
