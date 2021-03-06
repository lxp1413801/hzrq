#ifndef __m_nb_module_h__
#define __m_nb_module_h__
	#include <stdint.h>
	#include <stdbool.h>
	#ifdef __cplusplus
	extern "C"{
	#endif
	
	#define NB_PWR_PINS 		GPIO_PIN_12
	#define NB_PWR_PORT			GPIOC
	
	#define NB_RST_PINS 		GPIO_PIN_15
	#define NB_RST_PORT			GPIOA
	
	//void nb_module_power_on();
	//void nb_module_power_off();
	extern TaskHandle_t idTheadUdpId;
	//define event
	#define flg_NB_MODULE_UART_RECEIVED_LF 		(0x01ul<<0x00)
	#define flg_NB_MODULE_UART_RECEIVED_ERROR	(0x01ul<<0x01)
	
	
	#define flg_NB_PROCESS_MODULE_RF_CLOSE				(0x01ul<<0x02)
	//#define flg_NB_PROCESS_LOAD_BUFFER					(0x01ul<<0x03)
	#define flg_NB_PROCESS_SEND_REAL					(0x01ul<<0x04)
	#define flg_NB_PROCESS_SEND_OLD						(0x01ul<<0x05)
	
	#define flg_NB_MODULE_ALL_BITS (flg_NB_MODULE_UART_RECEIVED_LF | flg_NB_MODULE_UART_RECEIVED_ERROR)

	typedef enum
	{
		NO_ERROR=0x00,
		AT_CMD_RESP_TIME_OUT,
		AT_CMD_NRB_ERROR,
		AT_CMD_AT_ERROR,
		AT_CMD_CFUN_ERROR,
		AT_CMD_CMEE_ERROR,
		AT_CMD_CMI_ERROR,
		AT_CMD_CGSN_ERROR,
		AT_CMD_NBAND_ERROR,
		AT_CMD_CGDCONT_ERROR,
		AT_CMD_CGATT_ERROR,
		AT_CMD_CSQ_ERROR,
		AT_CMD_CEREG_ERROR,
		AT_CMD_NSOCR_ERROR,
		AT_CMD_NSOCL_ERROR,
		AT_CMD_NSOCT_ERROR,
		AT_CMD_NSORF_ERROR,
		AT_CMD_CGMR_ERROR,
		AT_CMD_OTHER_ERROR,
	}em_nbiotErrorCode;
	typedef enum 
	{
		NB_POWER_OFF=0x00,
		NB_POWER_ON,
		NB_CGATTED,
		NB_CONNECTED,
		NB_PSM,
		NB_ERROR,
	}em_nbModuleStatus,nbModuleStatus_t;
	//for psm
	typedef enum{
		NB_PSM_SM_NRZ,
		NB_PSM_SM_AT,
		NB_PSM_SM_ATI,
		NB_PSM_SM_CMEE,
		NB_PSM_SM_CGMR,
		NB_PSM_SM_CFUN,
		NB_PSM_SM_CFUNR,

		NB_PSM_SM_CGCCONT,
		NB_PSM_SM_CGCCONT_R,
		NB_PSM_SM_NCDP,
		NB_PSM_SM_NCDPR,
		NB_PSM_SM_CGATT,
		NB_PSM_SM_CGATTR,
		NB_PSM_SM_CGATTR_SLOW,
		NB_PSM_SM_CGATTR_FAST,
		
		NB_PSM_SM_CIMI,
		NB_PSM_SM_NCCID,
		NB_PSM_SM_CGSN,	
		NB_PSM_SM_CSQ,
		NB_PSM_SM_NUESTATS,
		NB_PSM_SM_QSECSWT,
		NB_PSM_SM_QSECSWT_R,
		
		NB_PSM_SM_SEND,
		NB_PSM_SM_RECEIVE,
		NB_PSM_SM_PSM,
		NB_PSM_SM_NO_NET,
		NB_PSM_SM_NO_RECEIVED,
	}em_nbPsmStateMachine,nbPsmStateMachine_t;
	extern nbPsmStateMachine_t nbPsmStateMachine;
	
	typedef struct{
		//uint8_t* sendBuf;
		//uint8_t* receivedBuf;
		//uint16_t sendCount;
		uint16_t receivedCount;
		nbModuleStatus_t nbStatus;
		em_nbiotErrorCode	errCode;
	}st_nbiotDescInf,nbiotDescInf_t;
	
	typedef enum{
		BC95B5UNDEF,
		BC95B5HB,
		BC95B5HC,
		BC95B5JB,
	}em_bc95MasVer_t;
	extern em_bc95MasVer_t bc95MainVer;
	typedef enum{
		SP0,SP1,SP2,SP3,SP4,SP5,SP6,SP7,
		JBSP1,
	
	}em_bc95Ver,bc95Ver_t;
	extern bc95Ver_t bc95SubVer;

	extern uint8_t CIMI[16];
	extern uint8_t	CGSN[16];
	extern uint8_t	ICCID[21];
	extern uint8_t SPVER[8];
	extern uint8_t EARFCN[8];
	extern uint8_t SNR[8];
	
	extern uint8_t nbAplReceivedBuffer[];
	extern uint16_t nbAplReceivedCount;
	
	//extern nbiotDescInf_t nbiotDescInf;
	//extern nbModuleStatus_t nbModuleStatus;
	extern void nb_module_pwr_off(void);
	
	extern void nb_module_hal_init(void);
	extern void nb_module_hal_deinit(void);
	//
	extern void m_thread_create_nb_udp(void);
	
	#ifdef __cplusplus
	}
	#endif


#endif
//file end
