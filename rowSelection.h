/*! \file   rowSelection.h
    \brief  Functions for row selection

    When user clicks any data block item, TAGInputs linked list in datablock  (which corresponds to that selected row) is updated with the current elements' values.
    And also current row's row id is stored in Datablock structure as a variable (currentRowRowIdTAGInput).
*/

#pragma once





/** \brief This is entry point function to be called whenever focus move from one row to another row in same or different block.
 *
 * \param currentSodiumSession MKSession*
 * \param blockname const char*
 * \param procedureName const char*
 * \param param YuarelParam*
 * \param totalHTTPFormVariableCount int
 * \return void
 *
 *  This function must be called from other native c functions if the user event/action implicitly moves focus from one data block row to the another.
 *
 *  This function can be called from other native c functions even if the user event/action does not result in focusing from one data block row to the another
 *  since the function checks the requested action/event is triggered
 *
 */
void __selectRowNativeResponse(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock, const char *procedureName);
void __runHTSQLRowSelectedFunction(SodiumSession *currentMKSession, HTSQLPage *page, const char *blockname, const char *procedureName);
void __setDatablockSelectedRowTAGInputItemsValues(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock);


void __sendSelectRowSuccessfulResponse(SodiumSession *currentMKSession, const char *blockname, const char *rowid);
void __sendRowIdReplaceResponse(SodiumSession *currentMKSession, const char *blockname, const char *oldRowId, const char *newRowId);
void __sendForeignKeyValuesResponse(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock);
void __sendSetSequenceValuesResponse(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock);

void __selectInternalRow(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock, const char *cFunctionNameToBeCalled);

void setCurrentBlocknameAndRowId(SodiumSession *currentMKSession, HTSQLPage *page, const char *blockname, const char *rowid);

/** \brief  __CALLER IS RESPONSIBLE FOR FREEING RETURN VALUE__
 *
 * \param currentSodiumSession MKSession*
 * \param pTAGDataBlock TAGDataBlock*
 * \param procedureName const char*
 * \param param YuarelParam*
 * \param totalHTTPFormVariableCount int
 * \return char*
 *
char *__getRowIdFromRequestParameters(SodiumSession *currentMKSession, const char *blockname, const char *procedureName, YuarelParam *param, int totalHTTPFormVariableCount);
*/

void __sendDeleteRejectedResponse(SodiumSession *currentMKSession, TAGDataBlock *pTAGDataBlock, const char *rowid);
void __sendInsertRejectedResponse(SodiumSession *currentMKSession, TAGDataBlock *pTAGDataBlock, const char *rowid);
void __sendUpdateRejectedResponse(SodiumSession *currentMKSession, TAGDataBlock *pTAGDataBlock, const char *rowid);

void __sendUpdateSuccessfulResponse(SodiumSession *currentMKSession, TAGDataBlock *pTAGDataBlock, const char *rowid);
void __sendDeleteSuccessfulResponse(SodiumSession *currentMKSession, TAGDataBlock *pTAGDataBlock, const char *rowid);
void __sendInsertSuccessfulResponse(SodiumSession *currentMKSession, TAGDataBlock *pTAGDataBlock, const char *rowid);

