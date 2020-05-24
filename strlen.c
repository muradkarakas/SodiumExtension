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

#include "strlen.h"


char *mkStrlen(SodiumSession *currentMKSession, const char *source) {

    // This function takes care of UTF8 encoding
    char *retval = NULL;
    char *posStr = mkMalloc(currentMKSession->heapHandle, 20, __FILE__, __LINE__);
    size_t len = 0;

    for (; *source; ++source) {
        if ((*source & 0xC0) != 0x80) {
            ++len;
        }
    }

	mkItoa(len, posStr);

    retval = mkStrdup(currentMKSession->heapHandle, posStr, __FILE__, __LINE__);

    mkFree(currentMKSession->heapHandle, posStr);

    return retval;
}
