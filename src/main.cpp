extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32l0xx.h"
}

#include "integer.hpp"
#include "stopwatch.hpp"
#include "io.hpp"


// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

static io_c io;

int main(void) {
	uint_c timeElapsed = StopwatchStop();
	timeElapsed.ToCStr(22, 3);
	puts(timeElapsed);

	// Infinite loop
	while (1) {
	}
	// Infinite loop, never return.
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
