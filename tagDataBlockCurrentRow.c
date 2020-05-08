#include "pch.h"

#include "tagDataBlockCurrentRow.h"

#include "tagConnection.h"
#include "lemon.html.h"
#include "tagInput.h"
#include "tagGeneric.h"
#include "htsqlFunction.imp.h"
#include "tagDataBlock.h"

void
SetTAGDatablockCurrentRowRowIdTAGInputValue(
    SodiumSession* session,
    HTSQLPage* page,
    TAGDataBlock* pTAGDataBlock,
    const char* rowidValue
    )
{
    if (pTAGDataBlock->keyColumnName == NULL) {
        SetTAGDataBlockRowIdColumnName(session, page, pTAGDataBlock);
    }

    if (pTAGDataBlock->keyColumnValue) {
        mkFree(session->heapHandle, pTAGDataBlock->keyColumnValue);
        pTAGDataBlock->keyColumnValue = NULL;
    }
    
    if (pTAGDataBlock->isSchemaObjectWritable == FALSE) {
        // for read only table we are generating special rowid beginning with "READONLY-" word
        pTAGDataBlock->keyColumnValue = getREADONLYRowid(session);
    }
    else {
        // For writable tables, we are generation single dummy rowid starting with "NEWROW-" string.
        // If datablock has more than one empty row, all rows' items have a new dummy row id for their all inputs.
        if (rowidValue) {
            pTAGDataBlock->keyColumnValue = mkStrdup(session->heapHandle, rowidValue, __FILE__, __LINE__);
        }
        else {
            pTAGDataBlock->keyColumnValue = getNEWROWRowid(session);
        }
    }
        
}

const char*
SetTAGDataBlockRowIdColumnName(
    SodiumSession* session,
    HTSQLPage* page,
    TAGDataBlock* pTAGDataBlock
    )
{
    char* retval = NULL;

    if (pTAGDataBlock) {
        if (pTAGDataBlock->keyColumnName) {
            retval = pTAGDataBlock->keyColumnName;
        }
    }

    if (retval == NULL) {
        if (pTAGDataBlock->tagConnection == NULL) {
            setDatablockConnection(session, page, pTAGDataBlock);
        }

        retval = DBInt_GetPrimaryKeyColumn(
                    pTAGDataBlock->tagConnection->mkDBConnection,
                    pTAGDataBlock->dataSchemaName,
                    pTAGDataBlock->dataSourceName,
                    1
                );
    }

    if (pTAGDataBlock->keyColumnName == NULL || strcmp(pTAGDataBlock->keyColumnName, retval) != 0) {
        pTAGDataBlock->keyColumnName = retval;
        SetTAGDataBlockWritable(session, page, pTAGDataBlock);
    }

    if (pTAGDataBlock->keyColumnName == NULL) {
        mkCoreDebug(__FILE__, __LINE__, "No primary key found for data blcok '", pTAGDataBlock->dataBlockName, "'", NULL);
    }
    return pTAGDataBlock->keyColumnName;
}

TAGGeneric  *printTAGDataBlockDynamicContentRows(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock, DBInt_Statement *stm, TAGGeneric *dynamicContentStartNode) {
    int i = 0;
	TAGConnection *tagConn = findTAGConnectionByName(currentMKSession, page, pTAGDataBlock->connectionName, true);

    char *postQueryProcedureNameToBeCalled = mkStrcat(currentMKSession->heapHandle, __FILE__, __LINE__,
                                                      pTAGDataBlock->dataBlockName,
														trgName_DatablockPostQuery,
                                                      NULL);

     /* We are storing location of first dynamic <tr> tag into the TAGGeneric queue.
      * So, we will use this later for fast access (without iterating).
      * This variables will be used later for AJAX calls
      */
    pTAGDataBlock->startTAG = dynamicContentStartNode;

    if (pTAGDataBlock->blockStatus == BLOCK_STATUS_CREATE_RECORD_MODE) {
        // empty row for new record creation by user
        setAndPrintTAGDataBlockCurrentRow(currentMKSession, page, pTAGDataBlock, NULL, postQueryProcedureNameToBeCalled);
        pTAGDataBlock->blockStatus = BLOCK_STATUS_EXECUTE_QUERY_MODE;
        i = 1;
    }

	bool notEOF = !DBInt_IsEof(tagConn->mkDBConnection, stm);

    // Iterating for each data block row (recordCount property)
    for( ; i < pTAGDataBlock->recordCount; i++) {
        // Call post_query HT/SQL procedure and and print record
        if (notEOF) {
            setAndPrintTAGDataBlockCurrentRow(currentMKSession, page, pTAGDataBlock, stm, postQueryProcedureNameToBeCalled);
        } else {
            setAndPrintTAGDataBlockCurrentRow(currentMKSession, page, pTAGDataBlock, NULL, postQueryProcedureNameToBeCalled);
        }
		notEOF = ! DBInt_Next(tagConn->mkDBConnection, stm);
    }
    mkFree(currentMKSession->heapHandle, postQueryProcedureNameToBeCalled);

    return pTAGDataBlock->endTAG;
}

