#include "pch.h"

#include "createConnection.h"
#include "tagConnection.h"

const char *
CreateConnection(
	SodiumSession *session, 
	HTSQLPage *page, 
	const char *pConnName, 
	const char *dbType, 
	const char *hostName,
	const char *pInstanceName, 
	const char *pUserName, 
	const char *pPassword
)
{
	const char * retVal = NULL;

	// connection is not defined before
	int dbt = SODIUM_NOTSET_SUPPORT;
	if (strcmp(dbType, "oracle") == 0) {
		dbt = SODIUM_ORACLE_SUPPORT;
	} else if (strcmp(dbType, "postgresql") == 0) {
		dbt = SODIUM_POSTGRESQL_SUPPORT;
	} else if (strcmp(dbType, "mysql") == 0) {
		dbt = SODIUM_MYSQL_SUPPORT;
	}
	else {
		return ETEXT(ERR_DB_DBTYPE_NOT_SUPPORTED);
	}

	TAGConnection *tagConnection = findTAGConnectionByName(session, page, pConnName, false);

	if (tagConnection == NULL) {

		DBInt_Connection *mkDBConnection = DBInt_CreateDBConnection(session->heapHandle, dbt, hostName, pInstanceName, pUserName, pPassword);	
		if (mkDBConnection && mkDBConnection->errText == NULL) {
			// conection to database is done
			tagConnection = createTAGConnection(session, page);
			setTAGConnectionPropertyConnectionName(session, tagConnection, pConnName);
			setTAGConnectionPropertyInstanceName(session, tagConnection, pInstanceName);
			setTAGConnectionPropertyUserName(session, tagConnection, pUserName);
			setTAGConnectionPropertyUserPassword(session, tagConnection, pPassword);			
			setTAGConnectionMKDBConnectionProperty(session, tagConnection, mkDBConnection);
			retVal = NULL;
		}
		else {
			// connection cannot be established for some reason
			mkDebug(session, SESSION_LOG_DETAIL_DB_EVENTS, "\n", ETEXT(ERR_DB_CONNECTION_CANNOT_BE_OPENED), (mkDBConnection->errText) ? mkDBConnection->errText:"", NULL);
			retVal = (mkDBConnection->errText) ? mkDBConnection->errText : "";
			DBInt_DestroyDBConnection(mkDBConnection);
		}	
	}
	else {
		// Connection is defined before
		// checking if it is a valid 
		if (tagConnection->mkDBConnection) {
			// closing/destroying connection
			DBInt_DestroyDBConnection(tagConnection->mkDBConnection);
		}

		setTAGConnectionPropertyInstanceName(session, tagConnection, pInstanceName);
		setTAGConnectionPropertyUserName(session, tagConnection, pUserName);
		setTAGConnectionPropertyUserPassword(session, tagConnection, pPassword);

		DBInt_Connection *mkDBConnection = DBInt_CreateDBConnection(session->heapHandle, dbt, hostName, pInstanceName, pUserName, pPassword);
		if (mkDBConnection && mkDBConnection->errText) {
			mkDebug(session, SESSION_LOG_DETAIL_DB_EVENTS, "\n", ETEXT(ERR_DB_CONNECTION_CANNOT_BE_OPENED), (mkDBConnection->errText) ? mkDBConnection->errText : "", NULL);
			retVal = (mkDBConnection->errText) ? mkDBConnection->errText : "";
			DBInt_DestroyDBConnection(mkDBConnection);
		}
		else {
			// Success
			retVal = NULL;
			setTAGConnectionMKDBConnectionProperty(session, tagConnection, mkDBConnection);
		}				
	}    
    return retVal;
}
