#include "pch.h"

#include "tagDataBlockDatabaseOperations.h"

#include "rowSelection.h"
#include "tagConnection.h"
#include "postDelete.h"
#include "postUpdate.h"
#include "postUpdate.h"
#include "postInsert.h"
#include "post.parser.imp.h"
#include "preUpdate.h"
#include "preInsert.h"
#include "preDelete.h"
#include "tagDataBlockCurrentRow.h"
#include "tagDataBlock.h"


void updateRejectionOccured(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGDataBlock, TAGConnection *tagConnection) {
	setBlockItemsValueStateToClear(session, pTAGDataBlock);
	DBInt_Statement *rs = getResultSetForSingleRow(session, page, pTAGDataBlock, pTAGDataBlock->keyColumnValue);
	if (rs) {
		__sendRefreshSingleBlockDataRow(session, page, pTAGDataBlock, pTAGDataBlock->keyColumnValue, rs);
	}
	else {
		__sendErrorResponse(session, page, "hata", "");
	}
	__sendUpdateRejectedResponse(session, pTAGDataBlock, pTAGDataBlock->keyColumnValue);
	DBInt_FreeStatement(tagConnection->mkDBConnection, rs);
}

void insertRejectionOccured(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGDataBlock) {
	setBlockItemsValueStateToClear(session, pTAGDataBlock);
	__sendInsertRejectedResponse(session, pTAGDataBlock, pTAGDataBlock->keyColumnValue);
	__sendRefreshSingleBlockDataRow(session, page, pTAGDataBlock, pTAGDataBlock->keyColumnValue, NULL);
}


DBInt_Statement * getResultSetForSingleRow(SodiumSession *session, HTSQLPage *page, TAGDataBlock * pTAGDataBlock, const char *rowid) {

	TAGConnection *tagConn = findTAGConnectionByName(session, page, pTAGDataBlock->connectionName, true);
	DBInt_Statement *stm = DBInt_CreateStatement(tagConn->mkDBConnection);
	
	char *sql = getTAGDataBlockSQLForSingleRow(session, page, pTAGDataBlock, rowid);
	
	DBInt_Prepare(tagConn->mkDBConnection, stm, sql);
	if (pTAGDataBlock->tagConnection->mkDBConnection->errText) {
		CLOG(ERR_DB_UNKNOWN_ERROR, pTAGDataBlock->tagConnection->mkDBConnection->errText);
		DBInt_FreeStatement(pTAGDataBlock->tagConnection->mkDBConnection, stm);
		stm = NULL;
	}
	else {
		DBInt_ExecuteSelectStatement(tagConn->mkDBConnection, stm, sql);
		if (pTAGDataBlock->tagConnection->mkDBConnection->errText) {
			CLOG(ERR_DB_UNKNOWN_ERROR, pTAGDataBlock->tagConnection->mkDBConnection->errText);
			DBInt_FreeStatement(pTAGDataBlock->tagConnection->mkDBConnection, stm);
			stm = NULL;
		}
		else {
			DBInt_First(tagConn->mkDBConnection, stm);
		}
	}
	
	mkFree(session->heapHandle, sql);
	return stm;
}


char *getTAGDataBlockSQLForSingleRow(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGDataBlock, const char *rowid) {

	char *lColumnNames = getTAGDataBlockColumnNames(session, page, pTAGDataBlock);
	char *lTableName = getTAGDataBlockTableName(session, page, pTAGDataBlock);  pTAGDataBlock->dataSourceName;

	const char *lRowIdColName = SetTAGDataBlockRowIdColumnName(session, page, pTAGDataBlock);

	char *sql = mkStrcat(session->heapHandle, __FILE__, __LINE__,
		"select ",
		lColumnNames,
		" from ",
		lTableName,
		" where ",
		lRowIdColName, " = '", rowid, "' ",
		NULL);

	if (pTAGDataBlock->joinCondition && page->currentRequestMethodType == REQUEST_TYPE_POST) {
		evaluateTAGDataBlockJoinCondition(session, page, pTAGDataBlock);
	}
	if (pTAGDataBlock->joinConditionEvaluated) {
		char *tmp = mkStrcat(session->heapHandle, __FILE__, __LINE__, sql, " and (", pTAGDataBlock->joinConditionEvaluated, ") ", NULL);
		mkFree(session->heapHandle, sql);
		sql = tmp;
	}

	mkFree(session->heapHandle, lTableName);
	return sql;
}



