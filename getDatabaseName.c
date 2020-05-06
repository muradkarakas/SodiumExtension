#include "stdafx.h"

#include "getDatabaseName.h"
#include "tagConnection.h"
#include "..\DBInt\db-interface.h"


char *__getDatabaseName(SodiumSession *currentMKSession, HTSQLPage *page, const char *connectionName) {
	char *retval = NULL;
	TAGConnection *tagConnection = findTAGConnectionByName(currentMKSession, page, connectionName, false);
	if (tagConnection && tagConnection->mkDBConnection) {
		retval = DBInt_GetDatabaseName(tagConnection->mkDBConnection);
	}
	else {
		// if connection name is not valid, returns nothing
	}
	return retval;
}