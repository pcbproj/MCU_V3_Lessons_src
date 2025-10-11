#include "stm32f4xx.h"
#include "sdcard.h"
#include "usart.h"
#include "gpio.h"
#include <stdio.h>
#include <stdlib.h> // for malloc()
#include<string.h>  // for strlen()
#include "ff.h"
#include "ffconf.h"
#include "diskio.h"

void RCC_Init(void);