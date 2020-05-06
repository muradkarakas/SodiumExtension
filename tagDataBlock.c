#include "stdafx.h"

#include "tagDataBlock.h"

#include "tagDataBlockCurrentRow.h"
#include "tagDataBlockDatabaseOperations.h"
#include "tagGeneric.h"
#include "lemon.html.h"
#include "tagInput.h"
#include "tagConnection.h"
#include "rowSelection.h"
#include "html.parser.imp.h"

TAGDataBlock  *createTAGDataBlock(SodiumSession *session, HTSQLPage *page) {

    TAGDataBlock *tagDataBlock = mkMalloc(session->heapHandle, sizeof(TAGDataBlock), __FILE__, __LINE__ );
    if (!tagDataBlock)
        return NULL;

    // Setting default values
    tagDataBlock->next           = NULL;

    tagDataBlock->dataBlockName  = NULL;
    tagDataBlock->masterBlockName= NULL;
	tagDataBlock->connectionName = mkStrdup(session->heapHandle, "default", __FILE__, __LINE__); 
	tagDataBlock->autoGeneratedColumns = NULL;
	tagDataBlock->tagConnection = NULL;
    tagDataBlock->orderByClause  = NULL;
    tagDataBlock->keyColumnName	 = NULL;
    tagDataBlock->keyColumnValue	= NULL;
    tagDataBlock->whereClause    = NULL;
    tagDataBlock->whereClauseEvaluated = NULL;
    tagDataBlock->executeQueryModeWhereClause = NULL;
    tagDataBlock->dataSourceName = NULL;
	tagDataBlock->dataSchemaName = NULL;
	tagDataBlock->isSchemaObjectWritable = FALSE;	
    tagDataBlock->insertAllowed  = YES;
    tagDataBlock->updateAllowed  = YES;
    tagDataBlock->deleteAllowed  = YES;
    tagDataBlock->joinCondition          =  NULL;
    tagDataBlock->joinConditionEvaluated =  NULL;

    tagDataBlock->recordCount       = 1;
    tagDataBlock->currentPageNo     = 1;

    tagDataBlock->visibility        = true;

    tagDataBlock->rootTAGGeneric    = NULL;
    tagDataBlock->tailTAGGeneric    = NULL;
    tagDataBlock->startTAG          = NULL;
    tagDataBlock->endTAG            = NULL;

    tagDataBlock->rootTAGInput      = NULL;
    tagDataBlock->currentTAGInput   = NULL;

    tagDataBlock->blockStatus       = BLOCK_STATUS_CLEAR_MODE;

    pushTAGDataBlock(session, page, tagDataBlock); 

    return tagDataBlock;
}

char*
GetDatablockKeyColumnValue(
	SodiumSession* session,
	HTSQLPage* page,
	TAGDataBlock* pTAGDataBlock
	)
{
	char* keyColumnValue = NULL;

	if (pTAGDataBlock->keyColumnName != NULL) {
		TAGInput* input = pTAGDataBlock->rootTAGInput;
		for (; input; input = input->next) {

			if (input->columnName == NULL ||		// if column name is empty, we are not adding the column into insert statement since it is a formula column. 
				(input->inputType != INPUT_TAG_TYPE_IMAGE && input->value == NULL) // All input items except images must have a  value 
				) {
				continue;
			}

			if (strcmp(input->columnName, pTAGDataBlock->keyColumnName) == 0) {
				keyColumnValue = input->value;
				break;
			}
		}
	}
	return keyColumnValue;
}

char *getTAGDataBlockIsRowIdExistsSQLCommandForOracle(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGDataBlock) {
	char *sql = NULL; 
	char *dbObjectPath = NULL;
	char *dictionaryTableName = NULL;
	if (pTAGDataBlock->dataSchemaName) {
		dictionaryTableName = " all_updatable_columns ";
		dbObjectPath = mkStrcat(session->heapHandle, __FILE__, __LINE__,
			" owner = upper('", pTAGDataBlock->dataSchemaName, "') and table_name = upper('", pTAGDataBlock->dataSourceName, "') ", NULL);
	}
	else {
		dictionaryTableName = " user_updatable_columns ";
		dbObjectPath = mkStrcat(session->heapHandle, __FILE__, __LINE__,
			" table_name = upper('", pTAGDataBlock->dataSourceName, "') ", NULL);
	}
	sql = mkStrcat(session->heapHandle, __FILE__, __LINE__,
		"select owner "
		"from ",
		dictionaryTableName,
		"where ",
		dbObjectPath, " and ",
		"	(updatable = 'YES' or deletable = 'YES' or insertable = 'YES') ",
		NULL);
	mkFree(session->heapHandle, dbObjectPath);
	return sql;
}

char *getTAGDataBlockIsRowIdExistsSQLCommandForPostgresql(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGDataBlock) {
	char *sql = NULL;
	char *schemaSql = NULL;

	if (pTAGDataBlock->dataSchemaName) {
		schemaSql = mkStrcat(session->heapHandle, __FILE__, __LINE__,
							"	relnamespace = (	select oid	"
							"						from	"
							"							pg_namespace a	"
							"						where	"
							"							nspname = '", pTAGDataBlock->dataSchemaName, "'	"
							"					) and ", 
							NULL);
	}

	sql = mkStrcat(session->heapHandle, __FILE__, __LINE__,
					"select pc.relhasoids "
					"from "
					"	pg_class pc	"
					"where	"
					"	relname = '", pTAGDataBlock->dataSourceName, "' and	",
					(schemaSql) ? schemaSql:"",
					"	pc.relhasoids = true ",
					NULL);

	mkFree(session->heapHandle, schemaSql);
	return sql;
}

