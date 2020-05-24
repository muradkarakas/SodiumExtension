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

#include "rollback.h"
#include "tagConnection.h"

void __rollbackSingleConnection(SodiumSession *session, HTSQLPage *page, TAGConnection *tagConnection);

void __rollback(SodiumSession *session, HTSQLPage *page, const char *connectionName) {
    mkDebug(session, SESSION_LOG_DETAIL_HTSQL_BUILTIN_CALL, "\n", "Rollback is executing...", NULL);

    TAGConnection *tagConnection = NULL;

    if (connectionName && strlen(connectionName)>0 ) {
		// "rollback" is executing on a spesific connection only
        tagConnection = findTAGConnectionByName(session, page, connectionName, true);
        tagConnection = findTAGConnectionByName(session, page, connectionName, false);
		__rollbackSingleConnection(session, page, tagConnection);
    } else {
		// rollback will be executed on all valid database connections
		tagConnection = session->rootTAGConnection;
		while (tagConnection) {
			__rollbackSingleConnection(session, page, tagConnection);
			tagConnection = tagConnection->next;
		}
    }

    
    mkDebug(session, SESSION_LOG_DETAIL_HTSQL_BUILTIN_CALL, "\n", "Done. (Rollback)", NULL);
}

void __rollbackSingleConnection(SodiumSession *session, HTSQLPage *page, TAGConnection *tagConnection) {
	if (tagConnection && tagConnection->mkDBConnection) {
		DBInt_Rollback(tagConnection->mkDBConnection);
		const char *errText = DBInt_GetLastErrorText(tagConnection->mkDBConnection);
		if (errText == NULL) {
			__sendRollbackSuccessfulMessage(session, page, tagConnection->connectionName);
		}
		else {
			char errcode[20];
			int errNo = DBInt_GetLastError(tagConnection->mkDBConnection);
			mkItoa(errNo, errcode);
			CLOG(ERR_DB_UNKNOWN_ERROR, tagConnection->mkDBConnection->errText);
		}
	}
	else {
		CLOG(ERR_DB_INVALID_CONNECTION, "");
	}
}

void __sendRollbackSuccessfulMessage(SodiumSession *session, HTSQLPage *page, const char *connectionName) {
    char *lastResponse = mkStrcat(session->heapHandle, __FILE__, __LINE__,
                                  "action = { "
                                                "\"type\"            : \"rollbackSuccessful\", "
                                                "\"connectionName\"   : \"", connectionName, "\" "
                                          " };",
                                  "addNewActionToTheResponseActionList(action);",
                                  NULL);
    mkPrint(session, lastResponse, NULL);
    mkFree(session->heapHandle, lastResponse);
}
