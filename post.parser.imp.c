/**
 * This file is part of Sodium Language project
 *
 * Copyright © 2020 Murad Karakaş <muradkarakas@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License v3.0
 * as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 *	https://choosealicense.com/licenses/gpl-3.0/
 */

#include "pch.h"

#include "post.parser.imp.h"
 
#include "postObjectGeneric.h"   
#include "tagDataBlock.h"
#include "lemon.post.h"
#include "message.h"
#include "postObjectResultSet.h"
#include "tagConnection.h"
#include "preObjectVariable.h"
 
void reduceifTrueAndFalseBlock(SodiumSession *session, HTSQLPage *page, const char *bodyLines) {
	 
	char *ifContents = mkStrcat(session->heapHandle, __FILE__, __LINE__, " { ", bodyLines, " } ", NULL);
	bool backup = page->conditionEvaluationResult;      /** There may be another condition statements like if or
														while which alter "conditionEvaluationResult" variable.  */
	parseAndExecutePostString(session, page, ifContents);
	page->conditionEvaluationResult = backup;
	mkFree(session->heapHandle, ifContents);
}
  
void parseAndExecutePostString(SodiumSession *session, HTSQLPage *page, const char *preObjectFunctionBody) {
	 
	yyscan_t scanner;
	YY_BUFFER_STATE post_scan_state; 
    bool returnStatementActive = false;

    if (preObjectFunctionBody == NULL ) {
        return;
    }
    if (page && page->currentCallStack && page->currentCallStack->returnStatementActive) {
        return;
    }   

	//int backupInner = page->currenHTSQLFunctionGenericInstance->functionDebugInfo->lineNumberInner;
	//page->ifCounter = 0;

	postlex_init_extra( session, &scanner );
    post_scan_state = post_scan_string(preObjectFunctionBody, scanner); //
	  
    void *pParser = postParseAlloc(malloc); 
	       
    /** MAIN PARSING LOOP*/        
    page->stopParsing = false;    
	  
    TokenPost token;   
    do {
        token.tokenCode      = postlex(scanner);
        token.tokenStrLength = postget_leng(scanner);
        token.tokenStr       = postget_text(scanner);
        token.lineNo         = postget_lineno(scanner);
		if (token.tokenCode == POST_SEMI_COLON) {
			postParse(pParser, token.tokenCode, token, session);
			token.tokenCode = POST_EXECUTER;
			postParse(pParser, token.tokenCode, token, session);
		}
        postParse(pParser, token.tokenCode, token, session);
		    
        if (page->currentCallStack && page->currentCallStack->returnStatementActive) {
            returnStatementActive = page->currentCallStack->returnStatementActive;
        } 

    } while (token.tokenCode != POST_END_OF_FILE &&
                token.tokenCode != POST_RETURN_PREFIX &&
                    returnStatementActive == false &&
                       page->stopParsing == false);
	
    if (returnStatementActive == false && page->stopParsing == false) {

        if (token.tokenCode != POST_RETURN_PREFIX) {
            /** Parse the last tokens as expected */
            postParse(pParser, 0, "", session);

        } else {

            /** Before ending parsing, Checking return statement has a return value or not */
            token.tokenCode      = postlex(scanner);
            token.tokenStrLength = postget_leng(scanner);
            token.tokenStr       = postget_text(scanner);
            token.lineNo         = postget_lineno(scanner);

            if (token.tokenCode == POST_SEMI_COLON) {
                /** "return statement has no value to return.  */
                postParse(pParser, token.tokenCode, token, session);
                token.tokenCode = POST_EXECUTER;
                postParse(pParser, token.tokenCode, token, session);

            } else {
                /** return statement has a value expression. let's process it  */
                postParse(pParser, token.tokenCode, token, session);

                /** processing until the first ";" character. This should be done since value may be a single expression or sequence of expression. */
                do {
                    token.tokenCode      = postlex(scanner);
                    token.tokenStrLength = postget_leng(scanner);
                    token.tokenStr       = postget_text(scanner);
                    token.lineNo         = postget_lineno(scanner);

                    postParse(pParser, token.tokenCode, token, session);
                } while (token.tokenCode != POST_SEMI_COLON /*&& session->stopParsing == false*/);

                token.tokenCode = POST_EXECUTER;
                postParse(pParser, token.tokenCode, token, session);
            }

        }
    }

    postParseFree(pParser, free );

	//page->ifCounter = backupIfCounter;
	//page->currenHTSQLFunctionGenericInstance->functionDebugInfo->lineNumberInner = backupInner;

    post_delete_buffer(post_scan_state, scanner);
    postlex_destroy(scanner);
}