void 
SetTAGDataBlockWritable(
	SodiumSession *session, 
	HTSQLPage *page, 
	TAGDataBlock *pTAGDataBlock
)
{
	pTAGDataBlock->isSchemaObjectWritable = FALSE;
	
	if (pTAGDataBlock->dataSourceName != NULL) {

		// Do not allow to edit sys and system schema table/views for oracle
		if (pTAGDataBlock->dataSchemaName) {
			if (strcmp(pTAGDataBlock->dataSchemaName, "SYS") == 0 || strcmp(pTAGDataBlock->dataSchemaName, "sys") == 0 ||
				strcmp(pTAGDataBlock->dataSchemaName, "SYSTEM") == 0 || strcmp(pTAGDataBlock->dataSchemaName, "system") == 0) {
				return;
			}
		}
		
		// If keyColumnName is not defined, datablock is marked as READ ONLY
		if (pTAGDataBlock->keyColumnName == NULL) {
			return;
		}

		pTAGDataBlock->isSchemaObjectWritable = TRUE;
		/*char* sql = NULL;
		switch (pTAGDataBlock->tagConnection->mkDBConnection->dbType) {
			case ORACLE: {
				sql = getTAGDataBlockIsRowIdExistsSQLCommandForOracle(session, page, pTAGDataBlock);
				break;
			}
			case POSTGRESQL: {
				sql = getTAGDataBlockIsRowIdExistsSQLCommandForPostgresql(session, page, pTAGDataBlock);
				break;
			}
		}
		DBInt_Statement* stm = DBInt_CreateStatement(pTAGDataBlock->tagConnection->mkDBConnection);
		DBInt_Prepare(pTAGDataBlock->tagConnection->mkDBConnection, stm, sql);
		const char* err = DBInt_GetLastErrorText(pTAGDataBlock->tagConnection->mkDBConnection);
		if (err) {
			CLOG(ERR_DB_UNKNOWN_ERROR, err);
		}
		else {
			DBInt_ExecuteSelectStatement(pTAGDataBlock->tagConnection->mkDBConnection, stm, sql);
			err = DBInt_GetLastErrorText(pTAGDataBlock->tagConnection->mkDBConnection);
			bool isEof = DBInt_IsEof(pTAGDataBlock->tagConnection->mkDBConnection, stm);
			if (err == NULL && isEof == false) {
				pTAGDataBlock->isSchemaObjectWritable = true;
			}
		}
		DBInt_FreeStatement(pTAGDataBlock->tagConnection->mkDBConnection, stm);
		mkFree(session->heapHandle, sql);*/
	}

	setTAGDataBlockDatabasePrivileges(session, page, pTAGDataBlock);
}

void setTAGDataBlockDatabasePrivileges(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGDataBlock) {
	if (pTAGDataBlock->dataSourceName == NULL) {
		return;
	}
	if (pTAGDataBlock->isSchemaObjectWritable == FALSE) {
		pTAGDataBlock->insertAllowed = NO;
		pTAGDataBlock->updateAllowed = NO;
		pTAGDataBlock->deleteAllowed = NO;
	}
	else {
		pTAGDataBlock->insertAllowed = YES;
		pTAGDataBlock->updateAllowed = YES;
		pTAGDataBlock->deleteAllowed = YES;
	}
}

void printTAGDataBlockDynamicContent(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGDataBlock) {

	if (pTAGDataBlock->tagConnection == NULL) {
		setDatablockConnection(session, page, pTAGDataBlock);
	}

	if (pTAGDataBlock->tagConnection) {
		//SetTAGDataBlockWritable(session, page, pTAGDataBlock);
		//setTAGDataBlockDatabasePrivileges(session, page, pTAGDataBlock);

		if (pTAGDataBlock->autoGeneratedColumns && pTAGDataBlock->rootTAGGeneric == NULL) {
			printTAGDataBlockDynamicContent_Auto(session, page, pTAGDataBlock);
		}
		printTAGDataBlockDynamicContent_NoAuto(session, page, pTAGDataBlock);
	}
	else {
		CLOG(ERR_DB_INVALID_CONNECTION, pTAGDataBlock->dataBlockName);
	}
}



