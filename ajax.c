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

#include "ajax.h"

#include "dispatcher.h"
#include "htsqlFunction.imp.h"
#include "tagControlBlock.h"
#include "tagDataBlock.h"
#include "refreshBlock.h"
#include "rowSelection.h"
#include "itemModified.h"
#include "rollback.h"
#include "commit.h"
#include "nodeExpanded.h"
#include "nodeSelected.h"

void serveAsAjaxRequest(SodiumSession *session, HTSQLPage *page) {

	BOOL lHtsql     = false;
	BOOL lNative    = false;

    const char *blockname     = GetQueryStringParam(session, page, "datablockname");
    if (blockname == NULL) {
        blockname     = GetQueryStringParam(session, page, "controlblockname");
    }
    const char *procedureName = GetQueryStringParam(session, page, "nativeproceduretorun");
    if (procedureName) {
        lNative = true;
    } else {
        procedureName = GetQueryStringParam(session, page, "htsqlproceduretorun");
        if (procedureName) {
            lHtsql = true;
        }
    }

    if (lNative && lHtsql) {
		CLOG(ERR_CORE_INVALID_REQUEST_TYPE_1, "");
        return;
    }
    if (!lNative && !lHtsql) {
		/* TODO: Eðer tablo veya viewdaki sutun sayýsý fazla olursa datablock row-selection event'ý oluþmuyor ve 'nativeproceduretorun' deðeri submit edilmediði için bu satýrdaki hata oluþuyor. Bir kýsýtlamamý var acaba */
		CLOG(ERR_CORE_INVALID_REQUEST_TYPE_2, "");
        return;
    }
	
    /* legal calls start here */
    addControlBlockDataItemsToLocalStack(session, page);

    /** If  "kill-session" native request is called, We cannot go. Release all memory for the session and return */
    if (strcmp(procedureName, "kill-session") == 0) {
        __sendKillSessionNativeResponse(session);

        /** PERFECTLY LEGAL "RETURN" statement
            Session dead now.
        */
        return;
    }
    if (lHtsql) {
        serveAsAjaxRequestTypeHTSQL(session, page, blockname, procedureName);
    } else if (lNative) {
        serveAsAjaxRequestTypeNative(session, page, blockname, procedureName);
    }
    destoryControlBlockVariables(session, page);
}



void serveAsAjaxRequestTypeNative(SodiumSession *currentMKSession, HTSQLPage *page, const char *blockname, const char *cFunctionNameToBeCalled) {
	
	TAGDataBlock *pTAGDataBlock = findTAGDataBlockByName(currentMKSession, page, blockname);

    if (strcmp(cFunctionNameToBeCalled, "refresh-block") == 0 || strcmp(cFunctionNameToBeCalled, "cancel-query") == 0) {
        __sendRefreshBlockNativeResponse(currentMKSession, page, pTAGDataBlock, cFunctionNameToBeCalled);
    } else if (strcmp(cFunctionNameToBeCalled, "execute-query") == 0) {
        __sendExecuteQueryNativeResponse(currentMKSession, page, pTAGDataBlock, cFunctionNameToBeCalled);
    } else if (strcmp(cFunctionNameToBeCalled, "next-page") == 0) {
        __sendNextPageNativeResponse(currentMKSession, page, pTAGDataBlock, cFunctionNameToBeCalled);
    } else if (strcmp(cFunctionNameToBeCalled, "first-page") == 0) {
        __sendFirstPageNativeResponse(currentMKSession, page, pTAGDataBlock, cFunctionNameToBeCalled);
    } else if (strcmp(cFunctionNameToBeCalled, "last-page") == 0) {
        __sendLastPageNativeResponse(currentMKSession, page, pTAGDataBlock, cFunctionNameToBeCalled);
    } else if (strcmp(cFunctionNameToBeCalled, "prev-page") == 0) {
        __sendPrevPageNativeResponse(currentMKSession, page, pTAGDataBlock, cFunctionNameToBeCalled);
    } else if (strcmp(cFunctionNameToBeCalled, "select-row") == 0) {
        __selectRowNativeResponse(currentMKSession, page, pTAGDataBlock, cFunctionNameToBeCalled);
    } else if (strcmp(cFunctionNameToBeCalled, "item-modified") == 0) {
        __itemModifiedNativeResponse(currentMKSession, page, pTAGDataBlock, cFunctionNameToBeCalled);
    } else if (strcmp(cFunctionNameToBeCalled, "rollback") == 0) {
        __rollback(currentMKSession, page, NULL);
        if (blockname && strlen(blockname) > 0) {
            __sendRefreshBlockNativeResponse(currentMKSession, page, pTAGDataBlock, cFunctionNameToBeCalled);
        }
    } else if (strcmp(cFunctionNameToBeCalled, "commit") == 0) {
        __commit(currentMKSession, page, NULL);
    } else if (strcmp(cFunctionNameToBeCalled, "delete-record") == 0) {
        __sendDeleteNativeResponse(currentMKSession, page, pTAGDataBlock, cFunctionNameToBeCalled);
    } else if (strcmp(cFunctionNameToBeCalled, "create-record") == 0) {
        __sendCreateRecordNativeResponse(currentMKSession, page, pTAGDataBlock, cFunctionNameToBeCalled);
    }
}

