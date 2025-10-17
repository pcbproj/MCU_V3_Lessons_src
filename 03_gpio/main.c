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


#define TIME 1000000
/*********************************************************************
*
*       main()
*
*  Function description
*   Application entry point.
*/

void RCC_Init(void);


int main(void) {    
    SystemInit();
    RCC_Init();

    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;           // SYSCFG clock enable
    SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI10_PE;  // connect PE10 to EXTI interrupt line EXTI10



    __enable_irq();                         // global IRQ enable
   
    EXTI->PR |= EXTI_PR_PR10;               // clear IRQ EXTI for PE10 pin
    EXTI->FTSR |= EXTI_FTSR_TR10;           // irq event by falling edge on pin PE10
    EXTI->IMR |= EXTI_IMR_IM10;             // irq enable for pin PE10
    NVIC_EnableIRQ(EXTI15_10_IRQn);


    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;    // включения тактирования порта GPIOE
    GPIOE->MODER |= GPIO_MODER_MODE13_0;    // PE13 output mode
    GPIOE->MODER &= ~GPIO_MODER_MODE10;     // PE10 input mode



    while(1){
         

    }
}


void EXTI15_10_IRQHandler(void){
    GPIOE->BSRR |= GPIO_BSRR_BS13;
    for(uint32_t i = 0; i < TIME; i++){};
    GPIOE->BSRR |= GPIO_BSRR_BR13;
    for(uint32_t i = 0; i < TIME; i++){};
    
    GPIOE->BSRR |= GPIO_BSRR_BS13;
    for(uint32_t i = 0; i < TIME; i++){};
    GPIOE->BSRR |= GPIO_BSRR_BR13;
    for(uint32_t i = 0; i < TIME; i++){};

    GPIOE->BSRR |= GPIO_BSRR_BS13;
    for(uint32_t i = 0; i < TIME; i++){};
    GPIOE->BSRR |= GPIO_BSRR_BR13;
    for(uint32_t i = 0; i < TIME; i++){};

    EXTI->PR |= EXTI_PR_PR10;    // clear IRQ EXTI for PE10 pin

}

/*************************** End of file ****************************/
