#include "pch.h"

#include "htsqlFunction.imp.h"

#include "yuarel.h"
#include "tagControlBlock.h"
#include "postObjectGeneric.h"
#include "callStack.h"
#include "post.parser.imp.h"
#include "preObjectGeneric.h"
#include "preObjectVariable.h"
#include "tagConnection.h"
#include "closure.h"

void addDataBlockDataItemsToLocalStack(SodiumSession *session, HTSQLPage *page, const char *blockname, const char *procedureName) {
    for(int index = 0; index < page->totalHTTPFormVariableCount; index++) {
        if (strcmp(page->formDataStructureSmall[index].key, "datablockname") != 0 &&
            strcmp(page->formDataStructureSmall[index].key, "htsqlproceduretorun") != 0 &&
            strcmp(page->formDataStructureSmall[index].key, "nativeproceduretorun") != 0)
        {
            char *cbItemName = page->formDataStructureSmall[index].decodedKey;
            if (cbItemName[0] != ':') {
                char *blockItemName = mkStrcat(session->heapHandle, __FILE__, __LINE__, ":", blockname, ".", page->formDataStructureSmall[index].decodedKey, NULL);
                postFunctionAddArgumentsOrVariables(session, page, POST_OBJECT_GENERIC_TYPE_FUNCTION_ARGS, PREOBJECTTYPE_VARIABLE_VARCHAR, blockItemName, page->formDataStructureSmall[index].decodedVal);
                mkFree(session->heapHandle, blockItemName);
            }
        }
    }
}

void addControlBlockDataItemsToLocalStack(SodiumSession *session, HTSQLPage *page) {
    for(int index = 0; index < page->totalHTTPFormVariableCount; index++) {
        char *cbItemName = page->formDataStructureSmall[index].decodedKey;
        if (cbItemName[0] == ':') {
            char *cbItemValue = page->formDataStructureSmall[index].decodedVal;
            if (cbItemValue && strlen(cbItemValue) > 0) {
                createControlBlockVariable(session, page, cbItemName, cbItemValue);
            } else { 
                createControlBlockVariable(session, page, cbItemName, NULL);
            }
        }   
    }
}

void postFunctionAddArgumentsOrVariables(SodiumSession *session, HTSQLPage *page, ObjectGenericType genericObjectType, PREOBJECTVARIABLETYPE primitiveVariableType, const char *pVariableName, const char *pPreObjectVariableValue) {
    PreObjectVariable *preObjectVariable = findPostObjectGenericPostObjectVariable(session, page, pVariableName);
    if (preObjectVariable) {
		CLOG(ERR_NAME_ALREADY_IN_USE, pVariableName);
    } else {
        PreObjectVariable *preObjectVariable = createPreObjectVariable(session, primitiveVariableType, pVariableName, pPreObjectVariableValue);
        createPostObjectGeneric(session, page, genericObjectType, pVariableName, preObjectVariable);
    } 
}

void createANewFunctionCallStackStartPoint(SodiumSession *session, HTSQLPage *page, const char *functionName) {

	CallStack *newClosure = mkMalloc(session->heapHandle, sizeof(CallStack), __FILE__, __LINE__);
	newClosure->closureName = mkStrdup(session->heapHandle, functionName, __FILE__, __LINE__);
	newClosure->preObjectFunction = NULL;
	newClosure->rootPostObjectGeneric = NULL;
	newClosure->currentPostObjectGeneric = NULL;
	newClosure->preObjectFunctionReturnValue = NULL;
	newClosure->lastPreObjectFunctionPointerExecuted = NULL;
	newClosure->next = NULL;
	newClosure->prev = NULL;

	if (page->currentCallStack == NULL) {
		page->currentCallStack = newClosure; 
	}
	else {
		page->currentCallStack->next = newClosure;
		newClosure->prev = page->currentCallStack;
		page->currentCallStack = newClosure;
	}	

    PreObjectVariable *preObjectVariable    = createPreObjectVariableVarchar(session, PREOBJECTTYPE_VARIABLE_VARCHAR, functionName, NULL);
	PostObjectGeneric *postObjectGeneric    = createPostObjectGeneric(session, page, POST_OBJECT_GENERIC_TYPE_FUNCTION, functionName, preObjectVariable);
	
	if (page->currentCallStack->functionDebugInfo == NULL) {
		page->currentCallStack->functionDebugInfo = CreateFunctionDebugInfo(session, page);
	}	
}


