/**
  ******************************************************************************
  * @file    stm8s_clk.c
  * @author  MCD Application Team
  * @version V2.3.0
  * @date    16-June-2017
  * @brief   This file contains all the functions for the CLK peripheral.
   ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "stm8s_clk.h"

/** @addtogroup STM8S_StdPeriph_Driver
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Private Constants ---------------------------------------------------------*/

/**
  * @addtogroup CLK_Private_Constants
  * @{
  */

CONST uint8_t HSIDivFactor[4] = {1, 2, 4, 8}; /*!< Holds the different HSI Divider factors */
CONST uint8_t CLKPrescTable[8] = {1, 2, 4, 8, 10, 16, 20, 40}; /*!< Holds the different CLK prescaler values */

/**
  * @}
  */

/* Public functions ----------------------------------------------------------*/
/**
  * @addtogroup CLK_Public_Functions
  * @{
  */

/**
  * @brief  Starts or Stops manually the clock switch execution.
  * @par Full description:
  * NewState parameter set the SWEN.
  * @param   NewState new state of SWEN, value accepted ENABLE, DISABLE.
  * @retval None
  */
void CLK_ClockSwitchCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONALSTATE_OK(NewState));
  
  if (NewState != DISABLE )
  {
    /* Enable the Clock Switch */
    CLK->SWCR |= CLK_SWCR_SWEN;
  }
  else
  {
    /* Disable the Clock Switch */
    CLK->SWCR &= (uint8_t)(~CLK_SWCR_SWEN);
  }
}

/**
  * @brief   Enables or disables the specified peripheral CLK.
  * @param   CLK_Peripheral : This parameter specifies the peripheral clock to gate.
  * This parameter can be any of the  @ref CLK_Peripheral_TypeDef enumeration.
  * @param   NewState : New state of specified peripheral clock.
  * This parameter can be any of the @ref FunctionalState enumeration.
  * @retval None
  */
void CLK_PeripheralClockConfig(CLK_Peripheral_TypeDef CLK_Peripheral, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONALSTATE_OK(NewState));
  assert_param(IS_CLK_PERIPHERAL_OK(CLK_Peripheral));
  
  if (((uint8_t)CLK_Peripheral & (uint8_t)0x10) == 0x00)
  {
    if (NewState != DISABLE)
    {
      /* Enable the peripheral Clock */
      CLK->PCKENR1 |= (uint8_t)((uint8_t)1 << ((uint8_t)CLK_Peripheral & (uint8_t)0x0F));
    }
    else
    {
      /* Disable the peripheral Clock */
      CLK->PCKENR1 &= (uint8_t)(~(uint8_t)(((uint8_t)1 << ((uint8_t)CLK_Peripheral & (uint8_t)0x0F))));
    }
  }
  else
  {
    if (NewState != DISABLE)
    {
      /* Enable the peripheral Clock */
      CLK->PCKENR2 |= (uint8_t)((uint8_t)1 << ((uint8_t)CLK_Peripheral & (uint8_t)0x0F));
    }
    else
    {
      /* Disable the peripheral Clock */
      CLK->PCKENR2 &= (uint8_t)(~(uint8_t)(((uint8_t)1 << ((uint8_t)CLK_Peripheral & (uint8_t)0x0F))));
    }
  }
}

/**
  * @brief  configures the Switch from one clock to another
  * @param   CLK_SwitchMode select the clock switch mode.
  * It can be set of the values of @ref CLK_SwitchMode_TypeDef
  * @param   CLK_NewClock choice of the future clock.
  * It can be set of the values of @ref CLK_Source_TypeDef
  * @param   ITState Enable or Disable the Clock Switch interrupt.
  * @param   CLK_CurrentClockState current clock to switch OFF or to keep ON.
  * It can be set of the values of @ref CLK_CurrentClockState_TypeDef
  * @note LSI selected as master clock source only if LSI_EN option bit is set.
  * @retval ErrorStatus this shows the clock switch status (ERROR/SUCCESS).
  */
