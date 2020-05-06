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