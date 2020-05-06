#pragma once

#include "htsql.h"


void __sendTreeNodeExpandedHTSQLResponse(SodiumSession *currentMKSession, HTSQLPage *page, const char *blockname);

void __sendTreeNodeExpandedHTSQLResponseMessage(SodiumSession *currentMKSession, HTSQLPage *page, const char *treename, const char *nodeid, const char *message);


