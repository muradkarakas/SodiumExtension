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

#include "pch.h"

#include "substr.h"

char *mkSubstr(SodiumSession *currentMKSession, char *source, int posint, int length) {
    
	char *pos = mkUTF8index(currentMKSession, source, posint-1);

    if (length == 0) {
        return mkStrdup(currentMKSession->heapHandle, pos, __FILE__, __LINE__);

    } else if (length > 0) {

        char *start = mkStrdup(currentMKSession->heapHandle, pos, __FILE__, __LINE__);
        char *end2   = mkUTF8index(currentMKSession, start, length);
        *end2 = '\0';
        char *retval = mkStrdup(currentMKSession->heapHandle, start, __FILE__, __LINE__);
        mkFree(currentMKSession->heapHandle, start);
        return retval;

    } if (length < 0) {

    }
    return NULL;
}