void printTAGDataBlockDynamicContent_Auto(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGDataBlock) {
	
	DBInt_Connection *mkConnection = pTAGDataBlock->tagConnection->mkDBConnection;
	DBInt_Statement *stm = DBInt_CreateStatement(mkConnection);
	
	char *sql = getTAGDataBlockSelectSQL(session, page, pTAGDataBlock);
	
	DBInt_ExecuteDescribe(mkConnection, stm, sql);

	if (mkConnection->errText) {
		CLOG(ERR_DB_UNKNOWN_ERROR, mkConnection->errText);
	}
	else {
		DBInt_ExecuteSelectStatement(mkConnection, stm, sql);
		
		if (mkConnection->errText) {
			CLOG(ERR_DB_UNKNOWN_ERROR, mkConnection->errText);
		} else {

			int colCount = DBInt_GetColumnCount(mkConnection, stm);
			if (colCount > 0) {

				// Creation of HEAD linked list
				TAGGeneric *tagGeneric = NULL; 
				tagGeneric = createTAGGeneric(session, page, pTAGDataBlock, TAG_TABLE_OPEN, "<table>");
				tagGeneric->isDynamicContent = false;
				tagGeneric = createTAGGeneric(session, page, pTAGDataBlock, TAG_HEAD_OPEN, "<thead>");
				tagGeneric->isDynamicContent = false;
				tagGeneric = createTAGGeneric(session, page, pTAGDataBlock, TAG_TR_OPEN, "<tr>");
				tagGeneric->isDynamicContent = false;

				for (int colIndex = 0; colIndex < colCount; colIndex++) {
					const char *colName = DBInt_GetColumnNameByIndex(mkConnection, stm, colIndex);
					HTSQL_COLUMN_TYPE colType = DBInt_GetColumnType(mkConnection, stm, colName);

					tagGeneric = createTAGGeneric(session, page, pTAGDataBlock, TAG_TD_OPEN, "<td>");
					tagGeneric->isDynamicContent = false;
					char *colTypeStr = NULL;
					switch (colType) {
						case HTSQL_COLUMN_TYPE_OBJECT: {
							colTypeStr = mkStrdup(session->heapHandle, "object", __FILE__, __LINE__);
							break;
						}
						case HTSQL_COLUMN_TYPE_DATE: {
							colTypeStr = mkStrdup(session->heapHandle, "date", __FILE__, __LINE__);
							break;
						}
						case HTSQL_COLUMN_TYPE_TEXT: {
							int colSize = DBInt_GetColumnSize(mkConnection, stm, colName);
							char int2Str[10];
							mkItoa(colSize, int2Str);
							colTypeStr = mkStrcat(session->heapHandle, __FILE__, __LINE__, "text (", int2Str, ")", NULL);
							break;
						}
						case HTSQL_COLUMN_TYPE_NUMBER: {
							colTypeStr = mkStrdup(session->heapHandle, "number", __FILE__, __LINE__);
							break;
						}
					}
					char *columnHeader = mkStrcat(session->heapHandle, __FILE__, __LINE__, colName, "<br/>", colTypeStr, NULL);
					tagGeneric = createTAGGeneric(session, page, pTAGDataBlock, HTMLTEXT, columnHeader);
					tagGeneric->isDynamicContent = false;
					mkFree(session->heapHandle, columnHeader);
					if (colTypeStr) {
						mkFree(session->heapHandle, colTypeStr);
					}
					tagGeneric = createTAGGeneric(session, page, pTAGDataBlock, TAG_TD_BLOCK_CLOSE, "</td>");
					tagGeneric->isDynamicContent = false;
				}
				tagGeneric = createTAGGeneric(session, page, pTAGDataBlock, TAG_TR_BLOCK_CLOSE, "</tr>");
				tagGeneric->isDynamicContent = false;
				tagGeneric = createTAGGeneric(session, page, pTAGDataBlock, TAG_THEAD_BLOCK_CLOSE, "</thead>");
				tagGeneric->isDynamicContent = false;

				// Creation of BODY linked list
				tagGeneric = createTAGGeneric(session, page, pTAGDataBlock, TAG_TBODY_OPEN, "<tbody>");
				tagGeneric->isDynamicContent = false;
				tagGeneric = createTAGGeneric(session, page, pTAGDataBlock, TAG_TR_OPEN, "<tr>");
				tagGeneric->isDynamicContent = true;

				for (int colIndex = 0; colIndex < colCount; colIndex++) {

					const char *colName = DBInt_GetColumnNameByIndex(mkConnection, stm, colIndex);
					tagGeneric = createTAGGeneric(session, page, pTAGDataBlock, TAG_TD_OPEN, "<td>");
					tagGeneric->isDynamicContent = true;

					HTSQL_COLUMN_TYPE colType = DBInt_GetColumnType(mkConnection, stm, colName);
					switch (colType) {
					case HTSQL_COLUMN_TYPE_DATE: {
						tagGeneric = createTAGGeneric(session, page, pTAGDataBlock, TAG_INPUT_OPEN, "<input");
						tagGeneric->tagInput = createTAGInput(session, page, pTAGDataBlock, true);
						tagGeneric->isDynamicContent = true;
						setCurrentTAGInputPropertyColumnName(session, page, tagGeneric->tagInput, colName);
						setCurrentTAGInputPropertyName(session, page, tagGeneric->tagInput, colName);
						setCurrentTAGInputPropertyDataType(session, page, tagGeneric->tagInput, "date");
						setCurrentTAGInputPropertyType(session, page, tagGeneric->tagInput, "\"text\"");
						tagGeneric = createTAGGeneric(session, page, pTAGDataBlock, TAG_INPUT_BLOCK_CLOSE, "</input>");
						tagGeneric->isDynamicContent = true;
						break;
					}
					case HTSQL_COLUMN_TYPE_TEXT:
					case HTSQL_COLUMN_TYPE_NUMBER: {
						tagGeneric = createTAGGeneric(session, page, pTAGDataBlock, TAG_INPUT_OPEN, "<input");
						tagGeneric->tagInput = createTAGInput(session, page, pTAGDataBlock, true);
						tagGeneric->isDynamicContent = true;
						setCurrentTAGInputPropertyColumnName(session, page, tagGeneric->tagInput, colName);
						setCurrentTAGInputPropertyName(session, page, tagGeneric->tagInput, colName);
						if (colType == HTSQL_COLUMN_TYPE_TEXT) {
							setCurrentTAGInputPropertyDataType(session, page, tagGeneric->tagInput, "char");
							int colSize = DBInt_GetColumnSize(mkConnection, stm, colName);
							char int2Str[10];
							mkItoa(colSize, int2Str);
							setCurrentTAGInputPropertyMaxLength(session, page, tagGeneric->tagInput, int2Str);
						}
						else if (colType == HTSQL_COLUMN_TYPE_NUMBER) {
							setCurrentTAGInputPropertyDataType(session, page, tagGeneric->tagInput, "number");
						}
						setCurrentTAGInputPropertyType(session, page, tagGeneric->tagInput, "\"text\"");
						tagGeneric = createTAGGeneric(session, page, pTAGDataBlock, TAG_INPUT_BLOCK_CLOSE, "</input>");
						tagGeneric->isDynamicContent = true;
						break;
					}
					case HTSQL_COLUMN_TYPE_OBJECT: {
						tagGeneric = createTAGGeneric(session, page, pTAGDataBlock, HTMLTEXT, "Object");
						tagGeneric->isDynamicContent = true;
						break;
					}
					case HTSQL_COLUMN_TYPE_LOB: {
						tagGeneric = createTAGGeneric(session, page, pTAGDataBlock, HTMLTEXT, "Large Object");
						tagGeneric->isDynamicContent = true;
						break;
					}
					}
					tagGeneric = createTAGGeneric(session, page, pTAGDataBlock, TAG_TD_BLOCK_CLOSE, "</td>");
					tagGeneric->isDynamicContent = true;
				}
				tagGeneric = createTAGGeneric(session, page, pTAGDataBlock, TAG_TR_BLOCK_CLOSE, "</tr>");
				tagGeneric->isDynamicContent = true;
				tagGeneric = createTAGGeneric(session, page, pTAGDataBlock, TAG_TBODY_BLOCK_CLOSE, "</tbody>");
				tagGeneric->isDynamicContent = false;
				tagGeneric = createTAGGeneric(session, page, pTAGDataBlock, TAG_TABLE_BLOCK_CLOSE, "</table>");
				tagGeneric->isDynamicContent = false;
			}
		} 
	}
	if (sql) {
		mkFree(session->heapHandle, sql);
	}
	DBInt_FreeStatement(pTAGDataBlock->tagConnection->mkDBConnection, stm);
}

