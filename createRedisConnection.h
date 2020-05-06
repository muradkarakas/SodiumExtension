#pragma once

#include "htsql.h"

redisContext	*__createRedisConnection(SodiumSession *currentMKSession, HTSQLPage *page, const char *serverAddress, const char *portNumber);

