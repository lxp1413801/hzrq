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

	/*
	typedef struct{
		//uint8_t* sendBuf;
		//uint8_t* receivedBuf;
		//uint16_t sendCount;
		uint16_t receivedCount;
		nbModuleStatus_t nbStatus;
		em_nbiotErrorCode	errCode;
	}st_nbiotDescInf,nbiotDescInf_t;
	*/
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
	
	
	extern uint16_t nbAplReceivedCount;
	
	extern volatile int16_t baudRateOffset;
	extern void nb_module_pwr_off(void);
	
	extern void nb_module_hal_init(void);
	extern void nb_module_hal_deinit(void);
	extern int16_t nb_module_cfun_off(void);
	extern int16_t nb_coap_psm_send_ready(void);
	extern int16_t nb_udp_cereg_coap(void);
	extern int16_t nb_module_receive_buffer_clear(void);
	extern int16_t nb_send_process(uint8_t* buf,uint16_t len);
	extern int16_t nb_received_process(void);
	extern int16_t nb_fota_process(uint8_t* rbuf,uint16_t rlen);
	extern int16_t np_received_extract_process(uint8_t* str,uint8_t* bin,uint16_t len);
	extern int16_t nb_coap_psm_release_ra(void);
	extern int16_t nb_coap_enter_psm(void);
	//
	
	
	#ifdef __cplusplus
	}
	#endif


#endif
//file end
