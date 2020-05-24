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




/** \brief
 *
 * \param currentMKSession MKSession*
 * \param source const char*            A text expression that is the base string from which the substring is created.
 * \param posint int                    The position at which the first character of the returned string begins. When position is 0 (zero), then it is treated as 1. When position is positive, then the function counts from the beginning of string to find the first character. When position is negative, then the function counts backward from the end of string.
 * \param length                        The length of the returned string. SUBSTR calculates lengths using characters as defined by the input character set. SUBSTRB uses bytes instead of characters. SUBSTRC uses Unicode complete characters. SUBSTR2 uses UCS2 code points. SUBSTR4 uses UCS4 code points. When you do not specify a value for this argument, then the function returns all characters to the end of string. When you specify a value that is less than 1, the function returns NA
 * \return char*
 *
 */
char *mkSubstr(SodiumSession *currentMKSession, char *source, int posint, int length);


