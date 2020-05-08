#include "pch.h"

DWORD
SendHttpJsonResponse(
	IN HANDLE        hReqQueue,
	IN PHTTP_REQUEST pRequest,
	IN USHORT        StatusCode,
	IN PSTR          pContent)
{
	HTTP_RESPONSE   response;
	HTTP_DATA_CHUNK dataChunk;
	DWORD           result;
	DWORD           bytesSent;

	//
	// Add a known header.
	//
	ADD_KNOWN_HEADER(response, HttpHeaderContentType, "application/json");

	if (pContent == NULL) {
		result = ERROR_INVALID_PARAMETER;
		goto Failed;
	}

	// 
	// Add an entity chunk.
	//
	dataChunk.DataChunkType = HttpDataChunkFromMemory;
	dataChunk.FromMemory.pBuffer = pContent;
	dataChunk.FromMemory.BufferLength = (ULONG)strlen(pContent);

	response.EntityChunkCount = 1;
	response.pEntityChunks = &dataChunk;


	// 
	// Because the entity body is sent in one call, it is not
	// required to specify the Content-Length.
	//

	result = HttpSendHttpResponse(
		hReqQueue,           // ReqQueueHandle
		pRequest->RequestId, // Request ID
		0,                   // Flags
		&response,           // HTTP response
		NULL,                // pReserved1
		&bytesSent,          // bytes sent  (OPTIONAL)
		NULL,                // pReserved2  (must be NULL)
		0,                   // Reserved3   (must be 0)
		NULL,                // LPOVERLAPPED(OPTIONAL)
		NULL                 // pReserved4  (must be NULL)
	);

	if (result != NO_ERROR) {
		wprintf(L"\nSendHttpJsonResponse failed with %lu \n", result);
	}

Failed:

	return result;
}