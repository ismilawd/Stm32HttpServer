#include "main.h"
#include "stm32f4xx_it.h"
#include <Debug.h>
volatile uint8_t FatFsCnt = 0;
volatile uint8_t Timer1, Timer2;

void SDTimer_Handler(void) {
	if (Timer1 > 0)
		Timer1--;

	if (Timer2 > 0)
		Timer2--;
}
extern void Uart_isr (UART_HandleTypeDef *huart);
extern UART_HandleTypeDef huart1;

void NMI_Handler(void)
{

}


void HardFault_Handler(void)
{
	//Debug_Clear();
	//Debug_WriteLine("Err HardFault");
  while (1)
  {
  }
}

void MemManage_Handler(void)
{
  while (1)
  {
  }
}

void BusFault_Handler(void)
{
  while (1)
  {
  }
}

void UsageFault_Handler(void)
{
  while (1)
  {
  }
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}
void SysTick_Handler(void)
{
	FatFsCnt++;
	if (FatFsCnt >= 10) {
		FatFsCnt = 0;
		SDTimer_Handler();
	}
  HAL_IncTick();
}
void USART1_IRQHandler(void)
{
	Uart_isr(&huart1);
  //HAL_UART_IRQHandler(&huart1);
}
