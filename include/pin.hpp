/*
 * pin.hpp
 *
 *  Created on: 15 дек. 2018 г.
 *      Author: moskvin
 */

#ifndef PIN_HPP_
#define PIN_HPP_

#include <cstdint>
extern "C" {
#include "stm32l0xx.h"
}


class port_c
{
public:
	using port_t = enum {
		A = GPIOA_BASE,
		B = GPIOB_BASE,
		C = GPIOC_BASE,
	};

	enum class mode_t : std::uint8_t {
		INPUT, OUTPUT, AF, AN
	};

	enum class outputType_t : std::uint8_t {
		PUSH_PULL, OPEN_DRAIN
	};

	enum class outputSpeed_t : std::uint8_t {
		LOW, MEDIUM, HIGH, VERY_HIGH
	};

	enum class pull_t : std::uint8_t {
		NO, UP, DOWN
	};

	enum class af_t : std::uint8_t {
		AF0, AF1, AF2, AF3, AF4, AF5, AF6, AF7
	};

protected:
	static std::uint32_t pinsInUse[];
	inline std::uint32_t& getUsageMask(port_t port) noexcept
			__attribute__((__always_inline__))
	{
		return (port == A) ? pinsInUse[0] : (port == B) ? pinsInUse[1] : pinsInUse[2];
	}
};

// generic template, never used
template<port_c::port_t port, std::uint32_t pin> class pin_c;

/**
 * GPIOA pin specialization
 */
template<std::uint32_t pin> class  pin_c<port_c::A, pin> : port_c
{

public:
	pin_c<port_c::A, pin>()
	{
		RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
		getUsageMask(port_c::A) |= (1 << pin);
		GPIOA->ODR &= ~(1 << pin);
	}

	~pin_c<port_c::A, pin>() {
		getUsageMask(port_c::A) &= ~(1 << pin);
		if (getUsageMask(port_c::A) == 0)
			RCC->IOPENR &= ~RCC_IOPENR_GPIOAEN;
	}


	/**
	 * - The output buffer is enabled in open drain/push-pull mode
	 * - The Schmitt trigger input is activated
	 * - The pull-up and pull-down resistors are activated depending on the value in the GPIOx_PUPDR register
	 * - The data present on the I/O pin are sampled into the input data register every AHB clock cycle
	 * - A read access to the input data register gets the I/O state
	 * - A read access to the output data register gets the last written value
	 */
	void ConfigAsOutput(port_c::outputType_t otype,
			port_c::outputSpeed_t stype = port_c::outputSpeed_t::HIGH,
			port_c::pull_t ptype = port_c::pull_t::NO) noexcept
	{
		const std::uint32_t _2bitFieldMask = ~(0x3 << (pin << 1));

		// set pin mode
		GPIOA->MODER &= _2bitFieldMask;
		GPIOA->MODER |= ((std::uint32_t)port_c::mode_t::OUTPUT << (pin << 1));

		// set pin output type
		GPIOA->OTYPER &= ~(1 << pin);
		GPIOA->OTYPER |= ((std::uint32_t)otype << pin);

		// set pin output speed
		GPIOA->OSPEEDR &= _2bitFieldMask;
		GPIOA->OSPEEDR |= ((std::uint32_t)stype << (pin << 1));

		// set pin pull type
		GPIOA->PUPDR &= _2bitFieldMask;
		GPIOA->PUPDR |= ((std::uint32_t)ptype << (pin << 1));
	}

	/**
	 * - The output buffer is disabled
	 * - The Schmitt trigger input is activated
	 * - The pull-up and pull-down resistors are activated depending on the value in the GPIOx_PUPDR register
	 * - The data present on the I/O pin are sampled into the input data register every AHB clock cycle
	 * - A read access to the input data register provides the I/O state
	 */
	void ConfigAsInput() noexcept
	{

	}

	/**
	 * - The output buffer can be configured in open-drain or push-pull mode
	 * - The output buffer is driven by the signals coming from the peripheral transmitter enable and data
	 * - The Schmitt trigger input is activated
	 * - The weak pull-up and pull-down resistors are activated or not depending on the value in the GPIOx_PUPDR register
	 * - The data present on the I/O pin are sampled into the input data register every AHB clock cycle
	 * - A read access to the input data register gets the I/O state
	 */
	void ConfigAsAlternate(port_c::af_t af,
			port_c::outputType_t otype=port_c::outputType_t::PUSH_PULL,
			port_c::outputSpeed_t stype = port_c::outputSpeed_t::HIGH) noexcept
	{
		const std::uint32_t _2bitFieldMask = ~(0x3 << (pin << 1));

		// set alternate function
		if (pin <= 7)
		{
			GPIOA->AFR[0] &= ~(0xF << (pin << 2));
			GPIOA->AFR[0] |= (std::uint32_t)af << (pin << 2);
		} else {
			GPIOA->AFR[1] &= ~(0xF << ((pin - 8) << 2));
			GPIOA->AFR[1] |= (std::uint32_t)af << ((pin - 8) << 2);
		}

		// set pin mode
		GPIOA->MODER &= _2bitFieldMask;
		GPIOA->MODER |= ((std::uint32_t)port_c::mode_t::AF << (pin << 1));

		// set pin output type
		GPIOA->OTYPER &= ~(1 << pin);
		GPIOA->OTYPER |= ((std::uint32_t)otype << pin);

		// set pin output speed
		GPIOA->OSPEEDR &= _2bitFieldMask;
		GPIOA->OSPEEDR |= ((std::uint32_t)stype << (pin << 1));
	}

	inline void Set() noexcept	__attribute__((always_inline))
	{
		GPIOA->BSRR |= (1 << pin);
	}

	inline void Reset() noexcept __attribute__((always_inline))
	{
		GPIOA->BRR |= (1 << pin);
	}
};

#endif /* PIN_HPP_ */
