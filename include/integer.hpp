/*
 * integer.hpp
 *
 *  Created on: 8 дек. 2018 г.
 *      Author: moskvin
 */

#ifndef INTEGER_HPP_
#define INTEGER_HPP_

#include <cstdint>
#include <cstring>

/**
 * fixed-point support
 */

class integer_basic
{
	static const std::size_t bufsize{33};

protected:
	static const std::uint32_t pow10Table[10];
	static char m_buf[bufsize];
};

/**
 * generic template, never used
 */
template <typename T> class integer;

/**
 * uint32_t specialization
 */
template<>
class integer<std::uint32_t> : integer_basic
{
	std::uint32_t m_number{0};

public:
	integer(std::uint32_t n) noexcept : m_number(n){ }
	integer(std::uint32_t n, std::uint32_t exponent) noexcept : m_number(n << exponent) { }

	inline operator const char*() noexcept { return m_buf; }
	inline operator std::uint32_t () noexcept { return m_number; }

	integer<std::uint32_t>& operator /= (const integer<std::uint32_t> &right) noexcept
	{
		m_number /= right.m_number;
		return *this;
	}

	integer<std::uint32_t>& operator *= (const integer<std::uint32_t> &right) noexcept
	{
		m_number *= right.m_number;
		return *this;
	}

	integer<std::uint32_t>& operator += (const integer<std::uint32_t> &right) noexcept
	{
		m_number += right.m_number;
		return *this;
	}

	integer<std::uint32_t>& operator -= (const integer<std::uint32_t> &right) noexcept
	{
		m_number -= right.m_number;
		return *this;
	}


	/**
	 * parameter r - selected depending by desired accuracy.
	 * for example, r may be assumed following: r + q >= p, where q - exponent of numerator, p - exponent of denominator
	 * for correct translate to text, you must put then it to ToCStr(r)
	 * return qr number, if p==q
	 */
	std::uint32_t FDiv(std::uint32_t fdivider, std::uint32_t r = 16) noexcept
	{
		std::uint64_t value = static_cast<std::uint64_t>(m_number) << r;
		value /= fdivider;
		m_number = static_cast<std::uint32_t>(value);
		return m_number;
	}

	std::uint32_t FMul(std::uint32_t multiplyer, std::uint32_t e = 16) noexcept
	{
		std::uint64_t value = static_cast<std::uint64_t>(m_number) * static_cast<std::uint64_t>(multiplyer);
		m_number = static_cast<std::uint32_t>(value >> e);
		return m_number;
	}

	/**
	 * Convert simple integer to c-string
	 */
	std::size_t ToCStr() noexcept
	{
		if (m_number == 0) {
			m_buf[0] = '0';
			m_buf[1] = 0;
			return 1;
		}
		char *ptr = m_buf, count;
		std::uint32_t pow10;
		bool bSignificantDigits = false;
		for (std::size_t i = 0; i < sizeof pow10Table / sizeof *pow10Table; i++)
		{
			pow10 = pow10Table[i];
			count = 0;
			while(m_number >= pow10)
			{
				count++;
				m_number -= pow10;
				bSignificantDigits = true;
			}
			if (bSignificantDigits)
				*ptr++ = static_cast<char>(count + '0');
		}
		*ptr = 0;
		return ptr - m_buf;
	}

	/**
	 * Convert fixed-point number to c-string
	 */
	std::size_t ToCStr(std::uint32_t exponent) noexcept
	{
		std::uint32_t value = m_number;	// save member
		// integer part
		m_number >>= exponent;
		char *ptr = m_buf + ToCStr();
		m_number = value;				// restore member
		if (m_number != 0)
		{
			*ptr = '.';
			const std::uint32_t QXX_MASK = (1 << exponent) - 1;
			for (std::uint32_t i = 0; (i < exponent) && ((value &= QXX_MASK) != 0); i++)
			{
				// fast multiply by 10
				value <<= 1;			// 2x
				value += value << 2;	// 5x
				*++ptr = static_cast<char>((value >> exponent) + '0');
			}
			*++ptr = 0;
		}
		return ptr - m_buf;
	}

