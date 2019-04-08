#include "../../includes/includes.h"
/*
#define __myReg_RCC_CR 			(*(( volatile uint32_t*)(0x40021000+0x0000)))
#define __myReg_RCC_ICSCR 		(*(( volatile uint32_t*)(0x40021000+0x0004)))
#define __myReg_RCC_CFGR 		(*(( volatile uint32_t*)(0x40021000+0x000c)))
#define RCC_CR_DEFAULT_VALUE 	(0x00000300ul)
#define RCC_ICSCR_DEFAULT_VALUE (0x0000b000ul)

#define m_clock_rcc_cr_default() do{__myReg_RCC_CR=RCC_CR_DEFAULT_VALUE;}while(0);
#define m_clock_rcc_cr_pll_on() do{__myReg_RCC_CR |= (0x01ul<<24);}while(0);
#define m_clock_rcc_cr_pll_off() do{__myReg_RCC_CR &= ~(0x01ul<<24);}while(0);

#define m_clock_rcc_cr_msi_on() do{__myReg_RCC_CR |= (0x01ul<<8);}while(0);
#define m_clock_rcc_cr_msi_off() do{__myReg_RCC_CR &= ~(0x01ul<<8);}while(0);

#define m_clock_rcc_cr_waite_msi_stable() while(!(__myReg_RCC_CR & (0x01ul<<9)));
#define m_clock_pcc_icscr_set_defalut() do{__myReg_RCC_ICSCR=RCC_ICSCR_DEFAULT_VALUE;}while(0);
#define m_clock_pcc_icscr_set_msi_rang(r) do{__myReg_RCC_ICSCR &= ~(0x07<<13);__myReg_RCC_ICSCR |= (x<<13);}while(0);
	*/

void m_clock_config_error_handler(void)
{
	mcu_error_reset();
	while(1)
	{
		__nop();
	}  	
}
void m_clock_disable_msi(void)
{
	__myReg_RCC_CR &= ~(0x01ul<<8);
}
void m_clock_enable_hsi(void)
{	
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};	
	RCC_OscInitStruct.OscillatorType    = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState          = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue  = RCC_HSICALIBRATION_DEFAULT;

	RCC_OscInitStruct.PLL.PLLState      = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLMUL        = RCC_PLLMUL_3;
	RCC_OscInitStruct.PLL.PLLDIV        = RCC_PLLDIV_2;
	RCC_OscInitStruct.PLL.PLLSource     = RCC_PLLSOURCE_HSI;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		// Initialization Error 
		m_clock_config_error_handler();
	}

	RCC_ClkInitStruct.ClockType       = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource    = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider   = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider  = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider  = RCC_HCLK_DIV1;
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
	{
// Initialization Error 
	//Error_Handler();
		m_clock_config_error_handler();
		//while(1);
	}

	
	
	
}
void m_clock_disable_hsi(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};	
	RCC_OscInitStruct.OscillatorType    = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState          = RCC_HSI_OFF;
	//RCC_OscInitStruct.HSICalibrationValue  = RCC_HSICALIBRATION_DEFAULT;

	RCC_OscInitStruct.PLL.PLLState      = RCC_PLL_NONE;
	RCC_OscInitStruct.PLL.PLLMUL        = RCC_PLLMUL_3;
	RCC_OscInitStruct.PLL.PLLDIV        = RCC_PLLDIV_2;
	RCC_OscInitStruct.PLL.PLLSource     = RCC_PLLSOURCE_HSI;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		/* Initialization Error */
		m_clock_config_error_handler();
	}	
}


void m_clock_set_system_clock_hsi(void)
{
	//m_clock_set_system_clock_msi();
	//m_clock_config_enable_msi();
	
	m_clock_select_msi();
	//m_clock_disable_hsi();	
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
	m_clock_enable_hsi();
	m_clock_disable_msi();
	//m_clock_select_hsi();
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
	__HAL_RCC_PWR_CLK_DISABLE();	
	
}


void m_clock_set_system_clock_msi(void)
{
	__HAL_RCC_PWR_CLK_ENABLE();
	//HAL_PWR_EnableBkUpAccess();
	//__HAL_RCC_BACKUPRESET_FORCE(); 
	//__HAL_RCC_BACKUPRESET_RELEASE();
	m_clock_config_enable_msi();
	m_clock_select_msi();
	m_clock_disable_hsi();
	// Enable Power Control clock 
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
	// Disable Power Control clock 
	__HAL_RCC_PWR_CLK_DISABLE();
}

