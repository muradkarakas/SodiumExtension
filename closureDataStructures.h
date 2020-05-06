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