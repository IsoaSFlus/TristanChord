#include "stm32f4xx_it.h"
#include "main.h"


void SysTick_Handler(void) {
	play_tones_irq();
}
