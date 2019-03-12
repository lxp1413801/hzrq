#include "stdint.h"
#include "stm32l0xx_hal.h"
#include "cmsis_os.h"

#include "m_aes.h"

void HAL_CRYP_MspInit(CRYP_HandleTypeDef *hcryp)
{
}

/**
  * @brief  DeInitializes the CRYP MSP.
  * @param  hcryp: CRYP handle pointer
  * @retval None
  */
void HAL_CRYP_MspDeInit(CRYP_HandleTypeDef *hcryp)
{
  /* Disable CRYP clock */
  __HAL_RCC_AES_CLK_DISABLE();

    /* Disable CRYP Interrupt */
    HAL_NVIC_DisableIRQ(AES_RNG_LPUART1_IRQn); 	

}


//file end
//

