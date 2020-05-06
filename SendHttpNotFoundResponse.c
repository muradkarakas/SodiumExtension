#include "stdafx.h"
#include "../SodiumServer/LoadExtensions.h"


DWORD
SendHttpNotFoundResponse(
	HttpContext* context
)
{
	HTTP_RESPONSE   response;
	HTTP_DATA_CHUNK dataChunk;
	DWORD           result;
	DWORD           bytesSent;
	char* szBuffer = NULL;

	//
	// Initialize the HTTP response structure.
	//
	INITIALIZE_HTTP_RESPONSE(&response, 200, "OK");

	ADD_KNOWN_HEADER(response, HttpHeaderContentType, "text/html");

	result =
		HttpSendHttpResponse(
			context->hReqQueue,           // ReqQueueHandle
			context->pRequest->RequestId, // Request ID
			HTTP_SEND_RESPONSE_FLAG_MORE_DATA,
			&response,       // HTTP response
			NULL,            // pReserved1
			NULL,      // bytes sent-optional
			NULL,            // pReserved2
			0,               // Reserved3
			NULL,            // LPOVERLAPPED
			NULL             // pReserved4
		);
	
	//
	// Add file referance
	//
	wchar_t page404[MAX_PATH] = L"";
	wcsncpy_s(page404, MAX_PATH, context->Current_Path, MAX_PATH);
	wcscat_s(page404, MAX_PATH, L"\\Sodium-Site\\builtin-pages\\404.html");
	HANDLE fileHandle = CreateFileW(page404, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

	dataChunk.DataChunkType = HttpDataChunkFromFileHandle;
	dataChunk.FromFileHandle.ByteRange.StartingOffset.QuadPart = 0;
	dataChunk.FromFileHandle.ByteRange.Length.QuadPart = HTTP_BYTE_RANGE_TO_EOF;
	dataChunk.FromFileHandle.FileHandle = fileHandle;

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