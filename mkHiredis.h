/**
 * This file is part of Sodium Language project
 *
 * Copyright © 2020 Murad Karakaþ <muradkarakas@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License v3.0
 * as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 *	https://choosealicense.com/licenses/gpl-3.0/
 */

#pragma once


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
