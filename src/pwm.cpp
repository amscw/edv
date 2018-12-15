/*
 * pwmgenerator.cpp
 *
 *  Created on: 11 дек. 2018 г.
 *      Author: venjega
 */

#include "pwm.hpp"

void PWMGenerator:: PwmGPIOInit(void)
{
  RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
  GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODE5)) \
                  | (GPIO_MODER_MODE5_1);
  GPIOA->AFR[0] |= (5 << (5 * 4));
}


void PWMGenerator::PwmTIMInit(void)
{
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
  TIM2->PSC = 15; //prescaler=16 -> f=1MHz
  TIM2->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE; //pwm mode 1 on OC1, enable preload register on OC1
  TIM2->CCER |= TIM_CCER_CC1E; //enable output on OC1
}


void PWMGenerator::PWMGeneratorStart(uint16_t T, uint16_t Q)
{
  TIM2->ARR = T-1; //Period=10us
  TIM2->CCR1 = Q; //high during 5us
  TIM2->CR1 |= TIM_CR1_CEN; //enable counter
  TIM2->EGR |= TIM_EGR_UG; //force update generation
}

void PWMGenerator::PWMGeneratorInit(void)
{
  PwmGPIOInit();
  PwmTIMInit();
}


