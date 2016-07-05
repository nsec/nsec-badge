#include "stm32f0xx_hal.h"
#include "stm32f0xx.h"
#include "stm32f0xx_it.h"

extern PCD_HandleTypeDef hpcd_USB_FS;
extern TSC_HandleTypeDef htsc;
extern DMA_HandleTypeDef hdma_usart1_tx;
//extern UART_HandleTypeDef huart1;

void NMI_Handler(void) {
}

void HardFault_Handler(void) {
  while (1) {
  }
}

void SysTick_Handler(void) {
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
}

void TSC_IRQHandler(void) {
  HAL_TSC_IRQHandler(&htsc);
}

void DMA1_Channel2_3_IRQHandler(void) {
  HAL_DMA_IRQHandler(&hdma_usart1_tx);
}

//void USART1_IRQHandler(void) {
//  HAL_UART_IRQHandler(&huart1);
//}

void USB_IRQHandler(void) {
  HAL_PCD_IRQHandler(&hpcd_USB_FS);
}

