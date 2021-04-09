/**
 ******************************************************************************

 File:		ESP DATA HANDLER
 Author:   ControllersTech
 Updated:  3rd Aug 2020

 ******************************************************************************
 Copyright (C) 2017 ControllersTech.com

 This is a free software under the GNU license, you can redistribute it and/or modify it under the terms
 of the GNU General Public License version 3 as published by the Free Software Foundation.
 This software library is shared with public for educational purposes, without WARRANTY and Author is not liable for any damages caused directly
 or indirectly by this software, read more about this on the GNU General Public License.

 ******************************************************************************
 */

#include "ESP_DATA_HANDLER.h"
#include "UartRingbuffer.h"
#include "stdio.h"
#include "string.h"
#include <fatfs_sd.h>
#include "fatfs.h"
#include <Debug.h>

#define MAX_BUFFER_SIZE 30720

extern UART_HandleTypeDef huart1;
extern FATFS fs;
extern FIL fil;
extern FRESULT fresult;
extern char fileBuffer[MAX_BUFFER_SIZE];

#define wifi_uart &huart1

char buffer[20];
/*****************************************************************************************************************************************/

void ESP_Init(char *SSID, char *PASSWD, char *STAIP) {
	char data[80];

	Debug_WriteLine("Ringbuf Init");
	Ringbuf_init();
	Debug_WriteLine("Ringbuf OK");

	Debug_Clear();
	Debug_WriteLine("RESET Wifi");
	Uart_sendstring("AT+RST\r\n");
	HAL_Delay(2000);
	Debug_WriteLine("RESET OK");

	HAL_Delay(200);
	/********* AT **********/
	Debug_WriteLine("TEST Wifi");
	Uart_flush();
	Uart_sendstring("AT\r\n");
	while (!(Wait_for("OK\r\n")))
		;
	Debug_WriteLine("TEST OK");
	HAL_Delay(200);

	/********* AT+CWMODE=1 **********/
	Debug_WriteLine("WORKSTATION MODE");
	Uart_flush();
	Uart_sendstring("AT+CWMODE=1\r\n");
	while (!(Wait_for("OK\r\n")))
		;
	Debug_WriteLine("MODE OK");
	HAL_Delay(200);

	/* Set Static IP Address */
	/********* AT+CWSTAIP=IPADDRESS **********/
	Debug_WriteLine("SET IP");
	Uart_flush();
	sprintf(data, "AT+CIPSTA=\"%s\"\r\n", STAIP);
	Uart_sendstring(data);
	while (!(Wait_for("OK\r\n")))
		;
	Debug_WriteLine("IP OK");
	HAL_Delay(200);

	/********* AT+CWJAP="SSID","PASSWD" **********/
	Debug_WriteLine("CONNECT");
	Uart_flush();
	sprintf(data, "AT+CWJAP=\"%s\",\"%s\"\r\n", SSID, PASSWD);
	Uart_sendstring(data);
	while (!(Wait_for("OK\r\n")))
		;
	Debug_WriteLine("CONNECT OK");
	HAL_Delay(200);

	/********* AT+CIPMUX **********/
	Debug_WriteLine("CIPMUX");
	Uart_flush();
	Uart_sendstring("AT+CIPMUX=1\r\n");
	while (!(Wait_for("OK\r\n")))
		;
	Debug_WriteLine("CIPMUX OK");
	HAL_Delay(200);

	/********* AT+CIPSERVER **********/
	Debug_WriteLine("SERVER");
	Uart_flush();
	Uart_sendstring("AT+CIPSERVER=1,80\r\n");
	while (!(Wait_for("OK\r\n")))
		;
	Debug_WriteLine("SERVER OK");
	HAL_Delay(200);

}

int Server_Send(char *str, int Link_ID) {
	int len = strlen(str);
	char data[80];
	Uart_flush();
	sprintf(data, "AT+CIPSEND=%d,%d\r\n", Link_ID, len);
	Uart_sendstring(data);
	while (!(Wait_for(">")))
		;
	Uart_sendstring(str);
	while (!(Wait_for("SEND OK")))
		;
	Uart_flush();
	sprintf(data, "AT+CIPCLOSE=%d\r\n", Link_ID);
	Uart_sendstring(data);
	while (!(Wait_for("OK\r\n")))
		;
	return 1;
}

void Server_Handle(char *str, int Link_ID) {
	memset(&fileBuffer, 0, sizeof fileBuffer);
	fresult = f_open(&fil, str, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
	f_gets(fileBuffer, MAX_BUFFER_SIZE, &fil);
	fresult = f_close(&fil);
	Server_Send(fileBuffer, Link_ID);

}

void Server_Start(void) {
	char buftostoreheader[128] = { 0 };
	char Link_ID;
	while (!(Get_after("+IPD,", 1, &Link_ID)))
		;
	Link_ID -= 48;
	Debug_Clear();
	Debug_WriteLine("Linked");
	Debug_WriteLine(&Link_ID);
	while (!(Copy_upto(" HTTP/1.1", buftostoreheader)))
		;
	if (Look_for("/config ", buftostoreheader) == 1) {
		Debug_Clear();
		Debug_WriteLine("Request");
		Debug_WriteLine("/config");

		Server_Handle("403.html", Link_ID);

		Debug_WriteLine("Responsed");
	}if (Look_for("/index ", buftostoreheader) == 1) {
		Debug_Clear();
		Debug_WriteLine("Request");
		Debug_WriteLine("/config");

		Server_Handle("index.html", Link_ID);

		Debug_WriteLine("Responsed");
	} else {
		Debug_Clear();
		Debug_WriteLine("Request");
		Debug_WriteLine("Not Found");

		Server_Handle("404.html", Link_ID);

		Debug_WriteLine("Responsed");
	}
}