DBInt_Statement *getTAGDataBlockRecordSet(SodiumSession *session, HTSQLPage *page, TAGDataBlock * pTAGDataBlock) {
    
	int firstRowNo;
		
	char *sql = getTAGDataBlockSelectSQL(session, page, pTAGDataBlock);

	DBInt_Statement *stm = DBInt_CreateStatement(pTAGDataBlock->tagConnection->mkDBConnection);
	DBInt_Prepare(pTAGDataBlock->tagConnection->mkDBConnection, stm, sql);
	
	if (pTAGDataBlock->tagConnection->mkDBConnection->errText) {
		CLOG(ERR_DB_UNKNOWN_ERROR, pTAGDataBlock->tagConnection->mkDBConnection->errText);
	}
	DBInt_ExecuteSelectStatement(pTAGDataBlock->tagConnection->mkDBConnection, stm, sql);
	if (pTAGDataBlock->tagConnection->mkDBConnection->errText) {
		CLOG(ERR_DB_UNKNOWN_ERROR, pTAGDataBlock->tagConnection->mkDBConnection->errText);
	}
	else {

		if (pTAGDataBlock->currentPageNo == 1) {
			firstRowNo = 1;
		}
		else {
			firstRowNo = ((pTAGDataBlock->currentPageNo - 1) * pTAGDataBlock->recordCount) + 1;
		}
		DBInt_Seek(pTAGDataBlock->tagConnection->mkDBConnection, stm, firstRowNo);
	
	}
	if (sql) {
		mkFree(session->heapHandle, sql);
	}

	return stm;
}

char *getTAGDataBlockColumnNames(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGDataBlock) {
	size_t size = 1500;
	char *lColumnNames = (char*)mkMalloc(session->heapHandle, size, __FILE__, __LINE__);
	*lColumnNames = '\0';

	// key column name 
	/*if (pTAGDataBlock->keyColumnName) {
		const char *lRowIdColumnName = SetTAGDataBlockRowIdColumnName(session, page, pTAGDataBlock);
		strcpy_s(lColumnNames, size, lRowIdColumnName);
		strcat_s(lColumnNames, size, " as \"rowid\"");
	}*/
	// other columns
	if (pTAGDataBlock->autoGeneratedColumns) {
		if (*lColumnNames) {
			strcat_s(lColumnNames, size, ","); 
			if (pTAGDataBlock->autoGeneratedColumns[0] == '*') {
				strcat_s(lColumnNames, size, pTAGDataBlock->dataSourceName);
				strcat_s(lColumnNames, size, ".");
			}
			strcat_s(lColumnNames, size, pTAGDataBlock->autoGeneratedColumns);
		}
		else {
			strcat_s(lColumnNames, size, pTAGDataBlock->autoGeneratedColumns); 
		}
	}
	else {
		TAGInput *input = pTAGDataBlock->rootTAGInput;
		while (input) {
			if (input->columnName) {
				if (*lColumnNames) {
					strcat_s(lColumnNames, size, ",");
				}
				strcat_s(lColumnNames, size, input->columnName);
			}
			input = input->next;
		}
	}

	return lColumnNames;
}

