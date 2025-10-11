#include "main.h"



int main(void) {

	GPIO_Init();
  
	while (1){
		LED_Toggle();
		Delay(TIME);

		LED_Toggle();
		Delay(TIME);
	
	}
	
}

/*************************** End of file ****************************/
