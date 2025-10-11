#include "usart.h"



void USART6_Init(void){

	RCC -> AHB1ENR |= RCC_AHB1ENR_GPIOCEN;							// включение тактирования GPIOC: PC6 = TX, PC7 = RX
	RCC -> APB2ENR |= RCC_APB2ENR_USART6EN;							// включение тактирования USART6 от шины APB2
	
	GPIOC -> MODER  |= GPIO_MODER_MODE6_1;							// Альтернативная функция для PC6 (USART1 - TX)
	GPIOC -> AFR[0] |= (8 << GPIO_AFRL_AFSEL6_Pos);					// AF8 для PC6
	GPIOC -> MODER  |= GPIO_MODER_MODE7_1;                           // Альтернативная функция для PC7 (USART1 - RX)
	GPIOC -> AFR[0] |= (8 << GPIO_AFRL_AFSEL7_Pos);					// AF8 для PC7

	/* Расчет скорости передачи данных:
		(84МГц/115200)/16 = 45.57; 
		Целая часть = 45 = 0x2D; 
		Дробная часть = 0.57*16 = 9 = 0x09 
	*/

	USART6 -> BRR |= 0x2D9;	// 115200
	
	USART6 -> CR1 |= USART_CR1_TE | USART_CR1_RE;					// Включение приемника и передатчика
	USART6 -> CR1 &= ~(USART_CR1_M) | ~(USART_CR1_PCE);              // 8-бит, без контроля четности
	USART6 -> CR2 &= ~(USART_CR2_STOP);                              // 1 стоповый бит
	USART6 -> CR1 |= USART_CR1_UE;                                   // Включение USART6
	USART6 -> CR1 |= USART_CR1_RXNEIE;

	NVIC_EnableIRQ(USART6_IRQn);	

}



void usart6_send(uint8_t data[], uint8_t len){
	for (uint8_t i=0; i < len; i++){
		USART6 -> DR = data[i];
		while ((USART6 -> SR & USART_SR_TXE) == 0){};
	}
}



uint8_t usart6_receive_byte(uint8_t *rx_byte){
	uint8_t timer = 0;
	while(!(USART6->SR & USART_SR_RXNE)){
		if(timer < 32) timer++;
		else return USART_ERR;
	}	
	*rx_byte = USART6 -> DR;
	return USART_OK;
}