#pragma once

#include "htsql.h"


void        dispatchSmallRequest(SodiumSession *currentMKSession, HTSQLPage *page);
void		printGetRequestTiming(SodiumSession *currentMKSession, HTSQLPage *page, clock_t start);


