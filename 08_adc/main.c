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


void RCC_Init(void);
void TIM1_Init(void);
void ADC1_Init(void);
void ADC_IRQHandler(void);




int main(void) {
	
	SystemInit();
	RCC_Init();
	TIM1_Init();
	
	ADC1_Init();
  


	while (1) {
	

		
	} // while(1){}
} // main()


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


void ADC1_Init(void){

	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;		// включение тактирования ADC1
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;	// Включение тактирования GPIOA

	GPIOA->MODER |= GPIO_MODER_MODE5;		// Аналоговый режим работы PA5 (ADC1_CH5)
	ADC1->SMPR2 |= ADC_SMPR2_SMP5_0;		// 12 + 15 циклов на конвертацию 
	ADC1->SQR1  &= ~(ADC_SQR1_L);			// длина последовательности = 1
	ADC1->SQR3 |= (5 << ADC_SQR3_SQ1_Pos); //  первая конвертация будет в канале 5
	ADC1->CR1 |= ADC_CR1_EOCIE;
	
	NVIC_EnableIRQ(ADC_IRQn);

	ADC1->CR2 |= ADC_CR2_CONT | ADC_CR2_ADON;	// непрерывный режим конвертации

	ADC1->CR2 |= ADC_CR2_SWSTART;	// запуск измерения

}





void ADC_IRQHandler(void){

	TIM1->CCR4 = (ADC1->DR * 1000 / 4096); 

	NVIC_ClearPendingIRQ(ADC_IRQn);

}



/*************************** End of file ****************************/
