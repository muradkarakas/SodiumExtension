#include "pch.h"
#include "../SodiumServer/LoadExtensions.h"


DWORD 
SendHttpForbiddenResponse(
	HttpContext* context
)
{
	HTTP_RESPONSE   response;
	DWORD           result;
	char* szBuffer = NULL;

	//
	// Initialize the HTTP response structure.
	//
	INITIALIZE_HTTP_RESPONSE(&response, 307, "Temporary Redirect");

	ADD_KNOWN_HEADER(response, HttpHeaderLocation, "/Sodium-Site/builtin-pages/403.html");

	result =
		HttpSendHttpResponse(
			context->hReqQueue,           // ReqQueueHandle
			context->pRequest->RequestId, // Request ID
			HTTP_SEND_RESPONSE_FLAG_DISCONNECT,
			&response,       // HTTP response
			NULL,            // pReserved1
			NULL,      // bytes sent-optional
			NULL,            // pReserved2
			0,               // Reserved3
			NULL,            // LPOVERLAPPED
			NULL             // pReserved4
		);
	/*
	// 
	// Add file referance
	//
	HANDLE fileHandle = CreateFileW(context->StatusCode_403_Forbidden_Path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

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
	*/
	return result;
}