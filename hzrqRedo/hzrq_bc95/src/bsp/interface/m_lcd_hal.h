#ifndef __m_lcd_hal_h__
#define __m_lcd_hal_h__

#ifdef __cplusplus
#endif
	/*
	#define LCD_PINS_AT_PORTA (uint32_t)(GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | \
								GPIO_PIN_6 | GPIO_PIN_7 | \
								GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10)

	//pins at portb for lcd
	#define LCD_PINS_AT_PORTB (uint32_t)(GPIO_PIN_0 | GPIO_PIN_1 |  \
								GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | \
								GPIO_PIN_10 | GPIO_PIN_11 |GPIO_PIN_12 | \
								GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15 | \
								GPIO_PIN_9)
	#define m_gpio_lcd_port_rcc_enable() __HAL_RCC_GPIOA_CLK_ENABLE();__HAL_RCC_GPIOB_CLK_ENABLE();	
	#define m_gpio_lcd_port_rcc_disable() __HAL_RCC_GPIOA_CLK_DISABLE();__HAL_RCC_GPIOB_CLK_DISABLE();	

	*/
	
	#define LCD_PINS_AT_PORTA (uint32_t)(GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_6 | GPIO_PIN_7 \
	| GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10)
	
	#define LCD_PINS_AT_PORTB (uint32_t)(GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_10 \
	| GPIO_PIN_11 | GPIO_PIN_5 | GPIO_PIN_9)
	
	#define LCD_PINS_AT_PORTC (uint32_t)(GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12)
	#define LCD_PINS_AT_PORTD (uint32_t)(GPIO_PIN_2)
	
	#define m_gpio_lcd_port_rcc_enable() do{ \
		__HAL_RCC_GPIOA_CLK_ENABLE(); \
		__HAL_RCC_GPIOB_CLK_ENABLE(); \
		__HAL_RCC_GPIOC_CLK_ENABLE(); \
		__HAL_RCC_GPIOD_CLK_ENABLE(); \
	}while(0);
	
	#define m_gpio_lcd_port_rcc_disable() do{ \
		__HAL_RCC_GPIOA_CLK_DISABLE(); \
		__HAL_RCC_GPIOB_CLK_DISABLE(); \
		__HAL_RCC_GPIOB_CLK_DISABLE(); \
		__HAL_RCC_GPIOD_CLK_DISABLE(); \
	}while(0);
	
	extern void m_lcd_class_init(void);
	extern void m_lcd_class_clear(void);
#ifdef __cplusplus
#endif

#endif
//file end
