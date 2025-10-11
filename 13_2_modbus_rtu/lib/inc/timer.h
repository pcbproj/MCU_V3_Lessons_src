#ifndef TIMER_H
#define TIMER_H

#include "stm32f407xx.h"

/******
Инициализация таймера TIM2 однократное срабатывание с прерыванием. 
*****/
void TIM2_InitOnePulseIRQ(void);


/******
Запуск таймера TIM2 
*****/
void TIM2_Start(uint16_t cycles_number);



#endif