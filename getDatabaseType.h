#pragma once


#include "htsql.h"

/*	Caller must free return value if it is not NULL.
*/
char *__getDatabaseType(SodiumSession *currentMKSession, HTSQLPage *page, const char *connectionName);


