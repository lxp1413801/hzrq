
#include "./includes/includes.h"

uint32_t uartReceivedIdleTime=0x00ul;

uint8_t uartSendBuf[MAX_UART_TX_BUFFER_LEN];
uint16_t uartSendBufCount=0;


uint8_t uartReceivedBuf[MAX_UART_RX_BUFFER_LEN];
uint16_t uartRceivedBufCount=0;

extern uint16_t nbAplReceivedCount;

#define uartRceivedBufCountUser nbAplReceivedCount
#define uartReceivedBufUser nbAplReceivedBuffer

UART_HandleTypeDef UartHandle={0};

UART_WakeUpTypeDef WakeUpSelection; 
bool uartInited=false;

static void usart_error_handler(void)
{
	mcu_error_reset();
	while(1){
		__nop();
	}
}
void m_gpio_config_lpusart1(void)
{
	GPIO_InitTypeDef GPIO_InitStruct={0};
	m_gpio_lpusart1_rcc_enable();
	GPIO_InitStruct.Pin       = USARTx_RXTX_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF2_LPUART1;	
	HAL_GPIO_Init(USARTx_PORT, &GPIO_InitStruct);
}

void m_gpio_config_lpusart1_reset(void)
{
	GPIO_InitTypeDef GPIO_InitStruct={0};
	m_gpio_lpusart1_rcc_enable();
	GPIO_InitStruct.Pin       = USARTx_RXTX_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull      = GPIO_NOPULL;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(USARTx_PORT, &GPIO_InitStruct);
}


uint16_t m_lpusart1_init(uint32_t baudrate)
{
	//m_clock_config_enable_lse();
	//if(uartInited)return 0;
	baudrate=baudrate;
#if cfg_SYSTEM_CLOCK_HSI
	__HAL_RCC_USARTx_CONFIG(RCC_USARTxCLKSOURCE_HSI);
#endif
	m_gpio_lpusart1_rcc_enable();
	UartHandle.Instance        = USARTx;
	HAL_UART_DeInit(&UartHandle); 

	//UartHandle.Init.BaudRate   = 9600;
	UartHandle.Init.BaudRate  =baudrate;
	UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
	UartHandle.Init.StopBits   = UART_STOPBITS_1;
	UartHandle.Init.Parity     = UART_PARITY_NONE;
	UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	UartHandle.Init.Mode       = UART_MODE_TX_RX;
	//UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if(HAL_UART_Init(&UartHandle) != HAL_OK)usart_error_handler();
	osDelay(10);
	m_lpusart1_received_start();		

	uartInited=true;
	return 1;
}
void m_lpusart1_disable_wakeup(void)
{
	HAL_UARTEx_DisableStopMode(&UartHandle);
}

void m_lpusart1_enable_wakeup(void)
{
	#if cfg_SYSTEM_CLOCK_HSI	==0
	//m_gpio_config_USARTx_input();
	#endif
}

int16_t m_lpusart1_deinit(void)
{
	USARTx_FORCE_RESET();
	USARTx_RELEASE_RESET();

	HAL_GPIO_DeInit(USARTx_PORT, USARTx_RXTX_PIN);
	HAL_UART_DeInit(&UartHandle);
	return 1;
}
/*
#define __myReg_lpusart1_ICR (*( ( volatile uint32_t *)(LPUART1_BASE+0x0020)))
#define __myReg_lpusart1_ISR (*( ( volatile uint32_t *)(LPUART1_BASE+0x001C)))
#define __myReg_lpusart1_TDR (*( ( volatile uint32_t *)(LPUART1_BASE+0x0028)))	
#define __myReg_lpusart1_RDR (*( ( volatile uint32_t *)(LPUART1_BASE+0x0024)))	
*/

void m_lpusart1_send_buf_poll(uint8_t *pData, uint16_t Size){
	uint16_t i;
	for(i=0;i<Size;i++){
		while(!(USARTx->ISR & USART_ISR_TXE));
		USARTx->TDR=*pData++;
		osDelay(1);
	}
}

void m_lpusart1_send_len(uint8_t* buf,uint16_t len)
{
	m_lpusart1_send_buf_poll(buf,len);
}

void m_lpusart1_send_byte_poll(uint8_t b)
{
	while(!(USARTx->ISR & USART_ISR_TXE));
	USARTx->TDR=b;
	osDelay(1);
}

void m_lpusart1_send_str_poll(uint8_t* str){
	while(*str!='\0'){
		while(!(USARTx->ISR & USART_ISR_TXE));
		USARTx->TDR=*str++;
		osDelay(1);
	}
}

void m_lpusart1_send_str(uint8_t* str)
{
	while(*str!='\0'){
		while(!(USARTx->ISR & USART_ISR_TXE));
		USARTx->TDR=*str++;
		osDelay(4);
	}	
}

