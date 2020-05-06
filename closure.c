#include "stdafx.h"

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
