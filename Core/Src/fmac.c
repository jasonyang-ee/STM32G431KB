/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    fmac.c
  * @brief   This file provides code for the configuration
  *          of the FMAC instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "fmac.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

FMAC_HandleTypeDef hfmac;
DMA_HandleTypeDef hdma_fmac_read;
DMA_HandleTypeDef hdma_fmac_write;
DMA_HandleTypeDef hdma_fmac_preload;

/* FMAC init function */
void MX_FMAC_Init(void)
{

  /* USER CODE BEGIN FMAC_Init 0 */

  /* USER CODE END FMAC_Init 0 */

  /* USER CODE BEGIN FMAC_Init 1 */

  /* USER CODE END FMAC_Init 1 */
  hfmac.Instance = FMAC;
  if (HAL_FMAC_Init(&hfmac) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN FMAC_Init 2 */

  /* USER CODE END FMAC_Init 2 */

}

void HAL_FMAC_MspInit(FMAC_HandleTypeDef* fmacHandle)
{

  if(fmacHandle->Instance==FMAC)
  {
  /* USER CODE BEGIN FMAC_MspInit 0 */

  /* USER CODE END FMAC_MspInit 0 */
    /* FMAC clock enable */
    __HAL_RCC_FMAC_CLK_ENABLE();

    /* FMAC DMA Init */
    /* FMAC_READ Init */
    hdma_fmac_read.Instance = DMA1_Channel4;
    hdma_fmac_read.Init.Request = DMA_REQUEST_FMAC_READ;
    hdma_fmac_read.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_fmac_read.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_fmac_read.Init.MemInc = DMA_MINC_ENABLE;
    hdma_fmac_read.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_fmac_read.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_fmac_read.Init.Mode = DMA_NORMAL;
    hdma_fmac_read.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_fmac_read) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(fmacHandle,hdmaOut,hdma_fmac_read);

    /* FMAC_WRITE Init */
    hdma_fmac_write.Instance = DMA1_Channel5;
    hdma_fmac_write.Init.Request = DMA_REQUEST_FMAC_WRITE;
    hdma_fmac_write.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_fmac_write.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_fmac_write.Init.MemInc = DMA_MINC_ENABLE;
    hdma_fmac_write.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_fmac_write.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_fmac_write.Init.Mode = DMA_NORMAL;
    hdma_fmac_write.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_fmac_write) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(fmacHandle,hdmaIn,hdma_fmac_write);

    /* FMAC_PRELOAD Init */
    hdma_fmac_preload.Instance = DMA1_Channel6;
    hdma_fmac_preload.Init.Request = DMA_REQUEST_MEM2MEM;
    hdma_fmac_preload.Init.Direction = DMA_MEMORY_TO_MEMORY;
    hdma_fmac_preload.Init.PeriphInc = DMA_PINC_ENABLE;
    hdma_fmac_preload.Init.MemInc = DMA_MINC_DISABLE;
    hdma_fmac_preload.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_fmac_preload.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_fmac_preload.Init.Mode = DMA_NORMAL;
    hdma_fmac_preload.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_fmac_preload) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(fmacHandle,hdmaPreload,hdma_fmac_preload);

  /* USER CODE BEGIN FMAC_MspInit 1 */

  /* USER CODE END FMAC_MspInit 1 */
  }
}

void HAL_FMAC_MspDeInit(FMAC_HandleTypeDef* fmacHandle)
{

  if(fmacHandle->Instance==FMAC)
  {
  /* USER CODE BEGIN FMAC_MspDeInit 0 */

  /* USER CODE END FMAC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_FMAC_CLK_DISABLE();

    /* FMAC DMA DeInit */
    HAL_DMA_DeInit(fmacHandle->hdmaOut);
    HAL_DMA_DeInit(fmacHandle->hdmaIn);
    HAL_DMA_DeInit(fmacHandle->hdmaPreload);
  /* USER CODE BEGIN FMAC_MspDeInit 1 */

  /* USER CODE END FMAC_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
