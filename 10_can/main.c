
/*
Обязательно поставить на плате джампер J4.

Используется интерфейс CAN2.

Периодическая отправка сообщений по CAN на ПК через CAN-USB-переходник.
Сообщение содержит состояние нажатых кнопок.
	период отправки сообщения CAN = 300 мс
	скорость передачи CAN = 500 кБит/сек
	FRAME_ID отправляемого сообщения 0x565
	поле данных = 8 байт
	байт 0 данных - состояние нажатых кнопок: 
		бит № 0 = кнопка S1 нажата
		бит № 1 = кнопка S2 нажата
		бит № 2 = кнопка S3 нажата
	Остальные байты данных - тестовые константы	 
 
Приём сообщений по CAN от ПК и передача поля данных в UART1.
Фильтрация входных CAN-сообщений по FRAME_ID = 0x567.  
	Скорость передачи USART1 115200 бит/сек. 
	8 бит данных,
	1 стоп бит,
	без бита четности
*/



#include <stdio.h>
#include <stdlib.h>
#include "stm32f4xx.h"


#define CAN_TX_TIME_MS		300		// время ожидания в мс для отправки сообщения по CAN
#define BTN_CHECK_MS		10		// период опроса кнопок в мс
#define	BTN_PRESS_CNT		4		// кол-во последовательных проверок состояния кнопки


#define RX_FRAME_ID			0x567	// FRAME_ID сообщений, которые мы принимаем. остальные игнорируем
#define TX_FRAME_ID			0x565	// FRAME_ID отправляемого сообщения
#define CAN_TX_DATA_LEN		8		// количество байт данных в отправляемом сообщении CAN



uint16_t CAN_TX_ms_count = 0;		// таймер для периодов отправки сообщений по CAN
uint16_t ms_count = 0;				// таймер для периодов опроса кнопок


char S1_cnt = 0; 	 // button S1 press couter
char S2_cnt = 0; 	 // button S2 press couter
char S3_cnt = 0; 	 // button S3 press couter

char S1_state = 0;   // S1 state: 1 = pressed, 0 = released
char S2_state = 0;   // S2 state: 1 = pressed, 0 = released
char S3_state = 0;   // S3 state: 1 = pressed, 0 = released



void RCC_Init(void);


void GPIO_Init(void){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;

	//-------- GPIO for buttons -------------------
	GPIOE -> PUPDR |= GPIO_PUPDR_PUPD10_0;
	GPIOE -> PUPDR |= GPIO_PUPDR_PUPD11_0;
	GPIOE -> PUPDR |= GPIO_PUPDR_PUPD12_0;
	   
	//-------- GPIO settings for LED1 LED2 LED3 --------
	GPIOE -> MODER |= GPIO_MODER_MODE13_0;
	GPIOE -> MODER |= GPIO_MODER_MODE14_0;
	GPIOE -> MODER |= GPIO_MODER_MODE15_0;
}




void BTN_Check(void){
	if ( ms_count > BTN_CHECK_MS){
		ms_count = 0;
		// Опрос кнопки S1
		if ((GPIOE->IDR & GPIO_IDR_ID10) == 0) {  // if S1 pressed
			if(S1_cnt < BTN_PRESS_CNT){  
				S1_cnt++;
				S1_state = 0;	// считаем кнопку S1 не нажатой
			}
			else S1_state = 1;	// считаем кнопку S1 нажатой
		}
		else{                   // if S1 released
			S1_state = 0;	// считаем кнопку S1 не нажатой
			S1_cnt = 0;
		}
		
		// Опрос кнопки S2
		if ((GPIOE->IDR & GPIO_IDR_ID11) == 0) {  // if S2 pressed
			if(S2_cnt < BTN_PRESS_CNT){
				S2_cnt++;
				S2_state = 0;
			}
			else S2_state = 1;
		}
		else{                   // if S2 released
			S2_state = 0;
			S2_cnt = 0;
		}
		
		// Опрос кнопки S3
		if ((GPIOE->IDR & GPIO_IDR_ID12) == 0) {  // if S3 pressed
			if(S3_cnt < BTN_PRESS_CNT){
				S3_cnt++;
				S3_state = 0;
			}
			else S3_state = 1;
		}
		else{                   // if S3 released
			S3_state = 0;
			S3_cnt = 0;
		}

	}
 }


