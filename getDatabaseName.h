#pragma once

#include "htsql.h"


/*	Caller must free return value if it is not NULL.
*/
char *__getDatabaseName(SodiumSession *currentMKSession, HTSQLPage *page, const char *connectionName);

