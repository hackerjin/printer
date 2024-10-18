#include"main.h"

void GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};


  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED_Pin|STB1_Pin|STB2_Pin|STB3_Pin
                          |STB4_Pin|STB5_Pin|STB6_Pin|MOTOR4_Pin
                          |MOTOR3_Pin|MOTOR2_Pin|MOTOR1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LAT_GPIO_Port, LAT_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(VH_EN_GPIO_Port, VH_EN_Pin, GPIO_PIN_RESET);

  //按键接上拉电阻，默认是高电平，按下则为低电平
  GPIO_InitStruct.Pin = BTN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(BTN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_Pin LAT_Pin */
  GPIO_InitStruct.Pin = LED_Pin|LAT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);



  /*Configure GPIO pins : STB1_Pin STB2_Pin STB3_Pin STB4_Pin
                           STB5_Pin STB6_Pin MOTOR4_Pin MOTOR3_Pin
                           MOTOR2_Pin MOTOR1_Pin */
  GPIO_InitStruct.Pin = STB1_Pin|STB2_Pin|STB3_Pin|STB4_Pin
                          |STB5_Pin|STB6_Pin|MOTOR4_Pin|MOTOR3_Pin
                          |MOTOR2_Pin|MOTOR1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  //缺纸检测端口
  GPIO_InitStruct.Pin = VHINT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(VHINT_GPIO_Port, &GPIO_InitStruct);



  /*Configure GPIO pin : VH_EN_Pin */
  GPIO_InitStruct.Pin = VH_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(VH_EN_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);


}