	/**
	 * Convert fixed-point number to string with fixed digits after comma
	 * param w width of fractional part
	 * WARNING! Stack usage - 32 bytes
	 * WARNING! Execution time 17.7 us @ 16MHz SYSCLK
	 */
	std::size_t ToCStr(std::uint32_t exponent, std::uint32_t w) noexcept
	{
		if (m_number != 0)
		{
			std::uint32_t value = m_number;	// save member
			char *ptr;

			// convert fractional part to binary-code-decimal
			w &= 0xF;					// 15 digits max
			std::uint32_t bcdNum = 0, bcdPos = (w - 1) << 2, last = 0;
			const std::uint32_t QXX_MASK = (1 << exponent) - 1;
			for (std::uint32_t i = 0; (i < exponent) && ((value &= QXX_MASK) != 0); i++)
			{
				// fast multiply by 10
				value <<= 1;			// 2x
				value += value << 2;	// 5x

				// shift digit into binary-code-decimal number
				if (i < w) {
					bcdNum |= (value >> exponent) << bcdPos;
					bcdPos -= 4;
				} else {
					last = static_cast<std::uint8_t>(value >> exponent);
					break;
				}
			}

			// round number
			if (last >= 5)
				bcdNum = bcdAdd(bcdNum, 1);

			// check carry into integer part
			value = m_number;				// save member
			m_number >>= exponent;
			std::uint32_t bcdMask = 0;
			for (std::uint32_t i = 0; i < w; i++)
				bcdMask |= 0xF << (i << 2);
			if (bcdNum & ~bcdMask)
				m_number += 1;				// +1

			// convert integer part to string
			ptr = m_buf + ToCStr();
			m_number = value;				// restore member

			// convert fractional part to string
			bcdPos = (w - 1) << 2;
			*ptr = '.';
			while (w--)
			{
				*++ptr = static_cast<char>(((bcdNum >> bcdPos) & 0xF) | 0x30);
				bcdPos -= 4;
			}
			// terminating null
			*++ptr = 0;
			return ptr - m_buf;
		} else {
			m_buf[0] = '0';
			m_buf[1] = 0;
			return 1;
		}
	}

	/**
	 * Convert c-string to integer
	 */
	std::uint32_t FromCStr(const char *str) noexcept
	{
		std::uint32_t value = 0;
		for (const char* p = str; *p; p++)
		{
			// fast multiply by 10
			value <<= 1;			// 2x
			value += value << 2;	// 5x
 			value += (*p - '0');
		}
		m_number = value;
		return value;
	}

	/**
	 * Convert c-string to fixed point integer
	 * WARNING! function changes the m_buf!
	 * WARNING! size of str must be less or equal m_buf!
	 */
	std::uint32_t FromCStr(const char * str, std::uint32_t e) noexcept
	{
		const char *delim = ".,";
		char *p1, *p2;

		// parse string
		std::strcpy(m_buf, str);
		p1 = strtok(m_buf, delim);
		p2 = strtok(NULL, delim);

		// convert integer part
		std::uint32_t value = FromCStr(p1);
		value <<= e;

		if (p2 != NULL)
		{
			// convert fractional part
			std::uint64_t f = FromCStr(p2);
			f <<= e;
			int i = sizeof pow10Table/ sizeof *pow10Table - strlen(p2) - 1;
			if (i < 0)
				i = 0;
			f /= static_cast<std::uint64_t>(pow10Table[i]);
			value |= static_cast<std::uint32_t>(f);
		}
		m_number = value;
		return value;
	}

private:
	inline std::uint32_t bcdAdd(std::uint32_t a, std::uint32_t b) const noexcept
	{
	    std::uint32_t carry = b + 0x66666666;
	    carry ^= (a + carry) ^ a;
	    a += b;
	    carry &= 0x11111111;
	    carry >>= 2;
	    a += carry;
	    carry >>= 1;
	    a += carry;
	    return a;
	}
}; // integer<std::uint32_t>

/**
 * aliases
 */
typedef integer<std::uint32_t> uint_c;

#endif /* INTEGER_HPP_ */
