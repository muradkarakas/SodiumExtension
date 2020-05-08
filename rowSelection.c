#include "pch.h"

#include "rowSelection.h"
#include "tagDataBlock.h"
#include "tagDataBlockCurrentRow.h"
#include "tagDataBlockDatabaseOperations.h"
#include "ajax.h"
#include "dispatcher.h"
#include "settingDataBlockItemValue.h"
#include "yuarel.h"
#include "tagConnection.h"
#include "htsqlFunction.imp.h"


// Entry point function for row selection
void __selectRowNativeResponse(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGDataBlock, const char *procedureName) {

    mkDebug(session, SESSION_LOG_DETAIL_HTSQL_TRIGGER, "\n", "Row selected in block (", pTAGDataBlock->dataBlockName, ") ... ", NULL);

    updateOrInsertAllDataBlocksCurrentRow(session, page);
    
    // Check whether the focus moved to different row
    const char *rowId = GetQueryStringParam(session, page, "rowid");

    if (page->currentRowId && rowId  &&
        strcmp(page->currentRowId, rowId) == 0 && strcmp(page->currentDatablockName, pTAGDataBlock->dataBlockName) == 0) {

        /** A database table/view can be shown in two or more different data block. In that case, their rowid will be the same.
            This does not mean same row is selected. So, we are checking the block name also.
        */
        __sendSelectRowSuccessfulResponse(session, pTAGDataBlock->dataBlockName, rowId);
        mkDebug(session, SESSION_LOG_DETAIL_HTSQL_TRIGGER, "\n", "Same row revisited in block (", pTAGDataBlock->dataBlockName, "). (", rowId, ")", NULL);

    } else {

        __setDatablockSelectedRowTAGInputItemsValues(session, page, pTAGDataBlock);
        __sendSetSequenceValuesResponse(session, page, pTAGDataBlock);

		increaseDebugIndentionCount(session);
        __runHTSQLRowSelectedFunction(session, page, pTAGDataBlock->dataBlockName, procedureName);
		decreaseDebugIndentionCount(session);

        __sendSelectRowSuccessfulResponse(session, pTAGDataBlock->dataBlockName, pTAGDataBlock->keyColumnValue);
        __sendForeignKeyValuesResponse(session, page, pTAGDataBlock);

        setCurrentBlocknameAndRowId(session, page, pTAGDataBlock->dataBlockName, pTAGDataBlock->keyColumnValue);
    }

    mkDebug(session, SESSION_LOG_DETAIL_HTSQL_TRIGGER, "\n", "done. (", pTAGDataBlock->keyColumnValue, ")", NULL);
}

void __selectInternalRow(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGDataBlock, const char *cFunctionNameToBeCalled) {
    __setDatablockSelectedRowTAGInputItemsValues(session, page, pTAGDataBlock);
    setCurrentBlocknameAndRowId(session, page, pTAGDataBlock->dataBlockName, pTAGDataBlock->keyColumnValue);
}



