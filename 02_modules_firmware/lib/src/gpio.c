#include "gpio.h"



void GPIO_Init(void){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
	GPIOE->MODER |= GPIO_MODER_MODE13_0;

}



void LED_Toggle(void){
	if(	LED_Check() ) {
		LED_ON;
	}
	else{
		LED_OFF;
	}
}



uint8_t LED_Check(void){
	if(GPIOE-> ODR & GPIO_ODR_OD13)
		return 1;
	else
		return 0;

}