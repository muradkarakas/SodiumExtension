/**
 * This file is part of Sodium Language project
 *
 * Copyright � 2020 Murad Karaka� <muradkarakas@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License v3.0
 * as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 *	https://choosealicense.com/licenses/gpl-3.0/
 */

#include "pch.h"

SODIUM_EXTENSION_API
void APIENTRY SodiumHttpRequestHandler(
	HttpContext* context
) {
	// �stek *.frmx veya *.sqlx uzant�l� bir dosya i�in mi ?
	int frmxFlag = wcscmp(context->Current_FileExt, L".frmx");
	int sqlxFlag = wcscmp(context->Current_FileExt, L".sqlx");
	if (context->Current_FileExt[0] != '\0' && frmxFlag != 0 && sqlxFlag != 0) {
		// Fark� bir dosya i�in yap�lan request'ler file system �zerinden okunarak g�nderiliyor
		SendHttpFileResponse(context, context->Current_Full_Path);
		goto Done;		
	}
	
	// *.sqlx uzant�l� istekler i�in "403 Forbidden" cevab� d�nd�r�yoruz
	if (sqlxFlag == 0) {
		SendHttpForbiddenResponse(context);
		goto Done;
	}

	// *.frmx uzant�l� dosya i�in istek al�nd�
	HandleFrmxRequest(context);

Done:

	return;
}




