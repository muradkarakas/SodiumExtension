/**
 * This file is part of Sodium Language project
 *
 * Copyright � 2020 Murad Karaka� <muradkarakas@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License v3.0
 * as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 *	https://choosealicense.com/licenses/gpl-3.0/
 */

#include "pch.h"

#include "replace.h"


char *mkReplace(SodiumSession *currentMKSession, const char * source, const char * const oldW, const char * const newW) {
	char *result;
	const char * s = source;
	size_t i;
	int cnt = 0;
	size_t newWlen = strlen(newW);
	size_t oldWlen = strlen(oldW);

	// Counting the number of times old word
	// occur in the string
	for (i = 0; s[i] != '\0'; i++)
	{
		if (strstr(&s[i], oldW) == &s[i])
		{
			cnt++;

			// Jumping to index after the old word.
			i += oldWlen - 1;
		}
	}

	// Making new string of enough length
	//result = (char *)malloc(i + cnt * (newWlen - oldWlen) + 1);
	size_t len = i + cnt * (newWlen - oldWlen) + 1;
	result = (char *)mkMalloc(currentMKSession->heapHandle, len + 1, __FILE__, __LINE__);

	i = 0;
	while (*s)
	{
		// compare the substring with the result
		if (strstr(s, oldW) == s) {
			strcat_s(&result[i], len-i, newW);
			i += newWlen;
			s += oldWlen;
		}
		else
			result[i++] = *s++;
	}

	result[i] = '\0';
	return result;
}