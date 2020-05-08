#include "pch.h"



void
HandleFrmxRequest(
	HttpContext* context
)
{
	InitHttpResponse(context);

	/** Create or get the http session instance*/
	SodiumSession* session = CreateOrGetSession(context);

	/** Destroy Idle sessions */
	destroyIdleSessions();

	// Before go, waiting for the termination of the previous http request of the same session  
	HANDLE currentRequestMutextHandle = mkWaitTillPreviousRequestOfTheCurrentSessionEnds(session);

	// Determine the request type and dispatch it to the correct handler 
	if (session->sessionReadyState != SESSION_READY_STATE_KILLED) {
		dispatchRequest(session);
	}

	if (session->sessionReadyState == SESSION_READY_STATE_INIT) {
		destroySession(session);
		mkFree(mkApplicationHeapHandle, session);
	}
	
	// Request handling done 
	ReleaseMutex(currentRequestMutextHandle);
}


SodiumSession*
CreateOrGetSession(
	HttpContext* context
)
{
	SodiumSession* session = NULL;

	HTTP_KNOWN_HEADER cookieHeader = context->pRequest->Headers.KnownHeaders[HttpHeaderCookie];
	if (cookieHeader.RawValueLength == 0) {
		// New user
		session = CreateSodiumSession(context, NULL);
		char cookie[100] = "";
		strcpy_s(cookie, 100, "SodiumSessionCookie=");
		strcat_s(cookie, 100, session->id);
		strcat_s(cookie, 100, ";");
		ADD_KNOWN_HEADER(context->response, HttpHeaderSetCookie, cookie);
	}
	else {
		// Known user
		session = GetSodiumSession(context, cookieHeader);
	}

	// For each request, we must init call stack
	InitCallstack(session);
	
	ULONG result = HttpSendHttpResponse(
		context->hReqQueue,           // ReqQueueHandle
		context->pRequest->RequestId, // Request ID
		HTTP_SEND_RESPONSE_FLAG_MORE_DATA,                   // Flags
		&context->response,           // HTTP response
		NULL,                // pReserved1
		NULL,          // bytes sent  (OPTIONAL)
		NULL,                // pReserved2  (must be NULL)
		0,                   // Reserved3   (must be 0)
		NULL,                // LPOVERLAPPED(OPTIONAL)
		NULL                 // pReserved4  (must be NULL)
	);

	return session;
}

