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
	const std::uint8_t prec{3};

	pin_c<port_c::A, 9> tx;
	pin_c<port_c::A, 10> rx;

	static const char *messages[];


public:
	enum class msg_t : std::uint8_t {
		PARAM_TEST,
		PARAM_TIME_ELAPSED,
	};

	void PrintMsg(msg_t mt, uint_c param, std::uint8_t e) noexcept;

	io_c() noexcept;
	~io_c() noexcept;
};


#endif /* TRACE_HPP_ */
