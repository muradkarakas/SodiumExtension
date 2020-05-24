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

#include "pch.h"

#include "callStack.h"

void
InitCallstack(
	SodiumSession* session
) 
{
	session->currentFunctionCallDepth = 0;
}

int 
GetCallstackDepth(
	SodiumSession *session, 
	HTSQLPage *page
) 
{
	int callstackDepth = 0;
	CallStack *current = page->currentCallStack;
	if (current == NULL) {
		return callstackDepth;
	}
	while (current) {
		callstackDepth++;
		current = current->prev;
	}
	return callstackDepth;
}

void 
ReNumberCallstack(
	SodiumSession *session, 
	HTSQLPage *page
) 
{
	int callstackDepth = GetCallstackDepth(session, page);
	CallStack *current = page->currentCallStack;
	for (int i = callstackDepth-1; current && i >= 0; i--) {
		current->preObjectFunction->callStackId = i;
		current = current->prev; 
	}
}

BOOL 
PushCallStack(
	SodiumSession *session, 
	HTSQLPage *page, 
	PreObjectGeneric *htsqlFuncionInstance
) 
{	
	session->currentFunctionCallDepth++;
		
	// Lets set "rootLocalPostObjectGenericObjects" variable to the current function's local variable and arguments root object instance. 
	// It is a shortcut to the all arguments & variables for the function currently executed. This variable will be used especially by the debugger.
//	page->currentCallStack->preObjectFunction->rootLocalPostObjectGenericObjects = page->currentCallStack->rootPostObjectGeneric;
	
	page->currentCallStack->preObjectFunction = htsqlFuncionInstance->value.preObjectFunction;

	if (IsDebuggerPresent()) {
		ReNumberCallstack(session, page);
	}

	return TRUE;
}

BOOL 
PopCallStack(
	SodiumSession *session, 
	HTSQLPage *page, 
	PreObjectGeneric *htsqlFuncionInstance
) 
{
	session->currentFunctionCallDepth--;
	
	if (IsDebuggerPresent()) {
		ReNumberCallstack(session, page);
	}
	return TRUE;
}