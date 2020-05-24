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






/* Callar must release return value if it is not null*/
char *__runSQLFile(SodiumSession *currentMKSession, HTSQLPage *page, const char *filePath);

/* Callar must release return value */
char*
GetDBScriptFileContent(
	SodiumSession* session,
	HTSQLPage* page,
	const char * filePath
);

/* Callar must release return value */
char* runScriptFileSQLCommand(SodiumSession *currentMKSession, HTSQLPage *page, DBInt_Connection *conn, const char *sql);


