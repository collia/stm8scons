/**
  ******************************************************************************
  * @file    stm8s_adc1.c
  * @author  MCD Application Team
  * @version V2.3.0
  * @date    16-June-2017
  * @brief   This file contains all the functions/macros for the ADC1 peripheral.
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
#include "stm8s_adc1.h"

/** @addtogroup STM8S_StdPeriph_Driver
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/

/**
  * @addtogroup ADC1_Public_Functions
  * @{
  */
/**
  * @brief  Deinitializes the ADC1 peripheral registers to their default reset values.
  * @param  None
  * @retval None
  */
void ADC1_DeInit(void)
{
  ADC1->CSR  = ADC1_CSR_RESET_VALUE;
  ADC1->CR1  = ADC1_CR1_RESET_VALUE;
  ADC1->CR2  = ADC1_CR2_RESET_VALUE;
  ADC1->CR3  = ADC1_CR3_RESET_VALUE;
  ADC1->TDRH = ADC1_TDRH_RESET_VALUE;
  ADC1->TDRL = ADC1_TDRL_RESET_VALUE;
  ADC1->HTRH = ADC1_HTRH_RESET_VALUE;
  ADC1->HTRL = ADC1_HTRL_RESET_VALUE;
  ADC1->LTRH = ADC1_LTRH_RESET_VALUE;
  ADC1->LTRL = ADC1_LTRL_RESET_VALUE;
  ADC1->AWCRH = ADC1_AWCRH_RESET_VALUE;
  ADC1->AWCRL = ADC1_AWCRL_RESET_VALUE;
}


/**
  * @brief  Initializes the ADC1 peripheral according to the specified parameters
  * @param   ADC1_ConversionMode: specifies the conversion mode
  * can be one of the values of @ref ADC1_ConvMode_TypeDef.
  * @param   ADC1_Channel: specifies the channel to convert
  * can be one of the values of @ref ADC1_Channel_TypeDef.
  * @param   ADC1_PrescalerSelection: specifies the ADC1 prescaler
  * can be one of the values of @ref ADC1_PresSel_TypeDef.
  * @param   ADC1_ExtTrigger: specifies the external trigger
  * can be one of the values of @ref ADC1_ExtTrig_TypeDef.
  * @param   ADC1_ExtTriggerState: specifies the external trigger new state
  * can be one of the values of @ref FunctionalState.
  * @param   ADC1_Align: specifies the converted data alignment
  * can be one of the values of @ref ADC1_Align_TypeDef.
  * @param   ADC1_SchmittTriggerChannel: specifies the schmitt trigger channel
  * can be one of the values of @ref ADC1_SchmittTrigg_TypeDef.
  * @param   ADC1_SchmittTriggerState: specifies the schmitt trigger state
  * can be one of the values of @ref FunctionalState.
  * @retval None
  */
