/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*                        The Embedded Experts                        *
**********************************************************************

-------------------------- END-OF-HEADER -----------------------------

File    : main.c
Purpose : Generic application start

*/

#include <stdio.h>
#include <stdlib.h>
#include "stm32f4xx.h"

/*********************************************************************
*
*       main()
*
*  Function description
*   Application entry point.

 */

void RCC_Init(void);


int main(void) {
	uint16_t i = 0;
	
	SystemInit();
	RCC_Init();
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;	// включение тактирования SYSCFG
	SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI10_PE;	// порт Е на линии EXTI10

    __enable_irq();							// Глобальное разрешение прерываний
    EXTI->PR |= EXTI_PR_PR0;				// Сброс запроса прерываний
    EXTI->FTSR |= EXTI_FTSR_TR0;			// Срабатывание прерывания по заднему фронту
    EXTI->IMR |= EXTI_IMR_IM0;				// Вкл. прерывание по входу 10
    NVIC_EnableIRQ(EXTI15_10_IRQn);			// разрешение прерывнаия в контроллере NVIC
	
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN | RCC_AHB1ENR_GPIOAEN;	// Включение тактирования порта Е, А
	
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;	// Включение тактирования USART1

	/* 84 MHz / 19200 / 16 = 273,44 // M = 273(0x111) // F=0,44*16=7(0x7)  */
	USART1->BRR = 0x1117;	// Baudrate = 19200 
	USART1->CR1 |= USART_CR1_RXNEIE | USART_CR1_TE | USART_CR1_RE;	// прерывание по приему 
																	// Вкл. передатчик и приемник
	USART1->CR1 &= ~(USART_CR1_M) | ~(USART_CR1_PCE);	// 8-бит, без контроля четности
	USART1->CR2 &= ~(USART_CR2_STOP);		// 1 стоповый бит
	NVIC_EnableIRQ(USART1_IRQn);			// Разрешение преывания в NVIC
	USART1->CR1 |= USART_CR1_UE;				// Включение USART1
	
	GPIOA->MODER |= GPIO_MODER_MODE9_1 | GPIO_MODER_MODE10_1;	// работа A9, A10 на альт. функцию
	GPIOA->AFR[1] |= (7<<GPIO_AFRH_AFSEL9_Pos) | (7<<GPIO_AFRH_AFSEL10_Pos);	// AF7 для A9, A10



	GPIOE->MODER |= GPIO_MODER_MODE13_0;	// Работа E13 на выход (LED1)
	GPIOE->MODER |= GPIO_MODER_MODE10;		// Работа E10 на вход (S1)

  
	while (1){
		
	}
}




void EXTI15_10_IRQHandler(void){
    
	USART1->DR = 1;	// отправка USART

    EXTI->PR |= EXTI_PR_PR10;	// сброс запроса прерывания 

}

/*************************** End of file ****************************/


void USART1_IRQHandler(void){
	
	if( (USART1->SR & USART_SR_RXNE) != 0 ){
		
		uint16_t RXc = ( (uint16_t)USART1->DR & (uint16_t)0x01FF); // считывем значение регистра DR

		switch(RXc){
			case '0':
				GPIOE->BSRR |= GPIO_BSRR_BS13;	// выключение светодиода PE13
				break;
			case '1':
				GPIOE->BSRR |= GPIO_BSRR_BR13;	// включение светодиода PE13
				break;
			default:
				break;
		}
	}
	
	NVIC_ClearPendingIRQ(USART1_IRQn);

}