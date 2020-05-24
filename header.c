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

#include "header.h"


void
SendHtmlResponseHeader(
	SodiumSession* session
)
{
	ADD_KNOWN_HEADER(session->context->response, HttpHeaderContentType, "text/html");
	session->whatIsPrinted = SESSION_PRINT_RESPONSE_HEADER;

	/*char *htmlHeader = mkStrcat(session->heapHandle, __FILE__, __LINE__,
		"Content-type: text/html", "\r\n",
		"charset: utf-8", "\r\n",
		"Set-Cookie: ", mkSessionCookieName, "=", session->id, "; ", " path=/;", "\r\n",
		"Cache-Control: no-cache, no-store, must-revalidate", "\r\n",
		"Pragma: no-cache", "\r\n",
		"Expires: 0", "\r\n\r\n",
		NULL
	);
	addHeaderEx(session, session->lpECB, htmlHeader);
	mkFree(session->heapHandle, htmlHeader);*/
}

void
SendAjaxResponseHeader(
	SodiumSession* session
) {
	ADD_KNOWN_HEADER(session->context->response, HttpHeaderContentType, "application/javascript");
	session->whatIsPrinted = SESSION_PRINT_RESPONSE_HEADER;

	/*char *htmlHeader = mkStrcat(session->heapHandle, __FILE__, __LINE__,
								"Content-Type: application/javascript", "\r\n",
								"charset: utf-8", "\r\n",
								"Cache-Control: no-cache, no-store, must-revalidate", "\r\n",
								"Pragma: no-cache", "\r\n",
								"Expires: 0", "\r\n\r\n",
								NULL
							);
	addHeaderEx(session, session->lpECB, htmlHeader);
	mkFree(session->heapHandle, htmlHeader);*/
}

void sendImageResponseHeader(SodiumSession *session) 
{
	ADD_KNOWN_HEADER(session->context->response, HttpHeaderContentType, "image/webp");
	/*char *htmlHeader = mkStrcat(session->heapHandle, __FILE__, __LINE__,
		"Content-Type: image/jpg", "\r\n",
		"charset: utf-8", "\r\n",
		"Cache-Control: no-cache, no-store, must-revalidate", "\r\n",
		"Pragma: no-cache", "\r\n",
		"Expires: 0", "\r\n\r\n",
		NULL
	);
	addHeaderEx(session, session->lpECB, htmlHeader);
	mkFree(session->heapHandle, htmlHeader);*/
}





void addHeaderEx(SodiumSession* session, LPEXTENSION_CONTROL_BLOCK lpECB, char* header) {
	HSE_SEND_HEADER_EX_INFO hdex;
	char st[] = "200 OK";
	hdex.pszHeader = header;
	hdex.pszStatus = st;
	hdex.cchHeader = (DWORD)strlen(header);
	hdex.cchStatus = sizeof(st);

	lpECB->ServerSupportFunction(lpECB->ConnID,
		HSE_REQ_SEND_RESPONSE_HEADER_EX,
		&hdex,
		NULL,
		NULL);
	session->whatIsPrinted = SESSION_PRINT_RESPONSE_HEADER;
}