void ADC1_Init(ADC1_ConvMode_TypeDef ADC1_ConversionMode, ADC1_Channel_TypeDef ADC1_Channel, ADC1_PresSel_TypeDef ADC1_PrescalerSelection, ADC1_ExtTrig_TypeDef ADC1_ExtTrigger, FunctionalState ADC1_ExtTriggerState, ADC1_Align_TypeDef ADC1_Align, ADC1_SchmittTrigg_TypeDef ADC1_SchmittTriggerChannel, FunctionalState ADC1_SchmittTriggerState)
{
  /* Check the parameters */
  assert_param(IS_ADC1_CONVERSIONMODE_OK(ADC1_ConversionMode));
  assert_param(IS_ADC1_CHANNEL_OK(ADC1_Channel));
  assert_param(IS_ADC1_PRESSEL_OK(ADC1_PrescalerSelection));
  assert_param(IS_ADC1_EXTTRIG_OK(ADC1_ExtTrigger));
  assert_param(IS_FUNCTIONALSTATE_OK(((ADC1_ExtTriggerState))));
  assert_param(IS_ADC1_ALIGN_OK(ADC1_Align));
  assert_param(IS_ADC1_SCHMITTTRIG_OK(ADC1_SchmittTriggerChannel));
  assert_param(IS_FUNCTIONALSTATE_OK(ADC1_SchmittTriggerState));
  
  /*-----------------CR1 & CSR configuration --------------------*/
  /* Configure the conversion mode and the channel to convert
  respectively according to ADC1_ConversionMode & ADC1_Channel values  &  ADC1_Align values */
  ADC1_ConversionConfig(ADC1_ConversionMode, ADC1_Channel, ADC1_Align);
  /* Select the prescaler division factor according to ADC1_PrescalerSelection values */
  ADC1_PrescalerConfig(ADC1_PrescalerSelection);
#if DEBUG  
  /*-----------------CR2 configuration --------------------*/
  /* Configure the external trigger state and event respectively
  according to NewState, ADC1_ExtTrigger */
  ADC1_ExternalTriggerConfig(ADC1_ExtTrigger, ADC1_ExtTriggerState);
  
  /*------------------TDR configuration ---------------------------*/
  /* Configure the schmitt trigger channel and state respectively
  according to ADC1_SchmittTriggerChannel & ADC1_SchmittTriggerNewState  values */
  ADC1_SchmittTriggerConfig(ADC1_SchmittTriggerChannel, ADC1_SchmittTriggerState);

  /* Enable the ADC1 peripheral */
  ADC1->CR1 |= ADC1_CR1_ADON;
#endif
}

/**
  * @brief  Enables or Disables the ADC1 peripheral.
  * @param  NewState: specifies the peripheral enabled or disabled state.
  * @retval None
  */
void ADC1_Cmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONALSTATE_OK(NewState));
  
  if (NewState != DISABLE)
  {
    ADC1->CR1 |= ADC1_CR1_ADON;
  }
  else /* NewState == DISABLE */
  {
    ADC1->CR1 &= (uint8_t)(~ADC1_CR1_ADON);
  }
}

/**
  * @brief  Configure the ADC1 prescaler division factor.
  * @param   ADC1_Prescaler: the selected precaler.
  * It can be one of the values of @ref ADC1_PresSel_TypeDef.
  * @retval None
  */
void ADC1_PrescalerConfig(ADC1_PresSel_TypeDef ADC1_Prescaler)
{
  /* Check the parameter */
  assert_param(IS_ADC1_PRESSEL_OK(ADC1_Prescaler));
  
  /* Clear the SPSEL bits */
  ADC1->CR1 &= (uint8_t)(~ADC1_CR1_SPSEL);
  /* Select the prescaler division factor according to ADC1_PrescalerSelection values */
  ADC1->CR1 |= (uint8_t)(ADC1_Prescaler);
}
#ifdef DEBUG
/**
  * @brief  Enables or disables the ADC1 Schmitt Trigger on a selected channel.
  * @param   ADC1_SchmittTriggerChannel specifies the desired Channel.
  * It can be set of the values of @ref ADC1_SchmittTrigg_TypeDef.
  * @param   NewState specifies Channel new status.
  * can have one of the values of @ref FunctionalState.
  * @retval None
  */