void __sendForeignKeyValuesResponse(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGDataBlock) {

    TAGInput *input = pTAGDataBlock->rootTAGInput;
    while(input) {
        if (input->columnName) {
            /** This is a database column and maybe it is a foreign key. */
            if (input->masterItemName) {
                /** This is an foreign key item. getting master key item and its value  */
                if (pTAGDataBlock->masterBlockName == NULL || strlen(pTAGDataBlock->masterBlockName) == 0) {
                    // Data block has foreign key columns but its master block name property is invalid
					CLOG(ERR_DATABLOCK_HAS_FK_BUT_NO_MASTER, pTAGDataBlock->dataBlockName);
                    goto outofloop;
                }

                TAGDataBlock *masterBlock = findTAGDataBlockByName(session, page, pTAGDataBlock->masterBlockName);
                if (masterBlock == NULL) {
                    // Master data block not found
					// Maybe it is a session variable. It that is the case, no error message will be shown
					if (strcmp(pTAGDataBlock->masterBlockName, "Session") != 0) {
						char *errMsg = mkStrcat(session->heapHandle, __FILE__, __LINE__, "'",
							pTAGDataBlock->dataBlockName, "->", pTAGDataBlock->masterBlockName,
							NULL);
						CLOG(ERR_MASTER_DATABLOCK_NOT_FOUND, errMsg);
						mkFree(session->heapHandle, errMsg);
					}
                    goto outofloop;
				}
				else {

					TAGInput *masterItem = findTAGDataBlockCurrentRowTAGInputItem(session, masterBlock, input->masterItemName);

					if (input->value == NULL) {
					setting:
						/** Foreign key does not have value. So, setting master key's value to the corresponding client and server variable  */
						// setting server foreign data block current row foreign key item value
						setTAGDatablockCurrentRowItemValue(session, page, pTAGDataBlock->dataBlockName, input->name, masterItem->value);
						// setting client foreign key input element value
						__sendSetDataBlockItemValueResponse(session, pTAGDataBlock->dataBlockName, pTAGDataBlock->keyColumnValue, input->masterItemName, masterItem->value, false);
					}
					else {
						/** Foreign key has value. Comparing it with the master key's value.
						If they are different, setting master key's value to the corresponding client and server variable  */
						if (masterItem->value) {
							// master key has value
							if (strcmp(input->value, masterItem->value) != 0) {
								goto setting;
								/** master key and foreign key value is different */
							}
						}
					}
				}
                
            }
        }
        input = input->next;
    }

outofloop:
    return;
}

void __setDatablockSelectedRowTAGInputItemsValues(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGDataBlock) {
    int rowIdIndex = -1;

    for(int index = 0; index < page->totalHTTPFormVariableCount; index++) {
        setTAGDatablockCurrentRowItemValue(session, page, pTAGDataBlock->dataBlockName, page->formDataStructureSmall[index].decodedKey, page->formDataStructureSmall[index].decodedVal);
        if (strcmp(page->formDataStructureSmall[index].key,"rowid") == 0) {
            rowIdIndex = index;
        }
    }
    if (rowIdIndex != -1) {
        SetTAGDatablockCurrentRowRowIdTAGInputValue(session, page, pTAGDataBlock, page->formDataStructureSmall[rowIdIndex].decodedVal);
    } else {
		CLOG(ERR_CORE_VALUE_NOT_FOUND, "rowid");
	}
}


void __sendSetSequenceValuesResponse(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGDataBlock) {

    TAGInput *input = pTAGDataBlock->rootTAGInput;
    while(input) {
        if (input->columnName) {
            /** This is a database column and maybe has a reference to sequence in db */
            if (input->sequenceName && (input->value == NULL || strlen(input->value) == 0))  {
                /** This item gets its value from a sequence if it is empty */

                /** Getting next sequence value from db */
                TAGConnection *tagConnection = findTAGConnectionByName(session, page, pTAGDataBlock->connectionName, true);
				// if sequence's schema name is not set, schema name of its data block will be used as schema name
				char *sequenceSchemaName = (input->sequenceSchemaName) ? input->sequenceSchemaName : pTAGDataBlock->dataSchemaName;
				// get the value
                char *newSequenceValue = getSequenceValue(session, page, tagConnection, sequenceSchemaName, input->sequenceName);
				if (newSequenceValue) {
					/** setting current row item value in the server memory */
					setTAGDatablockCurrentRowItemValue(session, page, pTAGDataBlock->dataBlockName, input->name, newSequenceValue);
					/** setting client input element value to sequence value */
					__sendSetDataBlockItemValueResponse(session, pTAGDataBlock->dataBlockName, pTAGDataBlock->keyColumnValue, input->name, newSequenceValue, false);
					mkFree(session->heapHandle, newSequenceValue);
				}
				else {
					// error occured while getting new sequence value from DB
				}
			}
        }
        input = input->next;
    }
}