void m_lpusart1_received_echo(uint8_t chr)
{

	m_lpusart1_send_byte_poll(chr);
	if(chr=='\r'){
		m_lpusart1_send_byte_poll('\n');
	}

}

void m_lpusart1_received_msg_post(void)
{
	if(uartRceivedBufCount){
		m_mem_cpy_len(uartReceivedBufUser,uartReceivedBuf,uartRceivedBufCount);
		uartRceivedBufCountUser=uartRceivedBufCount;
		uartRceivedBufCount=0;
	}else{
		return;
	}
	if((void*)idTheadUdpId){
		//m_uart_received_msg_post();
		osSignalSet( idTheadUdpId, flg_NB_MODULE_UART_RECEIVED_LF);
	}
	
}

HAL_StatusTypeDef m_lpusart1_received_irq_callback(void)
{
	uint32_t isr=USARTx->ISR;
	uint8_t chr;
	if(isr & (0x01ul<<5))
	{
		m_lpusart1_idle_time_reset();
		chr=(uint8_t)(USARTx->RDR );
		//m_lpusart1_received_echo(chr);
		if(uartRceivedBufCount<MAX_UART_RX_BUFFER_LEN){
			uartReceivedBuf[uartRceivedBufCount]=chr;
			uartRceivedBufCount++;
		}
		if(chr==0x0a){

		}
		return HAL_OK;
	}else{
		return HAL_BUSY; 
	}	
}


void m_lpusart1_received_err_callback(void)
{
	uint32_t i;
	uint32_t t32;
	
	t32=USARTx->CR1;
	t32 &= ~0x01ul;
	t32=USARTx->CR1=t32;
	i=1000;
	while(i)i--;
	t32 |= 0x01ul;
	t32=USARTx->CR1=t32;
	
	if((void*)idTheadUdpId){
		//m_uart_received_msg_post();
		osSignalSet( idTheadUdpId, flg_NB_MODULE_UART_RECEIVED_ERROR);
	}
	__nop();	
}



// void HAL_UARTEx_WakeupCallback(UART_HandleTypeDef *huart)
// {
	// __nop();
	// __HAL_UART_ENABLE_IT(&UartHandle, UART_IT_RXNE);
// }



void m_lpusart1_received_start(void){
	
	HAL_UART_Receive_IT(&UartHandle, (uint8_t*)uartReceivedBuf, sizeof(uartReceivedBuf));
	uartRceivedBufCount=0;
	
}



void m_lpusart1_idle_time_disable(void)
{
	uartReceivedIdleTime=0x00ul;
}

void m_lpusart1_idle_time_reset(void)
{
	uartReceivedIdleTime=(1000/configTICK_RATE_HZ);
}



void m_lpusart1_idle_time_hook(void)
{
	if(uartReceivedIdleTime>0){
		uartReceivedIdleTime+=(1000/configTICK_RATE_HZ);
		if(uartReceivedIdleTime>LPUSART1_RECEIVED_TIME_OUT){
			//uartReceivedIdleTime=(1000/configTICK_RATE_HZ);
			uartReceivedIdleTime=0;
			m_lpusart1_received_msg_post();
			
		}
	}
}



UART_HandleTypeDef UartHandleSe={0};
uint32_t __usartSeRxTimeOut;	
uint8_t __uartSeReceivedBuf[MAX_UART_SE_RX_BUFFER_LEN];
uint16_t __uartSeRceivedBufCount=0;

void m_gpio_config_usart1_se(void)
{
	GPIO_InitTypeDef GPIO_InitStruct={0};
	m_gpio_usart1_rcc_enable();
	GPIO_InitStruct.Pin       = USART_SE_RXTX_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF0_USART1;	
	HAL_GPIO_Init(USART_SE_PORT, &GPIO_InitStruct);
}

void m_gpio_config_usart1_se_reset(void)
{
	GPIO_InitTypeDef GPIO_InitStruct={0};
	m_gpio_usart1_rcc_enable();
	GPIO_InitStruct.Pin       = USART_SE_RXTX_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull      = GPIO_NOPULL;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(USART_SE_PORT, &GPIO_InitStruct);
}