void setAndPrintTAGDataBlockCurrentRow(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock, DBInt_Statement *stm, char *postQueryProcedureNameToBeCalled) {

    /* ###################################################################################################### */
    /* #########    HTSQL FUNCTION CALL DATABLOCK POST QUERY PROCEDURE     ################################## */
    /* ###################################################################################################### */

    /* Preparing to make a function call. Steps are explained in postObjectGeneric.h file. */
    /* Step.1 */    

    /* Step.2 */    createANewFunctionCallStackStartPoint(currentMKSession, page, postQueryProcedureNameToBeCalled);

    /* Step.3 */    postFunctionAddArgumentsOrVariables(currentMKSession, page, POST_OBJECT_GENERIC_TYPE_FUNCTION_ARGS, PREOBJECTTYPE_VARIABLE_VARCHAR, "datablockname", pTAGDataBlock->dataBlockName);

    /* Step.4 */    setTAGDataBlockCurrentRowValues(currentMKSession, page, pTAGDataBlock, stm);

    /* Step.5 */    runPostFunction(currentMKSession, page, postQueryProcedureNameToBeCalled, true);

    /* Step.6 */    /*
                    If the procedure is a function, result is ready in currentMKSession->rootPostObjectGeneric->postObjectVariable now.
                    post_query function is a procedure and does not have return value
                    */

    /* Step.7 */    cleanUpFunctionStack(currentMKSession, page);

    /* Step.8 */    

    /* ###################################################################################################### */
    /* #########    HTSQL FUNCTION CALL DATABLOCK POST QUERY PROCEDURE     ################################## */
    /* ###################################################################################################### */

    printTAGDataBlockCurrentRow(currentMKSession, page, pTAGDataBlock);
}

void setTAGDataBlockCurrentRowValues(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock, DBInt_Statement *stm) {
    TAGGeneric *dynamicContentStartNode = pTAGDataBlock->startTAG;
    TAGGeneric *currentNode = NULL;
    TAGGeneric *dynamicContentEndNode;
	TAGConnection *tagConn = findTAGConnectionByName(currentMKSession, page, pTAGDataBlock->connectionName, true);
	
    if (stm != NULL) {
		const char *rowidColumnName = SetTAGDataBlockRowIdColumnName(currentMKSession, page, pTAGDataBlock);
        if (rowidColumnName) {
		    const char *rowidValue = DBInt_GetColumnValueByColumnName(tagConn->mkDBConnection, stm, rowidColumnName);
            SetTAGDatablockCurrentRowRowIdTAGInputValue(currentMKSession, page, pTAGDataBlock, rowidValue);
        }
    } else {
        SetTAGDatablockCurrentRowRowIdTAGInputValue(currentMKSession, page, pTAGDataBlock, NULL);
    }
    currentNode = dynamicContentStartNode;

    while (currentNode && currentNode->isDynamicContent ) {

        if (currentNode->tagTOKEN == TAG_INPUT_OPEN || currentNode->tagTOKEN == TAG_SELECT_OPEN || currentNode->tagTOKEN == TAG_TEXTAREA_OPEN) {

            if (stm) {
                // new record fetched from db
                if (currentNode->tagInput->columnName) {

                    if (currentNode->tagInput->inputType != INPUT_TAG_TYPE_IMAGE) {
                        const char *inputValue = DBInt_GetColumnValueByColumnName(tagConn->mkDBConnection, stm, currentNode->tagInput->columnName);
                        setTAGDatablockCurrentRowItemValue(currentMKSession, page, pTAGDataBlock->dataBlockName, currentNode->tagInput->name, inputValue);
					}

                } else {
                    // formula column values will be NULL
                    setTAGDatablockCurrentRowItemValue(currentMKSession, page, pTAGDataBlock->dataBlockName, currentNode->tagInput->name, NULL);
                }
            } else {
                // all records already read. that is, the last record is already fetched. set the rest to NULL
                setTAGDatablockCurrentRowItemValue(currentMKSession, page, pTAGDataBlock->dataBlockName, currentNode->tagInput->name, NULL);
                // For images, additional to the value, some attributes also should be cleared
                if (currentNode->tagInput->inputType == INPUT_TAG_TYPE_IMAGE) {
                    setTAGInputPropertySrc(currentMKSession, page, pTAGDataBlock, currentNode->tagInput->name, NULL);
                    setTAGInputPropertyImageFileSize(currentMKSession, page, pTAGDataBlock, currentNode->tagInput->name, 0);
                }
            }

        }

        currentNode = currentNode->next;
    }
    dynamicContentEndNode = currentNode;
    /* We are storing location of last dynamic </tr> tag in the TAGGeneric queue.
     * So, we will use this later for fast access (without iterating)
     * This variables will be used later for AJAX calls
     */
    pTAGDataBlock->endTAG = dynamicContentEndNode;
}


