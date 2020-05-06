#pragma once

#include "htsql.h"


TAGDatalist *                         getCurrentTAGDatalist(SodiumSession *currentMKSession, HTSQLPage *page);
TAGDatalist *                             createTAGDatalist(SodiumSession *currentMKSession, HTSQLPage *page);
void                                        pushTAGDatalist(SodiumSession *currentMKSession, HTSQLPage *page, TAGDatalist *pTAGDatalist);
void                                  setCurrentTAGDatalist(SodiumSession *currentMKSession, HTSQLPage *page, TAGDatalist *tagDatalist);
void                                     destroyTAGDatalist(SodiumSession *currentMKSession, HTSQLPage *page, TAGDatalist *tagDatalist);
TAGDatalist *                         findTAGDatalistByName(SodiumSession *currentMKSession, HTSQLPage *page, const char *pDatalistName);

void                 setTAGDatalistPropertyId(SodiumSession *currentMKSession, HTSQLPage *page, char *pPropValue);

void     setTAGDatalistPropertyConnectionName(SodiumSession *currentMKSession, HTSQLPage *page, char *pPropValue);

