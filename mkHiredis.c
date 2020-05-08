#include "pch.h"

#include "mkHiredis.h"

char *redisDecr(SodiumSession *session, HTSQLPage *page, redisContext *c, const char *pKeyName) {
	char *ret = NULL;

	if (c == NULL) {
		ret = mkStrdup(session->heapHandle, ETEXT(ERR_REDIS_NO_VALID_CONNECTION), __FILE__, __LINE__);
	}
	else {

		redisReply *reply = NULL; //redisCommand(c, "DECR %s", pKeyName);
		if (reply) {
			char str[20];
			sprintf_s(str, sizeof(str), "%lld", reply->integer);
			ret = mkStrdup(session->heapHandle, str, __FILE__, __LINE__);
		}
		else {
			if (c->err == 1 && c->errstr) {
				ret = mkStrdup(session->heapHandle, c->errstr, __FILE__, __LINE__);
			}
			else {
				ret = mkStrdup(session->heapHandle, ETEXT(ERR_REDIS_UNKNOWN), __FILE__, __LINE__);
			}
		}

		if (reply) {
			//freeReplyObject(reply);
		}
	}

	return ret;
}

char *redisDecrBy(SodiumSession *session, HTSQLPage *page, redisContext *c, const char *pKeyName, const char *pByValue) {
	char *ret = NULL;

	if (c == NULL) {
		ret = mkStrdup(session->heapHandle, ETEXT(ERR_REDIS_NO_VALID_CONNECTION), __FILE__, __LINE__);
	}
	else {

		redisReply *reply = NULL; //redisCommand(c, "DECRBY %s %s", pKeyName, pByValue);
		if (reply) {
			char str[20];
			sprintf_s(str, sizeof(str), "%lld", reply->integer);
			ret = mkStrdup(session->heapHandle, str, __FILE__, __LINE__);
		}
		else {
			if (c->err == 1 && c->errstr) {
				ret = mkStrdup(session->heapHandle, c->errstr, __FILE__, __LINE__);
			}
			else {
				ret = mkStrdup(session->heapHandle, ETEXT(ERR_REDIS_UNKNOWN), __FILE__, __LINE__);
			}
		}

		if (reply) {
			//freeReplyObject(reply);
		}
	}

	return ret;
}

char *redisIncr(SodiumSession *session, HTSQLPage *page, redisContext *c, const char *pKeyName) {
	char *ret = NULL;

	if (c == NULL) {
		ret = mkStrdup(session->heapHandle, ETEXT(ERR_REDIS_NO_VALID_CONNECTION), __FILE__, __LINE__);
	}
	else {

		redisReply *reply = NULL; //redisCommand(c, "INCR %s", pKeyName);
		if (reply) {
			char str[20];
			sprintf_s(str, sizeof(str), "%lld", reply->integer);
			ret = mkStrdup(session->heapHandle, str, __FILE__, __LINE__);
		}
		else {
			if (c->err == 1 && c->errstr) {
				ret = mkStrdup(session->heapHandle, c->errstr, __FILE__, __LINE__);
			}
			else {
				ret = mkStrdup(session->heapHandle, ETEXT(ERR_REDIS_UNKNOWN), __FILE__, __LINE__);
			}
		}

		if (reply) {
			//freeReplyObject(reply);
		}
	}

	return ret;
}

char *redisIncrBy(SodiumSession *session, HTSQLPage *page, redisContext *c, const char *pKeyName, const char *pByValue) {
	char *ret = NULL;

	if (c == NULL) {
		ret = mkStrdup(session->heapHandle, ETEXT(ERR_REDIS_NO_VALID_CONNECTION), __FILE__, __LINE__);
	}
	else {

		redisReply *reply = NULL; //redisCommand(c, "INCRBY %s %s", pKeyName, pByValue);
		if (reply) {
			char str[20];
			sprintf_s(str, sizeof(str), "%lld", reply->integer);
			ret = mkStrdup(session->heapHandle, str, __FILE__, __LINE__);
		}
		else {
			if (c->err == 1 && c->errstr) {
				ret = mkStrdup(session->heapHandle, c->errstr, __FILE__, __LINE__);
			}
			else {
				ret = mkStrdup(session->heapHandle, ETEXT(ERR_REDIS_UNKNOWN), __FILE__, __LINE__);
			}
		}

		if (reply) {
			//freeReplyObject(reply);
		}
	}

	return ret;
}