ErrorStatus CLK_ClockSwitchConfig(CLK_SwitchMode_TypeDef CLK_SwitchMode, CLK_Source_TypeDef CLK_NewClock, FunctionalState ITState, CLK_CurrentClockState_TypeDef CLK_CurrentClockState)
{
  CLK_Source_TypeDef clock_master;
  uint16_t DownCounter = CLK_TIMEOUT;
  ErrorStatus Swif = ERROR;
  
  /* Check the parameters */
  assert_param(IS_CLK_SOURCE_OK(CLK_NewClock));
  assert_param(IS_CLK_SWITCHMODE_OK(CLK_SwitchMode));
  assert_param(IS_FUNCTIONALSTATE_OK(ITState));
  assert_param(IS_CLK_CURRENTCLOCKSTATE_OK(CLK_CurrentClockState));
  
  /* Current clock master saving */
  clock_master = (CLK_Source_TypeDef)CLK->CMSR;
  
  /* Automatic switch mode management */
  if (CLK_SwitchMode == CLK_SWITCHMODE_AUTO)
  {
    /* Enables Clock switch */
    CLK->SWCR |= CLK_SWCR_SWEN;
    
    /* Enables or Disables Switch interrupt */
    if (ITState != DISABLE)
    {
      CLK->SWCR |= CLK_SWCR_SWIEN;
    }
    else
    {
      CLK->SWCR &= (uint8_t)(~CLK_SWCR_SWIEN);
    }
    
    /* Selection of the target clock source */
    CLK->SWR = (uint8_t)CLK_NewClock;
    
    /* Wait until the target clock source is ready */
    while((((CLK->SWCR & CLK_SWCR_SWBSY) != 0 )&& (DownCounter != 0)))
    {
      DownCounter--;
    }
    
    if(DownCounter != 0)
    {
      Swif = SUCCESS;
    }
    else
    {
      Swif = ERROR;
    }
  }
  else /* CLK_SwitchMode == CLK_SWITCHMODE_MANUAL */
  {
    /* Enables or Disables Switch interrupt  if required  */
    if (ITState != DISABLE)
    {
      CLK->SWCR |= CLK_SWCR_SWIEN;
    }
    else
    {
      CLK->SWCR &= (uint8_t)(~CLK_SWCR_SWIEN);
    }
    
    /* Selection of the target clock source */
    CLK->SWR = (uint8_t)CLK_NewClock;
    
    /* Wait until the target clock source is ready */
    while((((CLK->SWCR & CLK_SWCR_SWIF) != 0 ) && (DownCounter != 0)))
    {
      DownCounter--;
    }
    
    if(DownCounter != 0)
    {
      /* Enables Clock switch */
      CLK->SWCR |= CLK_SWCR_SWEN;
      Swif = SUCCESS;
    }
    else
    {
      Swif = ERROR;
    }
  }
  if(Swif != ERROR)
  {
    /* Switch OFF current clock if required */
    if((CLK_CurrentClockState == CLK_CURRENTCLOCKSTATE_DISABLE) && ( clock_master == CLK_SOURCE_HSI))
    {
      CLK->ICKR &= (uint8_t)(~CLK_ICKR_HSIEN);
    }
    else if((CLK_CurrentClockState == CLK_CURRENTCLOCKSTATE_DISABLE) && ( clock_master == CLK_SOURCE_LSI))
    {
      CLK->ICKR &= (uint8_t)(~CLK_ICKR_LSIEN);
    }
    else if ((CLK_CurrentClockState == CLK_CURRENTCLOCKSTATE_DISABLE) && ( clock_master == CLK_SOURCE_HSE))
    {
      CLK->ECKR &= (uint8_t)(~CLK_ECKR_HSEEN);
    }
  }
  return(Swif);
}

/**
  * @brief  Configures the HSI clock dividers.
  * @param   HSIPrescaler : Specifies the HSI clock divider to apply.
  * This parameter can be any of the @ref CLK_Prescaler_TypeDef enumeration.
  * @retval None
  */
void CLK_HSIPrescalerConfig(CLK_Prescaler_TypeDef HSIPrescaler)
{
  /* check the parameters */
  assert_param(IS_CLK_HSIPRESCALER_OK(HSIPrescaler));
  
  /* Clear High speed internal clock prescaler */
  CLK->CKDIVR &= (uint8_t)(~CLK_CKDIVR_HSIDIV);
  
  /* Set High speed internal clock prescaler */
  CLK->CKDIVR |= (uint8_t)HSIPrescaler;
}

/**
  * @brief  Configures the HSI and CPU clock dividers.
  * @param  CLK_Prescaler Specifies the HSI or CPU clock divider to apply.
  * @retval None
  */
void CLK_SYSCLKConfig(CLK_Prescaler_TypeDef CLK_Prescaler)
{
  /* check the parameters */
  assert_param(IS_CLK_PRESCALER_OK(CLK_Prescaler));
  
  if (((uint8_t)CLK_Prescaler & (uint8_t)0x80) == 0x00) /* Bit7 = 0 means HSI divider */
  {
    CLK->CKDIVR &= (uint8_t)(~CLK_CKDIVR_HSIDIV);
    CLK->CKDIVR |= (uint8_t)((uint8_t)CLK_Prescaler & (uint8_t)CLK_CKDIVR_HSIDIV);
  }
  else /* Bit7 = 1 means CPU divider */
  {
    CLK->CKDIVR &= (uint8_t)(~CLK_CKDIVR_CPUDIV);
    CLK->CKDIVR |= (uint8_t)((uint8_t)CLK_Prescaler & (uint8_t)CLK_CKDIVR_CPUDIV);
  }
}


/**
  * @brief  Checks whether the specified CLK flag is set or not.
  * @par Full description:
  * @param   CLK_FLAG Flag to check.
  * can be one of the values of @ref CLK_Flag_TypeDef
  * @retval FlagStatus, status of the checked flag
  */
FlagStatus CLK_GetFlagStatus(CLK_Flag_TypeDef CLK_FLAG)
{
  uint16_t statusreg = 0;
  uint8_t tmpreg = 0;
  FlagStatus bitstatus = RESET;
  
  /* check the parameters */
  assert_param(IS_CLK_FLAG_OK(CLK_FLAG));
  
  /* Get the CLK register index */
  statusreg = (uint16_t)((uint16_t)CLK_FLAG & (uint16_t)0xFF00);
  
  
  if (statusreg == 0x0100) /* The flag to check is in ICKRregister */
  {
    tmpreg = CLK->ICKR;
  }
  else if (statusreg == 0x0200) /* The flag to check is in ECKRregister */
  {
    tmpreg = CLK->ECKR;
  }
  else if (statusreg == 0x0300) /* The flag to check is in SWIC register */
  {
    tmpreg = CLK->SWCR;
  }
  else if (statusreg == 0x0400) /* The flag to check is in CSS register */
  {
    tmpreg = CLK->CSSR;
  }
  else /* The flag to check is in CCO register */
  {
    tmpreg = CLK->CCOR;
  }
  
  if ((tmpreg & (uint8_t)CLK_FLAG) != (uint8_t)RESET)
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  
  /* Return the flag status */
  return((FlagStatus)bitstatus);
}


/**
  * @}
  */
  
/**
  * @}
  */
  

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