void ADC1_SchmittTriggerConfig(ADC1_SchmittTrigg_TypeDef ADC1_SchmittTriggerChannel, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_ADC1_SCHMITTTRIG_OK(ADC1_SchmittTriggerChannel));
  assert_param(IS_FUNCTIONALSTATE_OK(NewState));
  
  if (ADC1_SchmittTriggerChannel == ADC1_SCHMITTTRIG_ALL)
  {
    if (NewState != DISABLE)
    {
      ADC1->TDRL &= (uint8_t)0x0;
      ADC1->TDRH &= (uint8_t)0x0;
    }
    else /* NewState == DISABLE */
    {
      ADC1->TDRL |= (uint8_t)0xFF;
      ADC1->TDRH |= (uint8_t)0xFF;
    }
  }
  else if (ADC1_SchmittTriggerChannel < ADC1_SCHMITTTRIG_CHANNEL8)
  {
    if (NewState != DISABLE)
    {
      ADC1->TDRL &= (uint8_t)(~(uint8_t)((uint8_t)0x01 << (uint8_t)ADC1_SchmittTriggerChannel));
    }
    else /* NewState == DISABLE */
    {
      ADC1->TDRL |= (uint8_t)((uint8_t)0x01 << (uint8_t)ADC1_SchmittTriggerChannel);
    }
  }
  else /* ADC1_SchmittTriggerChannel >= ADC1_SCHMITTTRIG_CHANNEL8 */
  {
    if (NewState != DISABLE)
    {
      ADC1->TDRH &= (uint8_t)(~(uint8_t)((uint8_t)0x01 << ((uint8_t)ADC1_SchmittTriggerChannel - (uint8_t)8)));
    }
    else /* NewState == DISABLE */
    {
      ADC1->TDRH |= (uint8_t)((uint8_t)0x01 << ((uint8_t)ADC1_SchmittTriggerChannel - (uint8_t)8));
    }
  }
}
#endif
/**
  * @brief  Configure the ADC1 conversion on selected channel.
  * @param   ADC1_ConversionMode Specifies the conversion type.
  * It can be set of the values of @ref ADC1_ConvMode_TypeDef
  * @param   ADC1_Channel specifies the ADC1 Channel.
  * It can be set of the values of @ref ADC1_Channel_TypeDef
  * @param   ADC1_Align specifies the converted data alignment.
  * It can be set of the values of @ref ADC1_Align_TypeDef
  * @retval None
  */
void ADC1_ConversionConfig(ADC1_ConvMode_TypeDef ADC1_ConversionMode, ADC1_Channel_TypeDef ADC1_Channel, ADC1_Align_TypeDef ADC1_Align)
{
  /* Check the parameters */
  assert_param(IS_ADC1_CONVERSIONMODE_OK(ADC1_ConversionMode));
  assert_param(IS_ADC1_CHANNEL_OK(ADC1_Channel));
  assert_param(IS_ADC1_ALIGN_OK(ADC1_Align));
  
  /* Clear the align bit */
  ADC1->CR2 &= (uint8_t)(~ADC1_CR2_ALIGN);
  /* Configure the data alignment */
  ADC1->CR2 |= (uint8_t)(ADC1_Align);
  
  if (ADC1_ConversionMode == ADC1_CONVERSIONMODE_CONTINUOUS)
  {
    /* Set the continuous conversion mode */
    ADC1->CR1 |= ADC1_CR1_CONT;
  }
  else /* ADC1_ConversionMode == ADC1_CONVERSIONMODE_SINGLE */
  {
    /* Set the single conversion mode */
    ADC1->CR1 &= (uint8_t)(~ADC1_CR1_CONT);
  }
  
  /* Clear the ADC1 channels */
  ADC1->CSR &= (uint8_t)(~ADC1_CSR_CH);
  /* Select the ADC1 channel */
  ADC1->CSR |= (uint8_t)(ADC1_Channel);
}
#ifdef DEBUG
/**
  * @brief  Configure the ADC1 conversion on external trigger event.
  * @par Full description:
  * The selected external trigger event can be enabled or disabled.
  * @param   ADC1_ExtTrigger to select the External trigger event.
  * can have one of the values of @ref ADC1_ExtTrig_TypeDef.
  * @param   NewState to enable/disable the selected external trigger
  * can have one of the values of @ref FunctionalState.
  * @retval None
  */
void ADC1_ExternalTriggerConfig(ADC1_ExtTrig_TypeDef ADC1_ExtTrigger, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_ADC1_EXTTRIG_OK(ADC1_ExtTrigger));
  assert_param(IS_FUNCTIONALSTATE_OK(NewState));
  
  /* Clear the external trigger selection bits */
  ADC1->CR2 &= (uint8_t)(~ADC1_CR2_EXTSEL);
  
  if (NewState != DISABLE)
  {
    /* Enable the selected external Trigger */
    ADC1->CR2 |= (uint8_t)(ADC1_CR2_EXTTRIG);
  }
  else /* NewState == DISABLE */
  {
    /* Disable the selected external trigger */
    ADC1->CR2 &= (uint8_t)(~ADC1_CR2_EXTTRIG);
  }
  
  /* Set the selected external trigger */
  ADC1->CR2 |= (uint8_t)(ADC1_ExtTrigger);
}
#endif
/**
  * @brief  Get one sample of measured signal.
  * @param  None
  * @retval ConversionValue:  value of the measured signal.
  * @par Required preconditions:
  * ADC1 conversion finished.
  */
