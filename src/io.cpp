/*
 * trace.cpp
 *
 *  Created on: 12 дек. 2018 г.
 *      Author: venjega
 */

extern "C" {
#include "stm32l0xx.h"
#include <errno.h>
#include <sys/types.h>
}

#include "io.hpp"
#include "stdio.h"

const char *io_c::messages[] = {
	"test: ",
	"time elapsed, ms: ",
};

io_c::io_c() noexcept
{
	tx.ConfigAsAlternate(port_c::af_t::AF4);
	rx.ConfigAsAlternate(port_c::af_t::AF4);

	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	USART2->BRR = 160000 / 96;
	USART2->CR1 = USART_CR1_TE | USART_CR1_UE;

	// USART2->CR1 |= USART_CR1_RXNEIE;/* enable RXNE interrupt */
	// NVIC_SetPriority(USART2_IRQn, 0);
	// NVIC_EnableIRQ(USART2_IRQn);
}

io_c::~io_c() noexcept
{
	RCC->APB1ENR &= ~RCC_APB1ENR_USART2EN;
}

void io_c::PrintMsg(io_c::msg_t mt, uint_c param, std::uint8_t e)
{
	param.ToCStr(e, prec);
	fputs(messages[static_cast<int>(mt)], stdout);
	puts(param);
}

// ----------------------------------------------------------------------------
// When using retargetted configurations, the standard write() system call,
// after a long way inside newlib, finally calls this implementation function.

// Based on the file descriptor, it can send arrays of characters to
// different physical devices.

// Currently only the output and error file descriptors are tested,
// and the characters are forwarded to the trace device, mainly
// for demonstration purposes. Adjust it for your specific needs.

// For freestanding applications this file is not used and can be safely
// ignored.
extern "C" {
	ssize_t _write(int fd, const char* buf, size_t nbyte);

	ssize_t _write(int fd, const char* buf,	size_t nbyte)
	{
		const char *p = buf;
		// STDOUT and STDERR are routed to the trace device
		if (fd == 1 || fd == 2)
		{
			while(nbyte--)
			{
				USART2->TDR = (*p++);
			    while ((USART2->ISR & USART_ISR_TC) != USART_ISR_TC);
			    USART2->ICR = USART_ICR_TCCF;
			    return p - buf;
			}
		}

		errno = ENOSYS;
		return -1;
	}
}
