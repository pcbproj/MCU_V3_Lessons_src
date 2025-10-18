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
void DMA2_Stream0_Init(void);
void DMA2_Stream0_IRQHandler(void);

uint16_t buffer[8] __attribute__ ((section(".fast")));

int main(void) {
	
	SystemInit();				// инициализация системы
	RCC_Init();					// установка тактирования на 168 МГц
	TIM1_Init();				// настройка таймера TIM1
	ADC1_Init();				// Настройка ADC1
	DMA2_Stream0_Init();		// Настройка модуля DMA2



	while (1) {
	

		
	} // while(1){}
} // main()


void TIM1_Init(void){

	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;

	/*PE13 TIM1_CH3 pins*/
	GPIOE->MODER   |= GPIO_MODER_MODE13_1;			// Альтернативная функция для PE13
	GPIOE->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR13_1;	// highspeed
	GPIOE->AFR[1]  |= GPIO_AFRH_AFRH5_0;			// AF1 для PE13 (5-й набор бит в регистре)

	/* настройка таймера TIM1*/
	TIM1->PSC = 83;				// TIM1 clock 1 MHz
	TIM1->CR1 |= TIM_CR1_CMS;
	TIM1->ARR = 999;			// 999+1 counts number for TIM1. PWM freq = 1 kHz
	TIM1->CCR4 = 1;				
	TIM1->CCMR2	|= TIM_CCMR2_OC3M;		// PWM mode 2
	TIM1->CCMR2 &= ~(TIM_CCMR2_CC3S);	// режим работы - выход (OC3)
	TIM1->CCER |= TIM_CCER_CC3E;		// Включение OC3
	TIM1->BDTR |= TIM_BDTR_MOE;			// out put enable 
	TIM1->CR1  |= TIM_CR1_CEN;			// включение таймера TIM1
	TIM1->EGR  |= TIM_EGR_UG;			// Обновление регистров (событие UEV) 
	
}


void ADC1_Init(void){

	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;		// включение тактирования ADC1
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;	// Включение тактирования GPIOA

	GPIOA->MODER |= GPIO_MODER_MODE5;		// Аналоговый режим работы PA5 (ADC1_CH5)
	
	ADC->CCR |= ADC_CCR_ADCPRE;				// PCLK2 / 8

	ADC1->SMPR2 |= ADC_SMPR2_SMP5_2;		// 84 + 12 циклов на конвертацию 
	ADC1->SQR1  |= (0 << ADC_SQR1_L_Pos);	// длина последовательности = 1
	ADC1->SQR3 |= (5 << ADC_SQR3_SQ1_Pos);	//  первая конвертация будет в канале 5

	ADC1->CR2 |= ADC_CR2_CONT;				// непрерывный режим конвертации
	ADC1->CR2 |= ADC_CR2_DMA | ADC_CR2_DDS; // включение непрерывых запросов DMA

	ADC1->CR2 |= ADC_CR2_ADON;				// Включение АЦП
	
	ADC1->CR2 |= ADC_CR2_SWSTART;			// Запуск измерений
	
}



void DMA2_Stream0_Init(void){
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;

	/**  DMA 2 stream 0 channel 0 - ADC1  **/
	DMA2_Stream0->PAR = (uint32_t)&(ADC1->DR);					// адрес периферии
	DMA2_Stream0->M0AR = (uint32_t)buffer;						// Адрес в памяти
	DMA2_Stream0->NDTR = 8;										// кол-во передаваемфх данных
	DMA2_Stream0->FCR &= ~(DMA_SxFCR_DMDIS);					// прямой режим без FIFO
	DMA2_Stream0->CR  &= ~(DMA_SxCR_CHSEL);						// канал 0
	DMA2_Stream0->CR  &= ~(DMA_SxCR_MBURST | DMA_SxCR_PBURST);	// одиночная передача (не пакетная)
	DMA2_Stream0->CR  &= ~(DMA_SxCR_DBM);						// режим двойного буфера выключен
	DMA2_Stream0->CR  |= DMA_SxCR_PL;							// самый высокий приоритет
	DMA2_Stream0->CR  |= (DMA_SxCR_MSIZE_0 | DMA_SxCR_PSIZE_0); // размер даннных памяти и периферии полусловно (16 бит)
	DMA2_Stream0->CR  |= DMA_SxCR_MINC;							// инкрементирование адреса памяти включено
	DMA2_Stream0->CR  &= ~(DMA_SxCR_PINC);						// инкрементирование адреса периферии выключено
	DMA2_Stream0->CR  |= DMA_SxCR_CIRC;							// циклический режим работы 
	DMA2_Stream0->CR  &= ~(DMA_SxCR_DIR);						// передача данных из периферии в память
	DMA2_Stream0->CR  |= DMA_SxCR_TCIE;							// прерывание по завершению передачи

	NVIC_EnableIRQ(DMA2_Stream0_IRQn);							// разрешение прерывания потока 0 в NVIC

	DMA2_Stream0->CR |= DMA_SxCR_EN;							// Вклчение потока 0

}



void DMA2_Stream0_IRQHandler(void){
	uint8_t i;
	uint16_t ovr = 0;

	for(i = 0; i < 8; i++){
		ovr = ovr + buffer[i];
	}
	
	TIM1->CCR3 = ( (ovr/8)*1000 )/4096;		// вычисление средненго значения по 8-ми выборкам АЦП и запись его в таймер 
	
	DMA2->LIFCR |= DMA_LIFCR_CTCIF0;		
	NVIC_ClearPendingIRQ(DMA2_Stream0_IRQn);

}



/*************************** End of file ****************************/
