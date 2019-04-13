#ifndef __M_UART_H__
#define __M_UART_H__


	#ifdef __cplusplus
	extern "C" {
	#endif

#ifndef UART_ECHO_EN
	#define UART_ECHO_EN 0
#endif

	#define USARTx                           LPUART1
	#define USARTx_CLK_ENABLE()					__HAL_RCC_LPUART1_CLK_ENABLE()
	#define USARTx_CLK_DISABLE()				__HAL_RCC_LPUART1_CLK_DISABLE()
		
	#define m_gpio_lpusart1_rcc_enable()		__HAL_RCC_GPIOC_CLK_ENABLE()
	#define m_gpio_lpusart1_rcc_disable()		__HAL_RCC_GPIOC_CLK_DISABLE()
	
	#define m_lpusart1_rcc_enable()  do{ \
		__HAL_RCC_LPUART1_CLK_ENABLE(); \
		__HAL_RCC_GPIOC_CLK_ENABLE();}while(0);

	#define m_lpusart1_rcc_disable()  do{ \
		__HAL_RCC_LPUART1_CLK_DISABLE(); \
		__HAL_RCC_GPIOC_CLK_DISABLE();}while(0);	
		
	#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()
	#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()
	

	// #define USARTx_FORCE_RESET()             __HAL_RCC_LPUSART1_FORCE_RESET()
	// #define USARTx_RELEASE_RESET()           __HAL_RCC_LPUSART1_RELEASE_RESET()
	#define USARTx_FORCE_RESET()             __HAL_RCC_LPUART1_FORCE_RESET()
	#define USARTx_RELEASE_RESET()           __HAL_RCC_LPUART1_RELEASE_RESET()	

	/* Definition for USARTx Pins */
	#define USARTx_TX_PIN						GPIO_PIN_4
	#define USARTx_TX_GPIO_PORT					GPIOC
	#define USARTx_TX_AF						GPIO_AF2_LPUART1
	#define USARTx_RX_PIN						GPIO_PIN_5
	#define USARTx_RX_GPIO_PORT					GPIOC
	#define USARTx_RX_AF						GPIO_AF2_LPUART1
	#define USARTx_PORT 						GPIOC
	#define USARTx_RXTX_PIN						(GPIO_PIN_4 | GPIO_PIN_5)
	/* Definition for USARTx's NVIC */
	#define USARTx_IRQn                      LPUART1_IRQn
	#define USARTx_IRQHandler                LPUART1_IRQHandler


	/* Size of Trasmission buffer */
	#define TXBUFFERSIZE                      (COUNTOF(aTxBuffer) - 1)
	/* Size of Reception buffer */
	#define RXBUFFERSIZE                      TXBUFFERSIZE
	  
	/* Exported macro ------------------------------------------------------------*/
	#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
	/* Exported functions ------------------------------------------------------- */

	#define MAX_UART_TX_BUFFER_LEN (512)
	#define MAX_UART_RX_BUFFER_LEN (256)
	#define LPUSART1_RECEIVED_TIME_OUT 200
	
	extern uint8_t uartSendBuf[MAX_UART_TX_BUFFER_LEN];
	extern uint8_t uartReceivedBuf[MAX_UART_RX_BUFFER_LEN];
	extern uint16_t uartRceivedBufCount;
	extern uint16_t uartSendBufCount;

	
	#define uartRceivedBufCountUser nbAplReceivedCount
	#define uartReceivedBufUser nbAplReceivedBuffer


	extern UART_HandleTypeDef UartHandle;
	extern uint32_t lpuart1RxTimeOut;
	//

	extern int16_t m_lpusart1_deinit(void);
	extern uint16_t m_lpusart1_init(uint32_t baudrate);
	extern void m_lpusart1_enable_wakeup(void);
	extern void m_lpusart1_disable_wakeup(void);

	extern void m_lpusart1_received_start(void);
	extern void m_lpusart1_received_echo(uint8_t chr);
	extern void u_lpusart1_send_str(uint8_t* str);
	
	//
	extern void m_lpusart1_send_buf_poll(uint8_t *pData, uint16_t Size);
	extern void m_lpusart1_send_byte_poll(uint8_t b);
	extern void m_lpusart1_send_str_poll(uint8_t* str);
	extern void m_lpusart1_send_len(uint8_t* buf,uint16_t len);
	
	//
	extern void m_lpusart1_send_str(uint8_t* str);
	//
	extern void m_lpusart1_idle_time_disable(void);
	extern void m_lpusart1_idle_time_reset(void);
	extern void m_lpusart1_idle_time_hook(void);
	
	//<<--
	#define USART_SE USART1
	#define USART_SE_CLK_ENABLE()					__HAL_RCC_USART1_CLK_ENABLE()
	#define USART_SE_CLK_DISABLE()					__HAL_RCC_USART1_CLK_DISABLE()	
	
	#define usart1_se_clk_enable() __HAL_RCC_USART1_CLK_ENABLE()
	#define usart1_se_clk_disable() __HAL_RCC_USART1_CLK_DISABLE()
	
	#define m_gpio_usart1_rcc_enable()		__HAL_RCC_GPIOB_CLK_ENABLE()
	#define m_gpio_usart1_rcc_disable()		__HAL_RCC_GPIOB_CLK_DISABLE()
	
	#define m_usart1_se_rcc_enable()  do{usart1_se_clk_enable();m_gpio_usart1_rcc_enable();}while(0);
	#define m_usart1_se_rcc_disenable()  do{usart1_se_clk_disable();m_gpio_usart1_rcc_disable();}while(0);
	
	#define USART1_SE_FORCE_RESET()             __HAL_RCC_USART1_FORCE_RESET()
	#define USART1_SE_RELEASE_RESET()           __HAL_RCC_USART1_RELEASE_RESET()	
	
	#define USART_SE_TX_PIN						GPIO_PIN_6
	#define USART_SE_TX_GPIO_PORT				GPIOB
	#define USART_SE_TX_AF						GPIO_AF0_USART1
	#define USART_SE_RX_PIN						GPIO_PIN_7
	#define USART_SE_RX_GPIO_PORT				GPIOB
	#define USART_SE_RX_AF						GPIO_AF0_USART1
	#define USART_SE_PORT 						GPIOB
	#define USART_SE_RXTX_PIN					(USART_SE_TX_PIN | USART_SE_RX_PIN)	

	#define USART1_SE_IRQn                      USART1_IRQn
	#define USART1_SE_IRQHandler               USART1_IRQHandler
	
	extern UART_HandleTypeDef UartHandleSe;
	#define USART_SE_RECEIVED_TIME_OUT 100
	
	#define MAX_UART_SE_RX_BUFFER_LEN (256)
	extern uint8_t __uartSeReceivedBuf[MAX_UART_SE_RX_BUFFER_LEN];
	extern uint16_t __uartSeRceivedBufCount;
	
	extern uint32_t __usartSeRxTimeOut;
	
	extern uint16_t m_usart1_se_init(uint32_t baudrate);	
	extern uint16_t m_usart1_se_deinit(void);
	
	
	extern void m_usart1_se_idle_time_hook(void);
	extern void m_usart1_se_idle_time_reset(void);
	
	extern void m_usart1_se_send_str(uint8_t* str);
	extern void m_usart1_se_send_len(uint8_t* buf,uint16_t len);
	
	//-->>
	#ifdef __cplusplus
	}
	#endif

#endif
//file end
