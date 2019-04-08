#include "../../includes/includes.h"

#define __nop_delay() do{ \
__nop();__nop();__nop();__nop();__nop();__nop();__nop(); \
}while(0);

#define __nop_delay_1us() do{__nop();__nop();__nop();__nop();__nop();__nop();}while(0);
void delay_us(uint16_t us)
{	
	while(us--){
		__nop();
		__nop();
	}
}
//#define __nop_delay() delay_us(20);

void iic_pins_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct={0};
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH  ;
	GPIO_InitStruct.Pin	=IIC_ALL_PIN;
	HAL_GPIO_Init(IIC_PORT, &GPIO_InitStruct);	
	m_gpio_write(IIC_PORT,IIC_ALL_PIN,IIC_SCL_PIN | IIC_SDA_PIN);
	
	m_gpio_write(IIC_WRP_PORT,IIC_WRP_PIN,0);
}

void iic_pins_deinit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct={0};
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitStruct.Mode      = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull      = GPIO_NOPULL;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH  ;
	GPIO_InitStruct.Pin	=IIC_ALL_PIN;
	HAL_GPIO_Init(IIC_PORT, &GPIO_InitStruct);	
	m_gpio_write(IIC_PORT,IIC_ALL_PIN,IIC_ALL_PIN);
}

void iic_sda_mode_out(void)
{
	GPIO_InitTypeDef GPIO_InitStruct={0};
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH  ;
	GPIO_InitStruct.Pin	=IIC_SDA_PIN;
	HAL_GPIO_Init(IIC_SDA_PORT, &GPIO_InitStruct);	
}

void iic_sda_mode_in(void)
{
	GPIO_InitTypeDef GPIO_InitStruct={0};
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH  ;
	GPIO_InitStruct.Pin	=IIC_SDA_PIN;
	HAL_GPIO_Init(IIC_SDA_PORT, &GPIO_InitStruct);	
}

void iic_start(void)
{

	iic_sda_mode_out();
    
	iic_sda_hight();
	iic_scl_hight();
	delay_us(4);
	iic_sda_low();
	delay_us(4);
	iic_scl_low();
	delay_us(4);
}

void iic_stop(void)
{

	iic_sda_mode_out();
	iic_sda_low();
	delay_us(4);
	iic_scl_hight();
	delay_us(4);
	iic_sda_hight();
	delay_us(4);
}

void iic_send_ack(int8_t ack)
{

	iic_sda_mode_out();
	iic_scl_low();
	__nop_delay();
	if(ack){
		iic_sda_hight();
	}else{
		iic_sda_low();
	}
	__nop_delay();
	iic_scl_hight();
	__nop_delay();
	iic_scl_low();
	__nop_delay();
}

uint8_t iic_waite_ack(void)
{
	uint8_t timeout=200;

	iic_sda_mode_in();
    iic_scl_low();
    __nop_delay();
	iic_scl_hight();
    __nop_delay();
	while(timeout--){
		if(!iic_sda_get())break;
	}
	if(!timeout){
		iic_stop();
		return 0;
	}
	iic_scl_low();
    __nop_delay();
	return 1;	
}

uint8_t iic_send_byte(uint8_t x)
{
	uint8_t i;

	iic_sda_mode_out();
	iic_scl_low();
	__nop_delay();
	for(i=0;i<8;i++){       
		if(x&0x80){
			iic_sda_hight();
        }
		else{
			iic_sda_low();
        }
		__nop_delay();
		iic_scl_hight();
		//__nop_delay();
        __nop_delay();
		iic_scl_low();
        //__nop_delay();
        __nop_delay();
		x<<=1;
	}
	return iic_waite_ack();
}

uint8_t iic_received_byte(void)
{
	uint8_t i;
	uint8_t ret=0;

	iic_sda_mode_in();//iic_sda_mode_in();
	for(i=0;i<8;i++){
        ret<<=1;
		iic_scl_low();
        __nop_delay();
		iic_scl_hight();
        __nop_delay();
		if(iic_sda_get()!=0){
			ret|=1;
		}
	}
	iic_scl_low();
    __nop_delay();
	return ret;
}
uint8_t iic_received_byte_if_ack(uint8_t ack)
{
	uint8_t ret=0;
	ret=iic_received_byte();
	iic_send_ack(ack);
	return ret;
}