char *getTAGDataBlockTableName(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGDataBlock) {
	char *lTableName = NULL;
	if (pTAGDataBlock->dataSchemaName) {
		lTableName = mkStrcat(session->heapHandle, __FILE__, __LINE__, pTAGDataBlock->dataSchemaName, ".", pTAGDataBlock->dataSourceName, NULL);
	}
	else {
		lTableName = mkStrdup(session->heapHandle, pTAGDataBlock->dataSourceName, __FILE__, __LINE__);
	}
	return lTableName;
}

char *getTAGDataBlockSelectSQL(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGDataBlock) {

    char *lColumnNames = getTAGDataBlockColumnNames(session, page, pTAGDataBlock);
	char *lTableName = getTAGDataBlockTableName(session, page, pTAGDataBlock);

    char *sql = mkStrcat(session->heapHandle, __FILE__, __LINE__,
                            "select ",
                                lColumnNames,
                            " from ",
                                lTableName,
                            " where 1=1 ",
                            NULL );
    
	// join condition
	if (pTAGDataBlock->joinCondition && page->currentRequestMethodType == REQUEST_TYPE_POST) {
        evaluateTAGDataBlockJoinCondition(session, page, pTAGDataBlock);
    }
	if (pTAGDataBlock->joinConditionEvaluated) {
        char *tmp = mkStrcat(session->heapHandle, __FILE__, __LINE__, sql, " and (", pTAGDataBlock->joinConditionEvaluated, ") ", NULL);
        mkFree(session->heapHandle, sql);
        sql = tmp;
    }
	
	// where & execute query filter clause
    if (pTAGDataBlock->whereClause || pTAGDataBlock->executeQueryModeWhereClause) {

        if (pTAGDataBlock->whereClause && strlen(pTAGDataBlock->whereClause) > 0) {
            char *tmp = mkStrcat(session->heapHandle, __FILE__, __LINE__,
                                 sql,
                                 " and (",
                                 (pTAGDataBlock->whereClauseEvaluated) ?  pTAGDataBlock->whereClauseEvaluated : pTAGDataBlock->whereClause,
                                 ") ",
                                 NULL);
            mkFree(session->heapHandle, sql);
            sql = tmp;
        }
        if (pTAGDataBlock->executeQueryModeWhereClause && strlen(pTAGDataBlock->executeQueryModeWhereClause) > 0) {
            char *tmp = mkStrcat(session->heapHandle, __FILE__, __LINE__, sql, " and (", pTAGDataBlock->executeQueryModeWhereClause, ") ", NULL);
            mkFree(session->heapHandle, sql);
            sql = tmp;
        }
    }

	// order by clause
    if (pTAGDataBlock->orderByClause && strlen(pTAGDataBlock->orderByClause) > 0) {
        char *tmp = mkStrcat(session->heapHandle, __FILE__, __LINE__, sql, " order by ", pTAGDataBlock->orderByClause, NULL);
        mkFree(session->heapHandle, sql);
        sql = tmp;
    }

	// Free resources
	if (lColumnNames) {
		mkFree(session->heapHandle, lColumnNames);
	}
	if (lTableName) {
		mkFree(session->heapHandle, lTableName);
	}
    return sql;
}

char* getSequenceSQLCommand(SodiumSession *session, TAGConnection *connection, const char *sequenceSchemaName, const char *sequenceName) {
	size_t size = 500;
	char *sql = mkMalloc(session->heapHandle, size, __FILE__, __LINE__);

	switch(connection->mkDBConnection->dbType) {
		case SODIUM_ORACLE_SUPPORT: {			
			strcpy_s(sql, size, "select ");
			if (sequenceSchemaName) {
				strcat_s(sql, size, sequenceSchemaName);
				strcat_s(sql, size, ".");
			}
			strcat_s(sql, size, sequenceName);
			strcat_s(sql, size, ".nextval SEQVAL from dual");
			break;
		}
		case SODIUM_POSTGRESQL_SUPPORT: {
			strcpy_s(sql, size, "select nextval('");
			if (sequenceSchemaName) {
				strcat_s(sql, size, sequenceSchemaName);
				strcat_s(sql, size, ".");
			}
			strcat_s(sql, size, sequenceName);
			strcat_s(sql, size, "') seqval");
			break;
		}
		case SODIUM_MYSQL_SUPPORT: {
			ULONGLONG tickCount = GetTickCount64();
			sprintf_s(sql, size, "select %llu as seqval", tickCount);
			break;
		}
		case SODIUM_SQLSERVER_SUPPORT: {
			strcpy_s(sql, size, "SELECT NEXT VALUE FOR ");
			if (sequenceSchemaName) {
				strcat_s(sql, size, sequenceSchemaName);
				strcat_s(sql, size, ".");
			}
			strcat_s(sql, size, sequenceName);
			strcat_s(sql, size, " as seqval");
			break;
		}
	}
	
    return sql;
}

