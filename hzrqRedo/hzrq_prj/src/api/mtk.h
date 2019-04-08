#ifndef __mtk_h__
#define __mtk_h__
	#include <stdint.h>
	#ifdef __cplusplus
		extern "C"{
	#endif
	typedef enum{
		MTK_POWER_OFF,
		MTK_POWER_ON,
		MTK_REG_NET,
		MTK_REG_IOT,
		MTK_READY,
		MTK_PSM,
	}__mtkStateMachine_t;
	extern __mtkStateMachine_t mtkStateMachine;
	extern uint8_t 	CIMI[16];
	extern uint8_t	CGSN[16];
	extern uint8_t	ICCID[21];
	extern uint8_t 	SPVER[8];
	extern uint8_t 	EARFCN[8];	
	

	
	/*
	extern TaskHandle_t idTheadUdpId;
	#define flg_NB_MODULE_UART_RECEIVED_LF 		(0x01ul<<0x00)
	#define flg_NB_MODULE_COMM_PROCESS_REQ		(0x01ul<<0x01)
	#define flg_NB_MODULE_UART_RECEIVED_ERROR	(0x01ul<<0x02)
	#define flg_NB_MODULE_RF_CLOSE				(0x01ul<<0x03)
	#define flg_NB_MODULE_ALL_BITS (flg_NB_MODULE_UART_RECEIVED_LF | flg_NB_MODULE_UART_RECEIVED_ERROR)
	

	extern osMessageQId nbSendMsgQ;
	extern void m_thread_create_nb_mtk(void);
	*/
	//extern 

	
	
	extern int16_t l620_hal_power_off(void);
	extern int16_t l620_cfun_off(void);
	extern int16_t l620_send_ready(void);
	extern int16_t l620_send_process(uint8_t* sbuf,uint16_t slen);
	extern int16_t l620_received(uint8_t* rbuf,uint16_t rsize);
	extern int16_t l620_received_extract(uint8_t* rbuf,uint16_t len);
	extern void mtk_disable_sleep(void);
	
	#ifdef __cplusplus
		}
	#endif
#endif
//file end