void CAN2_Init(void){
	
	
	RCC -> AHB1ENR |=	RCC_AHB1ENR_GPIOBEN;			// включение тактирования GPIOB: PB5 = CAN2_RX, PB6 = CAN2_TX 
	RCC -> APB1ENR |=	RCC_APB1ENR_CAN1EN;				// включение тактирования CAN1
	RCC -> APB1ENR |=	RCC_APB1ENR_CAN2EN;				// включение тактирования CAN2
	
	
	GPIOB -> OSPEEDR |= GPIO_OSPEEDER_OSPEEDR5;			// максимальная частота работы вывода PB5
	GPIOB -> MODER |= GPIO_MODER_MODE5_1;				// настройка PB5 в альтернативный режим
	GPIOB -> AFR[0] |= (9U << GPIO_AFRL_AFSEL5_Pos);	// выбор альтернативной функции AF9 для PB5
	
	GPIOB -> OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6;			// максимальная частота работы вывода PB6
	GPIOB -> MODER |= GPIO_MODER_MODE6_1;				// настройка PB6 в альтернативный режим
	GPIOB -> AFR[0] |= (9U << GPIO_AFRL_AFSEL6_Pos);	// выбор альтернативной функции AF9 для PB6


	
	CAN2 -> MCR |= CAN_MCR_INRQ;						// переключение CAN2 в режим инициализации
	while((CAN2 -> MSR & CAN_MSR_INAK) == 0){};			// ожидание, пока CAN2  не переключится в режим инициализации


	CAN2 -> MCR |= CAN_MCR_NART;						// Отключение автоматической ретрансляции сообщений, вероятность неудачной передачи мала
	CAN2 -> MCR |= CAN_MCR_AWUM;						// Включение автоматического выхода из спящего режима после приема сообщения 
	CAN2 -> BTR = 0x00;								// сброс регистра BTR
	CAN2 -> BTR &= ~(CAN_BTR_SILM | CAN_BTR_LBKM);	// Отключение режимов Loop и Silent, нормальный режим работы
	
	/*	Настройка скорости передачи данных 500 кБит/сек:
		предделитель 6. 
		freq tq = 42 / 6 = 7 MHz
		CAN_Bit_time_tq_number = 7_000_000 Hz / 500_000 bit/s  = 14
		CAN_Bit_time = (1 + BS1 * BS2 ) = 14*tq

		BS1 + BS2 = 13*tq
		BS2 = BS1 / 7;
		BS2 = 13*tq / 7 = 1.86 ~= 2*tq
		BS1 = (13-2) * tq = 11*tq

	*/
	
	CAN2 -> BTR |= (5 << CAN_BTR_BRP_Pos);			// предделитель равен 6: 42 / 6 = 7 МГц частота тактирования CAN
	CAN2 -> BTR |= (10 << CAN_BTR_TS1_Pos);			// TS1 = 10, BS1 = 11
	CAN2 -> BTR |= (1 << CAN_BTR_TS2_Pos);			// TS2 = 1 , BS2 = 2
		
	// настройка фильтрации по FRAME_ID
	// filter list mode ID
	// принимаем сообщения только с RX_FRAME_ID = 0x567;
	
	/*
		Если есть два модуля CAN, то они имеют в наличии 28 фильтров, 
		с 0 по 13 для CAN1, и с 14 по 28 для CAN2.
	*/
	
	CAN1 -> FMR |= CAN_FMR_FINIT;							// переводим фильтры в режим инициализации
	CAN1 -> FM1R |= CAN_FM1R_FBM14;							// выбираем банк 14 в режиме List mode
	CAN1 -> FS1R &= ~(CAN_FS1R_FSC14);						// явно выбираем разрядность фильтра 16 бит 
	CAN1 -> FFA1R &= ~(CAN_FFA1R_FFA14);					// явно выбираем, что сообщение после фильтра 14 сохранится в FIFO0. бит сброшен в 0.
	CAN1 -> sFilterRegister[14].FR1 = (RX_FRAME_ID << 5);	// записываем значение FRAME_ID, сообщения с которым мы принимаем
	CAN1 -> FA1R |= (1 << CAN_FA1R_FACT14_Pos);				// активируем фильтр 14 для работы
	CAN1 -> FMR &= ~CAN_FMR_FINIT;							// переводим фильтры в активный режим 											

	CAN2 -> MCR &= ~(CAN_MCR_INRQ);							// переключение CAN2 в нормальный режим работы
	while((CAN2 -> MSR & CAN_MSR_INAK) != 0){};				// ожидание, пока CAN2 не переключится в нормальный режим 

}



