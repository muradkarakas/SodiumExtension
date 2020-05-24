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

typedef enum _Debug_Step_Type {
	DEBUG_STEP_TYPE_STEP_CONTUNIE,
	DEBUG_STEP_TYPE_STEP_OVER,
	DEBUG_STEP_TYPE_STEP_IN,
	DEBUG_STEP_TYPE_STEP_OUT
} Debug_Step_Type;

/*	Function_Debug_Info structure is used in post parse/execution phase
*/
typedef struct _Function_Debug_Info {
	wchar_t				fileURI[MAX_PATH];		// set by main lexer/parser routine 
	char				functionName[MAX_FUNCTION_NAME_LENGTH];
	short				lineNumberOuter;		// set by pre parse phase for each HTSQL function
	short				lineNumberInner;		// 
	//short				ifTotalLineCount;
	Debug_Step_Type		functionDebugStepType;				
} Function_Debug_Info;

/*	Session_Debug_Info structure is used in pre parse phase
*/
typedef struct _Session_Debug_Info {
	wchar_t			fileURI[MAX_PATH];	
	short			lineNumberOuter;
	// Last debug command type (which is executed by programmer) is stored in this variable. 
	Debug_Step_Type	sessionDebugStepType;
} Session_Debug_Info;

