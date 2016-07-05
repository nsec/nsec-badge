#include "stm32f0xx_hal.h"
#include "usb_device.h"

I2C_HandleTypeDef hi2c1;
IWDG_HandleTypeDef hiwdg;
SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;
TSC_HandleTypeDef htsc;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart1_tx;
WWDG_HandleTypeDef hwwdg;

TSC_IOConfigTypeDef tsc_ioconfig;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_IWDG_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI2_Init(void);
static void MX_TSC_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_WWDG_Init(void);
void MX_NVIC_Init(void);

static void Error_Handler(void) {
    while(1)
    { }
}

static uint8_t volatile tsc_iochannel = 1;

#define BUTTON_UP   0
#define BUTTON_DOWN 1
uint8_t tsc_iochannel_status[6] = {0};

void HAL_TSC_ConvCpltCallback(TSC_HandleTypeDef* p_htsc) {
    uint32_t acquisition_value = 0;

    if (HAL_TSC_GroupGetStatus(p_htsc, TSC_GROUP1_IDX) == TSC_GROUP_COMPLETED) {
        // Group 1 -
        acquisition_value = HAL_TSC_GroupGetValue(p_htsc, TSC_GROUP1_IDX);
         if (acquisition_value < 3) {
             // Button pressed
             if (tsc_iochannel_status[tsc_iochannel] == BUTTON_UP) {
                uint8_t spi_buffer[2] = {0};
                spi_buffer[0] = tsc_iochannel+1;
                spi_buffer[1] = 1; // button down
                HAL_SPI_Transmit(&hspi1, spi_buffer, 0x01, 0x100);
                tsc_iochannel_status[tsc_iochannel] = BUTTON_DOWN;
             }
         }
         else {
            // Button unpressed
            if (tsc_iochannel_status[tsc_iochannel] == BUTTON_DOWN) {
               uint8_t spi_buffer[2] = {0};
               spi_buffer[0] = tsc_iochannel+1;
               spi_buffer[1] = 2; // button down
               HAL_SPI_Transmit(&hspi1, spi_buffer, 0x01, 0x100);
               tsc_iochannel_status[tsc_iochannel] = BUTTON_UP;
            }
         }
    }

    if (HAL_TSC_GroupGetStatus(p_htsc, TSC_GROUP2_IDX) == TSC_GROUP_COMPLETED) {
        // Group 2 -
        acquisition_value = HAL_TSC_GroupGetValue(p_htsc, TSC_GROUP2_IDX);
        if (acquisition_value <= 1) {
            // Button pressed
            if (tsc_iochannel_status[tsc_iochannel+3] == BUTTON_UP) {
               uint8_t spi_buffer[2] = {0};
               spi_buffer[0] = tsc_iochannel+4;
               spi_buffer[1] = 1; // button down
               HAL_SPI_Transmit(&hspi1, spi_buffer, 0x01, 0x100);
               tsc_iochannel_status[tsc_iochannel+3] = BUTTON_DOWN;
            }
        }
        else {
           // Button unpressed
           if (tsc_iochannel_status[tsc_iochannel+3] == BUTTON_DOWN) {
              uint8_t spi_buffer[2] = {0};
              spi_buffer[0] = tsc_iochannel+4;
              spi_buffer[1] = 2; // button down
              HAL_SPI_Transmit(&hspi1, spi_buffer, 0x01, 0x100);
              tsc_iochannel_status[tsc_iochannel+3] = BUTTON_UP;
           }
        }
    }

    // Switch channel
    switch (tsc_iochannel) {
        case 0:
            tsc_ioconfig.ChannelIOs = TSC_GROUP1_IO2 | TSC_GROUP2_IO1;
            tsc_iochannel = 1;
            break;
        case 1:
            tsc_ioconfig.ChannelIOs = TSC_GROUP1_IO3 | TSC_GROUP2_IO2;
            tsc_iochannel = 2;
            break;
        case 2:
            tsc_ioconfig.ChannelIOs = TSC_GROUP1_IO4 | TSC_GROUP2_IO3;
            tsc_iochannel = 0;
            break;
    }

    // Configure the next buttons and call another conversion
    if (HAL_TSC_IOConfig(p_htsc, &tsc_ioconfig) != HAL_OK) {
        Error_Handler();
    }

    HAL_TSC_IODischarge(p_htsc, ENABLE);
    HAL_Delay(0.5);

    if (HAL_TSC_Start_IT(p_htsc) != HAL_OK) {
        Error_Handler();
    }
}

