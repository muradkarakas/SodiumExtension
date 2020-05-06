#include "stdafx.h"

#include "dispatcher.h"


#include "response.h"
#include "dispatcherBig.h"
#include "dispatcherSmall.h"
#include "yuarel.h"

void 
dispatchRequest(
    SodiumSession *session
)
{
    CreateResponse(session);

    HTSQLPage *page = FindHTSQLPageByName(session, session->context->Current_Full_Path);

    if (!page) {
        page = CreateHTSQLPage(session, session->context->Current_Full_Path);
        AddHTSQLPage(session, page);
    }

	InitCurrentPage(session, page);
    SetHttpVerb(session, page);
    SetRequestDispatcherType(session, page);

    ParseQueryStringParams(session, page);

    if (page->currentRequestDispatcherType == REQUEST_DISPATCHER_TYPE_BIG) {
        DispatchBigRequest(session, page);
    } else {
        dispatchSmallRequest(session, page);
    }
        
    if (session->sessionReadyState != SESSION_READY_STATE_KILLED) {
		DestroyResponse(session);
        FreeQueryStringParams(session, page);
	}
	
    session->lastActivityTime = clock();
}

const char *
GetQueryStringParam(
    SodiumSession *session, 
    HTSQLPage *page, 
    const char *paramName
) 
{
    if (page->totalHTTPFormVariableCount == 0) {
        return NULL;
    }
    for(int i=0; i < page->totalHTTPFormVariableCount; i++) {
        if (page->formDataStructureSmall[i].key) {
            if (strcmp(page->formDataStructureSmall[i].key, paramName) == 0) {
                return page->formDataStructureSmall[i].decodedVal;
            }
		}
		else {
			break;
		}
    }
    return NULL;
}

void 
ParseQueryStringParams(
    SodiumSession * session,
    HTSQLPage *page
) 
{
    /* Free previous call's settings */
    FreeQueryStringParams(session, page);

    char* queryString = NULL;
    char* sanitizedQueryString = NULL;

    int size = MAX_ACCEPTABLE_POSTED_FORM_ELEMENT_COUNT * sizeof(struct yuarel_param);
    page->formDataStructureSmall = mkMalloc(session->heapHandle, size, __FILE__, __LINE__);

    //if (page->currentRequestMethodType == REQUEST_TYPE_GET) {
        if (session->context->pRequest->CookedUrl.pQueryString != 0) {
            size_t qLen = (wcslen(session->context->pRequest->CookedUrl.pQueryString) + 1) * sizeof(char);
            queryString = mkMalloc(session->heapHandle, qLen, __FILE__, __LINE__);
            wcstombs_s(NULL, queryString, qLen, session->context->pRequest->CookedUrl.pQueryString, qLen);
            sanitizedQueryString = &(queryString[1]);
        }
    //}
    //else if (page->currentRequestMethodType == REQUEST_TYPE_POST)  {
        if (session->context->pRequest->pEntityChunks != NULL) {
            if (session->context->pRequest->pEntityChunks->FromMemory.BufferLength > 0) {
                size_t qLen = session->context->pRequest->pEntityChunks->FromMemory.BufferLength + 1;
                queryString = mkMalloc(session->heapHandle, qLen, __FILE__, __LINE__);
                strcpy_s(queryString, qLen, session->context->pRequest->pEntityChunks->FromMemory.pBuffer);
                sanitizedQueryString = queryString;
            }
        }
    //}

    page->totalHTTPFormVariableCount = yuarel_parse_query(sanitizedQueryString,
        '&',
        (struct yuarel_param*) & (page->formDataStructureSmall[page->totalHTTPFormVariableCount]),
        MAX_ACCEPTABLE_POSTED_FORM_ELEMENT_COUNT);

    if (page->totalHTTPFormVariableCount > 0) {
        /* yuarel_parse_query returns -1 if not found URL */
        for (int index = 0; index < page->totalHTTPFormVariableCount; index++) {
            page->formDataStructureSmall[index].decodedKey = url_decode(session, page->formDataStructureSmall[index].key);
            page->formDataStructureSmall[index].decodedVal = url_decode(session, page->formDataStructureSmall[index].val);
        }
    }
    else {
        page->totalHTTPFormVariableCount = 0;
    }
    //size = page->totalHTTPFormVariableCount * sizeof(struct yuarel_param);
    //mkReAlloc(session->heapHandle, page->formDataStructureSmall, size);
}

