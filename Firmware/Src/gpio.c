
  //******************************************************************************
  //* File Name          : gpio.c
  //* Description        : GPIO initialization
  //* Created			 : Jan Koprivec
  //* Date				 : 13.7.2017
  //******************************************************************************
#include "stm32f3xx_hal.h"
#include "gpio.h"
#include "defines.h"

void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, IND_LED_Pin|TEST_PAD1_Pin|TEST_PAD2_Pin|TEST_PAD3_Pin 
                          |TEST_PAD4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, IND_PE_Pin|IND_1P_Pin|IND_3P_Pin|RELAY_RESET_Pin|RELAY_CS3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, TRIM_CS1_Pin|TRIM_CS2_Pin|TRIM_CS3_Pin|TRIM_UD_Pin 
                          |USB_PU_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, RELAY_CS1_Pin|RELAY_CS2_Pin|RELAY_CS4_Pin|CURRENT_GAIN1_Pin|CURRENT_GAIN2_Pin|CURRENT_GAIN3_Pin
                          |RELAY_CS5_Pin, GPIO_PIN_RESET);
	

  /*Configure GPIO pins : IND_LED_Pin TEST_PAD1_Pin TEST_PAD2_Pin TEST_PAD3_Pin 
                           TEST_PAD4_Pin */
  GPIO_InitStruct.Pin = IND_LED_Pin|TEST_PAD1_Pin|TEST_PAD2_Pin|TEST_PAD3_Pin 
                          |TEST_PAD4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : IND_PE_Pin IND_1P_Pin IND_3P_Pin */
  GPIO_InitStruct.Pin = IND_PE_Pin|IND_1P_Pin|IND_3P_Pin|RELAY_RESET_Pin|RELAY_CS3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : TRIM_CS1_Pin TRIM_CS2_Pin TRIM_CS3_Pin TRIM_UD_Pin 
                           CURRENT_GAIN3_Pin CURRENT_GAIN2_Pin USB_PU_Pin */
  GPIO_InitStruct.Pin = TRIM_CS1_Pin|TRIM_CS2_Pin|TRIM_CS3_Pin|TRIM_UD_Pin|USB_PU_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_VBUS_Pin */
  GPIO_InitStruct.Pin = USB_VBUS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_VBUS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : RS485_DET_Pin */
  GPIO_InitStruct.Pin = RS485_DET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(RS485_DET_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : RELAY_CS1_Pin RELAY_CS2_Pin RELAY_CS3_Pin RELAY_CS4_Pin 
                           RELAY_CS5_Pin */
  GPIO_InitStruct.Pin = RELAY_CS1_Pin|RELAY_CS2_Pin|RELAY_CS4_Pin 
                          |RELAY_CS5_Pin|CURRENT_GAIN1_Pin|CURRENT_GAIN3_Pin
													|CURRENT_GAIN2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	
	  /*Configure GPIO pin : PA6 */
  GPIO_InitStruct.Pin = L1_SINHRO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(L1_SINHRO_GPIO_Port, &GPIO_InitStruct);
	

}


