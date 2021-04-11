#include "main.h"
#include "fatfs.h"
#include <fatfs_sd.h>
#include <lcd_i2cModule.h>
#include <stdio.h>
#include <Debug.h>
#include "ESP_DATA_HANDLER.h"
#include "UartRingbuffer.h"
#define MAX_BUFFER_SIZE 30720
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart1;
void SystemClock_Config(void);
static void GPIO_Init(void);
static void I2C1_Init(void);
static void SPI1_Init(void);
static void USART1_UART_Init(void);
FATFS fs;
FIL fil;
FRESULT fresult;
char fileBuffer[MAX_BUFFER_SIZE] = { 0 };

int bufSize(char *buf) {
	int i = 0;
	while (*buf++ != '\0')
		i++;
	return i;
}

void bufClear() {
	for (int i = 0; i < MAX_BUFFER_SIZE; i++) {
		fileBuffer[i] = '\0';
	}
}
int main(void) {
	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();
	/* Configure the system clock */
	SystemClock_Config();
	/* Initialize all configured peripherals */
	GPIO_Init();
	I2C1_Init();
	SPI1_Init();
	FATFS_Init();
	USART1_UART_Init();
	/* Initialize LCD module*/
	LCD_i2cDeviceCheck();
	LCD_Init();
	HAL_Delay(500);
	LCD_BackLight(LCD_BL_ON);
	HAL_Delay(500);

	Debug_WriteLine("Http Server");
	Debug_WriteLine("By @ismilawd");
	HAL_Delay(2000);
	Debug_Clear();
	Debug_WriteLine("Device Ready");
	HAL_Delay(1000);
	Debug_WriteLine("Try Mount SD");
	/* Mount SDCard */
	fresult = f_mount(&fs, "", 0);
	if (fresult != FR_OK) {
		Debug_WriteLine("Mounting SD Failed");
	} else {
		Debug_WriteLine("SD Mounted");
		HAL_Delay(1000);
		Debug_Clear();
	}
	fresult = f_open(&fil, "config.txt", FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
	if (fresult != FR_OK) {
		Debug_Clear();
		Debug_WriteLine("File Conf ERR");
	} else {
		f_gets(fileBuffer, MAX_BUFFER_SIZE, &fil);
		fresult = f_close(&fil);
		if (fresult != FR_OK) {
			Debug_Clear();
			Debug_WriteLine("Read Conf ERR");
		} else {
			int line = 0;
			int cs = 0;
			char ssid[10] = { 0 };
			char pass[10] = { 0 };
			char ipad[16] = { 0 };
			for (int i = 0; i < MAX_BUFFER_SIZE; i++) {
				char value = fileBuffer[i];
				if (value == 0 || value == '\0')
					break;
				if (value == ',') {
					line++;
					cs = 0;
					continue;
				}
				if (line == 0)
					ssid[cs] = value;
				if (line == 1)
					pass[cs] = value;
				if (line == 2)
					ipad[cs] = value;
				cs++;

			}
			Debug_Clear();
			Debug_WriteLine("WIFI SSID:");
			Debug_WriteLine(ssid);
			HAL_Delay(1000);

			Debug_Clear();
			Debug_WriteLine("WIFI PASS:");
			Debug_WriteLine(pass);
			HAL_Delay(1000);

			Debug_Clear();
			Debug_WriteLine("WIFI IP ADDR:");
			Debug_WriteLine(ipad);
			HAL_Delay(1000);

			Debug_WriteLine("ESP Init");
			ESP_Init(ssid, pass, ipad);
			Debug_Clear();
			Debug_WriteLine("WIFI Connected");
			Debug_WriteLine("Server Up");
			HAL_Delay(1000);
			Debug_Clear();
		}
	}
	/* Infinite loop */
	while (1) {
		Server_Start();
	}
}

void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 25;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
		Error_Handler();
	}
}

static void I2C1_Init(void) {
	hi2c1.Instance = I2C1;
	hi2c1.Init.ClockSpeed = 100000;
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
		Error_Handler();
	}
}

static void SPI1_Init(void) {
	/* SPI1 parameter configuration*/
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 10;
	if (HAL_SPI_Init(&hspi1) != HAL_OK) {
		Error_Handler();
	}
}

static void USART1_UART_Init(void) {
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart1) != HAL_OK) {
		Error_Handler();
	}
}

static void GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);

	/*Configure GPIO pin : PC4 */
	GPIO_InitStruct.Pin = GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

void Error_Handler(void) {
}

