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
#include "stm32f407xx.h"


#define CSLOW	GPIOE->BSRR |= GPIO_BSRR_BR3;	// nCS active 
#define CSHIGH	GPIOE->BSRR |= GPIO_BSRR_BS3;	// nCS not active


#define LED1	0x01
#define LED2	0x02
#define LED3	0x03
#define CLEAR	0xFF


#define EN_RST	0x66
#define RST		0x99
#define	WR_EN	0x06
#define	SECT_ER	0x20
#define	RD_SR1	0x05
#define	PG_PROG	0x02
#define	RD_DATA	0x03
#define ADDR	0x20FF00


#define LED1_ON GPIOE->BSRR |= GPIO_BSRR_BR13	// turn on LED1 by 0
#define LED2_ON GPIOE->BSRR |= GPIO_BSRR_BR14	// turn on LED2 by 0
#define LED3_ON GPIOE->BSRR |= GPIO_BSRR_BR15	// turn on LED3 by 0

#define LED1_OFF GPIOE->BSRR |= GPIO_BSRR_BS13	// turn OFF LED1 by 1
#define LED2_OFF GPIOE->BSRR |= GPIO_BSRR_BS14	// turn OFF LED2 by 1
#define LED3_OFF GPIOE->BSRR |= GPIO_BSRR_BS15	// turn OFF LED3 by 1


void RCC_Init( void );
void SPI2_Init( void );	
uint8_t w25send( uint8_t data );

uint8_t memrd = 0;


int main(void) {
	uint16_t i = 0;
	
	SystemInit();	// инициализация системы
	RCC_Init();		// установка тактирования на 168MHz
	SPI2_Init();	// Настройка и включение SPI2

	/* Светодиоды на РЕ13, РЕ14, РЕ15*/
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;	// включение тактирования порта GPIOE
	GPIOE->MODER |= GPIO_MODER_MODE13_0 | GPIO_MODER_MODE14_0 | GPIO_MODER_MODE15_0;	// режим работы на выход
	GPIOE->OTYPER &= ~(GPIO_OTYPER_OT13 | GPIO_OTYPER_OT14 | GPIO_OTYPER_OT15);			// выход push-pull
	GPIOE->PUPDR  &= ~(GPIO_PUPDR_PUPD13 | GPIO_PUPDR_PUPD14 | GPIO_PUPDR_PUPD15);		// без подтягивающих резисторов


	GPIOE->BSRR |= GPIO_BSRR_BS13;
	GPIOE->BSRR |= GPIO_BSRR_BS14;
	GPIOE->BSRR |= GPIO_BSRR_BS15;


	

	/** Запись в W25Q64 */
	CSLOW;
	w25send(PG_PROG);		// send comand Page Prog to FLASH memory
	// send adddres sector to be writed
	w25send( ( ADDR >> 16 ) & 0xFF );		
	w25send( ( ADDR >> 8 ) & 0xFF );	
	w25send( ( ADDR ) & 0xFF );	
	w25send(LED2);
	CSHIGH;
	
	// check for data writing complete
	CSLOW;
	w25send(RD_SR1);	// send command "read status register1"
	while( ( w25send(0x00) & 0x01 ) == 1 ){};	// wait while BUSY-bit is not cleared
	CSHIGH;

  
	while (1){
		
		/**** считывание W24Q64 ***/
		CSLOW;
		w25send(RD_DATA);		// send comand "Read data" to FLASH memory
		// send adddres to be readed
		w25send( ( ADDR >> 16 ) & 0xFF );		
		w25send( ( ADDR >> 8 ) & 0xFF );	
		w25send( ( ADDR ) & 0xFF );	
		memrd = w25send(0x00);		// read data byte from FLASH
		CSHIGH;
	
		switch(memrd){

		case LED1:
			LED1_ON;
			LED2_OFF;
			LED3_OFF;
			break;

		case LED2:
			LED1_OFF;
			LED2_ON;
			LED3_OFF;
			break;

		case LED3:
			LED1_OFF;
			LED2_OFF;
			LED3_ON;
			break;

		case CLEAR:
			LED1_ON;
			LED2_ON;
			LED3_ON;
			break;

		default: 
			LED1_OFF;
			LED2_OFF;
			LED3_OFF;
			break;
		}	

	}
}