char* getSequenceValue(SodiumSession *session, HTSQLPage *page, TAGConnection *ociConnection, const char *sequenceSchemaName, const char *sequenceName) {
	char *retVal = NULL;

	if (ociConnection == NULL) {
		CLOG(ERR_DB_INVALID_CONNECTION, "");
		return NULL;
	}
	
	DBInt_Connection *conn = ociConnection->mkDBConnection;
	if (conn == NULL) {
		CLOG(ERR_DB_INVALID_CONNECTION, "");
		return NULL;
	}

	// Some database does not have SEQUENCE. So getting a sequence value is different
	switch (ociConnection->mkDBConnection->dbType)
	{
		case SODIUM_MYSQL_SUPPORT:
		case SODIUM_POSTGRESQL_SUPPORT:
		case SODIUM_SQLSERVER_SUPPORT:
		case SODIUM_ORACLE_SUPPORT: {
			char* sql = getSequenceSQLCommand(session, ociConnection, sequenceSchemaName, sequenceName);
			DBInt_Statement* stm = DBInt_CreateStatement(ociConnection->mkDBConnection);
			DBInt_Prepare(ociConnection->mkDBConnection, stm, sql);
			if (conn->errText == NULL) {
				DBInt_ExecuteSelectStatement(ociConnection->mkDBConnection, stm, sql);
				if (conn->errText == NULL) {
					const char* seqVal = DBInt_GetColumnValueByColumnName(ociConnection->mkDBConnection, stm, "SEQVAL");
					retVal = mkStrdup(session->heapHandle, seqVal, __FILE__, __LINE__);
				}
			}
			if (conn->errText) {
				CLOG(ERR_DB_UNKNOWN_ERROR, conn->errText);

				// free resources
				mkFree(session->heapHandle, sql);
				DBInt_FreeStatement(ociConnection->mkDBConnection, stm);
				break;
			}
		}
		default:
			break;
	}

	return retVal;
}


char* getDeleteSQLCommandForRow(SodiumSession *session, TAGDataBlock *pTAGDataBlock) {
	size_t size = 500;
    char *sql = mkMalloc(session->heapHandle, size, __FILE__, __LINE__);
    strcpy_s(sql, size, "delete from ");
	if (pTAGDataBlock->dataSchemaName) {
		strcat_s(sql, size, pTAGDataBlock->dataSchemaName);
		strcat_s(sql, size, ".");
	}
    strcat_s(sql, size, pTAGDataBlock->dataSourceName);
	strcat_s(sql, size, " where ");

	strcat_s(sql, size, pTAGDataBlock->keyColumnName);
	if (pTAGDataBlock->keyColumnType == INPUT_TAG_DATA_TYPE_NUMBER) {
		strcat_s(sql, size, pTAGDataBlock->keyColumnValue);
	}
	else {
		strcat_s(sql, size, " = '");
		strcat_s(sql, size, pTAGDataBlock->keyColumnValue);
		strcat_s(sql, size, "'");
	}

    return sql;
}

