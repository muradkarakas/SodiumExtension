#include "pch.h"

#include "itemModified.h"
#include "tagInput.h"
#include "dispatcher.h"
#include "tagDataBlock.h"
#include "tagDataBlockCurrentRow.h"
#include "htsqlFunction.imp.h"


void __itemModifiedNativeResponse(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGDataBlock, const char *procedureName) {

    /** Free to return part */
    if (!pTAGDataBlock) {
		CLOG(ERR_DATABLOCK_NOT_FOUND, "");
        return;
    }

    const char *rowid       = GetQueryStringParam(session, page, "rowid");
    const char *itemName    = GetQueryStringParam(session, page, "item-name");
    const char *newValue    = GetQueryStringParam(session, page, "new-value");
    const char *oldValue    = GetQueryStringParam(session, page, "old-value");

    if (! rowid) {
		CLOG(ERR_CORE_INVALID_HTTP_REQUEST_PARAM, "rowid not exists in request");
        return;
    }
    if (! itemName) {
		CLOG(ERR_CORE_INVALID_HTTP_REQUEST_PARAM, "itemName not exists in request");
        return;
    }
    if (! newValue) {
		CLOG(ERR_CORE_INVALID_HTTP_REQUEST_PARAM, "itemValue not exists in request");
		return;
    }

    TAGInput *input = findTAGInputByName(session, page, pTAGDataBlock, itemName);
    if (input==NULL || input->columnName==NULL) {
        // This is a formula column. Cannot be updated.
		CLOG(ERR_FORMULA_COLUMN_VALUE_CANNOT_BE_UPDATED, itemName);
        return;
    }

    /** GO PART */

    char *itemValueDecoded = url_decode(session, newValue);
    char *itemValueDoubleDecoded = url_decode(session, itemValueDecoded);

    mkDebug(session,
            SESSION_LOG_DETAIL_HTSQL_ITEM,
            "\n",
            "Item modified... (", pTAGDataBlock->keyColumnValue, ") '",
            itemName, "' = '", itemValueDoubleDecoded, "'.",
            NULL);

    int isFound = modifyTAGDatablockCurrentRowItemValue(session, page, pTAGDataBlock->dataBlockName, itemName, itemValueDoubleDecoded);

    if (isFound==0) {
        char *functionNameToBeCalled = mkStrcat(session->heapHandle, __FILE__, __LINE__,
												pTAGDataBlock->dataBlockName,
                                                ".",
                                                (input->orjinalName) ? input->orjinalName:itemName, /* for radio boxes, we are using orginal name */
                                                NULL);
        runItemModifiedHTSQLTrigger(session, page, pTAGDataBlock, oldValue, functionNameToBeCalled);
        mkFree(session->heapHandle, functionNameToBeCalled);
    }
    /**  For html tag "textarea", we should double decode. */
    char *rowidDecoded = url_decode(session, rowid);
    char *itemValueDoubleDecodedEscaped = mkEscapeJavaScriptQuoteCharacter(session, itemValueDoubleDecoded);

    char *lastResponse1 = mkStrcat(session->heapHandle,
                                   __FILE__, __LINE__,
                                  "action = { ",
                                                "\"type\"            : \"itemModified\", ",
                                                "\"dataBlockName\"   : \"", pTAGDataBlock->dataBlockName, "\", ",
                                                "\"rowid\"           : \"", rowidDecoded, "\", ",
                                                "\"itemName\"        : \"", itemName, "\", ",
                                                "\"itemValue\"       : \"", itemValueDoubleDecodedEscaped, "\", ",
                                            " };",
                                  "addNewActionToTheResponseActionList(action);",
                                  NULL);

    mkPrint(session, lastResponse1, NULL);

    mkFree(session->heapHandle, lastResponse1);
    mkFree(session->heapHandle, rowidDecoded);
    mkFree(session->heapHandle, itemValueDecoded);
    mkFree(session->heapHandle, itemValueDoubleDecoded);
    mkFree(session->heapHandle, itemValueDoubleDecodedEscaped);

    mkDebug(session, SESSION_LOG_DETAIL_HTSQL_ITEM, "\n", "Done.", NULL);
    if (isFound==1) {
        mkDebug(session, SESSION_LOG_DETAIL_HTSQL_ITEM, " FAILED", NULL);
    }
}

void runItemModifiedHTSQLTrigger(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGDataBlock, const char *oldValue, const char *functionNameToBeCalled) {
 // - Step.1:\n

 // - Step.2:\n
	createANewFunctionCallStackStartPoint(session, page, functionNameToBeCalled);

 // - Step.3\n
    postFunctionAddArgumentsOrVariables(session, page, POST_OBJECT_GENERIC_TYPE_FUNCTION_ARGS, PREOBJECTTYPE_VARIABLE_VARCHAR, "datablockname", pTAGDataBlock->dataBlockName);
    postFunctionAddArgumentsOrVariables(session, page, POST_OBJECT_GENERIC_TYPE_FUNCTION_ARGS, PREOBJECTTYPE_VARIABLE_VARCHAR, "oldValue", oldValue);

 // - Step.4\n
    //    *  \n

 // - Step.5\n
    /*PreObjectFunction *functionExecuted = */
    runPostFunction(session, page, functionNameToBeCalled, true);

 // - Step.6.\n
    //session->rootPostObjectGeneric

 // - Step.7:\n
    cleanUpFunctionStack(session, page);

 // - Step.8:\n

}