redisContext *createRedisConnection(SodiumSession *session, HTSQLPage *page, const char* hostname, int port) {
	// default port 6379
	struct timeval timeout = { 0, 750000 }; // 0.75 seconds
	redisContext* c = NULL;//redisConnectWithTimeout(hostname, port, timeout);
	return c;
}

char *redisCommandPing(SodiumSession *session, HTSQLPage *page, redisContext *c) {
	char *ret = NULL;
	
	if (c == NULL) {
		ret = mkStrdup(session->heapHandle, ETEXT(ERR_REDIS_NO_VALID_CONNECTION), __FILE__, __LINE__);
	} 
	else {

		redisReply *reply = NULL; //redisCommand(c, "PING");
		if (reply) {
			ret = mkStrdup(session->heapHandle, reply->str, __FILE__, __LINE__);
		}
		else {
			if (c && c->err == 1 && c->errstr) {
				// if statement is required even though same is exists above since redisCommand(c, "PING") command set its properties again
				ret = mkStrdup(session->heapHandle, c->errstr, __FILE__, __LINE__);
			}
			else {
				ret = mkStrdup(session->heapHandle, ETEXT(ERR_REDIS_UNKNOWN), __FILE__, __LINE__);
			}
		}

		if (reply) {
			//freeReplyObject(reply);
		}
	}	
	
	return ret;
}

char *redisGet(SodiumSession *session, HTSQLPage *page, redisContext *c, const char *pKeyName) {
	char *ret = NULL;

	if (c == NULL) {
		ret = mkStrdup(session->heapHandle, ETEXT(ERR_REDIS_NO_VALID_CONNECTION), __FILE__, __LINE__);
	}
	else {

		redisReply *reply = NULL; //redisCommand(c, "GET %s", pKeyName);
		if (reply) {
			ret = mkStrdup(session->heapHandle, reply->str, __FILE__, __LINE__);
		}
		else {
			if (c->err == 1 && c->errstr) {
				ret = mkStrdup(session->heapHandle, c->errstr, __FILE__, __LINE__);
			}
			else {
				ret = mkStrdup(session->heapHandle, ETEXT(ERR_REDIS_UNKNOWN), __FILE__, __LINE__);
			}
		}

		if (reply) {
			//freeReplyObject(reply);
		}
	}
	
	return ret;
}

char *redisSet(SodiumSession *session, HTSQLPage *page, redisContext *c, const char *pKeyName, const char *pKeyValue) {

	char *ret = NULL;

	if (c == NULL) {
		ret = mkStrdup(session->heapHandle, ETEXT(ERR_REDIS_NO_VALID_CONNECTION), __FILE__, __LINE__);
	}
	else {
		
		redisReply *reply = NULL; //redisCommand(c, "SET %s %s", pKeyName, pKeyValue);
		if (reply) {
			ret = mkStrdup(session->heapHandle, reply->str, __FILE__, __LINE__);
		}
		else {
			if (c->err == 1 && c->errstr) {
				ret = mkStrdup(session->heapHandle, c->errstr, __FILE__, __LINE__);
			}
			else {
				ret = mkStrdup(session->heapHandle, ETEXT(ERR_REDIS_UNKNOWN), __FILE__, __LINE__);
			}
		} 

		if (reply) {
			//freeReplyObject(reply);
		}
	}

	return ret;
}

