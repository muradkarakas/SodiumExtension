#pragma once


#include "htsql.h"

void                                   __rollback(SodiumSession *currentMKSession, HTSQLPage *page, const char *connectionName);
void              __sendRollbackSuccessfulMessage(SodiumSession *currentMKSession, HTSQLPage *page, const char *connectionName);



