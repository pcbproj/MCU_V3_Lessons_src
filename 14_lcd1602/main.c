#include "main.h"

/*
Пример проекта с написанием строк на индикаторе (только латинские буквы):
показывает состояние нажатых кнопок

"BTN3 BTN2 BTN1"	-  постоянная строка. подписывает кнопки
" 0    1    0  "	- в нужном месте появляются "1" если кнопки нажата, и "0" если кнопка отпущена.
Включение курсора,
Позиционирование курсора на соответствующей кнопке
Включение Режима мигания курсора
*/


uint16_t BTN_ms_counter;



void SysTick_Handler(void){		// прервание от Systick таймера, выполняющееся с периодом 1 мкс

	timer_counter(&BTN_ms_counter);

}


int main(void) {
	
	uint8_t Line1_Text[LCD_CHAR_NUM_MAX] =		"BTN3  BTN2  BTN1";

	uint8_t Line2_Text000[LCD_CHAR_NUM_MAX] =	" 0     0     0  ";
	uint8_t Line2_Text001[LCD_CHAR_NUM_MAX] =	" 0     0     1  ";
	uint8_t Line2_Text010[LCD_CHAR_NUM_MAX] =	" 0     1     0  ";
	uint8_t Line2_Text011[LCD_CHAR_NUM_MAX] =	" 0     1     1  ";
	uint8_t Line2_Text100[LCD_CHAR_NUM_MAX] =	" 1     0     0  ";
	uint8_t Line2_Text101[LCD_CHAR_NUM_MAX] =	" 1     0     1  ";
	uint8_t Line2_Text110[LCD_CHAR_NUM_MAX] =	" 1     1     0  ";
	uint8_t Line2_Text111[LCD_CHAR_NUM_MAX] =	" 1     1     1  ";
	
	uint8_t ButtonState = 0;	
	uint8_t ButtonState_old = 0xFF;

	RCC_Init();

	SysTick_Config( SYSTICK_TIMER_CONST );
	
	GPIO_Init();

	LCD1602_PinsInit4bits();

	LCD1602_ScreenInit4bits();
	
	LED1_OFF();
	LED2_OFF();
	LED3_OFF();
	

	while (1){
	
		BTN_Check(&BTN_ms_counter, &ButtonState);
		

		if (ButtonState_old != ButtonState){

			LCD1602_SetDDRAMAddress(0x00);	
			LCD1602_WriteString4bits(Line1_Text, sizeof(Line1_Text));
			LCD1602_SetDDRAMAddress(0x40);
			
			
			switch (ButtonState){
			case 0:
				
				LCD1602_WriteString4bits(Line2_Text000, sizeof(Line2_Text000));
				break;
			case 1:
				LCD1602_WriteString4bits(Line2_Text001, sizeof(Line2_Text000));
				break;
			case 2:
				LCD1602_WriteString4bits(Line2_Text010, sizeof(Line2_Text000));
				break;
			case 3:
				LCD1602_WriteString4bits(Line2_Text011, sizeof(Line2_Text000));
				break;
			case 4:
				LCD1602_WriteString4bits(Line2_Text100, sizeof(Line2_Text000));
				break;
			case 5:
				LCD1602_WriteString4bits(Line2_Text101, sizeof(Line2_Text000));
				break;
			case 6:
				LCD1602_WriteString4bits(Line2_Text110, sizeof(Line2_Text000));
				break;
			case 7:
				LCD1602_WriteString4bits(Line2_Text111, sizeof(Line2_Text000));
				break;
			}
			
			LCD1602_CursorBlink_OFF();
			

			ButtonState_old = ButtonState;
			
		}

		
		if(ButtonState & 0x01){
			LCD1602_CursorBlink_ON(); 
			LCD1602_SetDDRAMAddress(0x4D);	
			LED1_ON();
		}
		else LED1_OFF();

		if(ButtonState & 0x02){
			LCD1602_CursorBlink_ON(); 
			LCD1602_SetDDRAMAddress(0x47); 
			LED2_ON();
		}
		else LED2_OFF();

		if(ButtonState & 0x04){
			LCD1602_CursorBlink_ON(); 
			LCD1602_SetDDRAMAddress(0x41);
			LED3_ON();
		}
		else LED3_OFF();

	}

}

/*************************** End of file ****************************/
