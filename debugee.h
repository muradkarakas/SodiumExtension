#pragma once

#include "htsql.h"

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