void m_usart1_se_received_start(void){
	
	HAL_UART_Receive_IT(&UartHandleSe, (uint8_t*)__uartSeReceivedBuf, sizeof(__uartSeReceivedBuf));
	uartRceivedBufCount=0;
	
}
//-------------------------------------------------------------------------------------------------------
uint16_t m_usart1_se_init(uint32_t baudRate)
{
	uint32_t t32;

	m_usart1_se_rcc_enable();
	UartHandleSe.Instance= USART_SE;
	HAL_UART_DeInit(&UartHandleSe); 
	
	//t32=baudrate_calc(baudRate);
	//UartHandle.Init.BaudRate   = t32;
	UartHandleSe.Init.BaudRate=baudRate;
	UartHandleSe.Init.WordLength = UART_WORDLENGTH_8B;
	UartHandleSe.Init.StopBits   = UART_STOPBITS_1;
	UartHandleSe.Init.Parity     = UART_PARITY_NONE;
	UartHandleSe.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	UartHandleSe.Init.Mode       = UART_MODE_TX_RX;
	//UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if(HAL_UART_Init(&UartHandleSe) != HAL_OK)usart_error_handler();
	osDelay(10);
	m_usart1_se_received_start();		


	return 1;
}

uint16_t m_usart1_se_deinit(void)
{
	m_usart1_se_rcc_enable();
	USARTx_FORCE_RESET();
	USARTx_RELEASE_RESET();
	HAL_GPIO_DeInit(USARTx_PORT, USARTx_RXTX_PIN);
	HAL_UART_DeInit(&UartHandle);
	return 1;
}

void m_usart1_se_send_buf_poll(uint8_t *pData, uint16_t Size){
	uint16_t i;
	//if(UartHandleSe.Instance != USART_SE)return;
	for(i=0;i<Size;i++){
		//while(!(__myReg_lpusart1_ISR & USART_ISR_TXE));
		while(!(USART_SE->ISR & USART_ISR_TXE));
		//__myReg_lpusart1_TDR=*pData++;
		USART_SE->TDR=*pData++;
		//osDelay(1);
		__nop();
		__nop();
	}
}

void m_usart1_se_send_len(uint8_t* buf,uint16_t len)
{
	m_usart1_se_send_buf_poll(buf,len);
}

void m_usart1_se_send_byte_poll(uint8_t b)
{
	//if(UartHandleSe.Instance != USART_SE)return;
	while(!(USART_SE->ISR & USART_ISR_TXE));
	USART_SE->TDR=b;
	//osDelay(1);
	__nop();
	__nop();	
}


void m_usart1_se_send_str_poll(uint8_t* str)
{
	//if(UartHandleSe.Instance != USART_SE)return;
	while(*str!='\0'){
		while(!(USART_SE->ISR & USART_ISR_TXE));
		USART_SE->TDR=*str++;
		//osDelay(1);
		__nop();
		__nop();		
	}
}

void m_usart1_se_send_str(uint8_t* str)
{
	//if(UartHandleSe.Instance != USART_SE)return;
	while(*str!='\0'){
		while(!(USART_SE->ISR & USART_ISR_TXE));
		USART_SE->TDR=*str++;
		//osDelay(4);
		__nop();
		__nop();		
	}	
}

void m_usart1_se_received_msg_post(void)
{
	if(__uartSeRceivedBufCount){
		//m_mem_cpy_len(uartReceivedBufUser,uartReceivedBuf,uartRceivedBufCount);
		//uartRceivedBufCountUser=uartRceivedBufCount;
		//__uartSeRceivedBufCount=0;
	}else{
		return;
	}
	
	// if((void*)idTheadUdpId){
		// osSignalSet( idTheadUdpId, flg_NB_MODULE_UART_RECEIVED_LF);
	// }
	if(__uartSeRceivedBufCount>0 && (void*)vTheadMainID){
		osSignalSet( vTheadMainID, flg_MAIN_THREAD_IR_RECEIVED);
	}
}

void m_usart1_se_received_echo(uint8_t chr)
{

	m_usart1_se_send_byte_poll(chr);
	if(chr=='\r'){
		m_usart1_se_send_byte_poll('\n');
	}

}

HAL_StatusTypeDef m_usart_se_received_irq_callback(void)
{
	uint32_t isr;
	isr=USART_SE->ISR;
	uint8_t chr;
	if(isr & (0x01ul<<5))
	{
		noEventTimeOut=60;
		m_usart1_se_idle_time_reset();			
		chr=(uint8_t)(USART_SE->RDR );
		m_usart1_se_received_echo(chr);
		if(__uartSeRceivedBufCount<MAX_UART_SE_RX_BUFFER_LEN){
			__uartSeReceivedBuf[__uartSeRceivedBufCount]=chr;
			__uartSeRceivedBufCount++;
		}
		if(chr=='\r'){
			m_usart1_se_received_msg_post();
		}
		return HAL_OK;
	}else{
		return HAL_BUSY; 
	}	
}





void m_usart1_se_received_err_callback(void)
{
	uint32_t i;
	uint32_t t32;
	
	t32=USART_SE->CR1;
	t32 &= ~0x01ul;
	t32=USART_SE->CR1=t32;
	i=1000;
	while(i)i--;
	t32 |= 0x01ul;
	t32=USART_SE->CR1=t32;	
	if((void*)vTheadMainID){
		osSignalSet( vTheadMainID, flg_MAIN_THREAD_IR_ERR);
	}
	__nop();		
}


