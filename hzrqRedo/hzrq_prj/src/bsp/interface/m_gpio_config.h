#ifndef __M_GPIO_CONFIG_H__
#define __M_GPIO_CONFIG_H__
	#ifdef __cplusplus
	extern "C" {
	#endif
	//define lcd pot and lcd pin
	
	typedef struct __PORT_PINS
	{
		GPIO_TypeDef * stPortx;
		uint32_t pins;
	}stPortPins_t;
	
	 
	#define __myReg_RCC_IOPENR (*( ( volatile uint32_t *)	(0x40021000+0x002c)))
	#define __myReg_GPIOA_MODER (*( ( volatile uint32_t *)	(0x50000000+0x0000)))
	#define __myReg_GPIOA_IDR (*( ( volatile uint32_t *)	(0x50000000+0x0010)))
	#define __myReg_GPIOA_ODR (*( ( volatile uint32_t *)	(0x50000000+0x0014)))			
	
	#define __myReg_GPIOB_MODER (*( ( volatile uint32_t *)	(0x50000400+0x0000)))	
	#define __myReg_GPIOB_IDR (*( ( volatile uint32_t *)	(0x50000400+0x0010)))
	#define __myReg_GPIOB_ODR (*( ( volatile uint32_t *)	(0x50000400+0x0014)))	
	
	#define __myReg_GPIOC_MODER (*( ( volatile uint32_t *)	(0x50000800+0x0000)))
		
	#define __myReg_GPIOC_IDR (*( ( volatile uint32_t *)	(0x50000800+0x0010)))
	#define __myReg_GPIOC_ODR (*( ( volatile uint32_t *)	(0x50000800+0x0014)))
		
	
	#define __myReg_GPIOD_MODER (*( ( volatile uint32_t *)	(0x50000c00+0x0000)))
	#define __myReg_GPIOE_MODER (*( ( volatile uint32_t *)	(0x50001000+0x0000)))
	#define __myReg_GPIOH_MODER (*( ( volatile uint32_t *)	(0x50001c00+0x0000)))
	#define __myReg_RCC_APB1ENR (*( ( volatile uint32_t *)	(0x40021000+0x0038)))
	//add by a.la.l. ,2017.10.17
	#define __myReg_EXTI_IMR (*( ( volatile uint32_t *)	(0x40010400+0x0000)))
	#define __myReg_EXTI_EMR (*( ( volatile uint32_t *)	(0x40010400+0x0004)))
	
	
	#define		PIN0_MODE_ANALOG	(0x03ul<<0)
	#define		PIN1_MODE_ANALOG	(0x03ul<<2)	
	#define		PIN2_MODE_ANALOG	(0x03ul<<3)
	#define		PIN3_MODE_ANALOG	(0x03ul<<6)	
	#define		PIN4_MODE_ANALOG	(0x03ul<<8)
	#define		PIN5_MODE_ANALOG	(0x03ul<<10)	
	#define		PIN6_MODE_ANALOG	(0x03ul<<12)
	#define		PIN7_MODE_ANALOG	(0x03ul<<14)	
	#define		PIN8_MODE_ANALOG	(0x03ul<<16)
	#define		PIN9_MODE_ANALOG	(0x03ul<<18)	
	#define		PIN10_MODE_ANALOG	(0x03ul<<20)
	#define		PIN11_MODE_ANALOG	(0x03ul<<22)	
	#define		PIN12_MODE_ANALOG	(0x03ul<<24)
	#define		PIN13_MODE_ANALOG	(0x03ul<<26)	
	#define		PIN14_MODE_ANALOG	(0x03ul<<28)
	#define		PIN15_MODE_ANALOG	(0x03ul<<30)
	
	
	/*------------------------------------------------------------------------
	* define port and pins for debug 
	* adc123in0 and adc 123in1,ain0 for Voltage monitoring,
	*-----------------------------------------------------------------------*/
	#define DEBUG_PORT		GPIOA
	#define DEBUG_PINS		(GPIO_PIN_13|GPIO_PIN_14)
	#define m_gpio_debug_reset() HAL_GPIO_DeInit(DEBUG_PORT,DEBUG_PINS);



	#define VAVLE_CTRL_PIN0	(uint32_t)(GPIO_PIN_11)
	#define VAVLE_CTRL_PIN1	(uint32_t)(GPIO_PIN_12)
	#define VAVLE_CTRL_PIN	(uint32_t)(VAVLE_CTRL_PIN0 | VAVLE_CTRL_PIN1)	
	#define VAVLE_CTRL_PORT GPIOA
	#define m_gpio_config_vavle_rcc_enable() __HAL_RCC_GPIOA_CLK_ENABLE()
	#define m_gpio_config_vavle_rcc_disable() __HAL_RCC_GPIOA_CLK_DISABLE()
	extern void m_gpio_config_vavle(void);
	extern void m_gpio_write(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint16_t x);
	#define m_vavle_on() m_gpio_write(VAVLE_CTRL_PORT,VAVLE_CTRL_PIN,VAVLE_CTRL_PIN0);
	#define m_vavle_off() m_gpio_write(VAVLE_CTRL_PORT,VAVLE_CTRL_PIN,VAVLE_CTRL_PIN1);
	#define m_vavle_stop() m_gpio_write(VAVLE_CTRL_PORT,VAVLE_CTRL_PIN,0);
	
	/*------------------------------------------------------------------------
	* define port and pins for led
	* led:pc6
	*-----------------------------------------------------------------------*/	
	/*
	#define LED_PIN (uint32_t)(GPIO_PIN_6)
	#define LED_PORT	GPIOC
	extern const stPortPins_t stLedPortPins[];

	#define m_gpio_config_led_rcc_enable() __HAL_RCC_GPIOC_CLK_ENABLE()
	#define m_gpio_config_led_rcc_disable() __HAL_RCC_GPIOC_CLK_DISABLE()
	
	#define led_on() HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET)
	
	#define led_off() HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET)
	
	#define led_on_xx() (__myReg_GPIOC_ODR &= (~LED_PIN));
	#define led_off_xx() (__myReg_GPIOC_ODR |= LED_PIN);
	
	#define led_toggle() HAL_GPIO_TogglePin(LED_PORT, LED_PIN)
	extern void m_gpio_config_led(void);
	extern void m_gpio_config_led_xx(void);
	*/
	
	/*------------------------------------------------------------------------
	* define port and pins for buzzer
	* led:pb2
	*-----------------------------------------------------------------------*/	
	#define BUZZER_PIN (uint32_t)(GPIO_PIN_2)
	#define BUZZER_PORT GPIOB
	
	#define m_gpio_config_buzzer_rcc_enable() __HAL_RCC_GPIOB_CLK_ENABLE()
	#define m_gpio_config_buzzer_rcc_disable() __HAL_RCC_GPIOB_CLK_DISABLE()
	
	#define buzzer_on() HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_SET)
	#define buzzer_off() HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_RESET)
	extern void m_gpio_config_buzzer(void);
	
	
	//<<--mod ok
	
	#define FLOW_PULSE_PIN (uint32_t)(GPIO_PIN_2)
	#define FLOW_PULSE_PORT	GPIOC
	#define m_gpio_flow_port_rcc_enable() __HAL_RCC_GPIOC_CLK_ENABLE()
	#define m_gpio_flow_port_rcc_disable() __HAL_RCC_GPIOC_CLK_DISABLE()
	extern void m_gpio_config_flow_pin(void);
	extern void m_gpio_config_flow_irq_disable(void);
	extern void m_gpio_config_flow_irq_re_enable(void);
	#define m_gpio_get_flow_pin_status() m_gpio_read(FLOW_PULSE_PORT,FLOW_PULSE_PIN) 

	extern void m_gpio_exti2_3_enable(void);
	extern void m_gpio_exti2_3_disable(void);	
	//for ste
	
	#define STE_PIN (uint32_t)(GPIO_PIN_3)
	#define STE_PORT	GPIOC
	#define m_gpio_ste_port_rcc_enable() __HAL_RCC_GPIOC_CLK_ENABLE()
	#define m_gpio_ste_port_rcc_disable() __HAL_RCC_GPIOC_CLK_DISABLE()
	extern void m_gpio_config_ste(void);
	extern void m_gpio_config_ste_irq_disable(void);
	extern void m_gpio_config_ste_irq_re_enable(void);	
	#define m_gpio_get_ste_pin_status() m_gpio_read(STE_PORT,STE_PIN)
	
	//#define m_gpio_ste_exit_enable() m_gpio_exti2_3_enable()
	//#define m_gpio_ste_exit_disable() m_gpio_exti2_3_disable();
	//-->>
	/*
	#define m_gpio_ste_exit_enable() do{ \
		HAL_NVIC_SetPriority(EXTI0_1_IRQn, 3, 0); \
		HAL_NVIC_EnableIRQ(EXTI0_1_IRQn); \
		}while(0);
	#define m_gpio_ste_exit_disable() do{ \
		HAL_NVIC_DisableIRQ(EXTI0_1_IRQn); \
		}while(0);	
	
	*/
	
	
	
	/*------------------------------------------------------------------------
	* define port and pins for key 0 input 
	* key0:pc9
	*-----------------------------------------------------------------------*/	
	#define KEY0_PIN (uint32_t)(GPIO_PIN_9)
	#define KEY0_PORT	GPIOC
	
	#define m_gpio_key0_port_rcc_enable() __HAL_RCC_GPIOC_CLK_ENABLE()
	#define m_gpio_key0_port_rcc_disable() __HAL_RCC_GPIOC_CLK_DISABLE()
	extern void m_gpio_config_key0_irq(void);
	extern void m_gpio_exti4_15_enable(void);
	extern void m_gpio_exti4_15_disable(void);
	
	extern void m_gpio_config_key0_irq_disable(void);
	extern void m_gpio_config_key0_irq_re_enable(void);


	//<<-- modified ok
	#define BATTARY_VOLTAGE_LEAK_PIN	(uint32_t)(GPIO_PIN_13)
	#define BATTARY_VOLTAGE_LEAK_PORT	GPIOC
	
	#define m_gpio_battary_leak_port_rcc_enable() __HAL_RCC_GPIOC_CLK_ENABLE()
	#define m_gpio_battary_leak_port_rcc_disable() __HAL_RCC_GPIOC_CLK_DISABLE()
	
	
	extern void m_gpio_config_ain_leak_enable(void);
	extern void m_gpio_config_ain_leak_disable(void);
	
	//-->>
	
	
	//<<--mod ok
	#define ALARM1_DECT_PIN (uint32_t)(GPIO_PIN_12)
	#define ALARM1_DECT_PORT	GPIOB
	#define m_gpio_alarm_dect_port_rcc_enable() __HAL_RCC_GPIOB_CLK_ENABLE()
	#define m_gpio_alarm_dect_port_rcc_disable() __HAL_RCC_GPIOB_CLK_DISABLE()	
	extern void m_gpio_config_alarm_dect(void);
	

	#define ALARM1_PIN (uint32_t)(GPIO_PIN_0)
	#define ALARM1_PORT	GPIOA
	extern void m_gpio_config_alarm(void);
	extern void m_gpio_config_alarm_irq_disable(void);
	extern void m_gpio_config_alarm_irq_re_enable(void);
	#define m_gpio_alarm_port_rcc_enable() __HAL_RCC_GPIOA_CLK_ENABLE()
	#define m_gpio_alarm_port_rcc_disable() __HAL_RCC_GPIOA_CLK_DISABLE()
	//	
	//-->>
	
	
	extern void m_gpio_config_lcd(void);
	extern void m_gpio_config_usart1(void);
	extern void m_gpio_config_usart1_input(void);
	extern void m_gpio_config_ainx(void);
	extern void m_gpio_config_ain_en(void);
	extern void m_gpio_config_ain_en_xx(void);
	
	extern void m_gpio_config_led(void);
	
	extern void m_gpio_into_lpw(void);
	
	#ifndef m_gpio_read
	#define m_gpio_read(GPIOx,GPIO_Pin) (GPIOx->IDR & GPIO_Pin)
	#endif
	
	#ifndef m_gpio_read
	extern m_gpio_read(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
	#endif
	
	extern void m_gpio_write(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint16_t x);
	extern void m_gpio_set_mode(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t mode);


		

		
	//<<--add for open the shell,mod ok
	//pb8
	#define SHELL_OPNE_DEC_PIN	(uint32_t)(GPIO_PIN_8)
	#define SHELL_OPNE_DEC_PORT	GPIOB
	#define m_shell_open_rcc_enable() __HAL_RCC_GPIOB_CLK_ENABLE()
	#define m_shell_open_rcc_disable() __HAL_RCC_GPIOB_CLK_DISABLE()	
	extern void m_gpio_config_shell_open(void);
	//-->>
	
	

	 
	//<<--add for adc
	#define BAT_ANIN_PIN (uint32_t)(GPIO_PIN_1)
	#define BAT_ANIN_PORT GPIOC
	extern void m_gpio_config_adcin11(void);
	//-->>

	//<<--add for ir received power
	#define IR_RX_PWR_PIN (uint32_t)(GPIO_PIN_0)
	#define IR_RX_PWR_PORT	GPIOC
	#define m_gpio_ir_rx_power_port_rcc_enable() __HAL_RCC_GPIOC_CLK_ENABLE()
	#define m_gpio_ir_rx_power_port_rcc_disable() __HAL_RCC_GPIOC_CLK_DISABLE()		
	
	//extern void m_gpio_config_ir_rx_pwr_enable(void);
	//extern void m_gpio_config_ir_rx_pwr_disable(void);
	
	extern void m_gpio_ir_rx_pwr_on(void);
	extern void m_gpio_ir_rx_pwr_off(void);
	//-->>
	
	//#define IR_PWR_PIN (uint32_t)(GPIO_PIN_6)
	//#define IR_PWR_PORT	GPIOC
	//#define m_gpio_ir_pwr_rcc_enable() __HAL_RCC_GPIOC_CLK_ENABLE()
	//#define m_gpio_ir_pwr_rcc_disable() __HAL_RCC_GPIOC_CLK_DISABLE()		
	//extern void m_gpio_ir_pwr_config(void);
	//extern void m_gpio_ir_pwr_on(void);
	//extern void m_gpio_ir_pwr_off(void);
	//extern void m_gpio_ir_sig_comfig(void);
	//extern void m_gpio_ir_sig_irq_enanle(void);
	//extern void m_gpio_ir_sig_irq_disanle(void);		
	//-->>
	#define NB_MTK_PWR_PIN	(uint32_t)(GPIO_PIN_7)
	#define NB_MTK_PWR_PORT	GPIOC

	#define NB_MTK_PSM_PIN	(uint32_t)(GPIO_PIN_5)
	#define NB_MTK_PSM_PORT	GPIOA
	
	#define NB_MTK_RST_PIN	(uint32_t)(GPIO_PIN_15)
	#define NB_MTK_RST_PORT	GPIOA			
		
		
	#ifdef __cplusplus
	}
	#endif
#endif
/*
user\interface\m_gpio_config.h(51): error:  #40: expected an identifier
*/
//file end
//@