// функция чтения из FIFO принятого сообщения по CAN
char CAN2_ReceiveMSG(uint16_t *frame_ID,			// идентификатор фрейма CAN
						uint16_t *data_len_bytes,	// длина поля данных в байтах 0 - 8 байт
						char rx_array[]				// массив байтов, принятый по CAN
					){
	
	if ((CAN2 -> RF0R & CAN_RF0R_FMP0) != 0){	// проверка FIFO0 не пустое? 
		*frame_ID = ((CAN2 -> sFIFOMailBox[0].RIR >> CAN_RI0R_STID_Pos) & 0x0FFF);			// вычитывание идентификатора,	
		*data_len_bytes = ((CAN2 -> sFIFOMailBox[0].RDTR >> CAN_RDT0R_DLC_Pos) & 0x000F);		// вычитывание DLC 
		
		for(uint16_t i=0; i < *data_len_bytes; i++){		// вычитывание данных сообщения из FIFO
			if (i < 4) {
				rx_array[i] = ((CAN2 -> sFIFOMailBox[0].RDLR >> 8*i) & 0x00FF);
			}
			else{
				rx_array[i] = ((CAN2 -> sFIFOMailBox[0].RDHR >> 8*(i-4)) & 0x00FF);
			}
		}
		CAN2 -> RF0R |= CAN_RF0R_RFOM0;		// освобождение FIFO0 выставлением бита RFOM в 1 
		return 0;
	}
	else{
		return 1;	// FIFO пустое. Ни чего не считали
	}
}





// функция передачи сообщения по CAN2
char CAN2_SendMSG(uint16_t frame_ID,			// идентификатор фрейма CAN 
					uint16_t data_len_bytes,	// длина поля данных в байтах 0 - 8 байт
					char tx_array[]				// массив байтов, для  отправки по CAN
					){

	// будем использовать для передачи собщений mailbox[0]
	if((CAN2 -> TSR & CAN_TSR_TME0) == 0){		// проверка, что mailbox[0] пустой
		return 1;								// возврат ошибки "mailbox[0] не пустой" завершение
	}
	else{
		CAN2 -> sTxMailBox[0].TIR = 0x0000;
		CAN2 -> sTxMailBox[0].TDTR = 0x0000;
		CAN2 -> sTxMailBox[0].TIR &= ~(CAN_TI0R_IDE);			// явно сбрасываем EXID, используем стандартный фрейм
		CAN2 -> sTxMailBox[0].TIR &= ~(CAN_TI0R_RTR);			// явно сбрасываем RTR, отправляем DATAT FRAME
		CAN2 -> sTxMailBox[0].TIR &= ~(CAN_TI0R_TXRQ);			// явно сбрасываем TXRQ, еще рано отправлять сообщение
		CAN2 -> sTxMailBox[0].TIR |= (frame_ID << 21);			// Запись идентификатора фрейма FRAME_ID
		
		CAN2 -> sTxMailBox[0].TDTR |= ((data_len_bytes & 0x000F) << CAN_TDT0R_DLC_Pos);	// Указать длину поля данных
			
		CAN2 -> sTxMailBox[0].TDLR = 0x0000;
		CAN2 -> sTxMailBox[0].TDHR = 0x0000;
		for(uint16_t i=0; i < data_len_bytes; i++){	// записать данные из массива в mailbox[0] для отправки 
			if(i < 4){
				CAN2 -> sTxMailBox[0].TDLR |= (tx_array[i] << 8*i);
			}
			else{
				CAN2 -> sTxMailBox[0].TDHR |= (tx_array[i] << 8*(i-4));
			}
		}
		

		CAN2 -> sTxMailBox[0].TIR |= CAN_TI0R_TXRQ;		// Начать отправку сообщения. TXRQ = 1
		
		if((CAN2 -> TSR & CAN_TSR_RQCP0) == 0){			// проверка, что сообщение отправлено 
			return ((CAN2 -> ESR & CAN_ESR_LEC) >> CAN_ESR_LEC_Pos);	// возвращяем код ошибки 
		}
		else{
			CAN2 -> TSR |= CAN_TSR_RQCP0;					// сброс бита RQCP0
			return 0;					
		}
	}
	
}


