#include "pch.h"

#include "commit.h"
#include "tagDataBlock.h"
#include "tagConnection.h"

void __commitSingleConnection(SodiumSession *currentMKSession, HTSQLPage *page, TAGConnection *tagConnection);

void __commit(SodiumSession *currentMKSession, HTSQLPage *page, const char *connectionName) {
    mkDebug(currentMKSession, SESSION_LOG_DETAIL_HTSQL_BUILTIN_CALL, "\n", "Commit is executing...", NULL);

    updateOrInsertAllDataBlocksCurrentRow(currentMKSession, page);

    TAGConnection *tagConnection = NULL;

    if (connectionName && strlen(connectionName)>0 ) {
		// "commit" is executing on a spesific connection only
        tagConnection = findTAGConnectionByName(currentMKSession, page, connectionName, true);
        tagConnection = findTAGConnectionByName(currentMKSession, page, connectionName, false);
		__commitSingleConnection(currentMKSession, page, tagConnection);
	} else {
		// commit will be executed on all valid database connections
		tagConnection = currentMKSession->rootTAGConnection;
		while (tagConnection) {
			__commitSingleConnection(currentMKSession, page, tagConnection);
			tagConnection = tagConnection->next;
		}
    }

    mkDebug(currentMKSession, SESSION_LOG_DETAIL_HTSQL_BUILTIN_CALL, "\n", "Done. (Commit)", NULL);
}


void __commitSingleConnection(SodiumSession * session, HTSQLPage *page, TAGConnection *tagConnection) {
	if (tagConnection && tagConnection->mkDBConnection) {
		bool err = DBInt_Commit(tagConnection->mkDBConnection);
		const char *errText = DBInt_GetLastErrorText(tagConnection->mkDBConnection);
		if (errText == NULL || err == false) {
			__sendCommitSuccessfulMessage(session, page, tagConnection->connectionName);
		}
		else {
			char errcode[20];
			int errNo = DBInt_GetLastError(tagConnection->mkDBConnection);
			mkItoa(errNo, errcode);
			char *str = mkStrcat(session->heapHandle, __FILE__, __LINE__, "DB ERROR:", errcode, DBInt_GetLastErrorText(tagConnection->mkDBConnection), NULL);
			CLOG(ERR_DB_UNKNOWN_ERROR, str);
			mkFree(session->heapHandle, str);
		}
	}
	else {
		CLOG(ERR_DB_INVALID_CONNECTION, "");
	}
}

void __sendCommitSuccessfulMessage(SodiumSession *currentMKSession, HTSQLPage *page, const char *connectionName) {
    char *lastResponse = mkStrcat(currentMKSession->heapHandle,
                                  __FILE__, __LINE__,
                                  "action = { "
                                                "\"type\"            : \"commitSuccessful\", "
                                                "\"connectionName\"  : \"", connectionName, "\" "
                                          " };",
                                  "addNewActionToTheResponseActionList(action);",
                                  NULL);
    mkPrint(currentMKSession, lastResponse, NULL);
    mkFree(currentMKSession->heapHandle, lastResponse);
}