void SPI2_Init(void){
	
	RCC->AHB1ENR  |= RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIOEEN;
	
	/*Выводы SPI2 для W25Q64 - PC3(MOSI), PC2(MISO), PB10(SCK), PE3(CS)*/
	GPIOC->MODER  |= GPIO_MODER_MODE3_1 | GPIO_MODER_MODE2_1;	// Альтернативная функция PC2 PC3 
	GPIOC->AFR[0] |= GPIO_AFRL_AFRL3_2 | GPIO_AFRL_AFRL3_0 | GPIO_AFRL_AFRL2_2 | GPIO_AFRL_AFRL2_0; // выбрана AF5 для PC2 PC3 
	GPIOC->PUPDR  |= GPIO_PUPDR_PUPD3_1 | GPIO_PUPDR_PUPD2_1;  // turn on pull-down registers
	
	/*Вывод PB10 */
	GPIOB->MODER  |= GPIO_MODER_MODE10_1 ;	// PB10 alternate function modes (SCLK)
	GPIOB->AFR[1] |= GPIO_AFRH_AFRH2_2 | GPIO_AFRH_AFRH2_0; // selecter SPI2 alternate function for PB10
	GPIOB->PUPDR  |= GPIO_PUPDR_PUPD10_1;  // turn on pull-down registers for PB10

	GPIOE->MODER |= GPIO_MODER_MODE3_0;		// PE3 = nCS output
	GPIOE->OTYPER &= ~(GPIO_OTYPER_OT3);		// output mode
	GPIOE->PUPDR |= GPIO_PUPDR_PUPD3_1; 
	GPIOE->BSRR  |= GPIO_BSRR_BS3;			// out 1 to nCS. not active 

	// SPI2 configuration
	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
	SPI2->CR1 |= SPI_CR1_BR | SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI;	// baud 164 kHz, Master mode, NSS application drive mode
	SPI2->CR1 &= ~(SPI_CR1_DFF);	// 8 bit frame mode
	SPI2->CR1 |= SPI_CR1_SPE;		// SPI2 enable
	

	CSLOW;
	w25send(EN_RST);		// send comand enable Reset to FLASH memory
	CSHIGH;
	
	CSLOW;
	w25send(RST);		// send comand Reset to FLASH memory
	CSHIGH;

	CSLOW;
	w25send(WR_EN);		// send comand WriteEnable to FLASH memory
	CSHIGH;
	
	CSLOW;
	w25send(SECT_ER);		// send comand Sector Erase to FLASH memory
	
	// send adddres sector to be erased
	w25send( ( ADDR >> 16 ) & 0xFF );	// Адрес в памяти 0x(20)FF00		
	w25send( ( ADDR >> 8 ) & 0xFF );	// Адрес в памяти 0x20(FF)00	
	w25send( ( ADDR ) & 0xFF );			// Адрес в памяти 0x20FF(00)	
	CSHIGH;

	// check for sector erase complete
	CSLOW;
	w25send(RD_SR1);	// send command "read status register1"
	while( ( w25send(0x00) & 0x01 ) == 1 ){};	// wait while BUSY-bit is not cleared
	CSHIGH;

	CSLOW;
	w25send(WR_EN);		// send comand WriteEnable to FLASH memory
	CSHIGH;
}
 


uint8_t w25send( uint8_t data ){
	SPI2->DR = data;
	while( ( SPI2->SR & SPI_SR_TXE ) == 0 ){};		// wait for transmitter empty and receiver not empty
	while( ( SPI2->SR & SPI_SR_RXNE ) == 0 ){};		// wait for receiver not empty

	return SPI2->DR;		// return received data through SPI2
	
}



/*************************** End of file ****************************/
