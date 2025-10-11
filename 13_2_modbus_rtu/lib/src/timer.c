#include "timer.h"


void TIM2_InitOnePulseIRQ(void){
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	/* settings timer TIM2*/
	TIM2->PSC = 41;					// TIM2 clock 1 MHz
	TIM2->CR1 |= TIM_CR1_OPM;		// One pulse counting mode 
	TIM2->DIER |= TIM_DIER_UIE;
	TIM2->EGR  |= TIM_EGR_UG;
	TIM2->SR   &= ~TIM_SR_UIF;
	
	NVIC_EnableIRQ(TIM2_IRQn);		// timer 2 interrupt enable
}




void TIM2_Start(uint16_t cycles_number){
	TIM2->ARR = cycles_number - 1;
	TIM2->CNT = 0x00000000;
	TIM2->SR &= ~(TIM_SR_UIF);		// clear UIF flag
	TIM2->CR1  |= TIM_CR1_CEN;		// timer 2 clock enable
}