void __sendUpdateSuccessfulResponse(SodiumSession *session, TAGDataBlock *pTAGDataBlock, const char *rowid) {
    char *lastResponse = mkStrcat(session->heapHandle, __FILE__, __LINE__,
                                  "action = { "
                                                "\"type\"            : \"updateSuccessful\", "
                                                "\"dataBlockName\"   : \"", pTAGDataBlock->dataBlockName, "\", ",
                                                "\"rowid\"           : \"", rowid, "\" "
                                          " };",
                                  // standart part
                                  "addNewActionToTheResponseActionList(action);",
                                  NULL);
    mkPrint(session, lastResponse, NULL);
    mkFree(session->heapHandle, lastResponse);
}

void __sendDeleteRejectedResponse(SodiumSession *session, TAGDataBlock *pTAGDataBlock, const char *rowid) {
    char *lastResponse = mkStrcat(session->heapHandle, __FILE__, __LINE__,
                                  "action = { "
                                                "\"type\"            : \"deleteRejected\", "
                                                "\"dataBlockName\"   : \"", pTAGDataBlock->dataBlockName, "\", ",
                                                "\"rowid\"           : \"", rowid, "\" "
                                          " };",
                                  // standart part
                                  "addNewActionToTheResponseActionList(action);",
                                  NULL);
    mkPrint(session, lastResponse, NULL);
    mkFree(session->heapHandle, lastResponse);
}

void __sendDeleteSuccessfulResponse(SodiumSession *session, TAGDataBlock *pTAGDataBlock, const char *rowid) {
    char *generatedRowId = getNEWROWRowid(session);
    char *lastResponse = mkStrcat(session->heapHandle, __FILE__, __LINE__,
                                  "action = { "
                                                "\"type\"            : \"deleteSuccessful\", "
                                                "\"dataBlockName\"   : \"", pTAGDataBlock->dataBlockName, "\", ",
                                                "\"rowid\"           : \"", rowid, "\", "
                                                "\"generatedRowid\"  : \"", generatedRowId, "\", "
                                          " };",
                                  // standart part
                                  "addNewActionToTheResponseActionList(action);",
                                  NULL);
    mkPrint(session, lastResponse, NULL);
    mkFree(session->heapHandle, lastResponse);
    mkFree(session->heapHandle, generatedRowId);
}

void __sendInsertSuccessfulResponse(SodiumSession *session, TAGDataBlock *pTAGDataBlock, const char *rowid) {
    char *lastResponse = mkStrcat(session->heapHandle, __FILE__, __LINE__,
                                  "action = { "
                                                "\"type\"            : \"insertSuccessful\", "
                                                "\"dataBlockName\"   : \"", pTAGDataBlock->dataBlockName, "\", ",
                                                "\"rowid\"           : \"", (rowid) ? rowid:"?", "\" "
                                          " };",
                                  // standart part
                                  "addNewActionToTheResponseActionList(action);",
                                  NULL);
    mkPrint(session, lastResponse, NULL);
    mkFree(session->heapHandle, lastResponse);
}

void __sendUpdateRejectedResponse(SodiumSession *session, TAGDataBlock *pTAGDataBlock, const char *rowid) {
    char *lastResponse = mkStrcat(session->heapHandle, __FILE__, __LINE__,
                                  "action = { "
                                                "\"type\"            : \"updateRejected\", "
                                                "\"dataBlockName\"   : \"", pTAGDataBlock->dataBlockName, "\", ",
                                                "\"rowid\"           : \"", rowid, "\" "
                                          " };",
                                  // standart part
                                  "addNewActionToTheResponseActionList(action);",
                                  NULL);
    mkPrint(session, lastResponse, NULL);
    mkFree(session->heapHandle, lastResponse);
}


void __sendInsertRejectedResponse(SodiumSession *session, TAGDataBlock *pTAGDataBlock, const char *rowid) {
    char *lastResponse = mkStrcat(session->heapHandle, __FILE__, __LINE__,
                                  "action = { "
                                                "\"type\"            : \"insertRejected\", "
                                                "\"dataBlockName\"   : \"", pTAGDataBlock->dataBlockName, "\", ",
                                                "\"rowid\"           : \"", rowid, "\" "
                                          " };",
                                  // standart part
                                  "addNewActionToTheResponseActionList(action);",
                                  NULL);
    mkPrint(session, lastResponse, NULL);
    mkFree(session->heapHandle, lastResponse);
}


