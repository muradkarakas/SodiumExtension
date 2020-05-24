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

#include "getDatabaseName.h"
#include "tagConnection.h"
#include "..\DBInt\db-interface.h"


char *__getDatabaseName(SodiumSession *currentMKSession, HTSQLPage *page, const char *connectionName) {
	char *retval = NULL;
	TAGConnection *tagConnection = findTAGConnectionByName(currentMKSession, page, connectionName, false);
	if (tagConnection && tagConnection->mkDBConnection) {
		retval = DBInt_GetDatabaseName(tagConnection->mkDBConnection);
	}
	else {
		// if connection name is not valid, returns nothing
	}
	return retval;
}