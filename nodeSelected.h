#pragma once

#include "htsql.h"

void __sendTreeNodeSelectedHTSQLResponse(SodiumSession *currentMKSession, HTSQLPage *page, const char *blockname);

void __sendTreeNodeSelectedHTSQLResponseMessage(SodiumSession *currentMKSession, HTSQLPage *page, const char *treename, const char *nodeid);

