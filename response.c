#include "stdafx.h"

#include "response.h"


void
InitHttpResponse(
	HttpContext* context
) 
{
	/* Setting http response status */
	INITIALIZE_HTTP_RESPONSE(&context->response, 200, "OK");

	/*	Setting default http header */
	ADD_KNOWN_HEADER(context->response, HttpHeaderConnection, "keep-alive");
	//ADD_KNOWN_HEADER(context->response, HttpHeaderPragma, "no-cache");
}

ULONG
FlushHttpResponse(
	HttpContext* context
)
{
	HTTP_DATA_CHUNK dataChunk;
	DWORD           result;
	DWORD           bytesSent;
	char*			szBuffer = NULL;

	dataChunk.DataChunkType = HttpDataChunkFromMemory;
	dataChunk.FromMemory.pBuffer = " ";
	dataChunk.FromMemory.BufferLength = 1;

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

	return result;
}


void	responseWriteChars(SodiumSession *session, HTSQLPage *page, char *startStr, ...) {
	
	size_t lSize;
	va_list vl;

	char *str = startStr;
	va_start(vl, startStr);

	while (str) {
		lSize = strnlen_s(str, 1024000);
		responseWriteByte(session, page, str, lSize);
		str = va_arg(vl, char*);
	};

	va_end(vl);
}

/*	
		-	if "allocatedSize" > 0:			USE BUFFER, SEND LATER

			-	if size of the parameter is less than 1024;
			1.	append the value of the parameter into to "responseBuffer" if it has enough unused space to accomadate
			2.	if buffer is full or does not have enough unused space left, increase of the "responseBuffer"  size.
			then repeat step 1.

			-	if size of the parameter is greater than 1024;
			increase of the "responseBuffer"  size then repeat step 1 above.
*/
void	responseWriteByte(SodiumSession *session, HTSQLPage *page, void *pBuffer, size_t pSize) {
	if (pBuffer == NULL || pSize == 0) {
		return;
	}
	HTSQLResponse *response = session->response;
	if (response) {
		//	if "allocatedSize" is 0 or "response->buffer" is null send parameter to the client
		if (response->allocatedSize == 0 || response->buffer == NULL) {
			_responseFlush(session, page, pBuffer, pSize);
		}
		else {
			size_t minSizeRequred = response->usedSize + pSize + sizeof(char);
			if (minSizeRequred > response->allocatedSize) {
				// current buffer size is not enough to accomodate new string
				size_t newSizeAllocated = minSizeRequred + HTTP_RESPONSE_BUFFER_CHUNK_SIZE;
				response->buffer = mkReAlloc(session->heapHandle, response->buffer, newSizeAllocated);
				response->allocatedSize = newSizeAllocated;
			}
			void* dest = (char*)response->buffer + response->usedSize;
			memcpy_s(dest, response->allocatedSize, pBuffer, pSize);
			response->usedSize += pSize;
		}
		
	}	
}


/*
-	calls "_responseFLush"
-	clears the buffer (puts '\0' into the first byte)
-	sets "usedSize" to 0
*/
void responseFlush(SodiumSession *session, HTSQLPage *page) {
	HTSQLResponse *response = session->response;
	if (response) {
		_responseFlush(session, page, response->buffer, response->usedSize);
	}
}

DWORD
__SendHttpResponse(
	HttpContext* context,
	char * szBuffer,
	ULONG size
)
{
	HTTP_DATA_CHUNK dataChunk;
	DWORD           result;
	DWORD           bytesSent;
	
	// 
	// Add an entity chunk.
	//
	dataChunk.DataChunkType = HttpDataChunkFromMemory;
	dataChunk.FromMemory.pBuffer = szBuffer;
	dataChunk.FromMemory.BufferLength = size;

	context->response.EntityChunkCount = 1;
	context->response.pEntityChunks = &dataChunk;
	
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

	return result;
}

/*
This is the lowest level function
-	sends the content of the responseBuffer to the client if it is not empty.
*/
void	_responseFlush(SodiumSession *session, HTSQLPage *page, void *buffer, size_t size) {
	if (buffer == NULL || size <= 0) {
		return;
	}
	if (session->whatIsPrinted == SESSION_PRINT_NOTHING) {
		CLOG(ERR_CORE_RESPONSE_HEADER_NOT_PRINTED_YET, "");
	}
	else {
		// Before sending content to the client, conent tpye must should been set to a value other than SESSION_PRINT_NOTHING.
		//session->lpECB->WriteClient(session->lpECB->ConnID, buffer, (DWORD*) &size, 0);
		__SendHttpResponse(session->context, buffer, (ULONG)size);
	}
}

void	responseDisableBuffering(SodiumSession *session, HTSQLPage *page) {
	HTSQLResponse *response = session->response;
	if (response) {
		// assert is buffer is set before
		if (response->buffer) {
			mkFree(session->heapHandle, response->buffer);
			response->buffer = 0;
		}
		response->allocatedSize = 0;
		response->usedSize = 0;
	}
}

void 
ResponseEnablingBuffering(
	SodiumSession *session 
)
{
	HTSQLResponse *response = session->response;
	if (response) {
		// assert is buffer is set before
		if (response->buffer) {
			mkCoreDebug(__FILE__, __LINE__, "daha once buffer enable edildi", NULL);
			assert(1 == 1);
		}
		response->buffer = mkMalloc(session->heapHandle, HTTP_RESPONSE_BUFFER_CHUNK_SIZE, __FILE__, __LINE__);
		response->allocatedSize = HTTP_RESPONSE_BUFFER_CHUNK_SIZE;
		response->usedSize = 0;
	}
}

void flushEnablingBuffering(SodiumSession *session, HTSQLPage *page) {
	HTSQLResponse *response = session->response;
	if (response) {
		if (response->buffer) {
			_responseFlush(session, page, response->buffer, response->usedSize);
			mkFree(session->heapHandle, response->buffer);
		}
		response->buffer = NULL;
		response->allocatedSize = 0;
		response->usedSize = 0;
	}
}

void 
CreateResponse(
	SodiumSession *session
) 
{
	HTSQLResponse *response = mkMalloc(session->heapHandle, sizeof(HTSQLResponse), __FILE__, __LINE__);
	response->buffer = NULL;
	response->allocatedSize = 0;
	response->usedSize = 0;
	session->response = response;

	ResponseEnablingBuffering(session);
}

void 
DestroyResponse(
	SodiumSession *session
	) 
{
	HTSQLResponse *response = session->response;
	
	flushEnablingBuffering(session, session->currentPage);

	if (response) {
		mkFree(session->heapHandle, response);
		response = NULL;
	}
	session->response = NULL;
}