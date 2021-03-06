/**
 * This file is part of Sodium Language project
 *
 * Copyright � 2020 Murad Karaka� <muradkarakas@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License v3.0
 * as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 *	https://choosealicense.com/licenses/gpl-3.0/
 */

#include "pch.h"

#include "refreshBlock.h"
#include "htsqlFunction.imp.h"
#include "yuarel.h"
#include "tagDataBlock.h"
#include "message.h"
#include "rowSelection.h"
#include "preDelete.h"
#include "delete.h"

bool runDatablockAccessHTSQLTriggerFunction(SodiumSession *session, HTSQLPage *page, TAGDataBlock *dataBlockInstance) {

    bool retval = true;
	char *trgFullName = mkStrcat(session->heapHandle, __FILE__, __LINE__,
		dataBlockInstance->dataBlockName,
		trgName_DatablockAccess,
		NULL);

    /** Step.1 */
    
    /** Step.2 */
	createANewFunctionCallStackStartPoint(session, page, trgFullName);

    /** Step.3 */

    /** Step.4 */

    /** Step.5 */
    PreObjectFunction *preObjectFunction = runPostFunction(session, page, trgFullName, true);

    /** Step.6 */
    if (preObjectFunction) {
        /** If data block "_access" function, we'll get its return value as the return value of this c function */
        if (page->currentCallStack->preObjectFunctionReturnValue) {
            if (page->currentCallStack->preObjectFunctionReturnValue->value.text) {
                retval  = (bool) (strcmp(page->currentCallStack->preObjectFunctionReturnValue->value.text, "true") == 0);
            } else {
				CLOG(ERR_FUNCTION_MUST_RETURN_BOOL, trgFullName);
            }
        } else {
            // Function declaration is invalid
			CLOG(ERR_FUNCTION_MUST_RETURN_BOOL, trgFullName);
        }
    } else {
        /** data block does not have "_access" function. this means it is a public function */
        retval = true;
    }

    /** Step.7 */
    cleanUpFunctionStack(session, page);

    /** Step.8 */

    mkFree(session->heapHandle, trgFullName);

    return retval;
}

void __sendRefreshBlockNativeResponse(SodiumSession *session, HTSQLPage *page, TAGDataBlock *dataBlockInstance, const char *cFunctionNameToBeCalled) {
	
	if (dataBlockInstance == NULL)
		return;

	EnterCriticalSection(&htsqlCriticalSection);

    mkDebug(session, SESSION_LOG_DETAIL_HTSQL_BUILTIN_CALL, "\n", "Data block is refreshing ... (", dataBlockInstance->dataBlockName, ") ...", NULL);
	increaseDebugIndentionCount(session);

    if (!dataBlockInstance) {
		CLOG(ERR_DATABLOCK_NOT_FOUND, dataBlockInstance->dataBlockName);
        return;
    }
    if (dataBlockInstance->dataSourceName == NULL && dataBlockInstance->autoGeneratedColumns == NULL) {
		CLOG(ERR_PROPERTY_CANNOT_BE_EMPTY, "data-source-name");
        return;
    }
    if (dataBlockInstance->connectionName == NULL) {
		CLOG(ERR_PROPERTY_CANNOT_BE_EMPTY, "connection-name");
        return; 
    }

    bool accessAllowed = runDatablockAccessHTSQLTriggerFunction(session, page, dataBlockInstance);
    if (accessAllowed) {
        // access allowed
        printTAGDataBlockDynamicContent(session, page, dataBlockInstance);
    } else {
        __sendDatablockAccessRejectedResponse(session, page, dataBlockInstance);
    }
	decreaseDebugIndentionCount(session);
    mkDebug(session, SESSION_LOG_DETAIL_HTSQL_BUILTIN_CALL, "\n", "Data block refreshed done. (", dataBlockInstance->dataBlockName, ")", NULL);

	LeaveCriticalSection(&htsqlCriticalSection);
}


void __sendDatablockAccessRejectedResponse(SodiumSession *session, HTSQLPage *page, TAGDataBlock *dataBlockInstance) {

    char *lastResponse = mkStrcat(session->heapHandle,
                                  __FILE__, __LINE__,
                                  "action = { "
                                                "\"type\"               : \"dataBlockAccessRejected\", "
                                                "\"dataBlockName\"      : \"", dataBlockInstance->dataBlockName, "\", "
                                                "\"clientActionCode\"   : \"0\" "
                                          " };",
                                  // For Tree element, DO NOT call "addNewActionToTheResponseActionList" function.
                                  "addNewActionToTheResponseActionList(action);",
                                  NULL);

    if (page->currentRequestMethodType == REQUEST_TYPE_GET) {
        mkPrint(session, "\n<script type=\"text/javascript\">\n", NULL);
    }

    mkPrint(session, lastResponse, NULL);

    if (page->currentRequestMethodType == REQUEST_TYPE_GET) {
        mkPrint(session, "\n</script>\n", NULL);
    }
    mkFree(session->heapHandle, lastResponse);
}



