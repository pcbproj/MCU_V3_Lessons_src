#include "gpio.h"




void GPIO_Init(void){

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;

	//-------- GPIO settings for LED1 LED2 LED3 --------
	GPIOE -> MODER |= GPIO_MODER_MODE13_0;
	GPIOE -> MODER |= GPIO_MODER_MODE14_0;
	GPIOE -> MODER |= GPIO_MODER_MODE15_0;

}

