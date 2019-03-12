#ifndef __mtk_h__
#define __mtk_h__
	#include <stdint.h>
	#ifdef __cplusplus
		extern "C"{
	#endif
	typedef enum{
		MTK_POWER_OFF,
		MTK_POWER_ON,
		MTK_REG_CSQ,
		MTK_REG_CGATT,
		MTK_REG_CEREG,
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
	
	extern uint8_t nbAplReceivedBuffer[MAX_UART_RX_BUFFER_LEN];
	extern uint16_t nbAplReceivedCount;	
	
	
	extern TaskHandle_t idTheadUdpId;
	#define flg_NB_MODULE_UART_RECEIVED_LF 		(0x01ul<<0x00)
	#define flg_NB_MODULE_UART_RECEIVED_ERROR	(0x01ul<<0x01)
	
	
	#define flg_NB_PROCESS_MODULE_RF_CLOSE				(0x01ul<<0x02)
	//#define flg_NB_PROCESS_LOAD_BUFFER					(0x01ul<<0x03)
	#define flg_NB_PROCESS_SEND_REAL					(0x01ul<<0x04)
	#define flg_NB_PROCESS_SEND_OLD						(0x01ul<<0x05)
	
	#define flg_NB_MODULE_ALL_BITS (flg_NB_MODULE_UART_RECEIVED_LF | flg_NB_MODULE_UART_RECEIVED_ERROR)
	
	//osMessageQDef(nbSendMsgQ, 5);
	extern osMessageQId nbSendMsgQ;
	extern void m_thread_create_nb_mtk(void);
	//extern 

	#ifdef __cplusplus
		}
	#endif
#endif
//file end