void __sendSelectRowSuccessfulResponse(SodiumSession *session, const char *blockname, const char *rowid) {
    char *lastResponse = mkStrcat(session->heapHandle, __FILE__, __LINE__,
                                  "action = { "
                                                        "\"type\"            : \"selectRowSuccessful\", "
                                                        "\"dataBlockName\"   : \"", blockname, "\", ",
                                                        "\"rowid\"           : \"", rowid, "\" "
                                                  " };",
                                  // standart part
                                  "addNewActionToTheResponseActionList(action);",
                                  NULL);
    mkPrint(session, lastResponse, NULL);
    mkFree(session->heapHandle, lastResponse);
}

void setCurrentBlocknameAndRowId(SodiumSession *session, HTSQLPage *page, const char *blockname, const char *rowid) {
    if (page->currentDatablockName != NULL) {
        mkFree(session->heapHandle, page->currentDatablockName);
        page->currentDatablockName = NULL;
    }
    if (page->currentRowId != NULL) {
        mkFree(session->heapHandle, page->currentRowId);
        page->currentRowId = NULL;
    }

    page->currentDatablockName = mkStrdup(session->heapHandle, blockname, __FILE__, __LINE__);
    page->currentRowId         = mkStrdup(session->heapHandle, rowid, __FILE__, __LINE__);
}


void __runHTSQLRowSelectedFunction(SodiumSession *session, HTSQLPage *page, const char *blockname, const char *procedureName) {
	clock_t start, end;
	start = clock();
    char cBuf[100];

    //mkDebug(session, SESSION_LOG_DETAIL_HTSQL_FUNCTION_CALL, "\n", "function ", blockname, ".", "row_selected", " is called if exists ...", NULL);

    char *functionNameToBeCalled = mkStrcat(session->heapHandle, __FILE__, __LINE__,
                                            blockname,
                                            trgName_DatablockRowSelected,
                                            NULL);

    /* Step.1 */    

    /* Step.2 */    createANewFunctionCallStackStartPoint(session, page, functionNameToBeCalled);

    /* Step.3 */    /* adding blockname of the item triggered the event which cause to run this code  */
                    postFunctionAddArgumentsOrVariables(session, page, POST_OBJECT_GENERIC_TYPE_FUNCTION_ARGS, PREOBJECTTYPE_VARIABLE_VARCHAR, "datablockname", blockname);

    /* Step.4 */    //Add HT/SQL function arguments and control block items's names/values to the stack by using following c function

    /* Step.5 */    /*PreObjectFunction *functionExecuted = */ runPostFunction(session, page, functionNameToBeCalled, true);

    /* Step.6 */    // return value in -> session->rootPostObjectGeneric

    /* Step.7 */    cleanUpFunctionStack(session, page);

    /* Step.8 */    

    mkFree(session->heapHandle, functionNameToBeCalled);

	end = clock();
    mkConvertFloatToChar((end - start) / CLOCKS_PER_SEC, cBuf, 100);
    
	//mkDebug(session, SESSION_LOG_DETAIL_HTSQL_FUNCTION_CALL, "\n", "Done in (", cBuf, ") msec.", NULL);
}



void __sendRowIdReplaceResponse(SodiumSession *session, const char *blockname, const char *oldRowId, const char *newRowId) {
    char *lastResponse = mkStrcat(session->heapHandle, __FILE__, __LINE__,
                                  "action = { "
                                                "\"type\"            : \"replaceRowId\", "
                                                "\"dataBlockName\"   : \"", blockname, "\", ",
                                                "\"oldrowid\"        : \"", oldRowId, "\", ",
                                                "\"newrowid\"        : \"", newRowId, "\" "
                                          " };",
                                  // standart part
                                  "addNewActionToTheResponseActionList(action);",
                                  NULL);
    mkPrint(session, lastResponse, NULL);
    mkFree(session->heapHandle, lastResponse);
}

