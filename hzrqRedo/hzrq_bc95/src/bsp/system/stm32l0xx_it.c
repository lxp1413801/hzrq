/**
  ******************************************************************************
  * @file    FreeRTOS\FreeRTOS_LowPower\Src\stm32l0xx_it.c
  * @author  MCD Application Team
  * @version V1.5.0
  * @date    8-January-2016
  * @brief   Main Interrupt Service Routines.
  ******************************************************************************
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

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "./inc/stm32l0xx_hal.h"
//#include "cmsis_os.h"
#include "stm32l0xx_it.h"
#include "../interface/m_uart.h"
#include "../interface/m_pwr_mng.h"
//#include "../interface/m_timer.h"
//#include "cmsis_os.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0+ Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
	__nop();
	__nop();
	mcu_error_reset();
	while(1);
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
	/* Go to infinite loop when Hard Fault exception occurs */
	mcu_error_reset();
	while (1);
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
	/* Go to infinite loop when Memory Manage exception occurs */
	mcu_error_reset();
	while (1);
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
	/* Go to infinite loop when Bus Fault exception occurs */
	mcu_error_reset();
	while (1);
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */

void UsageFault_Handler(void)
{
	/* Go to infinite loop when Usage Fault exception occurs */
	mcu_error_reset();
	while (1);
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
	/* Go to infinite loop when Usage Fault exception occurs */
	mcu_error_reset();
	while (1);
}


/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */

uint32_t HAL_GetTick(void)
{
	return uwTick;
}

void HAL_SetTick(uint32_t x)
{
	vPortEnterCritical();
	uwTick=x;
	vPortExitCritical();
}
void HAL_IncTick(void)
{
	uwTick++;
	
}
extern void uart4_idle_time_hook(void);
extern void ir_idle_time_hook(void);
//extern void lpusart1_idle_time_hook(void);
void SysTick_Handler(void)
{

	HAL_IncTick();
	//uart4_idle_time_hook();
	m_lpusart1_idle_time_hook();
	m_usart1_se_idle_time_hook();
	//m_usart1_se_idle_time_reset();
	//ir_idle_time_hook();
	osSystickHandler();
	//add by lxp
}

/******************************************************************************/
/*                 STM32L0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32l0xx.s).                                               */
/******************************************************************************/
/**
  * @brief  This function handles UART interrupt request.  
  * @param  None
  * @retval None
  * @Note   This function is redefined in "main.h" and related to DMA  
  *         used for USART data transmission     
  */
extern UART_HandleTypeDef UartHandle;
//#define USARTx_IRQHandler() HAL_UART_IRQHandler(&UartHandle);

void USART4_5_IRQHandler(void)
{
	vPortEnterCritical();
	HAL_UART_IRQHandler(&UartHandle);
	vPortExitCritical();
}
void USART1_IRQHandler(void)
{
	vPortEnterCritical();
	HAL_UART_IRQHandler(&UartHandleSe);
	vPortExitCritical();
}
void LPUSART1_IRQHandler(void)
{
	vPortEnterCritical();
	HAL_UART_IRQHandler(&UartHandle);
	vPortExitCritical();
}

/******************************************************************************/
/*                 STM32L0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32l0xx.s).                                               */
/******************************************************************************/
/**
  * @brief  This function handles ADC interrupt request.
  * @param  None
  * @retval None
  */

 COMP_HandleTypeDef   Comp2Handle;
extern ADC_HandleTypeDef    AdcHandle;
void ADC1_COMP_IRQHandler(void)
{
	vPortEnterCritical();
	HAL_COMP_IRQHandler(&Comp2Handle);
	HAL_ADC_IRQHandler(&AdcHandle);
	vPortExitCritical();
}
/**
  * @brief  This function handles external lines 4 to 15 interrupt request.
  * @param  None
  * @retval None
  */
//void EXTI4_15_IRQHandler(void)
//{
//  HAL_GPIO_EXTI_IRQHandler(TAMPER_BUTTON_PIN);
//}
//#define EXTI4_15_IRQHandler() HAL_GPIO_EXTI_IRQHandler()
//#define EXTI0_1_IRQHandler() HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0)
extern void m_gpio_exti0_1_irqhandler(void);
void EXTI0_1_IRQHandler(void)
{
	vPortEnterCritical();
	m_gpio_exti0_1_irqhandler();
	vPortExitCritical();
}
extern void m_gpio_exti2_3_irqhandler(void);
void EXTI2_3_IRQHandler(void)
{
	vPortEnterCritical();
	m_gpio_exti2_3_irqhandler();
	vPortExitCritical();
}
extern void m_gpio_exti4_15_irqhandler(void);
//#define 

void EXTI4_15_IRQHandler(void) 
{
	vPortEnterCritical();
	m_gpio_exti4_15_irqhandler();
	vPortExitCritical();
}
extern RTC_HandleTypeDef RtcHandle;
void RTC_IRQHandler(void)
{
	vPortEnterCritical();
	HAL_RTCEx_WakeUpTimerIRQHandler(&RtcHandle);
	vPortExitCritical();
}
//extern TIM_HandleTypeDef    TimHandle;
/*
void m_timer_2_irqhandler(void)
{
	HAL_TIM_IRQHandler(&TimHandle);
}
void TIM2_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&TimHandle);
}
*/
/******************************************************************************/
/*                 STM32L0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32l0xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
