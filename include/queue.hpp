/*
 * queue.hpp
 *
 *  Created on: 16 дек. 2018 г.
 *      Author: moskvin
 */

#ifndef QUEUE_HPP_
#define QUEUE_HPP_

#include <cstdint>
#include <cstring>
#include <type_traits>

// TODO: fucking overhead... may be std::uint32_t much better?

enum class queueFlags_t : std::uint8_t {
	EMPTY = 0x1,
	FULL = 0x2,
	OVERFLOW = 0x4,
} flags;

// fucking warnings...
queueFlags_t operator & (queueFlags_t a, queueFlags_t b) noexcept;
queueFlags_t operator &= (queueFlags_t &a, queueFlags_t b) noexcept;
queueFlags_t operator | (queueFlags_t a, queueFlags_t b) noexcept;
queueFlags_t operator |= (queueFlags_t &a, queueFlags_t b) noexcept;
queueFlags_t operator ~ (queueFlags_t a) noexcept;

queueFlags_t operator & (queueFlags_t a, queueFlags_t b) noexcept
{
	typedef std::underlying_type<queueFlags_t>::type t;
	return queueFlags_t(t(a) & t(b));
}

queueFlags_t operator &= (queueFlags_t &a, queueFlags_t b) noexcept
{
	typedef std::underlying_type<queueFlags_t>::type t;
	t tmp = static_cast<t>(a);
	tmp &= static_cast<t>(b);
	return (a = static_cast<queueFlags_t>(tmp));
}

queueFlags_t operator | (queueFlags_t a, queueFlags_t b) noexcept
{
	typedef std::underlying_type<queueFlags_t>::type t;
	return queueFlags_t(t(a) | t(b));
}

queueFlags_t operator |= (queueFlags_t &a, queueFlags_t b) noexcept
{
	typedef std::underlying_type<queueFlags_t>::type t;
	t tmp = static_cast<t>(a);
	tmp |= static_cast<t>(b);
	return (a = static_cast<queueFlags_t>(tmp));
}

queueFlags_t operator ~ (queueFlags_t a) noexcept
{
	typedef std::underlying_type<queueFlags_t>::type t;
	return queueFlags_t(~t(a));
}

template<std::size_t qsize, typename item_t = char>
class queue_c
{
	item_t buf[qsize];
	std::uint32_t count{0};
	int head{0}, tail{0};
	queueFlags_t flags{0};

public:

	queue_c() noexcept;
	~queue_c() noexcept;

	inline const queueFlags_t& Flags() const noexcept __attribute__((always_inline)) { return flags; }
	const std::uint32_t& Count() const noexcept __attribute__((always_inline)) { return count; }

	const queueFlags_t& PutItem(item_t item) noexcept;
	item_t GetItem() noexcept;
};

template<std::size_t qsize, typename item_t>
queue_c<qsize, item_t>::queue_c() noexcept : flags(queueFlags_t::EMPTY)
{
	memset(buf, 0, sizeof buf);
}

template<std::size_t qsize, typename item_t>
queue_c<qsize, item_t>::~queue_c() noexcept
{}

template<std::size_t qsize, typename item_t>
item_t queue_c<qsize, item_t>::GetItem() noexcept
{
	item_t item = 0;
	if (count > 0)
	{
		item = buf[tail];
		if (++tail == qsize)
			tail = 0;
		if (--count == 0)
			flags |= queueFlags_t::EMPTY;
		flags &= ~(queueFlags_t::FULL | queueFlags_t::OVERFLOW);
	}
	return item;
}

template<std::size_t qsize, typename item_t>
const queueFlags_t& queue_c<qsize, item_t>::PutItem(item_t item) noexcept
{
	buf[head] = item;
	if (++head == qsize)
		head = 0;

	flags &= ~queueFlags_t::EMPTY;

	if ((flags & queueFlags_t::FULL) == queueFlags_t::FULL)
		flags |= queueFlags_t::OVERFLOW;
	else if (++count == qsize)
		flags |= queueFlags_t::FULL;
	return flags;
};

/**
 * aliases
 */
typedef queue_c<128, char> textQueue_c;

#endif /* QUEUE_HPP_ */