void 
FreeQueryStringParams(
    SodiumSession *session, 
    HTSQLPage *page
) 
{
	if (page == NULL) {
		return;
	}
    if (page->formDataStructureSmall) {
        for(int index = 0; index < page->totalHTTPFormVariableCount; index++) {
                if (page->formDataStructureSmall[index].decodedKey) {
                    mkFree(session->heapHandle, page->formDataStructureSmall[index].decodedKey);
                    page->formDataStructureSmall[index].decodedKey = NULL;
                }
                if (page->formDataStructureSmall[index].decodedVal) {
                    mkFree(session->heapHandle, page->formDataStructureSmall[index].decodedVal);
                    page->formDataStructureSmall[index].decodedVal = NULL;
                }
        }
        mkFree(session->heapHandle, page->formDataStructureSmall);
        page->formDataStructureSmall = NULL;
    }
    if (page->formDataBufferSmallGET) {
        mkFree(session->heapHandle, page->formDataBufferSmallGET);
        page->formDataBufferSmallGET    = NULL;
    }
    page->totalHTTPFormVariableCount = 0;
}


void 
setRequestedFileName(
    SodiumSession *currentMKSession, 
    HTSQLPage *page
)
{
    if (page->currentRequestMethodType == REQUEST_TYPE_GET) {
         if (page->fileNameRequested) {
            mkFree(currentMKSession->heapHandle, page->fileNameRequested);
            page->fileNameRequested = NULL;
        }
        page->fileNameRequested = mkStrdupW(currentMKSession->heapHandle, currentMKSession->context->Current_Full_Path, __FILE__, __LINE__);
    } else {
        if (page->fileNameRequested && currentMKSession->lpECB->lpszPathInfo) {
            int cmpResult = wcscmp(page->fileNameRequested, currentMKSession->context->Current_Full_Path);
            if (cmpResult != 0) {
                    // Is someone hacking ?
                //char *a = "jjj";
                //mkDebug(currentMKSession, 0, "kk", NULL);
            }
        }
    }
}

const char *header1 = "multipart/form-data;";
const char *header2 = "Content-Disposition: form-data;";
const int headerSize1 = 20;
const int headerSize2 = 31;

void 
SetRequestDispatcherType(
    SodiumSession *session, 
    HTSQLPage *page
) 
{
    page->currentRequestDispatcherType = REQUEST_DISPATCHER_TYPE_SMALL;
    if (session->context->pRequest->EntityChunkCount > 0) {
        const char* requestBody = (char*)session->context->pRequest->pEntityChunks->FromMemory.pBuffer;
        int pos;
        pos = strncmp(requestBody, header1, headerSize1);
        if (pos == 0) {
            page->currentRequestDispatcherType = REQUEST_DISPATCHER_TYPE_BIG;
        }
        pos = strncmp(requestBody, header2, headerSize2);
        if (pos == 0) {
            page->currentRequestDispatcherType = REQUEST_DISPATCHER_TYPE_BIG;
        }
    }

    /*if (session->context->pRequest->Headers.KnownHeaders[HttpHeaderContentType].RawValueLength > 0) {
        char* context = session->context->pRequest->Headers.KnownHeaders[HttpHeaderContentType].pRawValue;
        int pos = strncmp(context, header, headerSize);
        if (pos == 0) {
            page->currentRequestDispatcherType = REQUEST_DISPATCHER_TYPE_BIG;
        }
    }*/

}

void 
SetHttpVerb(
    SodiumSession *session, 
    HTSQLPage *page
) 
{    
    if (session->context->pRequest->Verb == HttpVerbPOST) {
        page->currentRequestMethodType = REQUEST_TYPE_POST;
    } else if (session->context->pRequest->Verb == HttpVerbGET) {
        page->currentRequestMethodType = REQUEST_TYPE_GET;
    }
}
