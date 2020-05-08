/*! \file   tagDataBlockDatabaseOperations.h

    \brief  Contains functions for data block database DML operations.
*/

#pragma once




char          *getTAGDataBlockSQLForSingleRow(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock, const char *rowid);
DBInt_Statement *getResultSetForSingleRow(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock * pTAGDataBlock, const char *rowid);
void          __sendRefreshSingleBlockDataRow(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock, const char *rowid, DBInt_Statement *stm);

/** \brief
 *
 * \param currentSodiumSession MKSession*
 * \param pTAGDataBlock TAGDataBlock*
 * \param OCI_Statement   *stm
 * \return OCI_Resultset                                             *
 *
 *  __THIS FUNCTION CANNOT SEND RESPONSE ACTION TO THE CLIENT__ since it can be called by another function which already in another response action.
 *  \n
 *  If this function send a response action or calls another function which send response action, java script syntax error occurred.
 *  \n
 *  pTAGDataBlock's connection related properties (dataSourceName, connectionName, tagConnection vs) should be valid before calling this function.
 */
DBInt_Statement           *getTAGDataBlockRecordSet(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock * pTAGDataBlock);
char                       *getTAGDataBlockSelectSQL(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock);

/** \brief Returns database select command column names in comma separated. 
 *          __CALLER IS RESPONSIBLE TO FREE RETURN VALUE__
 *
 * \param pTAGDataBlock TAGDataBlock*
 * \return char  *
 *
 */
char    *getTAGDataBlockColumnNames(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock);

/** \brief Returns database select command table name.
*          __CALLER IS RESPONSIBLE TO FREE RETURN VALUE__
*
* \param pTAGDataBlock TAGDataBlock*
* \return char  *
*
*/
char	*getTAGDataBlockTableName(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock);


/** \brief Generates update or insert command for modified columns
 *
 * \param currentSodiumSession MKSession*
 * \param pTAGDataBlock TAGDataBlock*
 * \return char*                        Returns new row id if the command type is inserted. otherwise, return NULL. Caller must FREE returned value.
 *
 *  Old values are in:
 >      currentMKSession->currentRowId, currentMKSession->currentDatablockName
 *  New values are in:
 >      pTAGDataBlock->currentRowRowIdTAGInput->value, pTAGDataBlock->dataBlockName
 */
char* updateOrInsertDataBlockCurrentRow(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock);


void setBlockItemsValueStateToClear(SodiumSession *currentMKSession, TAGDataBlock *pTAGDataBlock);

/** \brief Updates the current row of a data block.
 *
 * \param currentSodiumSession MKSession*
 * \param pTAGDataBlock TAGDataBlock*
 * \return void
 *
 *  Updates database columns if :
 *      "pre_update" HT/SQL trigger function does not exists for data block,
 *      "pre_update" HT/SQL trigger function does exists and returns true,
 *      No error occurs
 */
void updateDataBlockCurrentRow(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock);


/** \brief Insert data block's dirty items into database
 *
 * \param currentSodiumSession MKSession*
 * \param pTAGDataBlock TAGDataBlock*
 * \return const char*                  If a new row is inserted into database successfully, returns the row id of the newly created row.
 *                                      Returns NULL if :
 *                                          Error occurs,
 *                                          "pre_insert" trigger return's "false".
 *										Caller is responsible to FREE return value if it is not null
 *
 */
char *insertDataBlockCurrentRow(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock);


/** \brief Deletes the current row of a data block.
 *
 * \param currentSodiumSession MKSession*
 * \param pTAGDataBlock TAGDataBlock*
 * \return void
 *
 *  Delete database row if :
 *      "pre_delete" HT/SQL trigger function does not exists for data block,
 *      "pre_delete" HT/SQL trigger function does exists and returns true,
 *      No error occurs
 */
void deleteDatablockCurrentRow(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock);


/** \brief
 *
 * \param currentSodiumSession MKSession*
 * \param pTAGDataBlock TAGDataBlock*
 * \return char*
 *
 *  __CALLER IS RESPONSIBLE FOR DESTROYING RETURN VALUE__
 */
char* getUpdateSQLCommandForRow(SodiumSession *currentMKSession, TAGDataBlock *pTAGDataBlock);

/** \brief
 *
 * \param currentSodiumSession MKSession*
 * \param pTAGDataBlock TAGDataBlock*
 * \return char*
 *
 *  __CALLER IS RESPONSIBLE FOR DESTROYING RETURN VALUE__
 */
char* getInsertSQLCommandForRow(SodiumSession *currentMKSession, TAGDataBlock *pTAGDataBlock);

/** \brief Return sql delete command as char*.
 *
 * \param currentSodiumSession MKSession*
 * \param pTAGDataBlock TAGDataBlock*
 * \return char*
 *
 *  __CALLER IS RESPONSIBLE FOR DESTROYING RETURN VALUE__
 */
char* getDeleteSQLCommandForRow(SodiumSession *currentMKSession, TAGDataBlock *pTAGDataBlock);

/*	If inssert operation is not possible for some reason, call this function to cleanup
*/
void insertRejectionOccured(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock);
/*	If update operation is not possible for some reason, call this function to cleanup
*/
void updateRejectionOccured(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock, TAGConnection *tagConnection);

/** \brief __CALLER IS RESPONSIBLE FOR RELASE RETURN VALUE if it is not null__
 *
 * \param currentSodiumSession MKSession*
 * \param ociConnection TAGConnection*
 * \param sequenceName const char*
 * \return char*
 *
 */
char* getSequenceValue(SodiumSession *currentMKSession, HTSQLPage *page, TAGConnection *ociConnection, const char *sequenceSchemaName, const char *sequenceName);

char* getSequenceSQLCommand(SodiumSession *currentMKSession, TAGConnection *connection, const char *sequenceSchemaName, const char *sequenceName);
void evaluateTAGDataBlockJoinCondition(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock);
void   evaluateTAGDataBlockWhereClause(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock);