char* getInsertSQLCommandForRow(SodiumSession *session, TAGDataBlock *pTAGDataBlock) {
    bool isNotFirstRow;
	size_t paramCount = 1;
	char paramCountStr[4];
	size_t size = 1500;
    TAGInput *input;
    char *sql = mkMalloc(session->heapHandle, size, __FILE__, __LINE__);
    strcpy_s(sql, size, "insert into ");
	if (pTAGDataBlock->dataSchemaName) {
		strcat_s(sql, size, pTAGDataBlock->dataSchemaName);
		strcat_s(sql, size, ".");
	}
    strcat_s(sql, size, pTAGDataBlock->dataSourceName);

    // ******
    // **** LOOP FOR COLUMN NAMES
    // ******
    isNotFirstRow = false;
    strcat_s(sql, size, " (");
    input = pTAGDataBlock->rootTAGInput;
    while(input != NULL) {
        /** if column name is empty, we are not adding the column into insert statement since it is a formula column. */
        if (input->columnName != NULL) {

            /** For insert statements, column's values entered by user will only be inserted.
                The exception is that if the column is foreign key or gets its value from sequence, it's value will be inserted automatically. */
            if (input->valueState == INPUT_TAG_VALUE_DIRTY || input->masterItemName != NULL || input->sequenceName != NULL) {
                if (isNotFirstRow) {
                    strcat_s(sql, size, ",");
                }
                strcat_s(sql, size, input->columnName);
                isNotFirstRow = true;
            }
        }
        input = input->next;
    }
    // ******
    // **** LOOP FOR VALUES
    // ******
    isNotFirstRow = false;
    strcat_s(sql, size, ") values (");

    input = pTAGDataBlock->rootTAGInput;
    while(input) {

        /** if column name is empty, we are not adding the column into insert statement since it is a formula column. */
        if (input->columnName) {

            /** For insert statements, values entered by user will be inserted.
                The exception is that if the column is foreign key or gets its value from sequence, it's value will be inserted automatically. */
            if (input->valueState == INPUT_TAG_VALUE_DIRTY || input->masterItemName || input->sequenceName) {
                if (isNotFirstRow) {
                    strcat_s(sql, size, ",");
                }
                if (input->inputType == INPUT_TAG_TYPE_IMAGE
					|| input->inputType == INPUT_TAG_TYPE_HIDDEN
                    || input->inputType == INPUT_TAG_TYPE_TEXT
                    || input->inputType == INPUT_TAG_TYPE_PASSWORD
                    || input->inputType == INPUT_TAG_TYPE_SELECT) {
                    
					switch(pTAGDataBlock->tagConnection->mkDBConnection->dbType) {
						case SODIUM_MYSQL_SUPPORT: {
							strcat_s(sql, size, "?");
							break;
						}
						case SODIUM_ORACLE_SUPPORT: {
							strcat_s(sql, size, ":");
							strcat_s(sql, size, input->name);
							break;
						}
						case SODIUM_POSTGRESQL_SUPPORT: {
							mkItoa(paramCount, paramCountStr);
							if (input->inputType == INPUT_TAG_TYPE_IMAGE) {
								strcat_s(sql, size, "$");
								strcat_s(sql, size, paramCountStr);
							}
							else {
								strcat_s(sql, size, "$");
								strcat_s(sql, size, paramCountStr);
							}							
							paramCount++;
							break;
						}
						case SODIUM_SQLSERVER_SUPPORT: {
							mkItoa(paramCount, paramCountStr);
							if (input->inputType == INPUT_TAG_TYPE_IMAGE) {
								strcat_s(sql, size, "?");
							}
							else {
								strcat_s(sql, size, "?");
							}							
							paramCount++;
							break;
						}
					}					
                
				} else {
                    strcat_s(sql, size, "'");
                    strcat_s(sql, size, (input->value) ? input->value:"");
                    strcat_s(sql, size, "'");
                }

                isNotFirstRow = true;
            }
        }
        input = input->next;
    }
    strcat_s(sql, size, " ) ");

    return sql;
}