int main(void) {
  HAL_Init();

  SystemClock_Config();

  // Init peripherals
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART3_UART_Init();
  MX_USB_DEVICE_Init();
  MX_I2C1_Init();
  MX_IWDG_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_TSC_Init();
  //MX_USART1_UART_Init();
  MX_WWDG_Init();

  // Init interrupts
  MX_NVIC_Init();

  // Config touch IO
  // Group1:
  //    TSC_GROUP1_IO2 + TSC_GROUP1_IO3 + TSC_GROUP1_IO4
  // Group2:
  //    TSC_GROUP2_IO1 + TSC_GROUP2_IO2 + TSC_GROUP2_IO3
  tsc_ioconfig.ChannelIOs = TSC_GROUP1_IO2 | TSC_GROUP2_IO1;
  // Samples both group at the same time
  tsc_ioconfig.SamplingIOs = TSC_GROUP1_IO1|TSC_GROUP2_IO4;
  tsc_ioconfig.ShieldIOs = 0;
  if (HAL_TSC_IOConfig(&htsc, &tsc_ioconfig) != HAL_OK) {
    Error_Handler();
  }

  HAL_TSC_IODischarge(&htsc, ENABLE);
  HAL_Delay(0.5);

  if (HAL_TSC_Start_IT(&htsc) != HAL_OK) {
    Error_Handler();
  }

  while (1) {
  }
}

void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSI48
                              |RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI48;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB|RCC_PERIPHCLK_USART1
                              |RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

void MX_NVIC_Init(void) {
  HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USART1_IRQn);
}

void MX_I2C1_Init(void) {
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x2000090E;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  HAL_I2C_Init(&hi2c1);

  HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE);
}

void MX_IWDG_Init(void) {
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_4;
  hiwdg.Init.Window = 4095;
  hiwdg.Init.Reload = 4095;
  HAL_IWDG_Init(&hiwdg);

}

void MX_SPI1_Init(void) {
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_16BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_HARD_OUTPUT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;

  HAL_SPI_Init(&hspi1);
}

void MX_SPI2_Init(void) {
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_4BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 7;
  hspi2.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi2.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;

  HAL_SPI_Init(&hspi2);
}

void MX_TSC_Init(void) {
  htsc.Instance = TSC;
  htsc.Init.CTPulseHighLength = TSC_CTPH_16CYCLES;
  htsc.Init.CTPulseLowLength = TSC_CTPL_16CYCLES;
  htsc.Init.SpreadSpectrum = ENABLE;
  htsc.Init.SpreadSpectrumDeviation = 64;
  htsc.Init.SpreadSpectrumPrescaler = TSC_SS_PRESC_DIV1;
  htsc.Init.PulseGeneratorPrescaler = TSC_PG_PRESC_DIV64;
  htsc.Init.MaxCountValue = TSC_MCV_255;
  htsc.Init.IODefaultMode = TSC_IODEF_OUT_PP_LOW;
  htsc.Init.SynchroPinPolarity = TSC_SYNC_POLARITY_FALLING;
  htsc.Init.AcquisitionMode = TSC_ACQ_MODE_NORMAL;
  htsc.Init.MaxCountInterrupt = DISABLE;
  htsc.Init.ChannelIOs = TSC_GROUP1_IO2|TSC_GROUP1_IO3|TSC_GROUP1_IO4|TSC_GROUP2_IO1
                    |TSC_GROUP2_IO2|TSC_GROUP2_IO3;
  htsc.Init.SamplingIOs = TSC_GROUP1_IO1|TSC_GROUP2_IO4;

  HAL_TSC_Init(&htsc);
}

void MX_USART1_UART_Init(void) {
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

  HAL_UART_Init(&huart1);
}

void MX_USART3_UART_Init(void) {
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 38400;
  huart3.Init.WordLength = UART_WORDLENGTH_7B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

  HAL_UART_Init(&huart3);
}

void MX_WWDG_Init(void) {
  hwwdg.Instance = WWDG;
  hwwdg.Init.Prescaler = WWDG_PRESCALER_1;
  hwwdg.Init.Window = 64;
  hwwdg.Init.Counter = 64;

  HAL_WWDG_Init(&hwwdg);
}

void MX_DMA_Init(void) {
  __HAL_RCC_DMA1_CLK_ENABLE();

  HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
}

void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct;

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
}

