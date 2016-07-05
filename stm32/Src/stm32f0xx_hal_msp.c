#include "stm32f0xx_hal.h"

extern DMA_HandleTypeDef hdma_usart1_tx;

void HAL_MspInit(void) {
  __HAL_RCC_SYSCFG_CLK_ENABLE();

  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c) {
  GPIO_InitTypeDef GPIO_InitStruct;
  if (hi2c->Instance == I2C1) {
    // PB8 -> I2C1_SCL
    // PB9 -> I2C1_SDA
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    __HAL_RCC_I2C1_CLK_ENABLE();
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c) {
  if (hi2c->Instance == I2C1) {
    __HAL_RCC_I2C1_CLK_DISABLE();
    // PB8 -> I2C1_SCL
    // PB9 -> I2C1_SDA
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8|GPIO_PIN_9);
  }
}

void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi) {
  // PA15 -> SPI1_NSS
  // PB3 -> SPI1_SCK
  // PB4 -> SPI1_MISO
  // PB5 -> SPI1_MOSI

  GPIO_InitTypeDef GPIO_InitStruct;
  if(hspi->Instance == SPI1) {
    __HAL_RCC_SPI1_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF0_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF0_SPI1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  }
  else if(hspi->Instance == SPI2) {
    __HAL_RCC_SPI2_CLK_ENABLE();

    // PB12 -> SPI2_NSS
    // PB13 -> SPI2_SCK
    // PB14 -> SPI2_MISO
    // PB15 -> SPI2_MOSI

    GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF0_SPI2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi) {
  if(hspi->Instance == SPI1) {
    __HAL_RCC_SPI1_CLK_DISABLE();

    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_15);
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5);
  }
  else if(hspi->Instance == SPI2) {
    __HAL_RCC_SPI2_CLK_DISABLE();

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);
  }
}

void HAL_TSC_MspInit(TSC_HandleTypeDef* htsc) {
  GPIO_InitTypeDef GPIO_InitStruct;
  if(htsc->Instance == TSC) {
    __HAL_RCC_TSC_CLK_ENABLE();

    // PA0 -> TSC_G1_IO1
    // PA1 -> TSC_G1_IO2
    // PA2 -> TSC_G1_IO3
    // PA3 -> TSC_G1_IO4
    // PA4 -> TSC_G2_IO1
    // PA5 -> TSC_G2_IO2
    // PA6 -> TSC_G2_IO3
    // PA7 -> TSC_G2_IO4

    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF3_TSC;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4
                          |GPIO_PIN_5|GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF3_TSC;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(TSC_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TSC_IRQn);
  }
}

void HAL_TSC_MspDeInit(TSC_HandleTypeDef* htsc) {
  if(htsc->Instance==TSC) {
    __HAL_RCC_TSC_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);
    HAL_NVIC_DisableIRQ(TSC_IRQn);
  }
}

void HAL_UART_MspInit(UART_HandleTypeDef* huart) {
  GPIO_InitTypeDef GPIO_InitStruct;
  if(huart->Instance == USART1) {
    __HAL_RCC_USART1_CLK_ENABLE();

    // PB6 -> USART1_TX
    // PB7 -> USART1_RX
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF0_USART1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    hdma_usart1_tx.Instance = DMA1_Channel2;
    hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_tx.Init.Mode = DMA_NORMAL;
    hdma_usart1_tx.Init.Priority = DMA_PRIORITY_LOW;
    HAL_DMA_Init(&hdma_usart1_tx);

    __HAL_LINKDMA(huart,hdmatx,hdma_usart1_tx);

    __HAL_RCC_TIM3_CLK_ENABLE();
    HAL_NVIC_SetPriority(TIM3_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);
  }
  else if(huart->Instance == USART3) {
    __HAL_RCC_USART3_CLK_ENABLE();

    // PB10 -> USART3_TX
    // PB11 -> USART3_RX
    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_USART3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart) {
  if(huart->Instance == USART1) {
    __HAL_RCC_USART1_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6|GPIO_PIN_7);
    HAL_DMA_DeInit(huart->hdmatx);
    HAL_NVIC_DisableIRQ(USART1_IRQn);
    __HAL_RCC_USART3_FORCE_RESET();
    __HAL_RCC_USART3_RELEASE_RESET();
  }
  else if(huart->Instance == USART3) {
    __HAL_RCC_USART3_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10|GPIO_PIN_11);
  }
}

void HAL_WWDG_MspInit(WWDG_HandleTypeDef* hwwdg) {
  if(hwwdg->Instance==WWDG) {
    __HAL_RCC_WWDG_CLK_ENABLE();
  }
}

void HAL_WWDG_MspDeInit(WWDG_HandleTypeDef* hwwdg) {
  if(hwwdg->Instance==WWDG) {
    __HAL_RCC_WWDG_CLK_DISABLE();
  }
}

