/**
  ******************************************************************************
  * File Name          : DAC.c
  * Description        : This file provides code for the configuration
  *                      of the DAC instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "dac.h"
#include "dds.h"
#include "tim.h"
/* USER CODE BEGIN 0 */
u16 DAC_CH1_BUFF[DAC_BUFFER_NUM];
u16 DAC_CH2_BUFF[DAC_BUFFER_NUM];

extern int ADC1_value[1024];
extern int ADC2_value[1024];
/* USER CODE END 0 */

DAC_HandleTypeDef hdac1;
DMA_HandleTypeDef hdma_dac1_ch1;
DMA_HandleTypeDef hdma_dac1_ch2;

/* DAC1 init function */
void MX_DAC1_Init(void)
{
  DAC_ChannelConfTypeDef sConfig = {0};

  /** DAC Initialization 
  */
  hdac1.Instance = DAC1;
  if (HAL_DAC_Init(&hdac1) != HAL_OK)
  {
    Error_Handler();
  }
  /** DAC channel OUT1 config 
  */
  sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
  sConfig.DAC_Trigger = DAC_TRIGGER_T4_TRGO;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_DISABLE;
  sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;
  if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /** DAC channel OUT2 config 
  */
  sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_DISABLE;
  if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_DAC_MspInit(DAC_HandleTypeDef* dacHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(dacHandle->Instance==DAC1)
  {
  /* USER CODE BEGIN DAC1_MspInit 0 */

  /* USER CODE END DAC1_MspInit 0 */
    /* DAC1 clock enable */
    __HAL_RCC_DAC12_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**DAC1 GPIO Configuration    
    PA4     ------> DAC1_OUT1
    PA5     ------> DAC1_OUT2 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* DAC1 DMA Init */
    /* DAC1_CH1 Init */
    hdma_dac1_ch1.Instance = DMA1_Stream6;
    hdma_dac1_ch1.Init.Request = DMA_REQUEST_DAC1;
    hdma_dac1_ch1.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_dac1_ch1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_dac1_ch1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_dac1_ch1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_dac1_ch1.Init.MemDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_dac1_ch1.Init.Mode = DMA_CIRCULAR;
    hdma_dac1_ch1.Init.Priority = DMA_PRIORITY_LOW;
    hdma_dac1_ch1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	
    if (HAL_DMA_Init(&hdma_dac1_ch1) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(dacHandle,DMA_Handle1,hdma_dac1_ch1);

    /* DAC1_CH2 Init */
    hdma_dac1_ch2.Instance = DMA1_Stream7;
    hdma_dac1_ch2.Init.Request = DMA_REQUEST_DAC2;
    hdma_dac1_ch2.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_dac1_ch2.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_dac1_ch2.Init.MemInc = DMA_MINC_ENABLE;
    hdma_dac1_ch2.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_dac1_ch2.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_dac1_ch2.Init.Mode = DMA_CIRCULAR;
    hdma_dac1_ch2.Init.Priority = DMA_PRIORITY_LOW;
    hdma_dac1_ch2.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	
    if (HAL_DMA_Init(&hdma_dac1_ch2) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(dacHandle,DMA_Handle2,hdma_dac1_ch2);

  /* USER CODE BEGIN DAC1_MspInit 1 */

  /* USER CODE END DAC1_MspInit 1 */
  }
}

void HAL_DAC_MspDeInit(DAC_HandleTypeDef* dacHandle)
{

  if(dacHandle->Instance==DAC1)
  {
  /* USER CODE BEGIN DAC1_MspDeInit 0 */

  /* USER CODE END DAC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_DAC12_CLK_DISABLE();
  
    /**DAC1 GPIO Configuration    
    PA4     ------> DAC1_OUT1
    PA5     ------> DAC1_OUT2 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4|GPIO_PIN_5);

    /* DAC1 DMA DeInit */
    HAL_DMA_DeInit(dacHandle->DMA_Handle1);
    HAL_DMA_DeInit(dacHandle->DMA_Handle2);
  /* USER CODE BEGIN DAC1_MspDeInit 1 */

  /* USER CODE END DAC1_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */
 
void HAL_DAC_ConvHalfCpltCallbackCh1(DAC_HandleTypeDef* hdac)
{
	for(int i = 0; i < 512; i++)      
    {
         DAC_CH1_BUFF[i] = ADC1_value[i] ;
    }
}

void HAL_DAC_ConvCpltCallbackCh1 (DAC_HandleTypeDef* hdac)
{
	for(int i = 512; i < 1024; i++)      
    {
         DAC_CH1_BUFF[i] = ADC1_value[i] ;
    }
}

void HAL_DACEx_ConvHalfCpltCallbackCh2(DAC_HandleTypeDef* hdac)
{
	for(int i = 0; i < 512; i++)      
    {
         DAC_CH2_BUFF[i] = ADC2_value[i];
    }
}

void HAL_DACEx_ConvCpltCallbackCh2(DAC_HandleTypeDef* hdac)
{
	for(int i = 512; i < 1024; i++)      
    {
         DAC_CH2_BUFF[i] = ADC2_value[i];
    }
}

void DAC_DMA_START()
{
 
	HAL_DAC_Start_DMA(&hdac1, DAC1_CHANNEL_1, (u32*)DAC_CH1_BUFF, DAC_BUFFER_NUM, DAC_ALIGN_12B_R);
	HAL_DAC_Start_DMA(&hdac1, DAC1_CHANNEL_2, (u32*)DAC_CH2_BUFF, DAC_BUFFER_NUM, DAC_ALIGN_12B_R);
	HAL_TIM_Base_Start(&htim4); //使能定时器3和定时器3更新中断：TIM_IT_UPDATE 

}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