void printTAGDataBlockStandartResponseScriptHeader(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGDataBlock) {
	/** HEADER */
	char *lastResponse1 = mkStrcat(session->heapHandle, __FILE__, __LINE__,
		"action = { ",
		"\"type\"            : \"refreshBlockNative\", ",
		"\"dataBlockName\"   : \"", pTAGDataBlock->dataBlockName, "\", ",
		"\"visibility\"      : \"", (pTAGDataBlock->visibility) ? "true" : "false", "\", ",
		"\"tbodyContent\"    : \"",
		NULL);
	if (page->currentRequestMethodType == REQUEST_TYPE_GET) {
		mkPrint(session, "\n<script type=\"text/javascript\">", NULL);
	}
	mkPrint(session, lastResponse1, NULL);
	page->isJavaScriptEscapeEnabled = true;
	mkFree(session->heapHandle, lastResponse1);
}

void printTAGDataBlockStandartResponseScriptFooter(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGDataBlock) {
	page->isJavaScriptEscapeEnabled = false;
	char *lastResponse2 = mkStrcat(session->heapHandle, __FILE__, __LINE__,
		"\" };",
		// standart part
		"addNewActionToTheResponseActionList(action);",
		NULL);
	mkPrint(session, lastResponse2, NULL);

	if (page->currentRequestMethodType == REQUEST_TYPE_GET) {
		mkPrint(session, "\n</script>\n", NULL);
	}

	mkFree(session->heapHandle, lastResponse2);
}

void printTAGDataBlockDynamicContent_NoAuto(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGDataBlock) {

	if (pTAGDataBlock->rootTAGGeneric == NULL) {

		mkDebug(session, SESSION_LOG_DETAIL_ASSERT, ETEXT(ERR_CORE_DATABLOCK_CONTENT_CANNOT_BE_SHOWN), 
			"Possible reasons: 1. XMLType columns cannot be shown 2. Insufficient privileges", NULL);
		/* It is possible to be NULL in some cases. For example; tables in XDB schema in Oracle cannot be shown since they have XMLType. */
		/** HEADER */
		printTAGDataBlockStandartResponseScriptHeader(session, page, pTAGDataBlock); 
		
		mkPrint(session, 
				"<tr><td>", 
				ETEXT(ERR_CORE_DATABLOCK_CONTENT_CANNOT_BE_SHOWN), 
				" Possible reasons: 1. XMLType columns cannot be shown",
				"</td></tr>", 
				NULL);
		
		/** FOOTER */
		printTAGDataBlockStandartResponseScriptFooter(session, page, pTAGDataBlock);
		return;
	}

	DBInt_Statement *stm = getTAGDataBlockRecordSet(session, page, pTAGDataBlock);

	if (pTAGDataBlock->tagConnection->mkDBConnection->errText) {
		CLOG(ERR_DB_UNKNOWN_ERROR, pTAGDataBlock->tagConnection->mkDBConnection->errText);
	}
	else {
		/** HEADER */
		printTAGDataBlockStandartResponseScriptHeader(session, page, pTAGDataBlock);

		/** BODY */
		TAGGeneric *lCurrentTAGGeneric = pTAGDataBlock->rootTAGGeneric;
		do {
			/*
			* we are stepping forward until first dynamic TAG (<tr>). Until that point, text will be send to the client just one time.
			*     <table>
			*         <thead>
			*             <tr>
			*                 <td></td>
			*                 <td></td>
			*             </tr>
			*         </thead>
			*         <tfoot>
			*             <tr>
			*                 <td></td>
			*                 <td></td>
			*             </tr>
			*         </tfoot>
			*         <body>
			*              ...printTAGDataBlockDynamicContentRows(lCurrentTAGGeneric);...'s result gets here getCurrentTAGDataBlock()->recordCount times
			*         </tbody>
			*     </table>
			*/
			if (lCurrentTAGGeneric->isDynamicContent) {
				/*
				*  <tr> tag and all contents will be repeated getCurrentTAGDataBlock()->recordCount times inside printTAGDataBlockDynamicContentRows function
				*/
				lCurrentTAGGeneric = printTAGDataBlockDynamicContentRows(session, page, pTAGDataBlock, stm, lCurrentTAGGeneric);
			}

			/*  rest of the dynamic content
			*    </tbody>
			*    <tfoot>
			*       <tr>
			*          <td></td>
			*          <td></td>
			*       </tr>
			*    </tfoot>
			*  </table>
			*/
			if (lCurrentTAGGeneric) {
				mkPrint(session, lCurrentTAGGeneric->tagSemanticValue, NULL);
				lCurrentTAGGeneric = lCurrentTAGGeneric->next;
			}
			
		} while (lCurrentTAGGeneric != NULL);

		/** FOOTER */
		printTAGDataBlockStandartResponseScriptFooter(session, page, pTAGDataBlock);
	}

	DBInt_FreeStatement(pTAGDataBlock->tagConnection->mkDBConnection, stm);
}

void setDatablockConnection(SodiumSession *session, HTSQLPage *page, TAGDataBlock * pTAGDataBlock) {
	TAGConnection *tagConnection = findTAGConnectionByName(session, page, pTAGDataBlock->connectionName, true);
	pTAGDataBlock->tagConnection = (tagConnection) ? tagConnection : NULL;
}

