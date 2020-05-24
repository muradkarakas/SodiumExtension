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

#include "breakpoints.imp.h"
 
void clearAllBreakpoints(SodiumSession *currentMKSession) {
	if (currentMKSession) {
		for (int i = 0; i < MAX_BREAKPOINT_COUNT; i++) {
			/*currentMKSession->breakpointsRoot[i].breakpoint.fileURI[0] = '\0';
			currentMKSession->breakpointsRoot[i].breakpoint.id = 0;
			currentMKSession->breakpointsRoot[i].breakpoint.lineNumber[0] = '\0';*/
			currentMKSession->breakpointsRoot[i].breakpoint.breakpointState = BREAKPOINT_STATE_DELETED;
		}
		currentMKSession->breakpointsRootAddress = &(currentMKSession->breakpointsRoot[0]);
	}
}
