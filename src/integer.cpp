/*
 * integer.cpp
 *
 *  Created on: 8 дек. 2018 г.
 *      Author: moskw
 */
#include "integer.hpp"

const std::uint32_t integer_basic::pow10Table[] = {
		1000000000,
		100000000,
		10000000,
		1000000,
		100000,
		10000,
		1000,
		100,
		10,
		1
};

char integer_basic::m_buf[bufsize] = { 0 };