char* getUpdateSQLCommandForRow(SodiumSession *session, TAGDataBlock *pTAGDataBlock) {
	size_t paramCount = 1;
	char paramCountStr[4];
	size_t size = 1500;
    bool isNotFirstRow = false;
    char *sql = mkMalloc(session->heapHandle, size, __FILE__, __LINE__);
    strcpy_s(sql, size, "update ");
	if (pTAGDataBlock->dataSchemaName) {
		strcat_s(sql, size, pTAGDataBlock->dataSchemaName);
		strcat_s(sql, size, ".");
	}
    strcat_s(sql, size, pTAGDataBlock->dataSourceName);
    strcat_s(sql, size, " set ");
    TAGInput *input = pTAGDataBlock->rootTAGInput;

    while(input != NULL) {
        if (input->columnName && input->valueState == INPUT_TAG_VALUE_DIRTY) {
            if (isNotFirstRow) {
                strcat_s(sql, size, ",");
            }
            strcat_s(sql, size, input->columnName);
            strcat_s(sql, size, "=");

            if (input->inputType == INPUT_TAG_TYPE_IMAGE
                || input->inputType == INPUT_TAG_TYPE_TEXT
                || input->inputType == INPUT_TAG_TYPE_PASSWORD
                || input->inputType == INPUT_TAG_TYPE_SELECT) {

				switch (pTAGDataBlock->tagConnection->mkDBConnection->dbType) {
					case SODIUM_MYSQL_SUPPORT: {
						strcat_s(sql, size, " ? ");
						break;
					}
					case SODIUM_ORACLE_SUPPORT: {
						strcat_s(sql, size, ":");
						strcat_s(sql, size, input->name);
						break;
					}
					case SODIUM_POSTGRESQL_SUPPORT: {
						mkItoa(paramCount, paramCountStr);
						strcat_s(sql, size, "$");
						strcat_s(sql, size, paramCountStr);
						paramCount++;
						break;
					}
					case SODIUM_SQLSERVER_SUPPORT: {
						strcat_s(sql, size, " ? ");
						paramCount++;
						break;
					}
				}

            } else {
                strcat_s(sql, size, "'");
                strcat_s(sql, size, input->value);
                strcat_s(sql, size, "'");
            }

            isNotFirstRow = true;
        }
        input = input->next;
    }
    strcat_s(sql, size, " where ");

	strcat_s(sql, size, pTAGDataBlock->keyColumnName);
	strcat_s(sql, size, " = '");
	strcat_s(sql, size, pTAGDataBlock->keyColumnValue);
	strcat_s(sql, size, "'");

    return sql;
}

void setBlockItemsValueStateToClear(SodiumSession *session, TAGDataBlock *pTAGDataBlock) {
    TAGInput *input = pTAGDataBlock->rootTAGInput;
    while(input != NULL) {
        if (input->valueState == INPUT_TAG_VALUE_DIRTY) {
            input->valueState = INPUT_TAG_VALUE_CLEAR;
        }
        input = input->next;
    }
}

void deleteDatablockCurrentRow(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGDataBlock) {
    char *sql = getDeleteSQLCommandForRow(session, pTAGDataBlock);

    TAGConnection *tagConnection = findTAGConnectionByName(session, page, pTAGDataBlock->connectionName, true);
	DBInt_Statement *stm = DBInt_CreateStatement(tagConnection->mkDBConnection);

	DBInt_Prepare(tagConnection->mkDBConnection, stm, sql);

    DBInt_ExecuteDeleteStatement(tagConnection->mkDBConnection, stm, sql);
	
    if (tagConnection->mkDBConnection->errText) {
		CLOG(ERR_DB_UNKNOWN_ERROR, tagConnection->mkDBConnection->errText);
    } else {
        runPostDeleteHTSQLTriggerFunction(session, page, pTAGDataBlock);
    }

	DBInt_FreeStatement(tagConnection->mkDBConnection, stm);
	mkFree(session->heapHandle, sql);
}