void setCurrentTAGDataBlockPropertyConnectionName(SodiumSession *session, HTSQLPage *page, char *pPropValue) {
	/*TAGConnection *tagConnection = findTAGConnectionByName(session, page, pPropValue, true);
	if (!tagConnection) {
		// invalid connection-name property
		CLOG(ERR_PROPERTY_INVALID, "connection-name");
	}*/
	TAGDataBlock *tagDataBlock = getCurrentTAGDataBlock(session, page);
	size_t len = strlen(pPropValue);
	if (tagDataBlock->connectionName == NULL) {
		tagDataBlock->connectionName = mkMalloc(session->heapHandle, len+1, __FILE__, __LINE__);
	}
	else {
		tagDataBlock->connectionName = mkReAlloc(session->heapHandle, tagDataBlock->connectionName, len + 1);
	}
	strcpy_s(tagDataBlock->connectionName, len + 1, pPropValue);
}

const char *getBlockItemColumnName(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGDataBlock, const char *itemName) {
    if (pTAGDataBlock) {
        TAGInput *tagInput = findTAGDataBlockCurrentRowTAGInputItem(session, pTAGDataBlock, itemName);
        if (tagInput) {
            return tagInput->columnName;
        }
    }
    return NULL;
}

void updateOrInsertAllDataBlocksCurrentRow(SodiumSession *session, HTSQLPage *page) {
    char *newRowId = NULL;
    TAGDataBlock *tagDatablock =  page->rootTAGDataBlock;
    while(tagDatablock) {
        newRowId = updateOrInsertDataBlockCurrentRow(session, page, tagDatablock);
        if (newRowId != NULL) {
            __sendRowIdReplaceResponse(session, tagDatablock->dataBlockName, tagDatablock->keyColumnValue, newRowId);
			__sendInsertSuccessfulResponse(session, tagDatablock, newRowId);
			mkFree(session->heapHandle, newRowId);
        }
        tagDatablock = tagDatablock->next;
    }

}

void destroyTAGDataBlock(SodiumSession *session, HTSQLPage *page, TAGDataBlock *tagDataBlock) {

    mkDebug(session, SESSION_LOG_DETAIL_HTSQL_BUILTIN_CALL, "\n", "Datablock destroyed: '", tagDataBlock->dataBlockName, "'", NULL);
    tagDataBlock->next = NULL;

    /*if (tagDataBlock->tagConnection) {
        tagDataBlock->tagConnection = NULL;
    }*/
    if (tagDataBlock->connectionName) {
        mkFree(session->heapHandle, tagDataBlock->connectionName);
        tagDataBlock->connectionName = NULL;
    }
    if (tagDataBlock->orderByClause) {
        mkFree(session->heapHandle, tagDataBlock->orderByClause);
        tagDataBlock->orderByClause = NULL;
    }
    if (tagDataBlock->whereClause) {
        mkFree(session->heapHandle, tagDataBlock->whereClause);
        tagDataBlock->whereClause = NULL;
    }
    if (tagDataBlock->dataSourceName) {
        mkFree(session->heapHandle, tagDataBlock->dataSourceName);
        tagDataBlock->dataSourceName = NULL;
    }
    if (tagDataBlock->dataBlockName) {
        mkFree(session->heapHandle, tagDataBlock->dataBlockName);
        tagDataBlock->dataBlockName = NULL;
    }
    if (tagDataBlock->masterBlockName) {
        mkFree(session->heapHandle, tagDataBlock->masterBlockName);
        tagDataBlock->masterBlockName = NULL;
    }
    if (tagDataBlock->executeQueryModeWhereClause) {
        mkFree(session->heapHandle, tagDataBlock->executeQueryModeWhereClause);
        tagDataBlock->executeQueryModeWhereClause = NULL;
    }
    if (tagDataBlock->joinCondition) {
        mkFree(session->heapHandle, tagDataBlock->joinCondition);
        tagDataBlock->joinCondition = NULL;
    }
    if (tagDataBlock->joinConditionEvaluated) {
        mkFree(session->heapHandle, tagDataBlock->joinConditionEvaluated);
        tagDataBlock->joinConditionEvaluated = NULL;
    }
	if (tagDataBlock->whereClauseEvaluated) {
		mkFree(session->heapHandle, tagDataBlock->whereClauseEvaluated);
		tagDataBlock->whereClauseEvaluated = NULL;
	}
    // destroy TAGGeneric linked list
	destroyTAGDatablockTAGGenericObjectInstances(session, page, tagDataBlock);


	if (tagDataBlock->keyColumnName != NULL) {
        mkFree(session->heapHandle, tagDataBlock->keyColumnName);
		tagDataBlock->keyColumnName = NULL;
	}
	if (tagDataBlock->keyColumnValue != NULL) {
        mkFree(session->heapHandle, tagDataBlock->keyColumnValue);
		tagDataBlock->keyColumnValue = NULL;
	}
}

void destroyTAGDatablockTAGGenericObjectInstances(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGDataBlock) {
	TAGGeneric *cur = pTAGDataBlock->rootTAGGeneric;
	while (cur) {
		TAGGeneric *tmp = cur->next;
		destroyTAGGeneric(session, page, cur);
		mkFree(session->heapHandle, cur);
		cur = tmp;
	}
	pTAGDataBlock->rootTAGGeneric = NULL;
	pTAGDataBlock->tailTAGGeneric = NULL;
	pTAGDataBlock->startTAG = NULL;
	pTAGDataBlock->endTAG = NULL;

	// do not free TAGInput list. They will be destroyed by TAGGeneric
	pTAGDataBlock->rootTAGInput = NULL;
	pTAGDataBlock->currentTAGInput = NULL;
}


TAGInput* findTAGDataBlockCurrentRowTAGInputItem(SodiumSession *session, TAGDataBlock *pTAGDataBlock, const char *tagInputItemName) {
    if (pTAGDataBlock == NULL || pTAGDataBlock->rootTAGInput == NULL || tagInputItemName == NULL) {
        return NULL;
    }
    TAGInput *tagInput = pTAGDataBlock->rootTAGInput;
    while(tagInput) {
        if (strcmp(tagInput->name, tagInputItemName) == 0) {
            return tagInput;
        }
        tagInput = tagInput->next;
    }
    return NULL;
}

