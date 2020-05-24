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



struct _Callable {
	char                *closureName;
	PreObjectFunction   *preObjectFunction;
	Function_Debug_Info	*functionDebugInfo;
	
	PostObjectGeneric	*rootPostObjectGeneric;		//	Holds the local variable of the user function to be executed in a linked list.
	PostObjectGeneric	*currentPostObjectGeneric;	//	Holds the local variable of the user function to be executed in a linked list.

	int                 parameterCount;
	PreObjectVariable	*preObjectFunctionParameters[MAX_FUNCTION_PARAMETERS_COUNT];

	PreObjectVariable   *preObjectFunctionReturnValue;

	/** \brief  This variable is used to get which user function was executed and returned/exited.
	*
	*  If user function is executed in a context which is expected to return a value but return nothing,
	*          error response message is send to the client using the properties of this variable.
	*/
	PreObjectFunction	*lastPreObjectFunctionPointerExecuted;

	BOOL				returnStatementActive;

	struct _Callable	*prev;
	struct _Callable	*next;
};