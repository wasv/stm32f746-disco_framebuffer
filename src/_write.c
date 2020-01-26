/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2014 Liviu Ionescu.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

// Do not include on semihosting and when freestanding
#if !defined(OS_USE_SEMIHOSTING) && !(__STDC_HOSTED__ == 0)

// ----------------------------------------------------------------------------

#include <errno.h>
#include "diag/Trace.h"
#include "bsp/stm32746g_discovery_lcd.h"

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

ssize_t
_write(int fd, const uint8_t *buf, size_t nbyte);

uint32_t cPos = 0;

ssize_t _write(int fd __attribute__((unused)),
		const uint8_t *buf __attribute__((unused)),
		size_t nbyte __attribute__((unused))) {
#if defined(TRACE)
  // STDOUT and STDERR are routed to the trace device
  if (fd == 1 || fd == 2)
    {
      return trace_write (buf, nbyte);
    }
#else
	if (fd == 1) {
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	} else if (fd == 2) {
		BSP_LCD_SetTextColor(LCD_COLOR_RED);
	} else {
		BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
	}
	uint16_t fWidth = BSP_LCD_GetFont()->Width;
	uint16_t fHeight = BSP_LCD_GetFont()->Height;

	uint32_t sWidth = BSP_LCD_GetXSize() / fWidth;
	uint32_t sHeight = BSP_LCD_GetYSize() / fHeight;

	size_t i = 0;
	for (; i < nbyte; i++) {
		if (buf[i] >= 0x20 && buf[i] < 0x7F) {
			BSP_LCD_DisplayChar((cPos % sWidth) * fWidth,
					(cPos / sWidth) * fHeight, buf[i]);
			cPos++;
		} else if (buf[i] == '\n') {
			cPos = cPos + sWidth - (cPos % sWidth);
			BSP_LCD_ClearStringLine(cPos/sWidth);
		} else if (buf[i] == '\r') {
			cPos = cPos - (cPos % sWidth);
		}
	}
	return i;
#endif // TRACE

	errno = ENOSYS;
	return -1;
}

// ----------------------------------------------------------------------------

#endif // !defined(OS_USE_SEMIHOSTING) && !(__STDC_HOSTED__ == 0)
