#include "./includes/includes.h"
TIM_HandleTypeDef    TimHandle={0};

//#define IR_BUFFER_LEN 8
uint8_t irRecevedBuf[IR_BUFFER_LEN];
uint16_t irRecevedByte=0x00;
uint16_t irSampleTimes=0x00;
uint8_t irdata=0x00;

extern uint32_t irTimeOut=0x00ul;

void __x_delay_30us(void)
{
	uint32_t i;
	for(i=0;i<60;i++){
		__nop();
		__nop();
	}
}
void ir_data_init(void)
{
	irRecevedByte=0;
	irSampleTimes=0;
}

void ir_received_ready(void)
{
	irTimeOut=0;
	irRecevedByte=0;
	irSampleTimes=0;
	//m_gpio_ir_pwr_config();
	//m_gpio_ir_pwr_on();
	m_gpio_ir_rx_pwr_on();
	osDelay(10);
	m_gpio_config_ir_rx_config();
	m_gpio_ir_rx_irq_enanle();
}

void ir_disable(void)
{
	m_timer_deinit();
	m_gpio_ir_rx_irq_disanle();
	m_gpio_ir_rx_pwr_off();
}

void m_timer_error_handler(void)
{
	__nop();
	mcu_error_reset();
	
}

void m_timer_init(void)
{
	TimHandle.Instance =TIMx;
	TimHandle.Init.Period            = 1;//10000 - 1;
	TimHandle.Init.Prescaler         = (HAL_RCC_GetHCLKFreq() / 1200ul) - 1;
	TimHandle.Init.ClockDivision     = 0;
	TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;	
	
	if(HAL_TIM_Base_Init(&TimHandle) != HAL_OK) {
		m_timer_error_handler();
	}	
	if (HAL_TIM_Base_Start_IT(&TimHandle) != HAL_OK){
		mcu_error_reset();
	}	
}
void m_timer_deinit(void)
{
	HAL_TIM_Base_Stop_IT(&TimHandle);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
  /*##-1- Enable peripheral clock #################################*/
  /* TIMx Peripheral clock enable */
  TIMx_CLK_ENABLE();
  
  /*##-2- Configure the NVIC for TIMx ########################################*/
  /* Set the TIMx priority */
  HAL_NVIC_SetPriority(TIMx_IRQn, 3, 0);

  /* Enable the TIMx global Interrupt */
  HAL_NVIC_EnableIRQ(TIMx_IRQn);
}
 
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	// m_gpio_change(IR_PWR_PORT,IR_PWR_PIN);
	// __nop();
	uint32_t t32=0;
	irTimeOut=1;
	irSampleTimes++;
	if(irSampleTimes==1){

		if(m_gpio_read(IR_RX_PORT,IR_RX_PIN)){
			//error
		}else{
			irdata=0x00;
		}
	}else if(irSampleTimes<19){
		
		if(irSampleTimes & 0x01){
			irdata>>=1;
			t32=m_gpio_read(IR_RX_PORT,IR_RX_PIN);
			//过采样
			__x_delay_30us();
			t32 |= m_gpio_read(IR_RX_PORT,IR_RX_PIN);
			__x_delay_30us();
			t32 |= m_gpio_read(IR_RX_PORT,IR_RX_PIN);
			__x_delay_30us();
			t32 |= m_gpio_read(IR_RX_PORT,IR_RX_PIN);			
			if(t32)irdata |= 0x80;
		}
	}else if(irSampleTimes==20){
		//irTimeOut=0;
		m_timer_deinit();
		m_gpio_ir_rx_irq_enanle();
		t32=m_gpio_read(IR_RX_PORT,IR_RX_PIN);
		//过采样
		__x_delay_30us();
		t32 |= m_gpio_read(IR_RX_PORT,IR_RX_PIN);
		__x_delay_30us();
		t32 |= m_gpio_read(IR_RX_PORT,IR_RX_PIN);
		__x_delay_30us();
		t32 |= m_gpio_read(IR_RX_PORT,IR_RX_PIN);		
		if(t32){
			if(irRecevedByte<IR_BUFFER_LEN){
				irRecevedBuf[irRecevedByte]=irdata;
			}
			irRecevedByte++;
			irSampleTimes=0;
		}else{
			//error
			__nop();
		}
		irdata=0x00;
		__nop();
	}

}

void ir_idle_time_hook(void)
{
	if(irTimeOut>0){
		irTimeOut++;
	}
	if(irTimeOut>200){

		ir_disable();
		irTimeOut=0;
		osSignalSet(vTheadEvenID,flg_EVENT_IR_RECEIVED);
		__nop();
	}
}
//file end
