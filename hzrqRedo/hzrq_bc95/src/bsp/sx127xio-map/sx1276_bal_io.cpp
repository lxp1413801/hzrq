#include "../../includes/includes.h"
#include "sx1276-bal.h"
//#include "../../emlib/inc/em_gpio.h"
#include "./sx1276_cpp.h"
/*
	typedef struct{
		bool enable;
		GPIO_Port_TypeDef port;
		unsigned int pin;
		GPIO_Mode_TypeDef type;
		unsigned int out;
	}st_Sx1276PortPinDef;
*/

typedef GPIO_TypeDef __ioDef_t;
typedef uint32_t __ioMode_t;



const st_Sx1276IOMap Sx1276IoMapMaster=
{
/*enable,port,		pin,type,				out*/
{true,	GPIOB,	GPIO_PIN_13,	GPIO_MODE_OUTPUT_PP,	1},	/*ioRST*/
{true,	GPIOA,	GPIO_PIN_4,		GPIO_MODE_OUTPUT_PP,	1},	/*ioNSS*/
{true,	GPIOA,	GPIO_PIN_5,		GPIO_MODE_OUTPUT_PP,	1},	/*ioSCK*/
{true,	GPIOA,	GPIO_PIN_6,		GPIO_MODE_OUTPUT_PP,	1},	/*ioMOSI*/
{true,	GPIOA,	GPIO_PIN_7,		GPIO_MODE_INPUT,		0},	/*ioMISO*/
{true,	GPIOA,	GPIO_PIN_0,		GPIO_MODE_INPUT,		0},	/*ioDIO0*/
{true,	GPIOA,	GPIO_PIN_1,		GPIO_MODE_INPUT,		0},	/*ioDIO1*/
{true,	GPIOA,	GPIO_PIN_2,		GPIO_MODE_INPUT,		0},	/*ioDIO2*/
{true,	GPIOA,	GPIO_PIN_3,		GPIO_MODE_INPUT,		0},	/*ioDIO3*/
{true,	GPIOB,	GPIO_PIN_10,	GPIO_MODE_INPUT,		0},	/*ioDIO4*/
{true,	GPIOB,	GPIO_PIN_11,	GPIO_MODE_INPUT,		0},	/*ioDIO5*/

{true,	GPIOB,	GPIO_PIN_15,	GPIO_MODE_OUTPUT_PP,	0},	/*swCtrlTX*/
{true,	GPIOB,	GPIO_PIN_14,	GPIO_MODE_OUTPUT_PP,	1},	/*swCtrlRX*/

{false,	(GPIO_TypeDef*)0,0,		0,						0},	/*pwrCtrlPA*/
{true,	GPIOB,GPIO_PIN_0,		GPIO_MODE_OUTPUT_PP,	0},	/*pwrCtrlLNA*/	
};
void cls_Sx1276::gpio_pin_mode_set(GPIO_TypeDef* port,uint32_t pin,uint32_t mode,uint32_t out){
	GPIO_InitTypeDef GPIO_InitStruct;
	//m_gpio_key0_port_rcc_enable();
	GPIO_InitStruct.Pin=pin;
	GPIO_InitStruct.Pull=GPIO_PULLUP;
	GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Mode=mode;
	HAL_GPIO_Init(port,&GPIO_InitStruct);
}

void cls_Sx1276::sx1276_bal_rf_pa_init(void){
	st_Sx1276PortPinDef* stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->pwrCtrlPA);
	if(stp->enable)GPIO_PinModeSet(stp->port,stp->pin,stp->type,stp->out);
}
void cls_Sx1276::sx1276_bal_rf_pa_on(void){
	st_Sx1276PortPinDef* stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->pwrCtrlPA);
	if(stp->enable)GPIO_PinOutSet(stp->port,stp->pin);
}
void cls_Sx1276::sx1276_bal_rf_pa_off(void)
{
	st_Sx1276PortPinDef* stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->pwrCtrlPA);
	if(stp->enable)	GPIO_PinOutClear(stp->port,stp->pin);
}

