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

#include "pch.h"

#include "postObjectResultSet.h"


#include "post.parser.imp.h"


PostObjectResultSet *createPostObjectResultSet(SodiumSession *session, TAGConnection *resultsetTAGConnection) {
    PostObjectResultSet *object = mkMalloc(session->heapHandle, sizeof(PostObjectResultSet), __FILE__, __LINE__);
    object->evaluatedSQL = NULL;
    object->ociStatement = NULL;
	object->resultsetTAGConnection = resultsetTAGConnection;
    return object;
}

void setPostObjectResultSetEvaluatedSQL(SodiumSession *session, PostObjectGeneric *postObjectGeneric, const char *newvalue) {
    if (postObjectGeneric && postObjectGeneric->value.preObjectVariable->value.postObjectResultSet) {
        if (postObjectGeneric->value.preObjectVariable->value.postObjectResultSet->evaluatedSQL) {
            mkFree(session->heapHandle, postObjectGeneric->value.preObjectVariable->value.postObjectResultSet->evaluatedSQL);
            postObjectGeneric->value.preObjectVariable->value.postObjectResultSet->evaluatedSQL = NULL;
        }
        postObjectGeneric->value.preObjectVariable->value.postObjectResultSet->evaluatedSQL = mkStrdup(session->heapHandle, newvalue, __FILE__, __LINE__);
    }
}

void destroyPostObjectResultSet(SodiumSession *session, PostObjectResultSet *object) {
    if (object->ociStatement) {
		// session->rootTAGConnection->mkDBConnection
		DBInt_FreeStatement(object->resultsetTAGConnection->mkDBConnection, object->ociStatement);
        object->ociStatement = NULL;
    }
    if (object->evaluatedSQL) {
        mkFree(session->heapHandle, object->evaluatedSQL);
        object->evaluatedSQL = NULL;
    }
    mkFree(session->heapHandle, object);
}


void setPostObjectResultSetResultSet(SodiumSession *session, HTSQLPage *page, PostObjectGeneric *postObjectGeneric, TAGConnection *resultsetTAGConnection, const char *pSql) {

    char *lSql = mkStrdup(session->heapHandle, pSql, __FILE__, __LINE__);
	
	evaluateResultSetQuery(session, page, postObjectGeneric, lSql);

	if (postObjectGeneric->value.preObjectVariable->value.postObjectResultSet->ociStatement) {
		DBInt_FreeStatement(resultsetTAGConnection->mkDBConnection, postObjectGeneric->value.preObjectVariable->value.postObjectResultSet->ociStatement);
        postObjectGeneric->value.preObjectVariable->value.postObjectResultSet->ociStatement = NULL;
    }

	postObjectGeneric->value.preObjectVariable->value.postObjectResultSet->ociStatement = DBInt_CreateStatement(resultsetTAGConnection->mkDBConnection);

	DBInt_Prepare(
        resultsetTAGConnection->mkDBConnection, 
        postObjectGeneric->value.preObjectVariable->value.postObjectResultSet->ociStatement, 
        postObjectGeneric->value.preObjectVariable->value.postObjectResultSet->evaluatedSQL);
	
    if (resultsetTAGConnection->mkDBConnection->errText) {
		CLOG(ERR_DB_UNKNOWN_ERROR, resultsetTAGConnection->mkDBConnection->errText);
	}
	else {
		DBInt_ExecuteSelectStatement(resultsetTAGConnection->mkDBConnection,
			postObjectGeneric->value.preObjectVariable->value.postObjectResultSet->ociStatement,
			postObjectGeneric->value.preObjectVariable->value.postObjectResultSet->evaluatedSQL);

		if (resultsetTAGConnection->mkDBConnection->errText) {
			CLOG(ERR_DB_UNKNOWN_ERROR, resultsetTAGConnection->mkDBConnection->errText);
		}
	}
	
    mkFree(session->heapHandle, lSql);
}

void evaluateResultSetQuery(SodiumSession *session, HTSQLPage *page, PostObjectGeneric *postObjectGeneric, const char *pSql) {

    char *sql = mkStrcat(session->heapHandle, __FILE__, __LINE__, "?--", postObjectGeneric->objectName, ">>", pSql, "--?", NULL);
    parseAndExecutePostString(session, page, sql);
    mkFree(session->heapHandle, sql);

}
