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

SODIUM_EXTENSION_API
void APIENTRY SodiumHttpRequestHandler(
	HttpContext* context
) {
	// Ýstek *.frmx veya *.sqlx uzantýlý bir dosya için mi ?
	int frmxFlag = wcscmp(context->Current_FileExt, L".frmx");
	int sqlxFlag = wcscmp(context->Current_FileExt, L".sqlx");
	if (context->Current_FileExt[0] != '\0' && frmxFlag != 0 && sqlxFlag != 0) {
		// Farký bir dosya için yapýlan request'ler file system üzerinden okunarak gönderiliyor
		SendHttpFileResponse(context, context->Current_Full_Path);
		goto Done;		
	}
	
	// *.sqlx uzantýlý istekler için "403 Forbidden" cevabý döndürüyoruz
	if (sqlxFlag == 0) {
		SendHttpForbiddenResponse(context);
		goto Done;
	}

	// *.frmx uzantýlý dosya için istek alýndý
	HandleFrmxRequest(context);

Done:

	return;
}