void m_usart1_se_idle_time_hook(void)
{
	if(__usartSeRxTimeOut>0){
		__usartSeRxTimeOut+=(1000/configTICK_RATE_HZ);
		if(__usartSeRxTimeOut>USART_SE_RECEIVED_TIME_OUT){
			__usartSeRxTimeOut=0;
			m_usart1_se_received_msg_post();
			
		}
	}
}

void m_usart1_se_idle_time_reset(void)
{
	__usartSeRxTimeOut=(1000/configTICK_RATE_HZ);
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{  
	if(huart->Instance==USARTx){
		m_gpio_config_lpusart1();
		m_lpusart1_rcc_enable();
		HAL_NVIC_SetPriority(USARTx_IRQn, 3, 1);
		HAL_NVIC_EnableIRQ(USARTx_IRQn);
	}else if(huart->Instance==USART_SE){
		m_gpio_config_usart1_se();
		m_usart1_se_rcc_enable();
		HAL_NVIC_SetPriority(USART1_SE_IRQn, 3, 1);
		HAL_NVIC_EnableIRQ(USART1_SE_IRQn);		
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	while(HAL_IS_BIT_SET(huart->Instance->ISR, UART_FLAG_RXNE))
	{
	}
	__HAL_UART_DISABLE_IT(huart, UART_IT_RXNE);

	/* Check if a transmit Process is ongoing or not */
	if(huart->State == HAL_UART_STATE_BUSY_TX_RX) 
	{
		huart->State = HAL_UART_STATE_BUSY_TX;
	}
	else
	{
		/* Disable the UART Parity Error Interrupt */
		__HAL_UART_DISABLE_IT(huart, UART_IT_PE);

		/* Disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
		__HAL_UART_DISABLE_IT(huart, UART_IT_ERR);

		huart->State = HAL_UART_STATE_READY;
	}
	//osSignalSet( vTheadGprsID, U_EVEN_FLG_RX);
	if(huart->Instance==USARTx){
		m_lpusart1_received_start();
	}else{
		m_usart1_se_received_start();
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	__nop();
}

void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USARTx){
		m_lpusart1_rcc_enable();
		USARTx_FORCE_RESET();
		USARTx_RELEASE_RESET();
		HAL_GPIO_DeInit(USARTx_PORT, USARTx_RXTX_PIN);
		HAL_NVIC_DisableIRQ(USARTx_IRQn);
	}else{
		m_usart1_se_rcc_enable();
		USART1_SE_FORCE_RESET();
		USART1_SE_RELEASE_RESET();
		HAL_GPIO_DeInit(USART_SE_PORT, USART_SE_RXTX_PIN);
		HAL_NVIC_DisableIRQ(USART1_SE_IRQn);		
	}
}




HAL_StatusTypeDef UART_Receive_IT(UART_HandleTypeDef *huart)
{
	HAL_StatusTypeDef sta;
	if(huart->Instance==USARTx){
		sta = m_lpusart1_received_irq_callback();
	}else if(huart->Instance==USART_SE){
		sta = m_usart_se_received_irq_callback();	
	}
	return sta;
}





void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{

	if(huart->Instance==USARTx){
		m_lpusart1_received_err_callback();
	}else if(huart->Instance==USART_SE){
		m_usart1_se_received_err_callback();	
	}
	

}

HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
  if((huart->State == HAL_UART_STATE_READY) || (huart->State == HAL_UART_STATE_BUSY_TX))
  {
    if((pData == NULL ) || (Size == 0)) 
    {
      return HAL_ERROR;
    }

    /* Process Locked */
    __HAL_LOCK(huart);

    //huart->pRxBuffPtr = pData;
    //huart->RxXferSize = Size;
    //huart->RxXferCount = Size;

    /* Computation of UART mask to apply to RDR register */
    UART_MASK_COMPUTATION(huart);

    huart->ErrorCode = HAL_UART_ERROR_NONE;
    /* Check if a transmit process is ongoing or not */
    if(huart->State == HAL_UART_STATE_BUSY_TX) 
    {
      huart->State = HAL_UART_STATE_BUSY_TX_RX;
    }
    else
    {
      huart->State = HAL_UART_STATE_BUSY_RX;
    }

    /* Enable the UART Parity Error Interrupt */
    __HAL_UART_ENABLE_IT(huart, UART_IT_PE);

    /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
    __HAL_UART_ENABLE_IT(huart, UART_IT_ERR);

    /* Process Unlocked */
    __HAL_UNLOCK(huart);

    /* Enable the UART Data Register not empty Interrupt */
    __HAL_UART_ENABLE_IT(huart, UART_IT_RXNE);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY; 
  }
}
//file end
