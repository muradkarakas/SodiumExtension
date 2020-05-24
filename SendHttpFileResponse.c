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

DWORD 
SendHttpFileResponse(
	HttpContext		*context,
	IN LPWSTR        pFilePath
)
{
	HTTP_RESPONSE   response;
	HTTP_DATA_CHUNK dataChunk;
	DWORD           result;
	DWORD           bytesSent;
	char			*szBuffer = NULL;
	DWORD			dwBytesRead;
	DWORD			fileSize;
	
	HANDLE fileHandle = CreateFileW(pFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

	if (fileHandle == INVALID_HANDLE_VALUE) {
		//SHOW_GET_LAST_ERROR_MESSAGE(L"\nCreateFile failed with %ws");
		//wprintf_s(L"\nFile name: %ws", pFilePath);
		result = GetLastError();
		if (result == ERROR_FILE_NOT_FOUND) {
			SendHttpNotFoundResponse(context);
		}
		goto Done;
	}
	
	fileSize = GetFileSize(fileHandle, NULL);
	if (INVALID_FILE_SIZE == fileSize) {
		SHOW_GET_LAST_ERROR_MESSAGE(L"  Source file not read from. Error %ws");
		result = GetLastError();
		goto Done;
	}

	szBuffer = malloc(fileSize);
	if (NULL == szBuffer ) {
		SHOW_GET_LAST_ERROR_MESSAGE(L"  Source file not read from. Error %ws");
		result = GetLastError();
		goto Done;
	}

	if (!ReadFile(fileHandle, szBuffer, fileSize, &dwBytesRead, NULL)) {
		SHOW_GET_LAST_ERROR_MESSAGE(L"  Source file not read from. Error %ws");
		result = GetLastError();
		goto Done;
	}

	//
	// Initialize the HTTP response structure.
	//
	INITIALIZE_HTTP_RESPONSE(&response, 200, "OK");

	//
	// Add a known header.
	//
	if (wcscmp(context->Current_FileExt, L".html") == 0 || wcscmp(context->Current_FileExt, L".htm") == 0)
		ADD_KNOWN_HEADER(response, HttpHeaderContentType, "text/html");
	else if (wcscmp(context->Current_FileExt, L".png") == 0)
		ADD_KNOWN_HEADER(response, HttpHeaderContentType, "image/png");
	else if (wcscmp(context->Current_FileExt, L".gif") == 0)
		ADD_KNOWN_HEADER(response, HttpHeaderContentType, "image/gif");
	else if (wcscmp(context->Current_FileExt, L".jpg") == 0 || wcscmp(context->Current_FileExt, L".jpeg") == 0)
		ADD_KNOWN_HEADER(response, HttpHeaderContentType, "image/jpeg");
	else if (wcscmp(context->Current_FileExt, L".ico") == 0)
		ADD_KNOWN_HEADER(response, HttpHeaderContentType, "image/x-icon");
	else if (wcscmp(context->Current_FileExt, L".js") == 0)
		ADD_KNOWN_HEADER(response, HttpHeaderContentType, "text/javascript");
	else if (wcscmp(context->Current_FileExt, L".css") == 0)
		ADD_KNOWN_HEADER(response, HttpHeaderContentType, "text/css");
		
	// getting System Time, Adding a day to it
	SYSTEMTIME		st = { 0 };
	FILETIME		ft = { 0 };
	ULARGE_INTEGER	u = { 0 };
	int				ONEDAY = 60 * 60 * 24; // 24 hours
	GetSystemTime(&st);
	SystemTimeToFileTime(&st, &ft);
	memcpy(&u, &ft, sizeof(u));
	u.QuadPart += ONEDAY * 10000000LLU;
	memcpy(&ft, &u, sizeof(ft));
	FileTimeToSystemTime(&ft, &st);


	// converting date time to expire string format. // expires: Sun, 22 Mar 2020 22:08:08 GMT
	TCHAR expireStr[200];
	result = GetDateFormat(LOCALE_INVARIANT, 0, &st, "ddd, dd MMM  yyyy", &expireStr[0], 200);
	expireStr[result-1] = ' ';
	result = GetTimeFormat(LOCALE_INVARIANT, 0, &st, "HH:mm:ss GMT", &expireStr[result], 100);
	// adding Expire header to response
	ADD_KNOWN_HEADER(response, HttpHeaderExpires, expireStr);

	result =
		HttpSendHttpResponse(
			context->hReqQueue,           // ReqQueueHandle
			context->pRequest->RequestId, // Request ID
			HTTP_SEND_RESPONSE_FLAG_MORE_DATA,
			&response,       // HTTP response
			NULL,            // pReserved1
			&bytesSent,      // bytes sent-optional
			NULL,            // pReserved2
			0,               // Reserved3
			NULL,            // LPOVERLAPPED
			NULL             // pReserved4
		);

	if (result != NO_ERROR) {
		wprintf(L"HttpSendHttpResponse failed with %lu \n", result);
		goto Done;
	}

	dataChunk.DataChunkType = HttpDataChunkFromMemory;
	dataChunk.FromMemory.pBuffer = (PVOID)szBuffer;
	dataChunk.FromMemory.BufferLength = dwBytesRead;

	// 
	// Because the entity body is sent in one call, it is not
	// required to specify the Content-Length.
	//

	result = HttpSendResponseEntityBody(
		context->hReqQueue,
		context->pRequest->RequestId,
		HTTP_SEND_RESPONSE_FLAG_DISCONNECT,           // This is the last send.
		1,           // Entity Chunk Count.
		&dataChunk,
		&bytesSent,
		NULL,
		0,
		NULL,
		NULL
	);

	if (result != NO_ERROR) {
		wprintf(L"HttpSendResponseEntityBody failed with %lu \n", result);
	}
	
Done:
	if (szBuffer)
		free(szBuffer);

	if (fileHandle)
		CloseHandle(fileHandle);

	return result;
}