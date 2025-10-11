#include "main.h"



void SysTick_Handler(void){
	timer_counter();
}


int main(void) {
	
	uint8_t rs485_rx_byte;
	uint8_t rs485_tx_array[12] = {"RS485 RX OK\n"};

	
	RCC_Init();

	GPIO_Init();
	
	USART6_Init();

	SysTick_Config(SYSTICK_TIMER_CONST);	

	LED1_OFF();
	LED2_OFF();
	LED3_OFF();
	
	
	while (1){
		
		if( !( usart6_receive_byte( &rs485_rx_byte) ) ){	// if received byte
			LED2_ON();
			Delay_ms(LED_BLINK_300ms);
			LED2_OFF();

			usart6_send(rs485_tx_array, sizeof(rs485_tx_array));	// send this byte back

		}


	}
}

/*************************** End of file ****************************/
