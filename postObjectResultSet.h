#pragma once

#include "htsql.h"




PostObjectResultSet		*createPostObjectResultSet(SodiumSession *currentMKSession, TAGConnection *resultsetTAGConnection);
void					destroyPostObjectResultSet(SodiumSession *currentMKSession, PostObjectResultSet *object);

void	setPostObjectResultSetResultSet(SodiumSession *currentMKSession, HTSQLPage *page, PostObjectGeneric *postObjectGeneric, TAGConnection *resultsetTAGConnection, const char *pSql);
void	evaluateResultSetQuery(SodiumSession *currentMKSession, HTSQLPage *page, PostObjectGeneric *postObjectGeneric, const char *pSql);

void	setPostObjectResultSetEvaluatedSQL(SodiumSession *currentMKSession, PostObjectGeneric *postObjectGeneric, const char *newvalue);


