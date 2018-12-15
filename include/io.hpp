/*
 * trace.hpp
 *
 *  Created on: 13 ���. 2018 �.
 *      Author: moskvin
 */

#ifndef TRACE_HPP_
#define TRACE_HPP_

#include "integer.hpp"
#include "pin.hpp"

class io_c
{
	pin_c<port_c::A, 9> tx;
	pin_c<port_c::A, 10> rx;

public:
	io_c() noexcept;
	~io_c() noexcept;
};


extern void TraceGPIOInit(void);
extern void TraceUSART1Init(void);
extern void writeUSART (uint8_t *pData);
extern void traceNumber (const char* msg, uint_c n);

#endif /* TRACE_HPP_ */
