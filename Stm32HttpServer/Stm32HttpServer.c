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

static void ThreadMain(void const *argument);

int main(void)
{
	HAL_Init();  
	
	__GPIOC_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.Pin = GPIO_PIN_1 | GPIO_PIN_2;

	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

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