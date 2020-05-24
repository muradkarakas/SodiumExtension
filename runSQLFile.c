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

#include "runSQLFile.h"
#include "tagConnection.h"


char *
__runSQLFile(
	SodiumSession *session, 
	HTSQLPage *page, 
	const char *filePath
) 
{
	char *retval = NULL;

	// check parameters are valid
	if (filePath == NULL || strlen(filePath) == 0) {
		CLOG(ERR_FUNCTION_PARAMETER_CANNOT_BE_NULL, "fileName");
		return retval;
	}

	// Get root connection
	DBInt_Connection *conn = getRootDBIntConnection(session, page);

	if (conn) {
		// Connection ok
		// Getting file content 
		char *sql = GetDBScriptFileContent(session, page, filePath);

		if (sql) {
			// SQL command must be seperated by ';' character in sql script file.
			size_t strmax = strlen(sql);
			const char	*delim = ";";
			char		*next_token;

			char *token = strtok_s(sql, delim, &next_token);
			while (token && retval == NULL) {
				retval = runScriptFileSQLCommand(session, page, conn, token);
				token = strtok_s(NULL, delim, &next_token);
			}
		}

		mkFree(session->heapHandle, sql);
	}
	else {
		// No valid root connection exists
		CLOG(ERR_DB_NO_CONNECTION_EXISTS, "default");
	}
	return retval;
}

char *
GetDBScriptFileContent(
	SodiumSession *session, 
	HTSQLPage *page, 
	const char *filePath
) 
{

	char *buffer = NULL;
	FILE *file = NULL;

	wchar_t *physicalPath = GetPhysicalPathForFile(session, filePath);

	// opening file
	errno_t err = _wfopen_s(&file, physicalPath, L"rb");
	if (file == NULL) {
		CLOG(ERR_FILE_CANNOT_BE_OPENED, filePath);
		return NULL;
	}

	// Getting file size 
	fseek(file, 0L, SEEK_END);
	DWORD fileSize = ftell(file);
	if (fileSize > 0) {
		// Reading file content into memory 
		buffer = mkMalloc(session->heapHandle, fileSize+2, __FILE__, __LINE__);
		fseek(file, 0L, SEEK_SET);
		fread(buffer, fileSize, 1, file);
	}

	// Free resources 
	fclose(file);
		
	mkFree(session->heapHandle, physicalPath);

	return buffer;
}



char* runScriptFileSQLCommand(SodiumSession *session, HTSQLPage *page, DBInt_Connection *conn, const char *sql) { 
	
	if (sql == NULL || mkIsStringEmpty(sql) == true) {
		return NULL;
	}

	char *retval = NULL;

	DBInt_Statement *stm = DBInt_CreateStatement(conn); 
	DBInt_Prepare(conn, stm, sql);
	if (conn->errText == NULL) {
		DBInt_ExecuteAnonymousBlock(conn, stm, sql);
		if (conn->errText) {
			retval = mkStrdup(session->heapHandle, conn->errText, __FILE__, __LINE__);
		}
	}
	else {
		retval = mkStrdup(session->heapHandle, conn->errText, __FILE__, __LINE__);
	}
	DBInt_FreeStatement(conn, stm);

	return retval;
}