void executePLSQLBlock(SodiumSession *session, HTSQLPage *page, const char *plSQL) {
	
	DBInt_Connection *conn = (session->rootTAGConnection) ? session->rootTAGConnection->mkDBConnection : NULL;
	if (conn == NULL) {
		return;
	}
	if (plSQL == NULL) {
		return;
	}

    char *plSQLBlock = mkStrcat(session->heapHandle, __FILE__, __LINE__, "begin ", plSQL, " end;", NULL);

	DBInt_Statement *stm = DBInt_CreateStatement(conn);
	DBInt_Prepare(conn, stm, plSQLBlock);
	if (conn->errText == NULL) {
		DBInt_ExecuteAnonymousBlock(conn, stm, plSQLBlock); 
		if (conn->errText) {
			CLOG(ERR_DB_UNKNOWN_ERROR, conn->errText);
		}
	}
	else {
		CLOG(ERR_DB_UNKNOWN_ERROR, conn->errText);
	}	
	DBInt_FreeStatement(conn, stm);
    mkFree(session->heapHandle, plSQLBlock);
}


void executeDeleteStatement(SodiumSession *session, HTSQLPage *page, const char *deleteSQL) {
    executeDMLStatement(session, page, SQL_COMMAND_TYPE_DELETE, deleteSQL);
}

void executeUpdateStatement(SodiumSession *session, HTSQLPage *page, const char *insertSQL) {
    executeDMLStatement(session, page, SQL_COMMAND_TYPE_UPDATE, insertSQL);
}

void executeInsertStatement(SodiumSession *session, HTSQLPage *page, const char *insertSQL) {
    executeDMLStatement(session, page, SQL_COMMAND_TYPE_INSERT, insertSQL);
}

