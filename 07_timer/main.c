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

#define LED1_ON  GPIOE->BSRR |= GPIO_BSRR_BR13;
#define LED1_OFF GPIOE->BSRR |= GPIO_BSRR_BS13;

/*********************************************************************
*
*       main()
*
*  Function description
*   Application entry point.

 */


void RCC_Init(void);
void TIM1_Init(void);

uint32_t time_ms = 0;		// прошедшее время
uint32_t delay_500ms = 0;	// задержка на 500 мс
uint32_t delay_10ms = 0;	// задержка на 10 мс


int main(void) {
	uint16_t i = 0;
	
	SystemInit();
	RCC_Init();

	TIM1_Init();

	SysTick_Config(168000);


	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;	// включение тактирования порта GPIOE
	GPIOE->MODER |= GPIO_MODER_MODE13_0;	// настройка вывода PE13 на выход
	GPIOE->OTYPER &= ~(GPIO_OTYPER_OT13);	// output push-pull mode
	GPIOE->PUPDR &= ~(GPIO_PUPDR_PUPD13);	// pull-up pull-down disabled
	
	LED1_OFF;			// выключить LED1 - выдать 1 на вывод PE13

  
	while (1) {
	/* мигание светодиодом LED1 (PE13) */
		if ( delay_500ms <= time_ms ) {
			if ( ( GPIOE->ODR & GPIO_ODR_OD13 ) == GPIO_ODR_OD13 )
				LED1_ON
			else 
				LED1_OFF
			
			delay_500ms = time_ms + 500;
		}
		

		// управление яркостью светодиода PE14 (LED2)
		if ( delay_10ms <= time_ms ) {
			if( ( TIM1->CCR4 ) >= 999)
				TIM1->CCR4 = 1;
			else
				TIM1->CCR4 = TIM1->CCR4 + 5;
			
			delay_10ms = time_ms + 10;
		}

		
	} // while(1){}
} // main()


void SysTick_Handler(void){
	time_ms++;
}

void TIM1_Init(void){

	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;

	/*PE14 TIM1_CH4 pins*/
	GPIOE->MODER   |= GPIO_MODER_MODE14_1;		// Альтернативная функция для PE14
	GPIOE->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR14_1;	// highspeed
	GPIOE->AFR[1]  |= GPIO_AFRH_AFRH6_0;			// AF1 для PE1 (6-й набор бит в регистре)

	/* настройка таймера TIM1*/
	TIM1->PSC = 83;				// TIM1 clock 1 MHz
	TIM1->CR1 |= TIM_CR1_CMS;
	TIM1->ARR = 999;			// 999+1 counts number for TIM1. PWM freq = 1 kHz
	TIM1->CCR4 = 1;				
	TIM1->CCMR2	|= TIM_CCMR2_OC4M;		// PWM mode 2
	TIM1->CCMR2 &= ~(TIM_CCMR2_CC4S);	// режим работы - выход (OC4)
	TIM1->CCER |= TIM_CCER_CC4E;		// Включение OC4
	TIM1->BDTR |= TIM_BDTR_MOE;			// out put enable 
	TIM1->CR1  |= TIM_CR1_CEN;			// включение таймера TIM1
	TIM1->EGR  |= TIM_EGR_UG;			// Обновление регистров (событие UEV) 
	
}

/*************************** End of file ****************************/
