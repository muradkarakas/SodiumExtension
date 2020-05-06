#include "stdafx.h"

#include "createRedisConnection.h"
#include "mkHiredis.h"

redisContext *__createRedisConnection(SodiumSession *currentMKSession, HTSQLPage *page, const char *serverAddress, const char *portNumber) {

	return  createRedisConnection(currentMKSession, page, serverAddress, atoi(portNumber));

}
 


