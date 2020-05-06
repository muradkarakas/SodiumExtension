#pragma once

#include <stddef.h>
#include <string.h>

#ifndef _LIBC
	#define __builtin_expect(expr, val)   (expr)
#endif

#undef memmem

void *mkMemmem(const void *haystack, size_t haystack_len, const void *needle, size_t needle_len);

