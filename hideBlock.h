#pragma once


#include "htsql.h"


void __sendHideBlockResponse(SodiumSession *currentMKSession, HTSQLPage *page, char *pBlockName, bool cascade);

