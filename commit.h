#pragma once

#include "htsql.h"

void                         __commit(SodiumSession*currentMKSession, HTSQLPage *page, const char *connectionName);
void    __sendCommitSuccessfulMessage(SodiumSession*currentMKSession, HTSQLPage *page, const char *connectionName);


