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

#include "nodeExpanded.h"

#include "dispatcher.h"
#include "htsqlFunction.imp.h"


void __sendTreeNodeExpandedHTSQLResponse(SodiumSession *session, HTSQLPage *page, const char *blockname) {
    const char *treename = GetQueryStringParam(session, page, "treename");
    const char *nodeid   = GetQueryStringParam(session, page, "nodeid");
    const char *nodetype   = GetQueryStringParam(session, page, "nodetype");

    char *localFunctionNameToBeCalled = mkStrcat(session->heapHandle, __FILE__, __LINE__,
                                                 blockname,
                                                 ".",
                                                 treename,
												 ".",
                                                 trgName_TreeNodeExpanded,
                                                 NULL
                                                 );

    /* ###################################################################################################### */
    /* #########    HTSQL FUNCTION CALL      ################################################################ */
    /* ###################################################################################################### */
    /* Preparing to make a function call. Steps are explained in postObjectGeneric.h file. */
    /* Step.1 */    

    /* Step.2 */    createANewFunctionCallStackStartPoint(session, page, localFunctionNameToBeCalled);

    /* Step.3 */    /* adding blockname of the item triggered the event which cause to run this code  */
                    postFunctionAddArgumentsOrVariables(session, page, POST_OBJECT_GENERIC_TYPE_FUNCTION_ARGS, PREOBJECTTYPE_VARIABLE_VARCHAR, "datablockname", blockname);
                    postFunctionAddArgumentsOrVariables(session, page, POST_OBJECT_GENERIC_TYPE_FUNCTION_ARGS, PREOBJECTTYPE_VARIABLE_VARCHAR, "node_id", nodeid);
                    postFunctionAddArgumentsOrVariables(session, page, POST_OBJECT_GENERIC_TYPE_FUNCTION_ARGS, PREOBJECTTYPE_VARIABLE_VARCHAR, "node_type", nodetype);

    /* Step.4 */    /* iterating again for ObjectGeneric creation with "datablockname" obtained abowe  */
                    addDataBlockDataItemsToLocalStack(session, page, blockname, localFunctionNameToBeCalled);

    /* Step.5 */    const char *ignorenotfound = GetQueryStringParam(session, page, "ignorenotfound");
                    PreObjectFunction *preObjectFunction = NULL;
                    if (ignorenotfound) {
                        preObjectFunction = runPostFunction(session, page, localFunctionNameToBeCalled, (ignorenotfound[0] == 't'));
                    } else {
                        preObjectFunction = runPostFunction(session, page, localFunctionNameToBeCalled, false);
                    }

    /* Step.6       If the procedure is a function, result is ready in page->rootPostObjectGeneric->postObjectVariable now. */
                    const char *retVal = NULL;
                    if (preObjectFunction) {
                        if (page->currentCallStack->preObjectFunctionReturnValue) {
                            if (page->currentCallStack->preObjectFunctionReturnValue->value.text) {
                                retVal = page->currentCallStack->preObjectFunctionReturnValue->value.text;
                            } else {
								CLOG(ERR_FUNCTION_MUST_RETURN_VALUE, "");
                            }
                        } else {
							CLOG(ERR_FUNCTION_MUST_RETURN_CHAR, "");
                        }
                    } else {
                        // No need to send no function found message since runPostFunction(... , false) will send error message (last argument).
                    }
                    __sendTreeNodeExpandedHTSQLResponseMessage(session, page, treename, nodeid, (retVal) ? retVal:"" );

    /* Step.7 */    cleanUpFunctionStack(session, page);

    /* Step.8 */    
    /* ###################################################################################################### */
    /* #########    HTSQL FUNCTION CALL      ################################################################ */
    /* ###################################################################################################### */

    mkFree(session->heapHandle, localFunctionNameToBeCalled);
}

void __sendTreeNodeExpandedHTSQLResponseMessage(SodiumSession *session, HTSQLPage *page, const char *treename, const char *nodeid, const char *message) {
    char *js = mkEscapeJavaScriptQuoteCharacter(session, message);
    char *lastResponse = mkStrcat(session->heapHandle,
                                  __FILE__, __LINE__,
                                  "action = { "
                                                "\"type\"       : \"treeNodeExpanded\", "
                                                "\"treename\"   : \"", treename, "\", "
                                                "\"nodeid\"     : \"", nodeid, "\", "
                                                "\"json\"       : \"", js, "\" "
                                          " };",
                                  // For Tree element, DO NOT call "addNewActionToTheResponseActionList" function.
                                  NULL);
    mkPrint(session, lastResponse, NULL);
    mkFree(session->heapHandle, lastResponse);
    mkFree(session->heapHandle, js);
}