void cls_Sx1276::sx1276_bal_rf_lna_init(void){
	st_Sx1276PortPinDef* stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->pwrCtrlLNA);
	if(stp->enable)GPIO_PinModeSet(stp->port,stp->pin,stp->type,stp->out);
}
void cls_Sx1276::sx1276_bal_rf_lna_on(void){
	st_Sx1276PortPinDef* stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->pwrCtrlLNA);
	if(stp->enable)GPIO_PinOutSet(stp->port,stp->pin);
}
void cls_Sx1276::sx1276_bal_rf_lna_off(void)
{
	st_Sx1276PortPinDef* stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->pwrCtrlLNA);
	if(stp->enable)	GPIO_PinOutClear(stp->port,stp->pin);
}
void cls_Sx1276::sx1276_bal_rf_sw_init(void)
{
	st_Sx1276PortPinDef* stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->swCtrlRX);
	if(stp->enable)GPIO_PinModeSet(stp->port,stp->pin,stp->type,stp->out);
	stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->swCtrlTX);
	if(stp->enable)GPIO_PinModeSet(stp->port,stp->pin,stp->type,stp->out);
}
void cls_Sx1276::sx1276_bal_rf_sw_tx(void)
{
	st_Sx1276PortPinDef* stp;
	stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->swCtrlRX);
	if(stp->enable)	GPIO_PinOutClear(stp->port,stp->pin);
	stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->swCtrlTX);
	if(stp->enable)	GPIO_PinOutSet(stp->port,stp->pin);
}
void cls_Sx1276::sx1276_bal_rf_sw_rx(void)
{
	st_Sx1276PortPinDef* stp;
	stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->swCtrlTX);
	if(stp->enable)	GPIO_PinOutClear(stp->port,stp->pin);
	stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->swCtrlRX);
	if(stp->enable)	GPIO_PinOutSet(stp->port,stp->pin);
}
void cls_Sx1276::sx1276_bal_rf_sw_all_off(void)
{
	st_Sx1276PortPinDef* stp;
	stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->swCtrlRX);
	if(stp->enable)	GPIO_PinOutClear(stp->port,stp->pin);
	stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->swCtrlTX);
	if(stp->enable)	GPIO_PinOutClear(stp->port,stp->pin);	
}
void 	cls_Sx1276::sx1276_bal_diox_init(void)
{
	//todo
}
void cls_Sx1276::sx1276_bal_reset_pin_init(void)
{
	st_Sx1276PortPinDef* stp;
	stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->ioRST);
	if(stp->enable)	GPIO_PinModeSet(stp->port,stp->pin,stp->type,stp->out);	
}
void 	cls_Sx1276::sx1276_bal_reset_pin_hight(void)
{
	st_Sx1276PortPinDef* stp;
	stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->ioRST);
	if(stp->enable)	GPIO_PinOutSet(stp->port,stp->pin);	
}
void 	cls_Sx1276::sx1276_bal_reset_pin_low(void)
{
	st_Sx1276PortPinDef* stp;
	stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->ioRST);
	if(stp->enable)	GPIO_PinOutClear(stp->port,stp->pin);
}
void 	cls_Sx1276::sx1276_bal_spi_port_init(void)
{
	st_Sx1276PortPinDef* stp;
	stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->ioNSS);
	if(stp->enable)GPIO_PinModeSet(stp->port,stp->pin,stp->type,stp->out);
	stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->ioSCK);
	if(stp->enable)GPIO_PinModeSet(stp->port,stp->pin,stp->type,stp->out);	
	stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->ioMISO);
	if(stp->enable)GPIO_PinModeSet(stp->port,stp->pin,stp->type,stp->out);	
	stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->ioMOSI);
	if(stp->enable)GPIO_PinModeSet(stp->port,stp->pin,stp->type,stp->out);		
}