void runPostFunctionFast(SodiumSession *session, HTSQLPage *page, PreObjectGeneric *htsqlFuncionInstance) {

	clock_t start, end;
	start = clock();
	char cBuf[100]; 

	//session->currentFunctionCallDepth++;
	mkDebug(session, SESSION_LOG_DETAIL_HTSQL_FUNCTION_CALL, "\n", " ", NULL);
	mkDebug(session, SESSION_LOG_DETAIL_HTSQL_FUNCTION_CALL, "\n", "function '", htsqlFuncionInstance->objectName, "' is running...", NULL);

	/** Return value of the HT/SQL function will be stored in the properties of the preObjectGeneric instance.
	We are not creating a new instance. Instead just referencing to that instance */
	
	PushCallStack(session, page, htsqlFuncionInstance);
	
	// ***** init debug info for currrent function
	page->currentCallStack->functionDebugInfo->lineNumberInner = 0;
	page->currentCallStack->returnStatementActive = false;
	if (session->sessionDebugInfo->sessionDebugStepType == DEBUG_STEP_TYPE_STEP_IN) {
		mkDebug(session, SESSION_LOG_DETAIL_HTSQL_FUNCTION_CALL, " (STEP IN)", NULL);
		page->currentCallStack->functionDebugInfo->functionDebugStepType = DEBUG_STEP_TYPE_STEP_OVER;
	}
	else {
		mkDebug(session, SESSION_LOG_DETAIL_HTSQL_FUNCTION_CALL, " (STEP OVER)", NULL);
		page->currentCallStack->functionDebugInfo->functionDebugStepType = DEBUG_STEP_TYPE_STEP_CONTUNIE;
	}
	// ***** end of init debug info

	// Backing up & Setting active database if it belongs to any one of the datablocks
	TAGConnection *backupOfActiveTAGConnection = session->activeTAGConnection;
	setActiveDatabaseConnectionForHTSQLFunction(session, page, htsqlFuncionInstance->objectName);

	parseAndExecutePostString(session, page, page->currentCallStack->preObjectFunction->preObjectFunctionBodyStrDeclaration);
	
	PopCallStack(session, page, htsqlFuncionInstance);

	if (IsDebuggerPresent()) {
		// Is this the last function to be executed and ended ?
		int callStackDepth = GetCallstackDepth(session, page);
		if (callStackDepth <= 1) {
			// Yes. Send continue command to debugger in order to send contiue command to IDE
			session->sessionDebugInfo->sessionDebugStepType = DEBUG_STEP_TYPE_STEP_CONTUNIE;
			//OutputDebugString("Continuing.\r\n");
		}
	}

	// restoring active database connection back
	if (backupOfActiveTAGConnection)
		session->activeTAGConnection = backupOfActiveTAGConnection;

	end = clock();
	clock_t timePassed = (end - start) / CLOCKS_PER_SEC;
	//mkConvertFloatToChar(timePassed, cBuf, 100);
	_ltoa_s(timePassed, cBuf, 100, 10);
	mkDebug(session, SESSION_LOG_DETAIL_HTSQL_FUNCTION_CALL, "\n", "Done in (", cBuf, ") msec. '", htsqlFuncionInstance->objectName, "'\n", NULL);
	//session->currentFunctionCallDepth--;
}

void setActiveDatabaseConnectionForHTSQLFunction(SodiumSession *session, HTSQLPage *page, const char *htsqlFunctionName) {

	char *blocknamePartOfTheFunctionNameStr = getBlocknameFromHTSQLFunctionName(session, page, htsqlFunctionName);
	if (blocknamePartOfTheFunctionNameStr == NULL) {
		return;
	}

	TAGDataBlock *tagDataBlock = findTAGDataBlockByName(session, page, blocknamePartOfTheFunctionNameStr);
	if (tagDataBlock && tagDataBlock->connectionName) {
		TAGConnection *tagConnection = findTAGConnectionByName(session, page, tagDataBlock->connectionName, true);
		if (tagConnection) {
			setActiveTAGConnection(session, page, tagConnection);
		}
	}
	
	mkFree(session->heapHandle, blocknamePartOfTheFunctionNameStr);
}

