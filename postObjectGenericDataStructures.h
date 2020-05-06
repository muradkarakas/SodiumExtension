#pragma once

#include "preObjectVariableDataStructures.h"
#include "postObjectResultSetDataStructures.h"


typedef enum _ObjectGenericType {
	POST_OBJECT_GENERIC_TYPE_VARIABLE,
	POST_OBJECT_GENERIC_TYPE_FUNCTION,
	POST_OBJECT_GENERIC_TYPE_FUNCTION_ARGS
} ObjectGenericType;


/**
*  ObjectGeneric data type is a linked list type.
Each ObjectGeneric instance stores post phase/local variable and point another ObjectGeneric instance for another variable.
Post phase/local variables can be any one of the predefined types. Allowed types are stored in each ObjectGeneric instance variable declared as is
>  ObjectGenericType            postObjectType
\n
\n
There is no need to define a new object type for phase/local variable variables.
So, PreObjectVariable variable type is used for pre and post variables.  */

struct _UserObjectGeneric {

	char					*objectName;
	ObjectGenericType       objectGenericType;
	
	union {
		PreObjectVariable	*preObjectVariable;
		PreObjectFunction   *preObjectFunction;		
	} value;

	struct _UserObjectGeneric *next;
};

