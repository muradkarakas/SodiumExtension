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

#include "debugee.h"

Function_Debug_Info	* 
CreateFunctionDebugInfo(
	SodiumSession *session, 
	HTSQLPage *page
) 
{
	Function_Debug_Info		*functionDebugInfo = mkMalloc(session->heapHandle, sizeof(Function_Debug_Info), __FILE__, __LINE__);
	functionDebugInfo->functionDebugStepType = DEBUG_STEP_TYPE_STEP_CONTUNIE;
	wcsncpy_s(functionDebugInfo->fileURI, MAX_PATH, session->sessionDebugInfo->fileURI, MAX_PATH);
	strcpy_s(functionDebugInfo->functionName, MAX_FUNCTION_NAME_LENGTH, page->currentCallStack->closureName);
	functionDebugInfo->lineNumberOuter = page->currentCallStack->currentPostObjectGeneric->value.preObjectFunction->lineNumberOuter;
	return functionDebugInfo;
}

void destroyFunctionDebugInfo(SodiumSession *currentMKSession, HTSQLPage *page) {
	if (page->currentCallStack->functionDebugInfo) {
		mkFree(currentMKSession->heapHandle, page->currentCallStack->functionDebugInfo);
		page->currentCallStack->functionDebugInfo = NULL;
	}
}