void __sendCreateRecordNativeResponse(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGDataBlock, const char *cFunctionNameToBeCalled) {
    mkDebug(session, SESSION_LOG_DETAIL_HTSQL_TRIGGER, "\n", "Create new record in (", pTAGDataBlock->dataBlockName, ") ...", NULL);
	increaseDebugIndentionCount(session);

	pTAGDataBlock->blockStatus = BLOCK_STATUS_CREATE_RECORD_MODE;
    __sendRefreshBlockNativeResponse(session, page, pTAGDataBlock, cFunctionNameToBeCalled);

	decreaseDebugIndentionCount(session);
    mkDebug(session, SESSION_LOG_DETAIL_HTSQL_TRIGGER, "\n", "Create new record done. (", pTAGDataBlock->dataBlockName, ")", NULL);
}

void __sendLastPageNativeResponse(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGDataBlock, const char *cFunctionNameToBeCalled) {
	if (pTAGDataBlock == NULL) {
		CLOG(ERR_CORE_INVALID_HTTP_REQUEST_PARAM, "No datablock name provided");
		return;
	}
	__sendMessageResponse(session, page, "Not implemented yet");
}

void __sendDeleteNativeResponse(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGDataBlock, const char *cFunctionNameToBeCalled) {

    __selectInternalRow(session, page, pTAGDataBlock, cFunctionNameToBeCalled);

    bool runTrigger = runPreDeleteHTSQLTriggerFunction(session, page, pTAGDataBlock);
    if (runTrigger == false) {
        __sendDeleteRejectedResponse(session, pTAGDataBlock, pTAGDataBlock->keyColumnValue);
        return;
    }

    clearClientMessage(session, page);
    __delete(session, page, pTAGDataBlock);
    const char *clientMessage = getClientMessage(session, page);
    if (clientMessage == NULL) {
        // everything is ok.
        __sendDeleteSuccessfulResponse(session, pTAGDataBlock, pTAGDataBlock->keyColumnValue);
        __sendRefreshBlockNativeResponse(session, page, pTAGDataBlock, cFunctionNameToBeCalled);
    }
}

void __sendFirstPageNativeResponse(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGDataBlock, const char *cFunctionNameToBeCalled) {
	if (pTAGDataBlock == NULL) {
		CLOG(ERR_CORE_INVALID_HTTP_REQUEST_PARAM, "No datablock name provided");
		return;
	}
    mkDebug(session, SESSION_LOG_DETAIL_HTSQL_TRIGGER, "\n", "First-page in (", pTAGDataBlock->dataBlockName, ") ...", NULL);
	increaseDebugIndentionCount(session);
	pTAGDataBlock->currentPageNo = 1;
    __sendRefreshBlockNativeResponse(session, page, pTAGDataBlock, cFunctionNameToBeCalled);
	decreaseDebugIndentionCount(session);
    mkDebug(session, SESSION_LOG_DETAIL_HTSQL_TRIGGER, "\n", "First-page done. (", pTAGDataBlock->dataBlockName, ")", NULL);
}


void __sendPrevPageNativeResponse(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGDataBlock, const char *cFunctionNameToBeCalled) {
	if (pTAGDataBlock == NULL) {
		CLOG(ERR_CORE_INVALID_HTTP_REQUEST_PARAM, "No datablock name provided");
		return;
	}
	mkDebug(session, SESSION_LOG_DETAIL_HTSQL_TRIGGER, "\n", "Previous-page in (", pTAGDataBlock->dataBlockName, ") ...", NULL);
	increaseDebugIndentionCount(session);

    if (pTAGDataBlock->currentPageNo > 1) {
		pTAGDataBlock->currentPageNo--;
        __sendRefreshBlockNativeResponse(session, page, pTAGDataBlock, cFunctionNameToBeCalled);
    }

	decreaseDebugIndentionCount(session);
    mkDebug(session, SESSION_LOG_DETAIL_HTSQL_TRIGGER, "\n", "Previous-page done. (", pTAGDataBlock->dataBlockName, ")", NULL);
}

