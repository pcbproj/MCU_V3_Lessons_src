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
#include "stm32f4xx.h"


#define TIME 500000
/*********************************************************************
*
*       main()
*
*  Function description
*   Application entry point.
*/
int main(void) {
    SystemInit();

    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;    // включения тактирования порта GPIOE
    GPIOE->MODER |= GPIO_MODER_MODE13_0;   // PE13 output

    


    while(1){
        GPIOE->BSRR |= GPIO_BSRR_BR13;      // PE13 clear to 0
        
        for(uint32_t i = 0; i < TIME; i++){}

        GPIOE->BSRR |= GPIO_BSRR_BS13;      // PE13 set to 1

        for(uint32_t i = 0; i < TIME; i++){} 


    
    }
}

/*************************** End of file ****************************/