void cls_Sx1276::sx1276_bal_spi_nss_hight(void)
{
	st_Sx1276PortPinDef* stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->ioNSS);
	if(stp->enable)	GPIO_PinOutSet(stp->port,stp->pin);	
}
void 	cls_Sx1276::sx1276_bal_spi_nss_low(void)
{
	st_Sx1276PortPinDef* stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->ioNSS);
	if(stp->enable)	GPIO_PinOutClear(stp->port,stp->pin);	
}
void cls_Sx1276::sx1276_bal_spi_sck_hight(void)
{
	st_Sx1276PortPinDef* stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->ioSCK);
	if(stp->enable)	GPIO_PinOutSet(stp->port,stp->pin);		
	//GPIO_PinOutSet(Sx1276PortIO->ioSCK.port,Sx1276PortIO->ioSCK.pin);
}
void cls_Sx1276::sx1276_bal_spi_sck_low(void)
{
	st_Sx1276PortPinDef* stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->ioSCK);
	if(stp->enable)	GPIO_PinOutClear(stp->port,stp->pin);		
	//GPIO_PinOutClear(Sx1276PortIO->ioSCK.port,Sx1276PortIO->ioSCK.pin);
}
void cls_Sx1276::sx1276_bal_spi_mosi_hight(void)
{
	st_Sx1276PortPinDef* stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->ioMOSI);
	if(stp->enable)	GPIO_PinOutSet(stp->port,stp->pin);		
	//GPIO_PinOutSet(Sx1276PortIO->ioMOSI.port,Sx1276PortIO->ioMOSI.pin);
}
void cls_Sx1276::sx1276_bal_spi_mosi_low(void)
{
	st_Sx1276PortPinDef* stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->ioMOSI);
	if(stp->enable)	GPIO_PinOutClear(stp->port,stp->pin);		
	//GPIO_PinOutClear(Sx1276PortIO->ioMOSI.port,Sx1276PortIO->ioMOSI.pin);
}
uint32_t cls_Sx1276::sx1276_bal_spi_get_miso(void)
{
	//st_Sx1276PortPinDef* stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->ioMISO);	
	//return GPIO_PinInGet(stp->port,stp->pin);
	return GPIO_PinInGet(Sx1276PortIO->ioMISO.port,Sx1276PortIO->ioMISO.pin);
}
uint32_t cls_Sx1276::sx1276_bal_get_dio0(void)
{
	uint32_t t32;
	st_Sx1276PortPinDef* stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->ioDIO0);
	if(stp->enable){
		 t32=GPIO_PinInGet(stp->port,stp->pin);
		return t32;
	}
	return 0;
}
uint32_t cls_Sx1276::sx1276_bal_get_dio1(void)
{
	st_Sx1276PortPinDef* stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->ioDIO1);
	if(stp->enable){
		return GPIO_PinInGet(stp->port,stp->pin);
	}
	return 0;
}
uint32_t cls_Sx1276::sx1276_bal_get_dio2(void)
{
	st_Sx1276PortPinDef* stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->ioDIO2);
	if(stp->enable){
		return GPIO_PinInGet(stp->port,stp->pin);
	}
	return 0;
}
uint32_t cls_Sx1276::sx1276_bal_get_dio3(void)
{
	st_Sx1276PortPinDef* stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->ioDIO3);
	if(stp->enable){
		return GPIO_PinInGet(stp->port,stp->pin);
	}
	return 0;
}
uint32_t cls_Sx1276::sx1276_bal_get_dio4(void)
{
	st_Sx1276PortPinDef* stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->ioDIO4);
	if(stp->enable){
		return GPIO_PinInGet(stp->port,stp->pin);
	}
	return 0;
}
uint32_t cls_Sx1276::sx1276_bal_get_dio5(void)
{	
	st_Sx1276PortPinDef* stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->ioDIO5);
	if(stp->enable){
		return GPIO_PinInGet(stp->port,stp->pin);
	}
	return 0;
}
void cls_Sx1276::sx1276_bal_set_dclk_output(void)
{
	st_Sx1276PortPinDef* stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->ioDIO1);
	if(stp->enable){
		GPIO_PinModeSet(stp->port,stp->pin,GPIO_MODE_OUTPUT_PP,1);
	}
}
void cls_Sx1276::sx1276_bal_set_data_output(void)
{
	st_Sx1276PortPinDef* stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->ioDIO2);
	if(stp->enable){
		GPIO_PinModeSet(stp->port,stp->pin,GPIO_MODE_OUTPUT_PP,1);
	}	
}
//
void cls_Sx1276::sx1276_bal_set_dio1_input(void)
{
	st_Sx1276PortPinDef* stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->ioDIO1);
	if(stp->enable){
		GPIO_PinModeSet(stp->port,stp->pin,stp->type,stp->out);
	}	
}
void cls_Sx1276::sx1276_bal_set_dio2_input(void)
{
	st_Sx1276PortPinDef* stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->ioDIO2);
	if(stp->enable){
		GPIO_PinModeSet(stp->port,stp->pin,stp->type,stp->out);
	}
}
//
void cls_Sx1276::sx1276_bal_set_dclk_hight(void)
{
	st_Sx1276PortPinDef* stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->ioDIO1);
	if(stp->enable){
		GPIO_PinOutSet(stp->port,stp->pin);	
	}		
}
void cls_Sx1276::sx1276_bal_set_dclk_low(void)
{
	st_Sx1276PortPinDef* stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->ioDIO1);
	if(stp->enable){
		GPIO_PinOutClear(stp->port,stp->pin);	
	}		
}
void cls_Sx1276::sx1276_bal_set_data_hight(void)
{
	st_Sx1276PortPinDef* stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->ioDIO2);
	if(stp->enable){
		GPIO_PinOutSet(stp->port,stp->pin);	
	}		
}
void cls_Sx1276::sx1276_bal_set_data_low(void)
{
	st_Sx1276PortPinDef* stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->ioDIO2);
	if(stp->enable){
		GPIO_PinOutClear(stp->port,stp->pin);	
	}		
}
void cls_Sx1276::sx1276_bal_set_data_toggle(void)
{
	st_Sx1276PortPinDef* stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->ioDIO2);
	if(stp->enable){
		GPIO_PinOutToggle(stp->port,stp->pin);	
	}		
}	
void cls_Sx1276::sx1276_bal_set_dclk_toggle(void)
{
	st_Sx1276PortPinDef* stp=(st_Sx1276PortPinDef*)(&Sx1276PortIO->ioDIO1);
	if(stp->enable){
		GPIO_PinOutToggle(stp->port,stp->pin);	
	}		
}
void cls_Sx1276::SX1276ReInitIo(void){
	
}
//file end
