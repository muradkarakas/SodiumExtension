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