void __sendNextPageNativeResponse(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGDataBlock, const char *cFunctionNameToBeCalled) {
	if (pTAGDataBlock == NULL) {
		CLOG(ERR_CORE_INVALID_HTTP_REQUEST_PARAM, "No datablock name provided");
		return;
	}
	mkDebug(session, SESSION_LOG_DETAIL_HTSQL_TRIGGER, "\n", "Next-page in (", pTAGDataBlock->dataBlockName, ") ...", NULL);
	increaseDebugIndentionCount(session);
	pTAGDataBlock->currentPageNo++;
    __sendRefreshBlockNativeResponse(session, page, pTAGDataBlock, cFunctionNameToBeCalled);
	decreaseDebugIndentionCount(session);
    mkDebug(session, SESSION_LOG_DETAIL_HTSQL_TRIGGER, "\n", "Next-page done. (", pTAGDataBlock->dataBlockName, ")", NULL);
}

void __sendRefreshBlockHTSQLResponse(SodiumSession *session, HTSQLPage *page, char *blockname) {
    char *lastResponse = mkStrcat(session->heapHandle, __FILE__, __LINE__,
                                  "action = { "
                                                "\"type\"            : \"refreshBlockHTSQL\", "
                                                "\"dataBlockName\"   : \"", blockname, "\" "
                                          " };",
                                  // standart part
                                  "addNewActionToTheResponseActionList(action);",
                                  NULL);
    mkPrint(session, lastResponse, NULL);
    mkFree(session->heapHandle, lastResponse);
}

void __sendExecuteQueryNativeResponse(SodiumSession *session, HTSQLPage *page, TAGDataBlock *dataBlockInstance, const char *cFunctionNameToBeCalled) {
    char *where = NULL;

    mkDebug(session, SESSION_LOG_DETAIL_HTSQL_TRIGGER, "\n", "Execute-Query executing ... (", dataBlockInstance->dataBlockName, ") ...", NULL);
	increaseDebugIndentionCount(session);

    for(int index = 0; index < page->totalHTTPFormVariableCount; index++) {

        if (strcmp(page->formDataStructureSmall[index].key, "datablockname") != 0 &&
            strcmp(page->formDataStructureSmall[index].key, "htsqlproceduretorun") != 0 &&
            strcmp(page->formDataStructureSmall[index].key, "nativeproceduretorun") != 0
            ) {

            if (page->formDataStructureSmall[index].decodedKey[0] == ':') {
                // Only data block level items will be added to the query. Control block item value is skipped.
                continue;
            }

            char *temp;
            const char *columnName = getBlockItemColumnName(session, page, dataBlockInstance, page->formDataStructureSmall[index].key);
            if (strlen(page->formDataStructureSmall[index].decodedVal) == 0) {
                temp = mkStrcat(session->heapHandle, __FILE__, __LINE__, columnName, " is null ", NULL);
            } else {
                temp = mkStrcat(session->heapHandle, __FILE__, __LINE__, columnName, " like '", page->formDataStructureSmall[index].decodedVal, "'", NULL);
            }

            if (where == NULL) {
                where = mkStrdup(session->heapHandle, temp, __FILE__, __LINE__);
            } else {
                char *temp1 = mkStrcat(session->heapHandle, __FILE__, __LINE__, where, " and ", temp, NULL);
                mkFree(session->heapHandle, where);
                where = temp1;
            }
            mkFree(session->heapHandle, temp);
        }
    }
    if (where==NULL || strlen(where)==0) {
        dataBlockInstance->executeQueryModeWhereClause = NULL;
    } else {
        dataBlockInstance->executeQueryModeWhereClause = mkStrdup(session->heapHandle, where, __FILE__, __LINE__);
    }

	dataBlockInstance->currentPageNo = 1;
    __sendRefreshBlockNativeResponse(session, page, dataBlockInstance, cFunctionNameToBeCalled);

    if (dataBlockInstance->executeQueryModeWhereClause) {
        mkFree(session->heapHandle, dataBlockInstance->executeQueryModeWhereClause);
        dataBlockInstance->executeQueryModeWhereClause = NULL;
    }

    if (where) {
        mkFree(session->heapHandle, where);
    }

	decreaseDebugIndentionCount(session);
    mkDebug(session, SESSION_LOG_DETAIL_HTSQL_TRIGGER, "\n", "Execute-Query done. (", dataBlockInstance->dataBlockName, ")", NULL);
}





