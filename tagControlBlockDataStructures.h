#pragma once


typedef struct _ControlBlockVariable {
	char *cbName;
	char *cbValue;
	struct _ControlBlockVariable *next;
} ControlBlockVariable;