void updateDataBlockCurrentRow(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGDataBlock) {

    TAGConnection *tagConnection = findTAGConnectionByName(session, page, pTAGDataBlock->connectionName, true);

	DBInt_Statement *stm = DBInt_CreateStatement(tagConnection->mkDBConnection);

    bool runTrigger = runPreUpdateHTSQLTriggerFunction(session, page, pTAGDataBlock);
    if (runTrigger == false) {
		updateRejectionOccured(session, page, pTAGDataBlock, tagConnection);
    } else {
        char *sql = getUpdateSQLCommandForRow(session, pTAGDataBlock);
        
		DBInt_Prepare(tagConnection->mkDBConnection, stm, sql);
		BindSQLVariables(session, page, pTAGDataBlock, stm, SQL_COMMAND_TYPE_UPDATE);
		DBInt_ExecuteUpdateStatement(tagConnection->mkDBConnection, stm, sql);

        if (tagConnection->mkDBConnection->errText) {
			CLOG(ERR_DB_UNKNOWN_ERROR, tagConnection->mkDBConnection->errText);
			updateRejectionOccured(session, page, pTAGDataBlock, tagConnection);
        } else {
            /** dirty items are updated to database, so we are setting items current value state to INPUT_TAG_VALUE_CLEAR
                With this modification, server do update once even if you move from one row to another  */
            setBlockItemsValueStateToClear(session, pTAGDataBlock);
            __sendUpdateSuccessfulResponse(session, pTAGDataBlock, pTAGDataBlock->keyColumnValue);
            runPostUpdateHTSQLTriggerFunction(session, page, pTAGDataBlock);
        }
        mkFree(session->heapHandle, sql);
    }

	DBInt_FreeStatement(tagConnection->mkDBConnection, stm);
}

void __sendRefreshSingleBlockDataRow(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGDataBlock, const char *rowid, DBInt_Statement *stm) {
    char *lastResponse = NULL;

    setTAGDataBlockCurrentRowValues(session, page, pTAGDataBlock, stm);

    lastResponse = mkStrcat(session->heapHandle, __FILE__, __LINE__,
                            "action = { "
                                            "\"type\"            : \"refreshDataBlockRow\", "
                                            "\"dataBlockName\"   : \"", pTAGDataBlock->dataBlockName, "\", ",
                                            "\"oldrowid\"        : \"", rowid, "\", ",  // it is the row id which will be read again from the database
                                            "\"newrowid\"        : \"", pTAGDataBlock->keyColumnValue, "\", ",  /*  it is generated row id if we are attempting to show an empty row.*/
                                            "\"innerRowHTML\"    : \"",
                            NULL
                          );
    mkPrint(session, lastResponse, NULL);
    mkFree(session->heapHandle, lastResponse);

    page->isJavaScriptEscapeEnabled = true;
    printTAGDataBlockCurrentRow(session, page, pTAGDataBlock);
    page->isJavaScriptEscapeEnabled = false;

    lastResponse = mkStrcat(session->heapHandle, __FILE__, __LINE__,
                            "\" "
                            " };",
                          "addNewActionToTheResponseActionList(action);",
                          NULL);

    mkPrint(session, lastResponse, NULL);
    mkFree(session->heapHandle, lastResponse);
}


