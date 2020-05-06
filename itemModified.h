#pragma once

#include "htsql.h"

void __itemModifiedNativeResponse(SodiumSession*currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock, const char *procedureName);
void runItemModifiedHTSQLTrigger(SodiumSession*currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock, const char *oldValue, const char *functionNameToBeCalled);