char *redisStrlen(SodiumSession *session, HTSQLPage *page, redisContext *c, const char *pKeyName) {

	char *ret = NULL;

	if (c == NULL) {
		ret = mkStrdup(session->heapHandle, ETEXT(ERR_REDIS_NO_VALID_CONNECTION), __FILE__, __LINE__);
	}
	else {

		redisReply *reply = NULL; //redisCommand(c, "STRLEN %s", pKeyName);
		if (reply) {
			char str[20];
			sprintf_s(str, sizeof(str), "%lld", reply->integer);
			ret = mkStrdup(session->heapHandle, str, __FILE__, __LINE__);
		}
		else {
			if (c->err == 1 && c->errstr) {
				ret = mkStrdup(session->heapHandle, c->errstr, __FILE__, __LINE__);
			}
			else {
				ret = mkStrdup(session->heapHandle, ETEXT(ERR_REDIS_UNKNOWN), __FILE__, __LINE__);
			}
		}

		if (reply) {
			//freeReplyObject(reply);
		}
	}

	return ret;
}

char *redisAppend(SodiumSession *session, HTSQLPage *page, redisContext *c, const char *pKeyName, const char *pKeyValue) {

	char *ret = NULL;

	if (c == NULL) {
		ret = mkStrdup(session->heapHandle, ETEXT(ERR_REDIS_NO_VALID_CONNECTION), __FILE__, __LINE__);
	}
	else {

		redisReply *reply = NULL; //redisCommand(c, "APPEND %s %s", pKeyName, pKeyValue);
		if (reply) {
			char str[20];
			sprintf_s(str, sizeof(str), "%lld", reply->integer);
			ret = mkStrdup(session->heapHandle, str, __FILE__, __LINE__);
		}
		else {
			if (c->err == 1 && c->errstr) {
				ret = mkStrdup(session->heapHandle, c->errstr, __FILE__, __LINE__);
			}
			else {
				ret = mkStrdup(session->heapHandle, ETEXT(ERR_REDIS_UNKNOWN), __FILE__, __LINE__);
			}
		}

		if (reply) {
			//freeReplyObject(reply);
		}
	}

	return ret;
}

char *redisDel(SodiumSession *session, HTSQLPage *page, redisContext *c, const char *pKeyName) {

	char *ret = NULL;

	if (c == NULL) {
		ret = mkStrdup(session->heapHandle, ETEXT(ERR_REDIS_NO_VALID_CONNECTION), __FILE__, __LINE__);
	}
	else {

		redisReply* reply = NULL; // redisCommand(c, "DEL %s", pKeyName);
		if (reply) {
			ret = mkStrdup(session->heapHandle, reply->str, __FILE__, __LINE__);
		}
		else {
			if (c->err == 1 && c->errstr) {
				ret = mkStrdup(session->heapHandle, c->errstr, __FILE__, __LINE__);
			}
		}

		if (reply) {
			//freeReplyObject(reply);
		}
	}
	
	return ret;
}

void closeRedisConnection(SodiumSession *session, redisContext *redis) {
	// Disconnects and frees the context 
	if (redis) {
		//redisFree(redis);
	}
}

int testHeredis() {
/*

	// Set a key using binary safe API 
	reply = redisCommand(c, "SET %b %b", "bar", (size_t)3, "hello", (size_t)5);
	printf("SET (binary API): %s\n", reply->str);
	freeReplyObject(reply);

	// Try a GET and two INCR 
	reply = redisCommand(c, "GET foo");
	printf("GET foo: %s\n", reply->str);
	freeReplyObject(reply);

	reply = redisCommand(c, "INCR counter");
	printf("INCR counter: %lld\n", reply->integer);
	freeReplyObject(reply);
	// again ... 
	reply = redisCommand(c, "INCR counter");
	printf("INCR counter: %lld\n", reply->integer);
	freeReplyObject(reply);

	// Create a list of numbers, from 0 to 9 
	reply = redisCommand(c, "DEL mylist");
	freeReplyObject(reply);
	for (j = 0; j < 10; j++) {
		char buf[64];

		snprintf(buf, 64, "%d", j);
		reply = redisCommand(c, "LPUSH mylist element-%s", buf);
		freeReplyObject(reply);
	}

	// Let's check what we have inside the list 
	reply = redisCommand(c, "LRANGE mylist 0 -1");
	if (reply->type == REDIS_REPLY_ARRAY) {
		for (j = 0; j < reply->elements; j++) {
			printf("%u) %s\n", j, reply->element[j]->str);
		}
	}
	freeReplyObject(reply);


	*/
	return 0;
}
