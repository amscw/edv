extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32l0xx.h"
}

#include "integer.hpp"
#include "stopwatch.hpp"
#include "io.hpp"
#include "queue.hpp"

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"


/** used formats:
 * q22 - elapsed time, returned by stopwatch
 * q17 - time to fire SWITCH_ON
 */


static io_c io;
static textQueue_c queue;

const char *buf = "20000.122";	// q17

int main(void) {
	uint_c timeElapsed = StopwatchStop();
	io.PrintMsg(io_c::msg_t::PARAM_TIME_ELAPSED, timeElapsed, 22);

	// test
	uint_c n(0);
	n.FromCStr(buf, 17);
	n.ToCStr(17, 3);
	puts(n);

	queue.PutItem('a');

	// Infinite loop
	while (1) {
	}
	// Infinite loop, never return.
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