void serveAsAjaxRequestTypeHTSQL(SodiumSession * session, HTSQLPage *page, const char *blockname, const char *htsqlProcedureNameToBeCalled) {

	char *functionNameToBeCalled = NULL;
	TAGDataBlock *pTAGDataBlock = findTAGDataBlockByName(session, page, blockname);

    updateOrInsertAllDataBlocksCurrentRow(session, page);

	/** for datablocks items, htsql function name is the concatenation of: blockname + '.' + buttonname */
	if (blockname) {
		functionNameToBeCalled = mkStrcat(session->heapHandle, __FILE__, __LINE__, blockname, ".", htsqlProcedureNameToBeCalled, NULL);
	}
	else {
		functionNameToBeCalled = mkStrdup(session->heapHandle, htsqlProcedureNameToBeCalled, __FILE__, __LINE__);
	}

	/** Find out the caller button is child of <controlblock> or <datablock> */
	const char *blockType = GetQueryStringParam(session, page, "blocktype");

    if (blockType[0] == 'd') {
        /** we are implicitly calling row selection c entry point function in case of caller html element is located on a new row and in a <datablock>. */
        __selectRowNativeResponse(session, page, pTAGDataBlock, htsqlProcedureNameToBeCalled);
    } 

    if (strcmp(htsqlProcedureNameToBeCalled, "tree_node_expanded") == 0) {
        __sendTreeNodeExpandedHTSQLResponse(session, page, blockname);
    } else if (strcmp(htsqlProcedureNameToBeCalled, "tree_node_selected") == 0) {
        __sendTreeNodeSelectedHTSQLResponse(session, page, blockname);
    } else {
        __sendHTSQLResponse(session, page, blockname, functionNameToBeCalled);
    }

    mkFree(session->heapHandle, functionNameToBeCalled);
}



void __sendHTSQLResponse(SodiumSession *session, HTSQLPage *page, const char *blockname, const char *functionNameToBeCalled) {

    const char *oldValue  = GetQueryStringParam(session, page, "old-value");

    /* ###################################################################################################### */
    /* #########    HTSQL FUNCTION CALL      ################################################################ */
    /* ###################################################################################################### */
    /* Preparing to make a function call. Steps are explained in postObjectGeneric.h file. */
    /* Step.1 */    

    /* Step.2 */    createANewFunctionCallStackStartPoint(session, page, functionNameToBeCalled);

    /* Step.3 */    /* adding blockname of the item triggered the event which cause to run this code  */
                    postFunctionAddArgumentsOrVariables(session, page, POST_OBJECT_GENERIC_TYPE_FUNCTION_ARGS, PREOBJECTTYPE_VARIABLE_VARCHAR, "datablockname", blockname);
                    postFunctionAddArgumentsOrVariables(session, page, POST_OBJECT_GENERIC_TYPE_FUNCTION_ARGS, PREOBJECTTYPE_VARIABLE_VARCHAR , "oldValue", oldValue);

    /* Step.4 */    /* iterating again for ObjectGeneric creation with "datablockname" obtained abowe  */
                    addDataBlockDataItemsToLocalStack(session, page, blockname, functionNameToBeCalled);

    /* Step.5 */    const char *ignorenotfound = GetQueryStringParam(session, page, "ignorenotfound");
                    if (ignorenotfound) {
                        runPostFunction(session, page, functionNameToBeCalled, (ignorenotfound[0] == 't'));
                    } else {
                        runPostFunction(session, page, functionNameToBeCalled, false);
                    }

    /* Step.6       If the procedure is a function, result is ready in session->rootPostObjectGeneric->postObjectVariable now. */

    /* Step.7 */    cleanUpFunctionStack(session, page);

    /* Step.8 */    
    /* ###################################################################################################### */
    /* #########    HTSQL FUNCTION CALL      ################################################################ */
    /* ###################################################################################################### */
}
