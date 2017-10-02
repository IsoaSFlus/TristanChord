/**
 ******************************************************************************
 * @file    main.c
 * @author  Ac6
 * @version V1.0
 * @date    01-December-2013
 * @brief   Default main function.
 ******************************************************************************
 */


#include "stm32f4xx.h"

#define TIME_OF_MIN_NOTE 16
#define MIN_NOTE_VALUE 16

float pitch_table[] = {
		0, 32.703, 34.648, 36.708, 38.891, 41.203, 43.654, 46.249, 48.999, 51.913, 55.000, 58.270, 61.735,
		130.81, 138.59, 146.83, 155.56, 164.81, 174.61, 185.00, 196.00, 207.65, 220.00, 233.08, 246.94,
		261.63, 277.18, 293.66, 311.13, 329.63, 349.23, 369.99, 392.00, 415.30, 440.00, 466.16, 493.88,
		523.25, 554.37, 587.33, 622.25, 659.26, 698.46, 739.99, 783.99, 830.61, 880.00, 932.33, 987.77,
		1046.5, 1108.7, 1174.7, 1244.5, 1318.5, 1396.9, 1480.0, 1568.0, 1661.2, 1760.0, 1864.7, 1975.5
};

uint8_t part_a[1024] = {0};
uint8_t part_b[1024] = {0};
uint8_t part_c[1024] = {0};
uint8_t part_d[1024] = {0};

uint16_t pitch_and_note_value_a[] = {2208,3004,3008,3004,2908,3304,3308,3304,3408,3508,3604,3608};
int size_of_a = (sizeof(pitch_and_note_value_a) / sizeof(pitch_and_note_value_a[0]));

uint16_t pitch_and_note_value_b[] = {8,4,2,1802,1804,1702};
int size_of_b = (sizeof(pitch_and_note_value_b) / sizeof(pitch_and_note_value_b[0]));

uint16_t pitch_and_note_value_c[] = {8,4,2,2802,2804,2702};
int size_of_c = (sizeof(pitch_and_note_value_c) / sizeof(pitch_and_note_value_c[0]));

uint16_t pitch_and_note_value_d[] = {8,4,2,2402,2404,2102};
int size_of_d = (sizeof(pitch_and_note_value_d) / sizeof(pitch_and_note_value_d[0]));

uint32_t current_time = 0;

uint32_t TIM_Clock = 0;







void gen_tones(uint8_t* part, uint16_t* pitch_and_note_value, int size_of_music) {
	int num_in_min_note_value = 0;
	for (int i = 0; i < size_of_music; ++i) {
		uint8_t tone_last_time = MIN_NOTE_VALUE / (pitch_and_note_value[i] % 100);
		for (int j = 0; j < tone_last_time*4; ++j) {
			part[num_in_min_note_value++] = pitch_and_note_value[i] / 100;
		}
	}
}

void play_tones_irq() {
	if (part_b[current_time] == 0) {
		TIM3->ARR = 1;
	} else {
		TIM3->ARR = 764;
		TIM3->PSC = (uint16_t)(TIM_Clock / (764 * pitch_table[part_b[current_time]]));
	}

	if (part_a[current_time] == 0) {
		TIM4->ARR = 1;
	} else {
		TIM4->ARR = 764;
		TIM4->PSC = (uint16_t)(TIM_Clock / (764 * pitch_table[part_a[current_time]]));
	}

	if (part_c[current_time] == 0) {
		TIM5->ARR = 1;
	} else {
		TIM5->ARR = 764;
		TIM5->PSC = (uint16_t)(TIM_Clock / (764 * pitch_table[part_c[current_time]]));
	}

	if (part_d[current_time] == 0) {
		TIM2->ARR = 1;
	} else {
		TIM2->ARR = 764;
		TIM2->PSC = (uint16_t)(TIM_Clock / (764 * pitch_table[part_d[current_time]]));
	}

	current_time++;
}

void TIM_Init() {
	GPIO_InitTypeDef GPIO_InitStructure;

	/* TIM3 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);//PB6
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//PC6
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);//PA0
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//PA5

	/* GPIOC clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	/* GPIOC Configuration: TIM3 CH1 (PC6), TIM3 CH2 (PC7), TIM3 CH3 (PC8) and TIM3 CH4 (PC9) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Connect TIM3 pins to AF2 */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_TIM5);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_TIM2);

}

void TIM_Config() {
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	/* Compute the prescaler value */
	uint16_t PrescalerValue = (uint16_t) (TIM_Clock / (764 * 261.63)) - 1;

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 1;
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);



	TIM_TimeBaseStructure.TIM_Period = 1;
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);



	TIM_TimeBaseStructure.TIM_Period = 1;
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);


	TIM_TimeBaseStructure.TIM_Period = 1;
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);


	/* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 382;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);


	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 382;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);


	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 382;
	TIM_OC1Init(TIM5, &TIM_OCInitStructure);

	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 382;
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);

	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC1PreloadConfig(TIM5, TIM_OCPreload_Enable);
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM4, ENABLE);
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_ARRPreloadConfig(TIM5, ENABLE);
	TIM_ARRPreloadConfig(TIM2, ENABLE);

//	/* TIM3 enable counter */
//	TIM_Cmd(TIM3, ENABLE);

}

int main(void)
{
	TIM_Clock = SystemCoreClock / 2;

	gen_tones(part_a, pitch_and_note_value_a, size_of_a);
	gen_tones(part_b, pitch_and_note_value_b, size_of_b);
	gen_tones(part_c, pitch_and_note_value_c, size_of_c);
	gen_tones(part_d, pitch_and_note_value_d, size_of_d);

	TIM_Init();
	TIM_Config();

	TIM_Cmd(TIM4, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
	TIM_Cmd(TIM5, ENABLE);
	TIM_Cmd(TIM2, ENABLE);

	if (SysTick_Config(SystemCoreClock / TIME_OF_MIN_NOTE)) {
		while(1);
	}

	for(;;);
}