void executeDMLStatement(SodiumSession *session, HTSQLPage *page, SQL_COMMAND_TYPE commandType, const char *singleSQLCommand) {
	PostObjectGeneric   *postObjectGeneric   = NULL;
    PostObjectResultSet *postObjectResultSet = NULL;

    const char *implicitInsertPostObjectGenericObject =  "LastDMLCursor";
    char rowsAffected[20];

    postObjectGeneric = findPostObjectGenericObject(session, page, implicitInsertPostObjectGenericObject);
    if (! postObjectGeneric) {
        postObjectResultSet = createPostObjectResultSet(session, getActiveTAGConnection(session, page));
		PreObjectVariable *preObjectVariable = createPreObjectVariableResultSet(session, implicitInsertPostObjectGenericObject);
		setPreObjectVariableValueRecordSet(session, preObjectVariable, postObjectResultSet);
        postObjectGeneric   = createPostObjectGeneric(session, page, PREOBJECTTYPE_VARIABLE_RESULTSET, implicitInsertPostObjectGenericObject, preObjectVariable);
    }

    evaluateResultSetQuery(session, page, postObjectGeneric, singleSQLCommand);

    if (postObjectGeneric->value.preObjectVariable->value.postObjectResultSet->ociStatement) {
		DBInt_FreeStatement(session->rootTAGConnection->mkDBConnection, postObjectGeneric->value.preObjectVariable->value.postObjectResultSet->ociStatement);
    }
    
	DBInt_Statement *stm = DBInt_CreateStatement(session->rootTAGConnection->mkDBConnection); //postObjectGeneric->postObjectResultSet->ociStatement = mkOCI_StatementCreate(session->rootTAGConnection->ociConnection, __FILE__, __LINE__);
	postObjectGeneric->value.preObjectVariable->value.postObjectResultSet->ociStatement = stm;

    clearClientMessage(session, page);

	switch (commandType) {
		case SQL_COMMAND_TYPE_UPDATE: {
			char *rowCountAffected = DBInt_ExecuteInsertStatement(session->rootTAGConnection->mkDBConnection, stm, postObjectGeneric->value.preObjectVariable->value.postObjectResultSet->evaluatedSQL);
            if (rowCountAffected) {
                mkFree(session->heapHandle, rowCountAffected);
            }
            break;
		}
		case SQL_COMMAND_TYPE_DELETE: {
			DBInt_ExecuteDeleteStatement(session->rootTAGConnection->mkDBConnection, stm, postObjectGeneric->value.preObjectVariable->value.postObjectResultSet->evaluatedSQL);
			break;
		}
		case SQL_COMMAND_TYPE_INSERT: {
            char* rowCountAffected = DBInt_ExecuteInsertStatement(session->rootTAGConnection->mkDBConnection, stm, postObjectGeneric->value.preObjectVariable->value.postObjectResultSet->evaluatedSQL);
            if (rowCountAffected) {
                mkFree(session->heapHandle, rowCountAffected);
            }
            break;
		}
	}
	/*
    mkOCI_ExecuteStmt(session,
                      page,
                      NULL,
                      postObjectGeneric->postObjectResultSet->ociStatement,
                      postObjectGeneric->postObjectResultSet->evaluatedSQL,
                      commandType,
                      false);
    */
	char *clientMessage = getClientMessage(session, page);
    if (clientMessage) {
		CLOG(ERR_DB_UNKNOWN_ERROR, clientMessage);
    } else {
        //mkItoa(OCI_GetAffectedRows(postObjectGeneric->postObjectResultSet->ociStatement), rowsAffected);
		mkItoa(DBInt_GetAffectedRows(session->rootTAGConnection->mkDBConnection, stm), rowsAffected);
		char *errMsg = mkStrcat(session->heapHandle, __FILE__, __LINE__, rowsAffected, " adet satır işlendi", NULL);
        __sendMessageResponse(session, page, errMsg);
        mkFree(session->heapHandle, errMsg);
    }

	DBInt_FreeStatement(session->rootTAGConnection->mkDBConnection, postObjectGeneric->value.preObjectVariable->value.postObjectResultSet->ociStatement);
    postObjectGeneric->value.preObjectVariable->value.postObjectResultSet->ociStatement = NULL;
    
	return;
}



char *convertToNullTerminatedChar(char *str, int sizeOfStr) {
	/*
    char *value = malloc(sizeOfStr+1);
    strncpy_s(value, sizeOfStr + 1, str, sizeOfStr);
    value[sizeOfStr] = '\0';
    return value;
	*/
	return NULL;
}

void postDestructor(SodiumSession *session, char *token) {
   mkPrint(session, token, NULL);
   free(token);
}

TAGDataBlock *getPostCurrentTAGDataBlock(SodiumSession *session, HTSQLPage *page) {
    PreObjectVariable   *preObjectVariableForBlockName = findPostObjectGenericPostObjectVariable(session, page, "datablockname");
    TAGDataBlock        *dataBlock                     = findTAGDataBlockByName(session, page, preObjectVariableForBlockName->value.text);
    return dataBlock;
}

void postTokenDestructor(SodiumSession *session, TokenPost token) {

} 

void postSyntaxError(SodiumSession *session, TokenPost t) {
    HTSQLPage *page = GetCurrentPage(session);
	CLOG(ERR_SYNTAX_ERROR_IN_SQLX_FILE, t.tokenStr);
}
   
/*
void resetPostExecutionEnv(SodiumSession *session, HTSQLPage *page) {
    page->ifCounter = 0;
}*/



void stopPostParserWithMessage(SodiumSession *session, HTSQLPage *page, const char *message, const char *reason) {

    /** The line below may be commented by c programmer since it MAY cause memory leak in some situation.

        DETAIL:
            Due to parser's algorithm, stopping parser in the middle of parsing phase may cause memory leak.
            Some production's outcome (like "expression") must be released by its consumers.
            But this is not possible if parsing process stopped.
    */
    page->stopParsing = true;

    __sendErrorResponse(session, page, message, reason);
}

