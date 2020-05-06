#include "stdafx.h"

#include "dispatcherBig.h"
 
#include "header.h"
#include "image.h"
#include "memmem.h"


void 
DispatchBigRequest(
    SodiumSession *session, 
    HTSQLPage *page
) 
{

    const char *boundry         = getBoundry(session);
    char *nativeproceduretorun  = getFormDataBig(session, boundry, "nativeproceduretorun");

    SendAjaxResponseHeader(session);

	mkDebug(session, SESSION_LOG_DETAIL_ISAPI, "\n\n######   POST (BIG) (", page->fileNameRequested, ")  \n", NULL);

	if (nativeproceduretorun) {
        if (strcmp(nativeproceduretorun, "putimage") == 0) {
            __sendPutImageResponse(session, page, boundry);
        }
    } else {
		CLOG(ERR_CORE_INVALID_HTTP_REQUEST, "");
    }

	if (session->sessionReadyState != SESSION_READY_STATE_KILLED) {
		mkDebug(session, SESSION_LOG_DETAIL_ISAPI, "\n\n######   POST (BIG) DONE (", page->fileNameRequested, ") \n", NULL);
	}

    mkFree(session->heapHandle, nativeproceduretorun);
}


void *getBinaryDataStartPosition(SodiumSession *session, const char *boundry, const char *name) {
    void *value = NULL;
    char *searchStr = mkStrcat(session->heapHandle, __FILE__, __LINE__,
        "Content-Disposition: form-data; name=\"", name, "\"; filename=\"",
        NULL
    );
    char *pos = strstr(session->context->pRequest->pEntityChunks->FromMemory.pBuffer, searchStr);
    if (pos) {
        char *start = pos + strlen(searchStr) + 4;
        start       = strstr(start, "\r\n");    // "content-type" line
        start       = strstr(start+2, "\r\n");  // enter
        start       = strstr(start+2, "\r\n");  // empty line
        start       = start + 2;                // binary data starts here
        value       = start;
    }
    mkFree(session->heapHandle, searchStr);
    return value;
}

char *getBinaryDataFileName(SodiumSession *session, const char *boundry, const char *imageInputName) {
    char *value = NULL;
    char *searchStr = mkStrcat(session->heapHandle, __FILE__, __LINE__,
        "Content-Disposition: form-data; name=\"", imageInputName, "\"; filename=\"",
        NULL
    );
    char *pos = strstr(session->context->pRequest->pEntityChunks->FromMemory.pBuffer, searchStr);
    if (pos) {
        char *start = pos + strlen(searchStr);
        char *end   = strstr(start, "\r\n");
        int size    = (int) (end - start - 1);
        char *tmp   = mkMalloc(session->heapHandle, size+2, __FILE__, __LINE__);
        strncpy_s(tmp, size + 2, start, size);
        tmp[size]   = '\0';
        value       = tmp;
    }
    mkFree(session->heapHandle, searchStr);
    return value;
}

char *getBinaryDataContentType(SodiumSession *session, const char *boundry, const char *imageInputName) {
    char *value = NULL;
    char *searchStr = mkStrcat(session->heapHandle, __FILE__, __LINE__,
        "Content-Disposition: form-data; name=\"", imageInputName, "\"; filename=\"",
        NULL
    );
    char *pos = strstr(session->context->pRequest->pEntityChunks->FromMemory.pBuffer, searchStr);
    if (pos) {
        char *cntTypeStr = "Content-Type: ";
        size_t   cntTypeStrLen = strlen(cntTypeStr);
        char *start = pos + strlen(searchStr);
        start       = strstr(start, "\r\n");
        start       = start + 2;
        if (start == strstr(start, cntTypeStr)) {
            char *end   = strstr(start, "\r\n");
            int size    = (int) (end - start);
            char *tmp   = mkMalloc(session->heapHandle, size+2, __FILE__, __LINE__);
            strncpy_s(tmp, size + 2, start+cntTypeStrLen, size-cntTypeStrLen);
            tmp[size]   = '\0';
            value       = tmp;
        }
    }
    mkFree(session->heapHandle, searchStr);
    return value;
}

const char *getBoundry(SodiumSession *session) {
    const char *datapos = strstr(session->context->pRequest->Headers.KnownHeaders[HttpHeaderContentType].pRawValue, "boundary=");
    if (datapos) {
        return datapos + 9;
    } else {
        return NULL;
    }
}

char *getFormDataBig(SodiumSession *session, const char *boundry, const char *formInputName) {
    char *value = NULL;
    char *searchStr = mkStrcat(session->heapHandle, __FILE__, __LINE__,
        "Content-Disposition: form-data; name=\"", formInputName, "\"",
        NULL
    );
    char *pos = mkMemmem(
        session->context->pRequest->pEntityChunks->FromMemory.pBuffer,
        session->context->pRequest->pEntityChunks->FromMemory.BufferLength,
        searchStr, 
        strlen(searchStr));
    if (pos) {
        char *start = pos + strlen(searchStr) + 4;
        char *end   = strstr(start, "\r\n");
        int size    = (int) (end - start);
        char *tmp   = mkMalloc(session->heapHandle, size+2, __FILE__, __LINE__);
        strncpy_s(tmp, size + 2, start, size);
        tmp[size]   = '\0';
        value       = tmp;
    }
    mkFree(session->heapHandle, searchStr);
    return value;
}
