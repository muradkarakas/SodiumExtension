#pragma once


#include "htsql.h"



/* Callar must release return value if it is not null*/
char *__runSQLFile(SodiumSession *currentMKSession, HTSQLPage *page, const char *filePath);

/* Callar must release return value */
char*
GetDBScriptFileContent(
	SodiumSession* session,
	HTSQLPage* page,
	const char * filePath
);

/* Callar must release return value */
char* runScriptFileSQLCommand(SodiumSession *currentMKSession, HTSQLPage *page, DBInt_Connection *conn, const char *sql);


