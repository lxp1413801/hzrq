#include "../../includes/includes.h"
RNG_HandleTypeDef RngHandle;
uint8_t m_rng_init(void){
	RngHandle.Instance = RNG;
	if (HAL_RNG_DeInit(&RngHandle) != HAL_OK)return 0;
	if (HAL_RNG_Init(&RngHandle) != HAL_OK)return 0;
	return 1;
}
uint8_t m_rng_reinit(void)
{
	//if (HAL_RNG_DeInit(&RngHandle) != HAL_OK)return 0;
	HAL_RNG_MspDeInit(&RngHandle);
	return 1;
}
uint8_t m_rng_generation(float *rng)
{
	uint32_t t32;
	float f=1.0f;
	do{
		if (HAL_RNG_GenerateRandomNumber(&RngHandle, &t32) == HAL_OK)break;
	}while(1);
	f=(float)t32;
	f=f/((float)(0xfffffffful));
	*rng=f;
	HAL_RNG_MspDeInit(&RngHandle);
	return 1;
}
void HAL_RNG_MspInit(RNG_HandleTypeDef *hrng)
{  

  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct = {0};
  
  /* Enable HSI48 Oscillator for USB/RNG */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }
  
  /* configure RNG clock for USB or RNG analog part */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }

  /* RNG Peripheral clock enable */
  __HAL_RCC_RNG_CLK_ENABLE();

}

/**
  * @brief RNG MSP De-Initialization
  *        This function freeze the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  * @param hrng: RNG handle pointer
  * @retval None
  */
void HAL_RNG_MspDeInit(RNG_HandleTypeDef *hrng)
{

	
	// add by lxp
	__HAL_RCC_RNG_CLK_ENABLE();	
	
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct = {0};
  
  /* Enable HSI48 Oscillator for USB/RNG */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_OFF;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }
  
  /* configure RNG clock for USB or RNG analog part */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }


	__HAL_RCC_RNG_CLK_DISABLE();
	  /* RNG Peripheral clock enable */
	  
	  /* Enable RNG reset state */
	  __HAL_RCC_RNG_FORCE_RESET();

	  /* Release RNG from reset state */
	  __HAL_RCC_RNG_RELEASE_RESET();  
		
} 
//file end
