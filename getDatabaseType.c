#include "stdafx.h"

#include "getDatabaseType.h"
#include "tagConnection.h"

#include "..\DBInt\db-interface.h"


char *__getDatabaseType(SodiumSession *currentMKSession, HTSQLPage *page, const char *connectionName) {
	char *retval = NULL;
	TAGConnection *tagConnection = findTAGConnectionByName(currentMKSession, page, connectionName, false);
	if (tagConnection && tagConnection->mkDBConnection) {
		const char *dbType = DBInt_GetDatabaseType(tagConnection->mkDBConnection);
		if (dbType) {
			retval = mkStrdup(currentMKSession->heapHandle, dbType, __FILE__, __LINE__);
		}
	}
	else {
		// if connection name is not valid, returns nothing
	}
	return retval;
}