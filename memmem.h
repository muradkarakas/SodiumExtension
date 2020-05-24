/**
 * This file is part of Sodium Language project
 *
 * Copyright © 2020 Murad Karakaþ <muradkarakas@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License v3.0
 * as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 *	https://choosealicense.com/licenses/gpl-3.0/
 */

#pragma once

#include <stddef.h>
#include <string.h>

#ifndef _LIBC
	#define __builtin_expect(expr, val)   (expr)
#endif

#undef memmem

void *mkMemmem(const void *haystack, size_t haystack_len, const void *needle, size_t needle_len);