char *getBlocknameFromHTSQLFunctionName(SodiumSession *session, HTSQLPage *page, const char *htsqlFunctionName) {
	if (htsqlFunctionName == NULL)
		return NULL;

	// check htsqlFunctionName has at least one period character.
	char *firstPeriodCharPosition = strchr(htsqlFunctionName, '.');
	if (firstPeriodCharPosition == NULL) {
		return NULL;
	}

	// check htsqlFunctionName has more than one period character.
	char *secondPeriodCharPosition = strchr(firstPeriodCharPosition+1, '.');
	if (secondPeriodCharPosition != NULL) {
		return NULL;
	}

	char *copyOfOrginalFunctionName = mkStrdup(session->heapHandle, htsqlFunctionName, __FILE__, __LINE__);

	firstPeriodCharPosition = strchr(copyOfOrginalFunctionName, '.');
	*((char*) firstPeriodCharPosition) = '\0';

	return copyOfOrginalFunctionName;
}

PreObjectFunction* runPostFunction(SodiumSession *session, HTSQLPage *page, const char *htsqlFunctionName, bool ignoreNotFound) {

    char *localHtsqlFunctionName = mkStrdup(session->heapHandle, htsqlFunctionName, __FILE__, __LINE__);

    PreObjectGeneric *preObjectGeneric = findPreGenericObjectByName(session, page, localHtsqlFunctionName);

    if (preObjectGeneric == NULL) {
        if (! ignoreNotFound) {
            if (strchr(htsqlFunctionName, '.') != NULL) {
                const char *htsqlFunctionNameWithoutBlockName = (char*) (strchr(htsqlFunctionName, '.') + 1);
                preObjectGeneric = findPreGenericObjectByName(session, page, htsqlFunctionNameWithoutBlockName);
                if (preObjectGeneric == NULL) {
                    mkDebug(session, SESSION_LOG_DETAIL_HTSQL_FUNCTION_CALL, "\n", ETEXT(ERR_FUNCTION_NOT_FOUND), localHtsqlFunctionName, NULL);
					CLOG(ERR_FUNCTION_NOT_FOUND, localHtsqlFunctionName);
                }
            } else {
				CLOG(ERR_FUNCTION_NOT_FOUND, localHtsqlFunctionName);
            }
        } else {
            //mkDebug(session, SESSION_LOG_DETAIL_HTSQL_FUNCTION_CALL, "\n", "Opsiyonel HT/SQL fonksiyon pass geçildi : '", localHtsqlFunctionName, "'", NULL);
        }
    }

    if (preObjectGeneric) {
		runPostFunctionFast(session, page, preObjectGeneric);
    } 

    mkFree(session->heapHandle, localHtsqlFunctionName);

    return (preObjectGeneric && preObjectGeneric->value.preObjectFunction) ? preObjectGeneric->value.preObjectFunction : NULL;
}


void cleanUpFunctionStack(SodiumSession *session, HTSQLPage *page) {
	if (page) {
		if (page->currentCallStack) {
			
			// destroying local variables
			PostObjectGeneric *object = page->currentCallStack->rootPostObjectGeneric;
			while (object) {
				PostObjectGeneric *objectTemp = (object->next) ? object->next : NULL;
				destroyPostObjectGeneric(session, object);
				mkFree(session->heapHandle, object);
				object = objectTemp;
			}
			page->currentCallStack->rootPostObjectGeneric = NULL;
			page->currentCallStack->currentPostObjectGeneric = NULL;

			// destroying function debug info structure
			destroyFunctionDebugInfo(session, page);

			// destroying closure name
			if (page->currentCallStack->closureName) {
				mkFree(session->heapHandle, page->currentCallStack->closureName);
				page->currentCallStack->closureName = NULL;
			}

			// destroying return value
			destroyPreObjectVariable(session, page->currentCallStack->preObjectFunctionReturnValue);
			page->currentCallStack->preObjectFunctionReturnValue = NULL;

			// modifying linked list before destroying closure
			if (page->currentCallStack->prev == NULL && page->currentCallStack->next == NULL) {
				// only element in linked list
				mkFree(session->heapHandle, page->currentCallStack);
				page->currentCallStack = NULL;
			}
			else if (page->currentCallStack->prev != NULL && page->currentCallStack->next == NULL) {
				// last element in linked list
				CallStack *prev = page->currentCallStack->prev;
				prev->lastPreObjectFunctionPointerExecuted = page->currentCallStack->preObjectFunction;
				mkFree(session->heapHandle, page->currentCallStack);
				prev->next = NULL;
				page->currentCallStack = prev;
			}
			
		}
	}	
}

