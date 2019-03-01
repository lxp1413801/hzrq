/*
 * THE FOLLOWING FIRMWARE IS PROVIDED: (1) "AS IS" WITH NO WARRANTY; AND 
 * (2)TO ENABLE ACCESS TO CODING INFORMATION TO GUIDE AND FACILITATE CUSTOMER.
 * CONSEQUENTLY, SEMTECH SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR
 * CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 * OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 * CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 * 
 * Copyright (C) SEMTECH S.A.
 */
/*! 
 * \file       sx1276-Hal.h
 * \brief      SX1276 Hardware Abstraction Layer
 *
 * \version    2.0.B2 
 * \date       May 6 2013
 * \author     Gregory Cristian
 *
 * Last modified by Miguel Luis on Jun 19 2013
 */
#ifndef __SX1276_BAL_H__
#define __SX1276_BAL_H__

#include "./inc/stm32l0xx_hal.h"
#include <stdint.h>
#include <stdbool.h>
#ifdef __cplusplus
extern "C"{
#endif
	typedef struct{
		bool enable;
		GPIO_TypeDef* port;
		uint32_t pin;
		uint32_t type;
		uint32_t out;
	}st_Sx1276PortPinDef;	

	typedef struct{
		st_Sx1276PortPinDef ioRST;
		st_Sx1276PortPinDef	ioNSS;
		st_Sx1276PortPinDef ioSCK;
		st_Sx1276PortPinDef ioMOSI;
		st_Sx1276PortPinDef	ioMISO;
		st_Sx1276PortPinDef ioDIO0;
		st_Sx1276PortPinDef ioDIO1;
		st_Sx1276PortPinDef ioDIO2;
		st_Sx1276PortPinDef ioDIO3;
		st_Sx1276PortPinDef ioDIO4;
		st_Sx1276PortPinDef ioDIO5;
		//
		st_Sx1276PortPinDef swCtrlTX;
		st_Sx1276PortPinDef swCtrlRX;
		//
		st_Sx1276PortPinDef	pwrCtrlPA;
		st_Sx1276PortPinDef	pwrCtrlLNA;
		
	}st_Sx1276IOMap;
	//define for stm32l07xx hal
	//HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)
	
	#define GPIO_PinOutSet(GPIOx, GPIO_Pin) 	m_gpio_write(GPIOx, GPIO_Pin, GPIO_Pin)
	#define GPIO_PinOutClear(GPIOx, GPIO_Pin) 	m_gpio_write(GPIOx, GPIO_Pin, 0)
	#define GPIO_PinInGet(GPIOx, GPIO_Pin)		(GPIOx->IDR & GPIO_Pin)
	#define GPIO_PinOutToggle(GPIOx, GPIO_Pin)	(GPIOx->ODR ^= GPIO_Pin)
	extern const st_Sx1276IOMap Sx1276IoMapMaster;
	extern const st_Sx1276IOMap	Sx1276IoMapSecondary;
	#define GPIO_PinModeSet(port,pin,mode,out) gpio_pin_mode_set(port,pin,mode,out)
	
#ifdef __cplusplus
}
#endif
#endif