/*
 *  Put the object in linked list and set current TAG object to NULL.
 */
void pushTAGDataBlock(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGObject) {
    if ( page->rootTAGDataBlock == NULL) {
        page->rootTAGDataBlock     = pTAGObject;
        page->currentTAGDataBlock  = pTAGObject;
    } else {
        page->currentTAGDataBlock->next = pTAGObject;
        page->currentTAGDataBlock       = pTAGObject;
    }
}




TAGDataBlock *getCurrentTAGDataBlock(SodiumSession *session, HTSQLPage *page) {
    return page->currentTAGDataBlock;
}



void setCurrentTAGDataBlock(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGObject) {
    page->currentTAGDataBlock = pTAGObject;
}



TAGDataBlock *findTAGDataBlockByName(SodiumSession *session, HTSQLPage *page, const char *pDataBlockName) {
    if (pDataBlockName == NULL) {
        return NULL;
    }
    TAGDataBlock *tagDataBlock = page->rootTAGDataBlock;
    while (tagDataBlock != NULL) {
        if (tagDataBlock->dataBlockName != NULL) {
            if ( strcmp(tagDataBlock->dataBlockName, pDataBlockName) == 0 ) {
                return tagDataBlock;
            }
        }
        tagDataBlock = tagDataBlock->next;
    }
    return NULL;
}

void setCurrentTAGDataBlockMasterPropertyBlockName(SodiumSession *session, HTSQLPage *page, const char *pPropValue) {
    TAGDataBlock *current = getCurrentTAGDataBlock(session, page);
    char *pPropValueStriped = mkStrdup(session->heapHandle, pPropValue, __FILE__, __LINE__);
    pPropValueStriped = stripTAGPropertyValue(pPropValueStriped);

    size_t len = strlen(pPropValueStriped);
    if (len == 0) {
        if (current->masterBlockName != NULL) {
            mkFree(session->heapHandle, current->masterBlockName);
            current->masterBlockName = NULL;
        }
        return;
    }
    if (current->masterBlockName != NULL) {
        current->masterBlockName = mkReAlloc(session->heapHandle, current->masterBlockName, len + 1);
    } else {
        current->masterBlockName = mkMalloc(session->heapHandle, len + 1, __FILE__, __LINE__);
    }
    strcpy_s(current->masterBlockName, len+1, pPropValueStriped);
    current->masterBlockName = stripTAGPropertyValue(current->masterBlockName);
    mkFree(session->heapHandle, pPropValueStriped);
}

void setCurrentTAGDataBlockPropertyBlockName(SodiumSession *session, HTSQLPage *page, const char *pPropValue) {
    TAGDataBlock *tagDataBlock = NULL;
    char *pPropValueStriped = mkStrdup(session->heapHandle, pPropValue, __FILE__, __LINE__);
    pPropValueStriped = stripTAGPropertyValue(pPropValueStriped);
	size_t len = strlen(pPropValueStriped);
    // tagConnection name must be unique
    tagDataBlock = findTAGDataBlockByName(session, page, pPropValueStriped);
    if ( tagDataBlock != NULL) {
        // Name is already in use
        stopHTMLParserWithMessage(session, page, ETEXT(ERR_NAME_ALREADY_IN_USE), pPropValueStriped);
    } else {
        tagDataBlock = getCurrentTAGDataBlock(session, page);
        if (tagDataBlock->dataBlockName == NULL) {
            tagDataBlock->dataBlockName = mkMalloc(session->heapHandle, len+1, __FILE__, __LINE__);
        } else {
            tagDataBlock->dataBlockName = mkReAlloc(session->heapHandle, tagDataBlock->dataBlockName, len + 1);
        }
        strcpy_s( tagDataBlock->dataBlockName, len+1, pPropValueStriped);
    }
    mkFree(session->heapHandle, pPropValueStriped);
}


void setCurrentTAGDataBlockPropertyInsertAllowed(SodiumSession *session, HTSQLPage *page,  char *pPropValue) {
    if ( strcmp(pPropValue, "\"yes\"") == 0 ) {
         getCurrentTAGDataBlock(session, page)->insertAllowed = YES;
         return;
    }
    if ( strcmp(pPropValue, "\"no\"") == 0 ) {
        getCurrentTAGDataBlock(session, page)->insertAllowed = NO;
        return;
    }
    mkPrint(session, "HATA: Gecersiz property degeri. Mumkun olan degerler: yes / no", 0);
}


void setCurrentTAGDataBlockPropertyUpdateAllowed(SodiumSession *session, HTSQLPage *page,  char *pPropValue) {
    if ( strcmp(pPropValue, "\"yes\"") == 0 ) {
         getCurrentTAGDataBlock(session, page)->updateAllowed = YES;
         return;
    }
    if ( strcmp(pPropValue, "\"no\"") == 0 ) {
        getCurrentTAGDataBlock(session, page)->updateAllowed = NO;
        return;
    }
    mkPrint(session, "HATA: Gecersiz property degeri. Mumkun olan degerler: yes / no", 0);
}


void setCurrentTAGDataBlockPropertyDeleteAllowed(SodiumSession *session, HTSQLPage *page,  char *pPropValue) {
    if ( strcmp(pPropValue, "\"yes\"") == 0 ) {
         getCurrentTAGDataBlock(session, page)->deleteAllowed = YES;
         return;
    }
    if ( strcmp(pPropValue, "\"no\"") == 0 ) {
        getCurrentTAGDataBlock(session, page)->deleteAllowed = NO;
        return;
    }
    mkPrint(session, "HATA: Gecersiz property degeri. Mumkun olan degerler: yes / no", 0);
}


