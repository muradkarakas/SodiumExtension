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

 /*! \file   tagDataBlockCurrentRow.h

    \brief  Contains functions for data block row selection.
*/

#pragma once





/** \brief Modifies the current value of the TAGInput's "value" property in a TAGDataBlock
 *
 * \param currentMKSession
 * \param dataBlockName     Datablock name of the TAGInput instance belongs to.
 * \param pPropName         Property name of the TAGInput whose value will be set
 * \param pPropValue        New value to be set
 * \return char*            Returns 0 if finds the block specified by dataBlockName and property specified by pPropName parameter.
 *                          If error occurs, returns the reason as int value.
 * \n                       0 -> success.
 * \n                       1 -> TAGInput not found
 *
 *  "valueState" proeprty of the TAGInput will be set to INPUT_TAG_VALUE_DIRTY.
 */
int     modifyTAGDatablockCurrentRowItemValue(SodiumSession *currentMKSession, HTSQLPage *page, const char *dataBlockName, const char *pPropName, const char *pPropValue);

/*
	Set and Returns the row id column name of the datablock or TAGConnection object. 
	Do not free return value.
*/
const char*
SetTAGDataBlockRowIdColumnName(
    SodiumSession* session,
    HTSQLPage* page,
    TAGDataBlock* pTAGDataBlock
);

/** \brief This function works as modifyTAG... version, the difference is this one does not set valueState to INPUT_TAG_VALUE_DIRTY value.
 *
 * \param currentSodiumSession MKSession*
 * \param dataBlockName const char*
 * \param pPropName const char*
 * \param pPropValue const char*
 * \return int
 *
 */
int        setTAGDatablockCurrentRowItemValue(SodiumSession *currentMKSession, HTSQLPage *page, const char *dataBlockName, const char *pPropName, const char *pPropValue);
void                                     
SetTAGDatablockCurrentRowRowIdTAGInputValue(
    SodiumSession *session, 
    HTSQLPage *page, 
    TAGDataBlock *pTAGDataBlock, 
    const char *rowidValue
);

char                              *getTAGDatablockCurrentRowItemValue(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *tagDataBlock, const char *pPropName);

/** \brief  Generates and send contents of "<TR>" tag in "<TBODY>" part of the table for each database row.
 *
 * \param currentMKSession
 * \param pTAGDataBlock             It is a TAGDataBlock instance pointer whose dynamic rows will be printed. This instance can be obtained by calling findTAGDataBlockByName(SodiumSession *currentMKSession, char *pDataBlockName).
 * \param OCI_Resultset *rs
 * \param dynamicContentStartNode
 * \return                           It is the last dynamic tag instance
 *
 */
TAGGeneric * printTAGDataBlockDynamicContentRows(	SodiumSession *currentMKSession, 
													HTSQLPage *page, 
													TAGDataBlock *pTAGDataBlock, 
													DBInt_Statement *stm, 
													TAGGeneric *dynamicContentStartNode);


void setAndPrintTAGDataBlockCurrentRow(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock, DBInt_Statement *stm, char *postQueryProcedureNameToBeCalled);
void setTAGDataBlockCurrentRowValues(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock, DBInt_Statement *stm);
void printTAGDataBlockCurrentRow(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock);

char *getNEWROWRowid(SodiumSession *currentMKSession);
char *getREADONLYRowid(SodiumSession *currentMKSession);

