#include "stdafx.h"

SODIUM_SHARED_API
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




