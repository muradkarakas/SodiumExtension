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

#include "preObjectVariableDataStructures.h"
#include "preObjectGenericDataStructures.h"



typedef struct _PreObjectFunction {

	int                                                             parameterCountDeclaration;
	PreObjectVariable  *preObjectFunctionParametersDeclaration[MAX_FUNCTION_PARAMETERS_COUNT];
	PreObjectVariable                                *preObjectFunctionReturnValueDeclaration;
	char                                                 *preObjectFunctionBodyStrDeclaration;

	// Points to local variables and argument linked list root object instance..
	// DO NOT DESTROY them. It is just a shortcut. They will be destroyed by creator.
	PostObjectGeneric	*rootLocalPostObjectGenericObjects;
	short				lineNumberOuter;
	int					callStackId;

} PreObjectFunction;