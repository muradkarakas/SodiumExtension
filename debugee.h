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

/*	*/
void setAllPreviousCallstackFunctionsDebugStepType(SodiumSession *currentMKSession, CallStack *cls, Debug_Step_Type stepType);

//void debuggerEventOccured(SodiumSession *currentMKSession, HTSQLPage *page, const char *message);

void debugeeCreatePipeServerThread(void);


void raiseBreakPointException(SodiumSession *currentMKSession);
void increaseInnerLineNumber(SodiumSession *currentMKSession);

/*	
	returns the breakpoint id if exists.
*/
int
IsBreakpointHit(
	SodiumSession* session,
	const wchar_t* fileURI,
	char* lineNumberStr
);

int
GetActiveBreakpointCount(
	SodiumSession* session
);

/*	Returns breakpoint type of an Active Breakpoint. 
	If pBreakpointId is not exists, returns -1
*/
BreakpointType getBreakpointType(SodiumSession *currentMKSession, int pBreakpointId);

/*	Sets the type of an Active Breakpoint to a new type.
*/
void setBreakpointType(SodiumSession *currentMKSession, int pBreakpointId, BreakpointType pBreakpointType);

/*	Deletes breakpoints
*/
void
DeleteBreakpoint(
	SodiumSession* session,
	int pBreakpointId
);
