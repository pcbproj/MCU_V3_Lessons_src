#ifndef USART_H
#define USART_H

#include "stm32f407xx.h"

#define USART_OK		0
#define USART_ERR		1

void USART6_Init(void);
void usart6_send(uint8_t data[], uint8_t len);
uint8_t usart6_receive_byte(uint8_t *rx_byte);



#endif