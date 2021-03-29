/**
  ******************************************************************************
  * @file    FreeRTOS/FreeRTOS_ThreadCreation/Src/main.c
  * @author  ismilawd
  * @version V1.0
  * @date    29-March-2021
  * @brief   Main program body
  */

#include <stm32f4xx_hal.h>
#include <../CMSIS_RTOS/cmsis_os.h>

osThreadId hThreadMain;
I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart6;

static void ThreadMain(void const *argument);
static void GPIO_Init();
static void RccClock_Config();
static void DMA_Init(void);
static void I2C1_Init(void);
static void USART6_UART_Init(void);


int main(void)
{
	HAL_Init();  
	RccClock_Config();
	GPIO_Init();
	DMA_Init();
	I2C1_Init();
	USART6_UART_Init();

	osThreadDef(thMain, ThreadMain, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
  
	hThreadMain = osThreadCreate(osThread(thMain), NULL);
  
	osKernelStart();

	for (;;) ;
}

void SysTick_Handler(void)
{
	HAL_IncTick();
	osSystickHandler();
}

static void ThreadMain(void const *argument)
{
	(void) argument;
  
	for (;;)
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
		osDelay(2000);
		
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
		osDelay(2000);
	}
}

#pragma region Configs

static void GPIO_Init()
{
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	
	//sd card insert pin
	{
		GPIO_InitTypeDef GPIO_InitStruct = { 0 };
		GPIO_InitStruct.Pin = GPIO_PIN_2;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	}
	//esp program mode pin
	{
		GPIO_InitTypeDef GPIO_InitStruct = { 0 };
		GPIO_InitStruct.Pin = GPIO_PIN_2;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	}
}
static void RccClock_Config()
{
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                              | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
	{
		//Error_Handler();
	}
}
static void DMA_Init(void)
{
	__HAL_RCC_DMA2_CLK_ENABLE();

	HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
	
	HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);

}
static void I2C1_Init(void)
{
	hi2c1.Instance = I2C1;
	hi2c1.Init.ClockSpeed = 100000;
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c1) != HAL_OK)
	{
		//Error_Handler();
	}

}
static void USART6_UART_Init(void)
{
	huart6.Instance = USART6;
	huart6.Init.BaudRate = 115200;
	huart6.Init.WordLength = UART_WORDLENGTH_8B;
	huart6.Init.StopBits = UART_STOPBITS_1;
	huart6.Init.Parity = UART_PARITY_NONE;
	huart6.Init.Mode = UART_MODE_TX_RX;
	huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart6.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart6) != HAL_OK)
	{
		//Error_Handler();
	}
}
#pragma endregion