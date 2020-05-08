#include "pch.h"

#include "closeRedisConnection.h"
#include "htsqlVariable.h"
#include "mkHiredis.h"


void __closeRedisConnection(SodiumSession *session, HTSQLPage *page, const char *connectionName) {

	PreObjectVariable *preObjectVariable = findRedisPreObjectVariable(session, page, connectionName, false);
	if (preObjectVariable) {
		if (preObjectVariable->value.redisContextValue) {
			closeRedisConnection(session, preObjectVariable->value.redisContextValue);
			preObjectVariable->value.redisContextValue = NULL;
		}
	}
	else {
		CLOG(ERR_NAME_IS_NOT_FOUND, connectionName);
	}

}

