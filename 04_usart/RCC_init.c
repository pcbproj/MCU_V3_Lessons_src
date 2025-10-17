#include "stm32f407xx.h"

void RCC_Init(void)
{
 RCC->CR |= RCC_CR_HSEON;
while ( (RCC->CR & RCC_CR_HSERDY) == 0 ) {}

 RCC->CFGR |= RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE2_DIV1 | RCC_CFGR_PPRE1_DIV2;
 RCC->CR &= ~(RCC_CR_PLLON); // Отключение PLL перед настройкой тактирования
 RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE; // Выбор HSE в качестве источника тактирования

/* тут настройка предделителей и умножителей для PLL с целью получить требуемую частоту 100 МГц
============================================================================================= */
 RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM);
 RCC->PLLCFGR |= RCC_PLLCFGR_PLLM_4 | RCC_PLLCFGR_PLLM_3 | RCC_PLLCFGR_PLLM_0; // /M=25
 RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLN);
 RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_7 | RCC_PLLCFGR_PLLN_6 | RCC_PLLCFGR_PLLN_3; // xN=200
 RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLP); // /P = 2 - PLLCLK = 100 MHz
/* ========================================================================================== */
 RCC->CR |= RCC_CR_PLLON; // подача тактирования на PLL
 while ( (RCC->CR & RCC_CR_PLLRDY) == 0 ) {} // Ожидание стабилизации частоты PLL

/*
настройка регистров FLASH-памяти.
Эта операция обязательна и выполняется только 1 раз при старте программы
====================================================================================
*/
 FLASH->ACR |= FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_3WS | FLASH_ACR_PRFTEN;
/* ================================================================================= */
 RCC->CFGR &= ~RCC_CFGR_SW;
 RCC->CFGR |= RCC_CFGR_SW_PLL; // подключение PLL к SYSCLK. Выбор PLL в качестве
 // основного источника тактирования
 while ( (RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL ) {} // Ожидание, пока PLL подключится к SYSCLK
}