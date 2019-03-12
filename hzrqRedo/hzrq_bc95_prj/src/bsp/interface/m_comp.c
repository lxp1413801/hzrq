#include "stdint.h"

#include "stm32l0xx_hal.h"
#include "cmsis_os.h"

#include "m_gpio_config.h"
#include "m_comp.h"

COMP_HandleTypeDef   Comp2Handle;
/**
  * @brief COMP MSP Initialization 
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  *           - NVIC configuration for COMP interrupt request enable
  * @param hcomp: COMP handle pointer
  * @retval None
  */
void HAL_COMP_MspInit(COMP_HandleTypeDef *hcomp)
{

	m_gpio_config_pmcomp();

	
	/*##-3- Configure the NVIC for COMPx #######################################*/
	/* Enable the COMPx IRQ Channel */
	HAL_NVIC_SetPriority(COMP2_IRQn, 3, 0);
	HAL_NVIC_EnableIRQ(COMP2_IRQn);
}

/**
  * @brief  DeInitializes the COMP MSP.
  * @param  hcomp: pointer to a COMP_HandleTypeDef structure that contains
  *         the configuration information for the specified COMP.  
  * @retval None
  */
void HAL_COMP_MspDeInit(COMP_HandleTypeDef *hcomp)
{
	/*##-1- De-initialize peripheral GPIO ######################################*/
	/* De-initialize the COMPx GPIO pin */
	HAL_GPIO_DeInit(COMP2_INP_PORT,COMP2_INP_PIN);
	/*##-2- Disable peripherals and GPIO clocks ################################*/
	/* Disable COMP peripheral clock ***************************************/
	__HAL_RCC_SYSCFG_CLK_DISABLE();

	/* Disable GPIO clock **************************************************/
	/* Note: In a real application, by disabling GPIO port, be cautious to not  */
	/* interfere with potential other peripherals using other GPIOs on the same */
	/* port.                                                                    */
	//COMPx_GPIO_CLK_DISABLE();
	m_gpio_pmcomp_rcc_disable();
	/*##-3- Disable the NVIC for COMP ##########################################*/
	HAL_NVIC_DisableIRQ(COMP2_IRQn);
}

void m_comp_error_handler(void)
{
	while(1)
	{
		__nop();
	}
}
/**
  * @brief  COMP configuration
  * @param  None
  * @retval None
  */
//static void COMP_Config(void)
void m_comp_config(void)
{
  /* COMP2 Init: the lower threshold is set to VREFINT/4 (1.22V /4 ~ 0.305 V) */
  /* but can be changed to other available possibilities.                     */
  Comp2Handle.Instance                   = COMP2;
  Comp2Handle.Init.InvertingInput        = COMP_INVERTINGINPUT_1_4VREFINT;
  Comp2Handle.Init.NonInvertingInput     = COMP_NONINVERTINGINPUT_IO2;
  Comp2Handle.Init.LPTIMConnection       = COMP_LPTIMCONNECTION_DISABLED;
  Comp2Handle.Init.OutputPol             = COMP_OUTPUTPOL_NONINVERTED;
  Comp2Handle.Init.Mode                  = COMP_MODE_LOWSPEED;
  Comp2Handle.Init.WindowMode            = COMP_WINDOWMODE_ENABLE;
  Comp2Handle.Init.TriggerMode           = COMP_TRIGGERMODE_IT_RISING_FALLING;


  if(HAL_COMP_Init(&Comp2Handle) != HAL_OK)
  {
    /* Initialization Error */
    m_comp_error_handler(); 
  }
  
  /* Enable VREFINT */
   HAL_COMPEx_EnableVREFINT();

}
void m_comp_start(COMP_HandleTypeDef *hcomp)
{
	HAL_COMP_Start_IT(hcomp);
}

void m_comp_stop(COMP_HandleTypeDef *hcomp)
{
	HAL_COMP_Stop_IT(hcomp);
}

void HAL_COMP_TriggerCallback(COMP_HandleTypeDef *hcomp)
{
	uint32_t comp2OutLevel;
  /* Check input voltage level: within the thresholds, */
  /* above the upper threshold or under the lower threshold */
  //InputVoltageLevel_Check();
	if(hcomp->Instance!=COMP2)return;
	comp2OutLevel=HAL_COMP_GetOutputLevel(hcomp);
	if(comp2OutLevel==COMP_OUTPUTLEVEL_LOW)
	{
		//power down
	}
	else
	{
		//power up
	}
	//add 
}
//file end
//@
