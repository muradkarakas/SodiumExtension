#pragma once

redisContext	*__createRedisConnection(SodiumSession *currentMKSession, HTSQLPage *page, const char *serverAddress, const char *portNumber);

