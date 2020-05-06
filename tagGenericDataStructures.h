#pragma once

#include "tagInputDataStructures.h"

typedef struct structTAGGeneric {

	int         tagTOKEN;
	char        *tagSemanticValue;
	BOOL        isDynamicContent;
	TAGInput    *tagInput;
	void        *next;
} TAGGeneric;