/*
void m_clock_set_system_clock_msi(void)
{
	__HAL_RCC_PWR_CLK_ENABLE();

	__myReg_RCC_CR=RCC_CR_DEFAULT_VALUE;
	__myReg_RCC_ICSCR=RCC_ICSCR_DEFAULT_VALUE;
	__myReg_RCC_ICSCR &= ~(0x07ul<<13);
	__myReg_RCC_ICSCR |= (0x06ul<<13);
	while(!(__myReg_RCC_CR & (0x01ul<<9)));
	__myReg_RCC_CFGR &= ~0x03ul;
	// Enable Power Control clock 
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
	// Disable Power Control clock 
	__HAL_RCC_PWR_CLK_DISABLE();
	//SystemCoreClock = 0x00400000;
}
*/
void m_clock_select_msi(void)
{
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	/* Select MSI as system clock source and configure the HCLK, PCLK1 and PCLK2 
	 clocks dividers */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;  
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0)!= HAL_OK)
	{
		/* Initialization Error */
		//while(1); 
		m_clock_config_error_handler();
	}	
}

void m_clock_set_rtc_clock(void)
{
	__HAL_RCC_RTC_CONFIG(RCC_RTCCLKSOURCE_LSE);
}
void m_clock_config_enable_msi(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct={0};

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
	//RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_5;
	RCC_OscInitStruct.MSICalibrationValue=0x00;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct)!= HAL_OK)
	{
		/* Initialization Error */
		m_clock_config_error_handler();
	}
}

void m_clock_config_disable_msi(void)
{
	RCC_OscInitTypeDef RCC_OscInitStructure;

	RCC_OscInitStructure.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStructure.LSIState = RCC_MSI_OFF;
	RCC_OscInitStructure.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStructure) !=  HAL_OK)
	{
		//Error_Handler();
		m_clock_config_error_handler();
	}
	
}
void m_clock_config_enable_lsi(void)
{
	RCC_OscInitTypeDef RCC_OscInitStructure;

	RCC_OscInitStructure.OscillatorType = RCC_OSCILLATORTYPE_LSI;
	RCC_OscInitStructure.LSIState = RCC_LSI_ON;
	RCC_OscInitStructure.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStructure) !=  HAL_OK)
	{
		//Error_Handler();
		m_clock_config_error_handler();
	}
}

void m_clock_config_disable_lsi(void)
{
	RCC_OscInitTypeDef RCC_OscInitStructure;

	RCC_OscInitStructure.OscillatorType = RCC_OSCILLATORTYPE_LSI;
	RCC_OscInitStructure.LSIState = RCC_LSI_OFF;
	RCC_OscInitStructure.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStructure) !=  HAL_OK)
	{
		//Error_Handler();
		m_clock_config_error_handler();
	}
}
void m_clock_config_enable_lse(void)
{
	RCC_OscInitTypeDef RCC_OscInitStructure;
	RCC_OscInitStructure.OscillatorType = RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStructure.LSIState = RCC_LSE_ON;
	RCC_OscInitStructure.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStructure) !=  HAL_OK)
	{
		//Error_Handler();
		m_clock_config_error_handler();
	}	
}

void m_clock_config_disable_lse(void)
{
	RCC_OscInitTypeDef RCC_OscInitStructure;
	RCC_OscInitStructure.OscillatorType = RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStructure.LSIState = RCC_LSE_OFF;
	RCC_OscInitStructure.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStructure) !=  HAL_OK)
	{
		//Error_Handler();
		m_clock_config_error_handler();
	}		
}



HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
	
	portNVIC_SYSTICK_LOAD = ( configCPU_CLOCK_HZ / configTICK_RATE_HZ ) - 1UL;
	portNVIC_SYSTICK_CTRL = portNVIC_SYSTICK_CLK | portNVIC_SYSTICK_INT | portNVIC_SYSTICK_ENABLE;
  //HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/configTICK_RATE_HZ);
	HAL_NVIC_SetPriority(SysTick_IRQn, TickPriority ,0);
   /* Return function status */
  return HAL_OK;
}

/*
extern __IO  uint32_t uwTick;

void HAL_IncTick(void)
{
	uwTick++;
	if(uart4RxTimeOut>0)
	{
		uart4RxTimeOut++;
		if(uart4RxTimeOut>UART_IDLE_TIME)
		{
			uart4RxTimeOut=0;
			HAL_UART_RxCpltCallback(&UartHandle);
		}
	}
}
*/
//file end
//@
