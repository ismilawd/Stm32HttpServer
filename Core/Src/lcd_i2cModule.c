/*Includes---------------------------------------------------------*/
#include "lcd_i2cModule.h"
#include "lcd_userConf.h"
#include <string.h>
#include <stdio.h>
#ifdef LCD_16x2
#define line_MAX 2
#define chr_MAX 16

static const uint8_t Cursor_Data[line_MAX][chr_MAX] = {
  {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F}, //1. line DDRAM address
  {0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F}, //2. line DDRAM address
};
#endif

static uint8_t Mask_Data = 0xf0; //Select upper bits.
static uint8_t data[4], data_M, data_L, data_BL;
static uint8_t line_pos = 1; //hold line position, default is 1. line.
static uint8_t str_len = 0; //follow the string lenght.

void LCD_i2cDeviceCheck(void)
{
	/* Checks if target device is ready for communication. */
	/* 3 is number of trials, 1000ms is timeout */
	HAL_Delay(50);
	hi2cx_define();
	while (HAL_I2C_IsDeviceReady(&hi2cx, i2cDeviceAddr, 3, 1000) != HAL_OK) 
	{	
		
	}
}

void LCD_Set_Command(uint8_t cmd)
{
	data_M = cmd & Mask_Data;        //Most significant bit
	data_L = (cmd << 4) & Mask_Data; //Least significant bit
	
	//For backlight On/off
	data_M |= data_BL;
	data_L |= data_BL;
	
	data[0] = data_M | LCD_E;  //Enable E pin, RS=0
	data[1] = data_M;          //Disable E pin, RS=0
	data[2] = data_L | LCD_E;
  data[3] = data_L;
	
	hi2cx_define();	
	HAL_I2C_Master_Transmit(&hi2cx, i2cDeviceAddr, (uint8_t*)data, 4, 200);
}

void LCD_Write_Data(uint8_t datax)
{
	data_M = datax & Mask_Data;        //Most significant bit
	data_L = (datax << 4) & Mask_Data; //Least significant bit
	
	//For backlight On/off
	data_M |= data_BL;
	data_L |= data_BL;
	 	
	data[0] = data_M | LCD_E|LCD_RS;  //Enable E pin, RS=1
	data[1] = data_M | LCD_RS;        //Disable E pin, RS=1
	data[2] = data_L | LCD_E|LCD_RS;
  data[3] = data_L | LCD_RS;  
	
	hi2cx_define();
	HAL_I2C_Master_Transmit(&hi2cx, i2cDeviceAddr, (uint8_t*)data, 4, 200);
}

void LCD_Clear(void)
{
	LCD_Set_Command(LCD_CLEAR_DISPLAY);
	HAL_Delay(10);
	str_len = 0;
	line_pos = 1;
}

void LCD_SetCursor(int line_x, int chr_x)
{
  line_pos = line_x; //hold line position.	
	
	if(((line_x >=1 && line_x <= line_MAX) && (chr_x >=1 && chr_x <= chr_MAX)))
	{		
		LCD_Set_Command(LCD_SET_DDRAMADDR | Cursor_Data[line_x - 1][chr_x - 1]);		
	}
}

void LCD_Send_String(char str[], uint8_t mode)
{	 
	static char *buffer[BFR_MAX];
	static uint8_t i[4] = {chr_MAX,chr_MAX,chr_MAX,chr_MAX}; //i follows the ch position while sliding.
  static uint8_t c[4] = {0, 0, 0, 0}; //c follows the each ch of the str buffer while sliding.
  static uint8_t ch_len = 0; //follow the string lenght.
  str_len = 0;
	

	switch(mode)
	{			
		case STR_NOSLIDE:
			
			while (*str) 
			{
				LCD_Write_Data (*str++);
				str_len++;
				if(str_len == chr_MAX)
				{
					LCD_SetCursor(line_pos + 1, 1);
					str_len = 0;
				}			
			}	
			
			break;
		
		case STR_SLIDE:
		
		  for(int a = 0; a < BFR_MAX; a++)
		  buffer[a]=str++;
						
			ch_len = strlen(*buffer);
				 		
			LCD_SetCursor(line_pos, i[line_pos - 1]);	
				  			
			for(int k = c[line_pos - 1];k < ch_len; k++) 
			LCD_Write_Data (*buffer[k]);
								
			i[line_pos - 1]--;
      
			if(i[line_pos -1] == 0)
			{
				i[line_pos - 1] = 1;
				c[line_pos - 1]++;
        if(c[line_pos - 1] == ch_len)
					{					
						i[line_pos - 1] = chr_MAX;
						c[line_pos - 1] = 0;
						ch_len = 0;						
					}												
			}
		 			
			break;	 
	}	
}

void LCD_Print(char const *ch, float value)
{
	char data_ch[BFR_MAX]; //default data size:100.
	
	sprintf(data_ch, ch, value);
	LCD_Send_String(data_ch, STR_NOSLIDE);	
}

void LCD_BackLight(uint8_t light_state)
{
	if(light_state == LCD_BL_ON)
	{
    data_BL = LCD_BL_ON;		
		LCD_Write_Data(0x20); //Empty character
	}
	else if (light_state == LCD_BL_OFF)
	{
		data_BL = LCD_BL_OFF;
		LCD_Write_Data(0x20);
	}
}

void LCD_Init(void)
{
	LCD_Set_Command(LCD_CLEAR_DISPLAY);
	HAL_Delay(1000);
	LCD_Set_Command(LCD_RETURN_HOME);
	HAL_Delay(5);
	LCD_Set_Command(LCD_FUNCTION_SET|MODE_4B|MODE_2L|MODE_5X8_DOTS);
	HAL_Delay(5);
	LCD_Set_Command(LCD_DISPLAY_CONTROL|DISPLAY_ON|CURSOR_OFF|BLINK_OFF);
	HAL_Delay(5);
	LCD_Set_Command(LCD_SET_DDRAMADDR);
	HAL_Delay(500);
}
