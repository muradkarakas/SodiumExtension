#pragma once

#include "stdafx.h"
#include "../SodiumServer/LoadExtensions.h"


void
GetWindowsErrorMessageText(
	wchar_t* buf, 
	int bufSize);


#define			MAX_ULONG_STR			((ULONG) sizeof("4294967295"))

#define			SHOW_GET_LAST_ERROR_MESSAGE(str)	{													\
														wchar_t errStr[MAX_PATH];						\
														GetWindowsErrorMessageText(errStr, MAX_PATH);	\
														wprintf(str, errStr);							\
													}

#define ADD_KNOWN_HEADER(Response, HeaderId, RawValue)               \
    do                                                               \
    {                                                                \
        (Response).Headers.KnownHeaders[(HeaderId)].pRawValue =      \
                                                          (RawValue);\
        (Response).Headers.KnownHeaders[(HeaderId)].RawValueLength = \
            (USHORT) strlen(RawValue);                               \
    } while(FALSE)

#define INITIALIZE_HTTP_RESPONSE( resp, status, reason )    \
    do                                                      \
    {                                                       \
        RtlZeroMemory( (resp), sizeof(*(resp)) );           \
        (resp)->StatusCode = (status);                      \
        (resp)->pReason = (reason);                         \
        (resp)->ReasonLength = (USHORT) strlen(reason);     \
    } while (FALSE)

#define ALLOC_MEM(cb) HeapAlloc(GetProcessHeap(), 0, (cb))

#define FREE_MEM(ptr) HeapFree(GetProcessHeap(), 0, (ptr))

DWORD
SendHttpResponse(
	HttpContext * context,
	IN PSTR          pEntityString
);

DWORD	SendHttpResponse1(
	IN HANDLE        hReqQueue,
	IN PHTTP_REQUEST pRequest,
	IN USHORT        StatusCode,
	IN PSTR          pReason,
	IN PSTR          pEntity
);

DWORD	SendHttpJsonResponse(
	IN HANDLE        hReqQueue,
	IN PHTTP_REQUEST pRequest,
	IN USHORT        StatusCode,
	IN PSTR          pContent);

DWORD	SendHttpFileResponse(
	HttpContext		*context,
	IN LPWSTR		 pFilePath
);

DWORD	SendHttpPostResponse(
	IN HANDLE        hReqQueue,
	IN PHTTP_REQUEST pRequest
);

DWORD
SendHttpForbiddenResponse(
	HttpContext		*context
);

DWORD
SendHttpNotFoundResponse(
	HttpContext * context
);