void printTAGDataBlockCurrentRow(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock) {
    TAGGeneric *dynamicContentStartNode = pTAGDataBlock->startTAG;
    TAGGeneric *currentNode = NULL;

    /* iterating for row printing */
    currentNode = dynamicContentStartNode;
    if (currentNode->isDynamicContent) {

        do {  
            switch (currentNode->tagTOKEN) {
                case TAG_TEXTAREA_OPEN:
                case TAG_SELECT_OPEN:
                case TAG_INPUT_OPEN:   {
                    /*if ( notEOF == FALSE ) {
                        Step.7 cleanUpFunctionStack(currentMKSession, page);
                    }*/
                    printDataBlockDataItemInstance(currentMKSession, page, currentNode->tagInput);
                    break;
                }
                default: {
                    // default action for TAGGeneric object
                    mkPrint(currentMKSession, currentNode->tagSemanticValue, NULL);
                }
            }
           currentNode = currentNode->next;

        } while (currentNode && currentNode->isDynamicContent );

    } else {
        mkPrint(currentMKSession, currentNode->tagSemanticValue, NULL);
    }
}


char *getREADONLYRowid(SodiumSession *currentMKSession) {
	char *randomStr = mkStrRandom(currentMKSession, 3, __FILE__, __LINE__);
	char *newRowId = mkStrcat(currentMKSession->heapHandle, __FILE__, __LINE__, "READONLY-", randomStr, NULL);
	mkFree(currentMKSession->heapHandle, randomStr);
	return newRowId;
}

char *getNEWROWRowid(SodiumSession *currentMKSession) {
    char *randomStr = mkStrRandom(currentMKSession, 3, __FILE__, __LINE__);
    char *newRowId  = mkStrcat(currentMKSession->heapHandle, __FILE__, __LINE__, "NEWROW-", randomStr, NULL);
    mkFree(currentMKSession->heapHandle, randomStr);
    return newRowId;
}

char *getTAGDatablockCurrentRowItemValue(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *tagDataBlock, const char *pPropName) {
    TAGInput *input = findTAGInputByName(currentMKSession, page, tagDataBlock, pPropName);
    if (input == NULL) {
        return NULL; //"Input not found"
    }
    return input->value;
}

int  setTAGDatablockCurrentRowItemValue(SodiumSession *currentMKSession, HTSQLPage *page, const char *dataBlockName, const char *pPropName, const char *pPropValue) {
	TAGDataBlock *tagDataBlock = findTAGDataBlockByName(currentMKSession, page, dataBlockName);
    TAGInput *input = findTAGInputByName(currentMKSession, page, tagDataBlock, pPropName);
    if (input == NULL ||
        input->inputType == INPUT_TAG_TYPE_BUTTON) {
        return 1; //"Input not found"
    }
    if (input->value != NULL) {
        mkFree(currentMKSession->heapHandle, input->value);
        input->value = NULL;
    }
    input->value = mkStrdup(currentMKSession->heapHandle, pPropValue, __FILE__, __LINE__);

    // IMPORTANT: we are not setting value state to "INPUT_TAG_VALUE_CLEAR" here. INPUT_TAG_VALUE_CLEAR means that it is the data same as database.
    // It is modified after read from database it is should remain as INPUT_TAG_VALUE_DIRTY.
    //input->valueState = INPUT_TAG_VALUE_CLEAR;

    return 0;
}




int  modifyTAGDatablockCurrentRowItemValue(SodiumSession *currentMKSession, HTSQLPage *page, const char *dataBlockName, const char *pPropName, const char *pPropValue) {
	TAGDataBlock *tagDataBlock = findTAGDataBlockByName(currentMKSession, page, dataBlockName);
    TAGInput *input = findTAGInputByName(currentMKSession, page, tagDataBlock, pPropName);
    if (input == NULL) {
        return 1; //"Input not found"
    }
    if (input->value != NULL) {
        mkFree(currentMKSession->heapHandle, input->value);
        input->value = NULL;
    }
    input->value = mkStrdup(currentMKSession->heapHandle, pPropValue, __FILE__, __LINE__);
    input->valueState = INPUT_TAG_VALUE_DIRTY;
    return 0;
}
