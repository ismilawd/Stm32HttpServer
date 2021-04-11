#ifndef UARTRINGBUFFER_H_
#define UARTRINGBUFFER_H_

#include "stm32f4xx_hal.h"

#define UART_BUFFER_SIZE 30720

typedef struct
{
  unsigned char buffer[UART_BUFFER_SIZE];
  volatile unsigned int head;
  volatile unsigned int tail;
} ring_buffer;


void Ringbuf_init(void);

int Uart_read(void);

void Uart_write(int c);

void Uart_sendstring(const char *s);

void Uart_printbase (long n, uint8_t base);

int IsDataAvailable(void);

int Look_for (char *str, char *buffertolookinto);

void GetDataFromBuffer (char *startString, char *endString, char *buffertocopyfrom, char *buffertocopyinto);

void Uart_flush (void);

int Uart_peek();

int Copy_upto (char *string, char *buffertocopyinto);

int Get_after (char *string, uint8_t numberofchars, char *buffertosave);

int Wait_for (char *string);

void Uart_isr (UART_HandleTypeDef *huart);


#endif /* UARTRINGBUFFER_H_ */
