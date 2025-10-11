#include "delay.h"



void Delay(uint32_t time){
	for(uint32_t i = 0; i < time; i++){
		__NOP();
	}
}

