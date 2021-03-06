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

 /*! \file   tagDataBlock.h

    \brief  Functions and data definitions for implementation of Data Block tag.


    INFO:
    -----
    All data blocks have html tag properties, and html content part. 
	TAGDataBlock structure holds all data block html tag properties and contents in order to print them later. \n
    Content part of data block must have only one html tag which is
    \n
    \n
    >    "<table></table>"
    \n
    "<table>" tag can have:
    >    "<thead></thead>"  <br>
    >    "<tfoot></tfoot>"  <br>
    >    "<tbody></tbody>"
    \n
    "<TBODY>" tag must have only one "<TR>...</TR>" tag.
    \n
    \n
    All contents of data block (head, foot and body) are converted to TAGGeneric object instances and stored as a linked list.
    Linked list can be iterated by starting with rootTAGGeneric variable.
    tailTAGGeneric variable is the last TAGGeneric instance which show the last tag "</tr>" or text in the "<TBODY>".
    \n
    \n
    All html tags and texts inside "<TBODY>" are called DYNAMIC tag/content. Dynamic content is as repeated as row count of the select query result.
    TAGGeneric object has __isDynamicContent__ property to indicate it is dynamic or not.
    \n
    \n
    First dynamic tag instance address is stored in startTAG variable. Last dynamic tag instance address is stored in endTAG variable.
    startTAG and endTAG instances are assigned during *.frmx file parsing phase. Until the file is reloaded, these values remain same.
    So, there is no need to iterate to find them in rootTAGGeneric linked list.
    \n

*/

#pragma once






/* object definitions & manipulation */
void                destroyTAGDataBlock(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGObject);
void				destroyTAGDatablockTAGGenericObjectInstances(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock);
TAGDataBlock *      createTAGDataBlock(SodiumSession *currentMKSession, HTSQLPage *page);
TAGDataBlock *		getCurrentTAGDataBlock(SodiumSession *currentMKSession, HTSQLPage *page);
void                pushTAGDataBlock(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGObject);

void				setDatablockConnection(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock * pTAGDataBlock);


/** \brief  DO NOT FREE RETURN VALUE
*/
char*
GetDatablockKeyColumnValue(
    SodiumSession* session,
    HTSQLPage* page,
    TAGDataBlock* pTAGDataBlock
    );

/** \brief If finds the data block instance, return it as TAGDataBlock *, otherwise returns NULL.
 *
 * \param currentSodiumSession MKSession*
 * \param pDataBlockName char*
 * \return TAGDataBlock       *
 *
 */
TAGDataBlock  *  findTAGDataBlockByName(SodiumSession *currentMKSession, HTSQLPage *page, const char *pDataBlockName);
void             setCurrentTAGDataBlock(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGObject);
void       setCurrentTAGDataBlockPropertyBlockName(SodiumSession *currentMKSession, HTSQLPage *page, const char *pPropValue);
void     setCurrentTAGDataBlockPropertyRecordCount(SodiumSession *currentMKSession, HTSQLPage *page,  char *pPropValue);
void     setCurrentTAGDataBlockPropertyWhereClause(SodiumSession *currentMKSession, HTSQLPage *page,  char *pPropValue);
void   setCurrentTAGDataBlockPropertyInsertAllowed(SodiumSession *currentMKSession, HTSQLPage *page,  char *pPropValue);
void   setCurrentTAGDataBlockPropertyUpdateAllowed(SodiumSession *currentMKSession, HTSQLPage *page,  char *pPropValue);
void   setCurrentTAGDataBlockPropertyDeleteAllowed(SodiumSession *currentMKSession, HTSQLPage *page,  char *pPropValue);
void   setCurrentTAGDataBlockPropertyOrderByClause(SodiumSession *currentMKSession, HTSQLPage *page,  char *pPropValue);
void  setCurrentTAGDataBlockPropertyDataSourceName(SodiumSession *currentMKSession, HTSQLPage *page,  char *pPropValue);
void   setCurrentTAGDataBlockPropertyDataSchemaName(SodiumSession *currentMKSession, HTSQLPage *page, char *pPropValue);
void  setCurrentTAGDataBlockPropertyConnectionName(SodiumSession *currentMKSession, HTSQLPage *page,  char *pPropValue);
void setCurrentTAGDataBlockMasterPropertyBlockName(SodiumSession *currentMKSession, HTSQLPage *page, const char *pPropValue);
void   setCurrentTAGDataBlockPropertyJoinCondition(SodiumSession *currentMKSession, HTSQLPage *page,  char *pPropValue);
void setCurrentTAGDataBlockPropertyAutoGeneratedColumns(SodiumSession *currentMKSession, HTSQLPage *page, char *pPropValue);
void setCurrentTAGDataBlockPropertyKeyColumnName(SodiumSession* currentMKSession, HTSQLPage* page, const char* pPropValue);

void setTAGDataBlockPropertyJoinConditionEvaluated(SodiumSession *currentMKSession, TAGDataBlock *pTAGDatablock, const char *pPropValue);
void setTAGDataBlockPropertyJoinCondition(SodiumSession *currentMKSession, TAGDataBlock *tagDatablock, char *pPropValue);
void setTAGDataBlockPropertyWhereClause(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *tagDatablock, char *pPropValue);
void setTAGDataBlockPropertyWhereClauseEvaluated(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *tagDatablock, char *pPropValue);
void updateOrInsertAllDataBlocksCurrentRow(SodiumSession *currentMKSession, HTSQLPage *page);

void setTAGDataBlockPropertyVisibility(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *tagDatablock, bool pPropValue);

void setTAGDataBlockDatabasePrivileges(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock);


/** \brief Generates and send all the contents of the data block.
 *
 * \param currentMKSession
 * \param pTAGDataBlock        TAGDataBlock instance address which its all contents will be printed. This instance can be obtained by calling findTAGDataBlockByName(SodiumSession *currentMKSession, char *pDataBlockName).
 * \return void
 *
 */
void	printTAGDataBlockDynamicContent(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock);
void	printTAGDataBlockDynamicContent_NoAuto(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock);
void	printTAGDataBlockDynamicContent_Auto(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock);
void	printTAGDataBlockStandartResponseScriptHeader(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock);
void	printTAGDataBlockStandartResponseScriptFooter(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock);

TAGInput* findTAGDataBlockCurrentRowTAGInputItem(SodiumSession *currentMKSession, TAGDataBlock *pTAGDataBlock, const char *tagInputItemName);

const char *getBlockItemColumnName(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock, const char *itemName);

void _setTAGDatablockDataSourceName(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *current, const char *pPropValue);
void _setTAGDataBlockSchemaName(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *current, const char *pPropValue);

void setCurrentTAGDataBlockPropertyDataSchemaSourceName(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *dataBlockInstance, const char *pSchemaName, const char *pSourceName);

void
SetTAGDataBlockWritable(
    SodiumSession* session,
    HTSQLPage* page,
    TAGDataBlock* pTAGDataBlock
);

/*	Returns SQL command to check if data block pTAGDataBlock datasource has a column which identifies the row 
	CALLER MUST FREE RESULT
*/
char	*getTAGDataBlockIsRowIdExistsSQLCommandForOracle(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock);

/*	Returns SQL command to check if data block pTAGDataBlock datasource has a column which identifies the row
	CALLER MUST FREE RESULT
*/
char	*getTAGDataBlockIsRowIdExistsSQLCommandForPostgresql(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock);