void setCurrentTAGDataBlockPropertyKeyColumnName(SodiumSession* session, HTSQLPage* page, const char* pPropValue) {
    TAGDataBlock * current = getCurrentTAGDataBlock(session, page);;
	size_t len = strlen(pPropValue);
    if (len == 0) {
        if (current->keyColumnName != NULL) {
            mkFree(session->heapHandle, current->keyColumnName);
            current->keyColumnName = NULL;
        }
        return;
    }
    if (current->keyColumnName != NULL) {
        current->keyColumnName = mkReAlloc(session->heapHandle, current->whereClause, len + 1);
    } else {
        current->keyColumnName = mkMalloc(session->heapHandle, len + 1, __FILE__, __LINE__);
    }
    strcpy_s(current->keyColumnName, len+1, pPropValue);
    current->keyColumnName = stripTAGPropertyValue(current->keyColumnName);

	SetTAGDataBlockWritable(session, page, current);
}

void setTAGDataBlockPropertyWhereClause(SodiumSession *session, HTSQLPage *page, TAGDataBlock *tagDatablock, char *pPropValue) {
    TAGDataBlock *current = tagDatablock;
	size_t len = strlen(pPropValue);
    if (len == 0) {
        if (current->whereClause != NULL) {
            mkFree(session->heapHandle, current->whereClause);
            current->whereClause = NULL;
        }
        return;
    }
    if (current->whereClause != NULL) {
        current->whereClause = mkReAlloc(session->heapHandle, current->whereClause, len + 1);
    } else {
        current->whereClause = mkMalloc(session->heapHandle, len + 1, __FILE__, __LINE__);
    }
    strcpy_s(current->whereClause, len+1, pPropValue);
    current->whereClause = stripTAGPropertyValue(current->whereClause);

    evaluateTAGDataBlockWhereClause(session, page, tagDatablock);
}

void setTAGDataBlockPropertyWhereClauseEvaluated(SodiumSession *session, HTSQLPage *page, TAGDataBlock *tagDatablock, char *pPropValue) {
    TAGDataBlock *current = tagDatablock;
	size_t len = strlen(pPropValue);
    if (len == 0) {
        if (current->whereClauseEvaluated) {
            mkFree(session->heapHandle, current->whereClauseEvaluated);
            current->whereClauseEvaluated = NULL;
        }
        return;
    }
    if (current->whereClauseEvaluated) {
        current->whereClauseEvaluated = mkReAlloc(session->heapHandle, current->whereClauseEvaluated, len + 1);
    } else {
        current->whereClauseEvaluated = mkMalloc(session->heapHandle, len + 1, __FILE__, __LINE__);
    }
    strcpy_s(current->whereClauseEvaluated, len+1, pPropValue);
    current->whereClauseEvaluated = stripTAGPropertyValue(current->whereClauseEvaluated);
}

void setTAGDataBlockPropertyVisibility(SodiumSession *session, HTSQLPage *page, TAGDataBlock *tagDatablock, bool pPropValue) {
    tagDatablock->visibility = pPropValue;
}

void setCurrentTAGDataBlockPropertyWhereClause(SodiumSession *session, HTSQLPage *page,  char *pPropValue) {
    TAGDataBlock *current = getCurrentTAGDataBlock(session, page);
	size_t len = strlen(pPropValue);
    if (len == 0) {
        if (current->whereClause != NULL) {
            mkFree(session->heapHandle, current->whereClause);
            current->whereClause = NULL;
        }
        return;
    }
    if (current->whereClause != NULL) {
        current->whereClause = mkReAlloc(session->heapHandle, current->whereClause, len + 1);
    } else {
        current->whereClause = mkMalloc(session->heapHandle, len + 1, __FILE__, __LINE__);
    }
    strcpy_s(current->whereClause, len+1, pPropValue);
    current->whereClause = stripTAGPropertyValue(current->whereClause);
}



void setTAGDataBlockPropertyJoinConditionEvaluated(SodiumSession *session, TAGDataBlock *pTAGDatablock, const char *pPropValue) {
	size_t len = strlen(pPropValue);

	if (pTAGDatablock->joinConditionEvaluated) {
		mkFree(session->heapHandle, pTAGDatablock->joinConditionEvaluated);
        pTAGDatablock->joinConditionEvaluated = NULL;
    }
    
	if (len == 0) {
		return;
	}

    if (pTAGDatablock->joinConditionEvaluated) {
        pTAGDatablock->joinConditionEvaluated = mkReAlloc(session->heapHandle, pTAGDatablock->joinConditionEvaluated, len + 1);
    } else {
        pTAGDatablock->joinConditionEvaluated = mkMalloc(session->heapHandle, len + 1, __FILE__, __LINE__);
    }
    strcpy_s(pTAGDatablock->joinConditionEvaluated, len+1, pPropValue);
}

void setTAGDataBlockPropertyJoinCondition(SodiumSession *session, TAGDataBlock *pTAGDatablock, char *pPropValue) {
	size_t len = strlen(pPropValue);
    if (len == 0) {
        if (pTAGDatablock->joinCondition != NULL) {
            mkFree(session->heapHandle, pTAGDatablock->joinCondition);
            pTAGDatablock->joinCondition = NULL;
        }
        return;
    }
    if (pTAGDatablock->joinCondition != NULL) {
        pTAGDatablock->joinCondition = mkReAlloc(session->heapHandle, pTAGDatablock->joinCondition, len + 1);
    } else {
        pTAGDatablock->joinCondition = mkMalloc(session->heapHandle, len + 1, __FILE__, __LINE__);
    }
    strcpy_s(pTAGDatablock->joinCondition, len+1, pPropValue);
}

void setCurrentTAGDataBlockPropertyJoinCondition(SodiumSession *session, HTSQLPage *page, char *pPropValue) {
    pPropValue = stripTAGPropertyValue(pPropValue);
    TAGDataBlock *current = getCurrentTAGDataBlock(session, page);
    if (current) {
        setTAGDataBlockPropertyJoinCondition(session, current, pPropValue);
    }
}

