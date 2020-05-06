#pragma once

#include "htsql.h"

#define BREAKPOINT_LINENUMBER_LENGTH	10

typedef enum _BreakpointType {
	BREAKPOINT_TYPE_PERMINENT,
	BREAKPOINT_TYPE_TEMPORARY
} BreakpointType;

typedef enum _BreakpointState {
	BREAKPOINT_STATE_DELETED,
	BREAKPOINT_STATE_ACTIVE
} BreakpointState;

typedef struct _Breakpoint {
	int		id;
	wchar_t	fileURI[MAX_PATH];
	char	lineNumber[BREAKPOINT_LINENUMBER_LENGTH];
	BreakpointState		breakpointState;
	BreakpointType		breakpointType;
} Breakpoint;

typedef struct _Breakpoint_Node {
	Breakpoint			breakpoint;	
} Breakpoint_Node;
