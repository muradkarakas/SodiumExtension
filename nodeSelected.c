#include "stdafx.h"

#include "nodeSelected.h"

#include "dispatcher.h"
#include "htsqlFunction.imp.h"


void __sendTreeNodeSelectedHTSQLResponse(SodiumSession *currentMKSession, HTSQLPage *page, const char *blockname) {
    const char *treename        = GetQueryStringParam(currentMKSession, page, "treename");
    const char *nodeid          = GetQueryStringParam(currentMKSession, page, "nodeid");
    const char *parentnodeid    = GetQueryStringParam(currentMKSession, page, "parentnodeid");
    const char *nodetype   = GetQueryStringParam(currentMKSession, page, "nodetype");

    char *localFunctionNameToBeCalled = mkStrcat(currentMKSession->heapHandle, __FILE__, __LINE__,
                                                 blockname,
                                                 ".",
                                                 treename,
												 ".",
                                                 trgName_TreeNodeSelected,
                                                 NULL
                                                 );

    /* ###################################################################################################### */
    /* #########    HTSQL FUNCTION CALL      ################################################################ */
    /* ###################################################################################################### */
    /* Preparing to make a function call. Steps are explained in postObjectGeneric.h file. */
    /* Step.1 */    

    /* Step.2 */    createANewFunctionCallStackStartPoint(currentMKSession, page, localFunctionNameToBeCalled);

    /* Step.3 */    /* adding blockname of the item triggered the event which cause to run this code  */
                    postFunctionAddArgumentsOrVariables(currentMKSession, page, POST_OBJECT_GENERIC_TYPE_FUNCTION_ARGS, PREOBJECTTYPE_VARIABLE_VARCHAR, "datablockname", blockname);
                    postFunctionAddArgumentsOrVariables(currentMKSession, page, POST_OBJECT_GENERIC_TYPE_FUNCTION_ARGS, PREOBJECTTYPE_VARIABLE_VARCHAR, "node_id", nodeid);
                    postFunctionAddArgumentsOrVariables(currentMKSession, page, POST_OBJECT_GENERIC_TYPE_FUNCTION_ARGS, PREOBJECTTYPE_VARIABLE_VARCHAR, "parent_id", parentnodeid);
                    postFunctionAddArgumentsOrVariables(currentMKSession, page, POST_OBJECT_GENERIC_TYPE_FUNCTION_ARGS, PREOBJECTTYPE_VARIABLE_VARCHAR, "node_type", nodetype);

    /* Step.4 */    /* iterating again for ObjectGeneric creation with "datablockname" obtained abowe  */
                    addDataBlockDataItemsToLocalStack(currentMKSession, page, blockname, localFunctionNameToBeCalled);

    /* Step.5 */    const char *ignorenotfound = GetQueryStringParam(currentMKSession, page, "ignorenotfound");
                    PreObjectFunction *preObjectFunction = NULL;
                    if (ignorenotfound) {
                        preObjectFunction = runPostFunction(currentMKSession, page, localFunctionNameToBeCalled, (ignorenotfound[0] == 't'));
                    } else {
                        preObjectFunction = runPostFunction(currentMKSession, page, localFunctionNameToBeCalled, true);
                    }

    /* Step.6       If the procedure is a function, result is ready in page->rootPostObjectGeneric->postObjectVariable now. */
                    //const char *retVal = preObjectFunction->preObjectFunctionReturnValue->varcharValue;
                    if (preObjectFunction) {
                        __sendTreeNodeSelectedHTSQLResponseMessage(currentMKSession, page, treename, nodeid);
                    }

    /* Step.7 */    cleanUpFunctionStack(currentMKSession, page);

    /* Step.8 */    
    /* ###################################################################################################### */
    /* #########    HTSQL FUNCTION CALL      ################################################################ */
    /* ###################################################################################################### */

    mkFree(currentMKSession->heapHandle, localFunctionNameToBeCalled);
}

void __sendTreeNodeSelectedHTSQLResponseMessage(SodiumSession *currentMKSession, HTSQLPage *page, const char *treename, const char *nodeid) {
    char *lastResponse = mkStrcat(currentMKSession->heapHandle,
                                  __FILE__, __LINE__,
                                  "action = { "
                                                "\"type\"       : \"treeNodeSelectedSuccessful\", "
                                                "\"treename\"   : \"", treename, "\", "
                                                "\"nodeid\"     : \"", nodeid, "\", "
                                          " };",
                                  // For Tree element, DO NOT call "addNewActionToTheResponseActionList" function.
                                  NULL);
    mkPrint(currentMKSession, lastResponse, NULL);
    mkFree(currentMKSession->heapHandle, lastResponse);
}
