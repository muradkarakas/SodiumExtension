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
    char *pos = strstr(session->context->RequestBody, searchStr);
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
    char *pos = strstr(session->context->RequestBody, searchStr);
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
    char *pos = strstr(session->context->RequestBody, searchStr);
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

char *getFormDataBig(SodiumSession *session, const char *boundry, const char *formInputName) 
{
    char* requestBody = session->context->RequestBody;
    ULONG requestBodySize = session->context->RequestBodySize;
    char* pos = NULL;
    char *value = NULL;
    char *searchStr = mkStrcat(session->heapHandle, __FILE__, __LINE__,
        "Content-Disposition: form-data; name=\"", formInputName, "\"",
        NULL
    );
    
    // First, looking if whole request body read or not previously
    if (requestBody) {
        pos = mkMemmem(
            requestBody,
            requestBodySize,
            searchStr, 
            strlen(searchStr));
    }
    
    if (pos == NULL) {
        
        // not found in "header content type string"
        // searching in "header content body"
        if (session->context->pRequest->Headers.KnownHeaders[HttpHeaderContentLength].pRawValue) {
            int size = session->context->pRequest->Headers.KnownHeaders[HttpHeaderContentLength].RawValueLength;
            
            // getting request header body size
            char* contentLengthSize = mkMalloc(session->heapHandle, size + sizeof(char), __FILE__, __LINE__);
            memcpy_s(contentLengthSize, size, session->context->pRequest->Headers.KnownHeaders[HttpHeaderContentLength].pRawValue, size);
            int HeaderSize = atoi(contentLengthSize);
            mkFree(session->heapHandle, contentLengthSize);

            // getting whole request body
            session->context->RequestBody = mkMalloc(session->heapHandle, HeaderSize, __FILE__, __LINE__);
            ULONG  result = HttpReceiveRequestEntityBody(
                session->context->hReqQueue,
                session->context->pRequest->RequestId,
                HTTP_RECEIVE_REQUEST_ENTITY_BODY_FLAG_FILL_BUFFER,
                session->context->RequestBody,
                HeaderSize,
                &session->context->RequestBodySize,
                NULL
                );

            if (session->context->RequestBodySize > 0) {
                requestBody = session->context->RequestBody;
                requestBodySize = session->context->RequestBodySize;
            }
            
        } 
        else if (session->context->pRequest->pEntityChunks) {
            
            // last chance to look in entity chunks received
            if (session->context->pRequest->pEntityChunks->FromMemory.BufferLength > 0) {
                requestBody = session->context->pRequest->pEntityChunks->FromMemory.pBuffer;
                requestBodySize = session->context->pRequest->pEntityChunks->FromMemory.BufferLength;
            }
        }

        // searching for 'formInputName' in whole request content body
        if (requestBody) {
            pos = mkMemmem( requestBody,
                            requestBodySize,
                            searchStr,
                            strlen(searchStr));
        }
    }

    // if 'formInputName' parameter is found in request, we are searching for value
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
