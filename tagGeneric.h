#pragma once

#include "htsql.h"




void                                                  destroyTAGGeneric(SodiumSession *currentMKSession, HTSQLPage *page, TAGGeneric *pTagGeneric);
TAGGeneric *createTAGGeneric(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTagDataBlock, int pTagTOKEN, char *tagSemanticValue);
TAGGeneric *createOrMergeTAGGeneric(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTagDataBlock, int pTagTOKEN, char *tagSemanticValue);