void setCurrentTAGDataBlockPropertyOrderByClause(SodiumSession *session, HTSQLPage *page,  char *pPropValue) {
    TAGDataBlock *current = getCurrentTAGDataBlock(session, page);
	size_t len = strlen(pPropValue);
    if (len == 0) {
        if (current->orderByClause != NULL) {
            mkFree(session->heapHandle, current->orderByClause);
            current->orderByClause = NULL;
        }
        return;
    }
    if (current->orderByClause != NULL) {
        current->orderByClause = mkReAlloc(session->heapHandle, current->orderByClause, len + 1);
    } else {
        current->orderByClause = mkMalloc(session->heapHandle, len + 1, __FILE__, __LINE__);
    }
    strcpy_s(current->orderByClause, len+1, pPropValue);
    current->orderByClause = stripTAGPropertyValue(current->orderByClause);
}


void setCurrentTAGDataBlockPropertyRecordCount(SodiumSession *session, HTSQLPage *page,   char *pPropValue) {
    char *pPropValueTemp = mkStrdup(session->heapHandle, pPropValue, __FILE__, __LINE__);
    pPropValueTemp = stripTAGPropertyValue(pPropValueTemp);
    int recordCount = atoi(pPropValueTemp);
    getCurrentTAGDataBlock(session, page)->recordCount = recordCount;
    mkFree(session->heapHandle, pPropValueTemp);
}

void setCurrentTAGDataBlockPropertyAutoGeneratedColumns(SodiumSession *session, HTSQLPage *page, char *pPropValue) {
	
	TAGDataBlock *current = getCurrentTAGDataBlock(session, page);
	size_t len = strlen(pPropValue);
	if (len == 0) {
		if (current->autoGeneratedColumns != NULL) {
			mkFree(session->heapHandle, current->autoGeneratedColumns);
			current->autoGeneratedColumns = NULL;
		}
		return;
	}
	if (current->autoGeneratedColumns != NULL) {
		current->autoGeneratedColumns = mkReAlloc(session->heapHandle, current->autoGeneratedColumns, len + 1);
	}
	else {
		current->autoGeneratedColumns = mkMalloc(session->heapHandle, len + 1, __FILE__, __LINE__);
	}
	strcpy_s(current->autoGeneratedColumns, len + 1, pPropValue);
	current->autoGeneratedColumns = stripTAGPropertyValue(current->autoGeneratedColumns);
}

void setCurrentTAGDataBlockPropertyDataSchemaSourceName(SodiumSession *session, HTSQLPage *page, TAGDataBlock *dataBlockInstance, const char *pSchemaName, const char *pSourceName) {
	// destroying dynamic/static contents of datablock
	destroyTAGDatablockTAGGenericObjectInstances(session, page, dataBlockInstance);
	
	// Setting rowid setting to initial state. 
	if (dataBlockInstance->keyColumnName) {
		mkFree(session->heapHandle, dataBlockInstance->keyColumnName);
		dataBlockInstance->keyColumnName = NULL;
	}

	_setTAGDatablockDataSourceName(session, page, dataBlockInstance, pSourceName);
	_setTAGDataBlockSchemaName(session, page, dataBlockInstance, pSchemaName);
	
	SetTAGDataBlockRowIdColumnName(session, page, dataBlockInstance);
}

void _setTAGDatablockDataSourceName(SodiumSession *session, HTSQLPage *page, TAGDataBlock *current, const char *pPropValue) {
	size_t len = strlen(pPropValue);
	if (len == 0) {
		if (current->dataSourceName != NULL) {
			mkFree(session->heapHandle, current->dataSourceName);
			current->dataSourceName = NULL;
		}
		return;
	}
	if (current->dataSourceName != NULL) {
		current->dataSourceName = mkReAlloc(session->heapHandle, current->dataSourceName, len + 1);
	}
	else {
		current->dataSourceName = mkMalloc(session->heapHandle, len + 1, __FILE__, __LINE__);
	}
	strcpy_s(current->dataSourceName, len + 1, pPropValue);
	current->dataSourceName = stripTAGPropertyValue(current->dataSourceName);

	SetTAGDataBlockWritable(session, page, current);
}

void setCurrentTAGDataBlockPropertyDataSourceName(SodiumSession *session, HTSQLPage *page,  char *pPropValue) {

    TAGDataBlock *current = getCurrentTAGDataBlock(session, page);
	
	// destroying dynamic/static contents of datablock
	destroyTAGDatablockTAGGenericObjectInstances(session, page, current);
	
	// Setting rowid setting to initial state. 
	//current->isRowIdExists = '\0';

	_setTAGDatablockDataSourceName(session, page, current, pPropValue);
}

void _setTAGDataBlockSchemaName(SodiumSession *session, HTSQLPage *page, TAGDataBlock *current, const char *pPropValue) {
	size_t len = strlen(pPropValue);
	if (len == 0) {
		if (current->dataSchemaName != NULL) {
			mkFree(session->heapHandle, current->dataSchemaName);
			current->dataSchemaName = NULL;
		}
		return;
	}
	if (current->dataSchemaName != NULL) {
		current->dataSchemaName = mkReAlloc(session->heapHandle, current->dataSchemaName, len + 1);
	}
	else {
		current->dataSchemaName = mkMalloc(session->heapHandle, len + 1, __FILE__, __LINE__);
	}
	strcpy_s(current->dataSchemaName, len + 1, pPropValue);
	current->dataSchemaName = stripTAGPropertyValue(current->dataSchemaName);
}

void setCurrentTAGDataBlockPropertyDataSchemaName(SodiumSession *session, HTSQLPage *page, char *pPropValue) {

	TAGDataBlock *current = getCurrentTAGDataBlock(session, page);

	// destroying dynamic/static contents of datablock
	destroyTAGDatablockTAGGenericObjectInstances(session, page, current);
	
	// Setting rowid setting to initial state. 
	//current->isRowIdExists = '\0';

	_setTAGDataBlockSchemaName(session, page, current, pPropValue);
}





