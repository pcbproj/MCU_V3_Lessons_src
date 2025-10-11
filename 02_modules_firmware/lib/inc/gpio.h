#ifndef GPIO_H
#define GPIO_H

#include "stm32f407xx.h"

// LED1 = PE13

#define LED_ON		GPIOE->BSRR |= GPIO_BSRR_BR13
#define LED_OFF		GPIOE->BSRR |= GPIO_BSRR_BS13


void GPIO_Init(void);

void LED_Toggle(void);

uint8_t LED_Check(void);


#endif

