#include "stdint.h"
#include "stm32l0xx_hal.h"
//#include "m_gpio_config.h"
#include "m_lcd_hal.h"
#include "m_lcd.h"
#include "m_gpio_config.h"

void m_lcd_class_init(void)
{
	//LCDHandle.State=0;
	LCDHandle.Instance = LCD;
	LCDHandle.Init.MuxSegment=0;
	LCDHandle.Init.Prescaler = LCD_PRESCALER_2;
	//LCDHandle.Init.Prescaler =LCD_PRESCALER_8;
	//LCDHandle.Init.Divider = LCD_DIVIDER_31;
	LCDHandle.Init.Divider =LCD_DIVIDER_16;
	LCDHandle.Init.Duty = LCD_DUTY_1_8;
	LCDHandle.Init.Bias = LCD_BIAS_1_4;
	LCDHandle.Init.HighDrive=LCD_HIGHDRIVE_0;

	LCDHandle.Init.VoltageSource = LCD_VOLTAGESOURCE_INTERNAL;
	LCDHandle.Init.Contrast = LCD_CONTRASTLEVEL_4;
	
	//LCDHandle.Init.VoltageSource =LCD_VOLTAGESOURCE_EXTERNAL;
	
	LCDHandle.Init.DeadTime = LCD_DEADTIME_0;
	LCDHandle.Init.PulseOnDuration = LCD_PULSEONDURATION_7;
//LCDHandle.Init.HighDrive = DISABLE;
	LCDHandle.Init.BlinkMode = LCD_BLINKMODE_OFF;
	LCDHandle.Init.BlinkFrequency = LCD_BLINKFREQUENCY_DIV8;
	/* Initialize the LCD */
	HAL_LCD_Init(&LCDHandle);
}

void m_lcd_class_clear(void)
{
	HAL_LCD_Clear(&LCDHandle); 
}

void m_gpio_config_lcd(void)
{

	GPIO_InitTypeDef GPIO_InitStruct;
	m_gpio_lcd_port_rcc_enable();
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_NOPULL;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH  ;
	GPIO_InitStruct.Alternate = GPIO_AF1_LCD;
	GPIO_InitStruct.Pin	=LCD_PINS_AT_PORTA;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin=LCD_PINS_AT_PORTB;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin=LCD_PINS_AT_PORTC;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);	

	GPIO_InitStruct.Pin=LCD_PINS_AT_PORTD;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);		
	//m_gpio_set_mode(GPIOA,LCD_PINS_AT_PORTA,GPIO_AF1_LCD);
	//m_gpio_set_mode(GPIOB,LCD_PINS_AT_PORTB,GPIO_AF1_LCD);
	//m_gpio_set_mode(GPIOC,LCD_PINS_AT_PORTC,GPIO_AF1_LCD);
	//m_gpio_set_mode(GPIOD,LCD_PINS_AT_PORTD,GPIO_AF1_LCD);
}

void HAL_LCD_MspInit(LCD_HandleTypeDef *hlcd)
{
	
	//if(LCDHandle.State)return;
	
	m_gpio_config_lcd();
	/*##-6- Enable LCD peripheral Clock ########################################*/
	__HAL_RCC_LCD_CLK_ENABLE();
	//LCDHandle.State=1;
	LCDHandleStat=1;
}

void HAL_LCD_MspDeInit(LCD_HandleTypeDef *hlcd)
{
  /* Enable LCD reset state */
  __HAL_RCC_LCD_FORCE_RESET();
  
  /* Release LCD from reset state */
  __HAL_RCC_LCD_RELEASE_RESET();
}

//file end
