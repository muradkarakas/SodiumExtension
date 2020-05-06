#pragma once

#include "htsql.h"
#include "hiredis\hiredis.h"


redisContext	*createRedisConnection(SodiumSession *currentMKSession, HTSQLPage *page, const char* hostname, int port);
void			closeRedisConnection(SodiumSession *currentMKSession, redisContext *newConnectionInstance);


// Commands
char *redisCommandPing(SodiumSession *currentMKSession, HTSQLPage *page, redisContext *c);
char *redisSet(SodiumSession *currentMKSession, HTSQLPage *page, redisContext *c, const char *pKeyName, const char *pKeyValue);
char *redisGet(SodiumSession *currentMKSession, HTSQLPage *page, redisContext *c, const char *pKeyName);
char *redisDel(SodiumSession *currentMKSession, HTSQLPage *page, redisContext *c, const char *pKeyName);
char *redisIncr(SodiumSession *currentMKSession, HTSQLPage *page, redisContext *c, const char *pKeyName);
char *redisDecr(SodiumSession *currentMKSession, HTSQLPage *page, redisContext *c, const char *pKeyName);
char *redisIncrBy(SodiumSession *currentMKSession, HTSQLPage *page, redisContext *c, const char *pKeyName, const char *pByValue);
char *redisDecrBy(SodiumSession *currentMKSession, HTSQLPage *page, redisContext *c, const char *pKeyName, const char *pByValue);
char *redisAppend(SodiumSession *currentMKSession, HTSQLPage *page, redisContext *c, const char *pKeyName, const char *pKeyValue);
char *redisStrlen(SodiumSession *currentMKSession, HTSQLPage *page, redisContext *c, const char *pKeyName);