char *insertDataBlockCurrentRow(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGDataBlock) {
	
	char *retVal = NULL;
	TAGConnection *tagConnection = findTAGConnectionByName(session, page, pTAGDataBlock->connectionName, true);

    bool runTrigger = runPreInsertHTSQLTriggerFunction(session, page, pTAGDataBlock);
    if (runTrigger == false) {
		insertRejectionOccured(session, page, pTAGDataBlock);
        return NULL;
    }

    char *sql = getInsertSQLCommandForRow(session, pTAGDataBlock);

	/*
	switch (pTAGDataBlock->tagConnection->mkDBConnection->dbType) {
		case SODIUM_ORACLE_SUPPORT: {
			sqlWithReturning = mkStrcat(session->heapHandle, __FILE__, __LINE__, sql, " returning CAST(", pTAGDataBlock->keyColumnName, " AS varchar2(100)) into :new_row_id", NULL);
			break;
		}
		case SODIUM_POSTGRESQL_SUPPORT: {
			sqlWithReturning = mkStrcat(session->heapHandle, __FILE__, __LINE__, sql, " returning ", pTAGDataBlock->keyColumnName, NULL);
			break;
		}
	}
	*/
    DBInt_Statement *stm = DBInt_CreateStatement(tagConnection->mkDBConnection);	//OCI_Statement   *stm = mkOCI_StatementCreate(tagConnection->ociConnection, __FILE__, __LINE__);
	
	DBInt_Prepare(tagConnection->mkDBConnection, stm, sql); 
	
	char* keyColumnValue = GetDatablockKeyColumnValue(session, page, pTAGDataBlock);

	BindSQLVariables(session, page, pTAGDataBlock, stm, SQL_COMMAND_TYPE_INSERT);
	
	//DBInt_RegisterString(tagConnection->mkDBConnection, stm, ":new_row_id", 25);
	
	char * rowCountAffected = DBInt_ExecuteInsertStatement(tagConnection->mkDBConnection, stm, sql);
	if (rowCountAffected) {
		mkFree(session->heapHandle, rowCountAffected);
	}
	
	if (tagConnection->mkDBConnection->errText) { 
		CLOG(ERR_DB_UNKNOWN_ERROR, tagConnection->mkDBConnection->errText);
		insertRejectionOccured(session, page, pTAGDataBlock);
	}
	else {		
		/** dirty items are updated to database, so we are setting items current value state to INPUT_TAG_VALUE_CLEAR
		With this modification, server do update once even if you move from one row to another  */
		retVal = mkStrdup(session->heapHandle, keyColumnValue, __FILE__, __LINE__);
		setBlockItemsValueStateToClear(session, pTAGDataBlock);
		runPostInsertHTSQLTriggerFunction(session, page, pTAGDataBlock);
	}    

    mkFree(session->heapHandle, sql);
	DBInt_FreeStatement(tagConnection->mkDBConnection, stm);
    return retVal;
}



char* updateOrInsertDataBlockCurrentRow(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGDataBlock) {
    bool isModified = false;

    TAGInput *input = pTAGDataBlock->rootTAGInput;

    while(input) {
        if (input->columnName && input->valueState == INPUT_TAG_VALUE_DIRTY) {
			if (strcmp(input->columnName, "rowid") != 0) {
				isModified = true;
			}
            break;
        }
        input = input->next;
    }

    if (isModified) {
        // some item(s) modified
        if (strncmp(pTAGDataBlock->keyColumnValue, "NEWROW", 6) != 0) {
            updateDataBlockCurrentRow(session, page, pTAGDataBlock);
            return NULL;
        } else {
            return insertDataBlockCurrentRow(session, page, pTAGDataBlock);
        }
    }
    return NULL;
}

void evaluateTAGDataBlockJoinCondition(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGDataBlock) {
    char *joinStament = mkStrcat(session->heapHandle, __FILE__, __LINE__, "#--", ":", pTAGDataBlock->dataBlockName, ">>", pTAGDataBlock->joinCondition, "--#", NULL);
    parseAndExecutePostString(session, page, joinStament);
    mkFree(session->heapHandle, joinStament);
}

void evaluateTAGDataBlockWhereClause(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGDataBlock) {
    char *whereClause = mkStrcat(session->heapHandle, __FILE__, __LINE__, "$--", ":", pTAGDataBlock->dataBlockName, ">>", pTAGDataBlock->whereClause, "--$", NULL);
    parseAndExecutePostString(session, page, whereClause);
    mkFree(session->heapHandle, whereClause);
}



