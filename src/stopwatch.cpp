/*
 * stopwatch.c
 *
 *  Created on: 10 дек. 2018 г.
 *      Author: venjega
 */

extern "C" {
#include "stm32l0xx.h"
}
#include "stopwatch.hpp"

static std::uint32_t updates = 0;

extern "C" void TIM21_IRQHandler()
{
	if (TIM21->SR & TIM_SR_UIF)
	{
		TIM21->SR &= ~TIM_SR_UIF;
		if (++updates > 10)
			updates = 0;		// 1024ms max
	}
}

extern "C" void StopwatchInit(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_TIM21EN;
#if 0 // setup TIM21 in external clock mode 1
	// set 011 to TI1_RMP -> TI1 input connected to MSI
	TIM21->OR |= TIM21_OR_TI1_RMP_0 | TIM21_OR_TI1_RMP_1;
	// set 10 to CC2S -> CC2 channel is input mapped to TI1
	TIM21->CCMR1 |= TIM_CCMR1_CC2S_1;
	// set '111' to SMS ->External clock mode 1;
	// set '101' to TS -> Filtered Timer Input 1
	TIM21->SMCR = TIM_SMCR_SMS | TIM_SMCR_TS_0 | TIM_SMCR_TS_2;
	TIM21->CR1 |= TIM_CR1_ARPE; 				// ARR buffered
	TIM21->PSC = 20;							// 1 tick = 10us
	TIM21->ARR = 49999; 						// update every 0.5s
#endif
	TIM21->CR1 |= TIM_CR1_ARPE;
	TIM21->PSC = 31;							// 1 tick = 8us
	TIM21->ARR = 49999;
	TIM21->EGR |= TIM_EGR_UG;					// do update now!

#ifdef DEBUG_STOPWATCH
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
	GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODE5)) | (GPIO_MODER_MODE5_0);
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEED5 | GPIO_OSPEEDER_OSPEED5_0 | GPIO_OSPEEDER_OSPEED5_1;
#endif // DEBUG_STOPWATCH

	NVIC_EnableIRQ(TIM21_IRQn);
}

extern "C" void StopwatchStart (void)
{
	TIM21->CNT = 0;
	TIM21->SR &= ~TIM_SR_UIF;
	TIM21->DIER |= TIM_DIER_UIE;
#ifdef DEBUG_STOPWATCH
	GPIOA->BSRR = (1<<5); 				// set DBG pin to '1' PA5
#endif // DEBUG_STOPWATCH
	TIM21->CR1 |= TIM_CR1_CEN; 			//TIM enable
}


uint_c StopwatchStop(void)
{
	uint_c _1tick_ms(8389);				// 1 tick in [ms], q22
	uint_c _50000ticks_ms(419430400);	// 50000 ticks in [ms], q22

	TIM21->DIER &= ~TIM_DIER_UIE;
	TIM21->CR1 &= ~TIM_CR1_CEN;

#ifdef DEBUG_STOPWATCH
	GPIOA->BRR = (1 << 5); 				// set DBG pin to '0'
#endif // DEBUG_STOPWATCH

	uint_c timeElapsed(TIM21->CNT);
	timeElapsed *= _1tick_ms;

	if (updates > 0)
	{
		uint_c timeToAdd(updates);
		timeToAdd *= _50000ticks_ms;
		timeElapsed += timeToAdd;
		updates = 0;
	}

	return timeElapsed;
}