uint16_t ADC1_GetConversionValue(void)
{
  uint16_t temph = 0;
  uint8_t templ = 0;
  
  if ((ADC1->CR2 & ADC1_CR2_ALIGN) != 0) /* Right alignment */
  {
    /* Read LSB first */
    templ = ADC1->DRL;
    /* Then read MSB */
    temph = ADC1->DRH;
    
    temph = (uint16_t)(templ | (uint16_t)(temph << (uint8_t)8));
  }
  else /* Left alignment */
  {
    /* Read MSB first*/
    temph = ADC1->DRH;
    /* Then read LSB */
    templ = ADC1->DRL;
    
    temph = (uint16_t)((uint16_t)((uint16_t)templ << 6) | (uint16_t)((uint16_t)temph << 8));
  }
  
  return ((uint16_t)temph);
}

/**
  * @brief  Checks the specified ADC1 flag status.
  * @param   Flag: ADC1 flag.
  * can be one of the values of @ref ADC1_Flag_TypeDef.
  * @retval FlagStatus Status of the ADC1 flag.
  */
FlagStatus ADC1_GetFlagStatus(ADC1_Flag_TypeDef Flag)
{
  uint8_t flagstatus = 0;
  uint8_t temp = 0;
  
  /* Check the parameters */
  assert_param(IS_ADC1_FLAG_OK(Flag));
#ifdef DEBUG
  if ((Flag & 0x0F) == 0x01)
  {
    /* Get OVR flag status */
    flagstatus = (uint8_t)(ADC1->CR3 & ADC1_CR3_OVR);
  }
  else if ((Flag & 0xF0) == 0x10)
  {
    /* Get analog watchdog channel status */
    temp = (uint8_t)(Flag & (uint8_t)0x0F);
    if (temp < 8)
    {
      flagstatus = (uint8_t)(ADC1->AWSRL & (uint8_t)((uint8_t)1 << temp));
    }
    else
    {
      flagstatus = (uint8_t)(ADC1->AWSRH & (uint8_t)((uint8_t)1 << (temp - 8)));
    }
  }
  else  /* Get EOC | AWD flag status */
#endif
  {
    flagstatus = (uint8_t)(ADC1->CSR & Flag);
  }
  return ((FlagStatus)flagstatus);
  
}

/**
  * @brief  Clear the specified ADC1 Flag.
  * @param   Flag: ADC1 flag.
  * can be one of the values of @ref ADC1_Flag_TypeDef.
  * @retval None
  */
void ADC1_ClearFlag(ADC1_Flag_TypeDef Flag)
{
  uint8_t temp = 0;
  
  /* Check the parameters */
  assert_param(IS_ADC1_FLAG_OK(Flag));
#ifdef DEBUG
  if ((Flag & 0x0F) == 0x01)
  {
    /* Clear OVR flag status */
    ADC1->CR3 &= (uint8_t)(~ADC1_CR3_OVR);
  }
  else if ((Flag & 0xF0) == 0x10)
  {
    /* Clear analog watchdog channel status */
    temp = (uint8_t)(Flag & (uint8_t)0x0F);
    if (temp < 8)
    {
      ADC1->AWSRL &= (uint8_t)~(uint8_t)((uint8_t)1 << temp);
    }
    else
    {
      ADC1->AWSRH &= (uint8_t)~(uint8_t)((uint8_t)1 << (temp - 8));
    }
  }
  else  /* Clear EOC | AWD flag status */
#endif
  {
    ADC1->CSR &= (uint8_t) (~Flag);
  }
}
  

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
