#include "stdint.h"
#include "stm32l0xx_hal.h"
#include "m_buzzer.h"
#include "m_gpio_config.h"
#include "cmsis_os.h"
//extern void m_gpio_write(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint16_t x);
void m_buzzer_deinit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct={0};
	
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_InitStruct.Pin=BUZZER_PINS;
	GPIO_InitStruct.Mode=GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	//
	HAL_GPIO_Init(BUZZER_PORT, &GPIO_InitStruct);		
}
void m_buzzer_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct={0};
	
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_InitStruct.Pin=BUZZER_PINS;
	GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	//
	HAL_GPIO_Init(BUZZER_PORT, &GPIO_InitStruct);	
	m_buzzer_off();
}
void m_buzzer_on(void)
{
	m_gpio_write(BUZZER_PORT,BUZZER_PINS,BUZZER_PINS);
}
void m_buzzer_off(void)
{
	m_gpio_write(BUZZER_PORT,BUZZER_PINS,0);
}

void m_buzzer_start(void)
{
	m_buzzer_init();
	m_buzzer_on();
	osDelay(200);
	m_buzzer_off();
}

//file end