// настройка USART1 для передачи принятых данных в ПК по USART1
void USART1_Init(void){	
	
	RCC -> AHB1ENR |= RCC_AHB1ENR_GPIOAEN;							// включение тактирования GPIOA: PA9 = TX, PA10 = RX
	RCC -> APB2ENR |= RCC_APB2ENR_USART1EN;							// включение тактирования USART1 от шины APB2

	
	GPIOA -> MODER  |= GPIO_MODER_MODE9_1;							// Альтернативная функция для PA9 (USART1 - TX)
	GPIOA -> AFR[1] |= (7 << GPIO_AFRH_AFSEL9_Pos);					// AF7 для PA9
	GPIOA -> MODER  |= GPIO_MODER_MODE10_1;                           // Альтернативная функция для PA10 (USART1 - RX)
	GPIOA -> AFR[1] |= (7 << GPIO_AFRH_AFSEL10_Pos);					// AF7 для PA10

	
	/* Расчет скорости передачи данных:
		(84МГц/115200)/16 = 45.57; 
		Целая часть = 45 = 0x2D; 
		Дробная часть = 0.57*16 = 9 = 0x09 
	*/

	USART1 -> BRR |= 0x2D9;	// 115200
	
	/* Включение приемника и передатчика */
	USART1 -> CR1 |= USART_CR1_TE | USART_CR1_RE; 
	USART1 -> CR1 &= ~(USART_CR1_M) | ~(USART_CR1_PCE);              // 8-бит, без контроля четности
	USART1 -> CR2 &= ~(USART_CR2_STOP);                              // 1 стоповый бит
	USART1 -> CR1 |= USART_CR1_UE;                                   // Включение USART1

}





void usart_send(uint8_t data[], uint32_t len) {
	for (uint32_t i=0; i < len; i++){
		USART1 -> DR = data[i];
		while ((USART1 -> SR & USART_SR_TXE) == 0){};
	}
}


 void SysTick_Handler(void){		// прервание от Systick таймера, выполняющееся с периодом 1000 мкс
	ms_count++;			
	CAN_TX_ms_count++;
}



int main(void) {
	
	char can_tx_data_bytes[CAN_TX_DATA_LEN] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 };	// байт данных для отправки по CAN1, содержит состояния кнопок
	char can_rx_data_bytes[CAN_TX_DATA_LEN] = {};					// байты данных, принимаемые по CAN
	uint16_t can_rx_frame_id = 0;
	uint16_t can_rx_data_len = 0;
	char can_err_code = 0;

	
	RCC_Init();

	GPIO_Init();

	USART1_Init();

	CAN2_Init();

	
	SysTick_Config(84000);		// настройка SysTick таймера на время отрабатывания = 1 мс
								// 84000 = (AHB_freq / время_отрабатывания_таймера_в_мкс)
								// 84000 = 84_000_000 Гц / 1000 мкс;

  
	// выключаем все светодиоды (1 = OFF, 0 = ON)
	GPIOE -> BSRR |= GPIO_BSRR_BS13;
	GPIOE -> BSRR |= GPIO_BSRR_BS14;
	GPIOE -> BSRR |= GPIO_BSRR_BS15;


	while (1){

		BTN_Check();

		can_tx_data_bytes[0] = ((S3_state << 2) | (S2_state << 1) | S1_state);	// запись текущего состояния кнопок в can_tx_data_bytes[0]

		if (CAN_TX_ms_count == CAN_TX_TIME_MS){	// отправка CAN-сообщения с кнопками
			CAN_TX_ms_count = 0;
			can_err_code = CAN2_SendMSG(TX_FRAME_ID, CAN_TX_DATA_LEN, can_tx_data_bytes) ;	// отправка сообщения по CAN

			// мигание светодиодом LED1. Переключение его состояния.
			if(GPIOE -> ODR & GPIO_ODR_OD13) GPIOE -> BSRR |= GPIO_BSRR_BR13;
			else GPIOE -> BSRR |= GPIO_BSRR_BS13;
		}

		can_err_code = CAN2_ReceiveMSG(&can_rx_frame_id, &can_rx_data_len, can_rx_data_bytes);

		if( can_err_code == 0 ){		// чтение сообщения из непустого FIFO0
			
			GPIOE -> BSRR |= GPIO_BSRR_BR14;	// включение светодиода LED2

			usart_send((uint8_t*)can_rx_data_bytes, can_rx_data_len);		// отправка байтов данных в USART1

			GPIOE -> BSRR |= GPIO_BSRR_BS14;	// гашение светодиода LED2

		}

 
	}
}

/*************************** End of file ****************************/
