#include "main.h"



void SysTick_Handler(void){
	timer_counter();
}




void USART6_IRQHandler(void){
	if(USART6->SR & USART_SR_RXNE){

		ModbusReception();
	}

	NVIC_ClearPendingIRQ(USART6_IRQn);
}




void TIM2_IRQHandler(void){
	TIM2->SR &= ~(TIM_SR_UIF);	// clear UIF flag
	ModbusTimersIRQ();
	NVIC_ClearPendingIRQ(TIM2_IRQn);

}




int main(void) {
	
	uint8_t modbus_req_rx[256];
	uint8_t modbus_rx_len;
	uint8_t modbus_err;
	uint8_t modbus_answer_tx[256];
	uint8_t modbus_answer_len;
	

	
	RCC_Init();

	__enable_irq();


	GPIO_Init();
	
	USART6_Init();

	

	SysTick_Config(SYSTICK_TIMER_CONST);	

	TIM2_InitOnePulseIRQ();

	LED1_OFF();
	LED2_OFF();
	LED3_OFF();
	
	ModbusTimerStart(DELAY_3_5_BYTE_US);

	
	while (1){
		
			modbus_err = RequestParsingOperationExec();

	}
}

/*************************** End of file ****************************/
