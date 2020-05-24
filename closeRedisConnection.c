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

