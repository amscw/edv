/*
 * stopwatch.hpp
 *
 *  Created on: 11 ���. 2018 �.
 *      Author: moskvin
 */

#ifndef STOPWATCH_HPP_
#define STOPWATCH_HPP_

#include "integer.hpp"

extern "C" void StopwatchInit(void);
extern "C" void StopwatchStart(void);
uint_c StopwatchStop (void);

#endif /* STOPWATCH_HPP_ */
