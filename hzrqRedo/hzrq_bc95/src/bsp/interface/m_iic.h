#ifndef __m_iic_h__
#define __m_iic_h__

#ifdef __cplusplus
extern "C" {
#endif

	enum
	{
		IIC_NACK=1,
		IIC_ACK=0,
	};

	#define IIC_PWR_PIN		GPIO_PIN_15
	#define IIC_PWR_PORT	GPIOB
	//rd0<>scl
	#define IIC_SCL_PIN		GPIO_PIN_13
	#define IIC_SCL_PORT	GPIOB
	//rd11<>sda
	#define IIC_SDA_PIN		GPIO_PIN_14
	#define IIC_SDA_PORT 	GPIOB

	#define IIC_ALL_PIN	(IIC_PWR_PIN | IIC_SCL_PIN | IIC_SDA_PIN)
	#define IIC_PORT	GPIOB
	
	#define iic_pwr_hight() m_gpio_write(IIC_PWR_PORT,IIC_PWR_PIN,IIC_PWR_PIN)
	#define iic_pwr_low()	m_gpio_write(IIC_PWR_PORT,IIC_PWR_PIN,0)
	
	#define	iic_sda_hight() m_gpio_write(IIC_SDA_PORT,IIC_SDA_PIN,IIC_SDA_PIN)
	#define	iic_sda_low() 	m_gpio_write(IIC_SDA_PORT,IIC_SDA_PIN,0)
	
	#define iic_sda_get() 	m_gpio_read(IIC_SDA_PORT,IIC_SDA_PIN)
	
	#define	iic_scl_hight() m_gpio_write(IIC_SCL_PORT,IIC_SCL_PIN,IIC_SCL_PIN);
	#define	iic_scl_low() 	m_gpio_write(IIC_SCL_PORT,IIC_SCL_PIN,0);


	
	/*
	extern void iic_sda_mode_out(void);
	extern void iic_sda_mode_in(void);
	
	extern void iic_scl_mode_out(void);
	extern void iic_scl_mode_in(void);
	*/
	
	extern void iic_start(void);
	extern void iic_stop(void);
	extern uint8_t iic_send_byte(uint8_t x);
	extern uint8_t iic_read_byte(uint8_t slaveAddr,uint8_t addr);
	extern uint8_t iic_waite_ack(void);
	extern uint8_t iic_received_byte(void);
	extern uint8_t iic_received_byte_if_ack(uint8_t ack);
	
	extern void iic_pins_init(void);
	extern void iic_pins_deinit(void);

	/*
	typedef struct{
		uint8_t slaveAddr;
        void (*deviceVddOn)(void);
        void (*deviceVddOff)(void);
		void (*iic_start)(void);
		void (*iic_stop)(void);
		uint8_t (*iic_send_byte)(uint8_t);
		uint8_t (*iic_waite_ack)(void);
		uint8_t (*iic_received_byte_if_ack)(uint8_t ack);
	}st_iicDeviceObj,iicDeviceObj_t;
	*/
	
#ifdef __cplusplus
}
#endif

#endif
