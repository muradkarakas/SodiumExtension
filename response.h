/*
	USAGE: 
	In order to use response buffering, call the functions below sequentially.

	Step.1:	Enable buffering 
	responseEnablingBuffering(currentMKSession, page);

	Step.2:	Call mkPrint now. All printed values will be append to the buffer instead of sending to the client	
	mkPrint(.....

	Step.2: Send buffer content to the client or use it for different purposes.
	responseFlush(currentMKSession, page);
	
	Step.3: Stop buffering
	responseDisableBuffering(currentMKSession, page);

*/

#pragma once

#include "htsql.h"


void
InitHttpResponse(
	HttpContext* context
);

ULONG
FlushHttpResponse(
	HttpContext* context
);

void	
CreateResponse(
	SodiumSession *session
);

void	
DestroyResponse(
	SodiumSession *session
);


/* This function;
	-	should be called just before each HTTP request 
	-	does not flush the current buffer. Deveolper should call "responseFlush" if developer want to send response to the client.
*/
void	
ResponseEnablingBuffering(
	SodiumSession *session
);

/*	-	Check the value of the "allocatedSize"
		+	if it is 0, do nothing,
		+	if it is > 0, calls "responseFlush"
	-	Set the "allocatedSize" to 0
*/
void	responseDisableBuffering(SodiumSession *currentMKSession, HTSQLPage *page);


/*	-	calls "responseWriteByte" for each string parameter until encounter to NULL 
*/
void	responseWriteChars(SodiumSession *currentMKSession, HTSQLPage *page, char *startStr, ...);


/*	-	if "allocatedSize" is 0:		DO NOT USE BUFFER, SEND NOW
		calls "__responseFlush" function

	-	if "allocatedSize" > 0:			USE BUFFER, SEND LATER
		
			-	if size of the parameter is less than 1024;
				1.	append the value of the parameter into to "responseBuffer" if it has enough unused space to accomadate
				2.	if buffer is full or does not have enough unused space left, increase of the "responseBuffer"  size.
					then repeat step 1.

			-	if size of the parameter is greater than 1024;	
				increase of the "responseBuffer"  size then repeat step 1 above.
*/
void	responseWriteByte(SodiumSession *currentMKSession, HTSQLPage *page, void *pBuffer, size_t pSize);

/*
	Sends the buffer content to the client and clear buffers
*/
void	responseFlush(SodiumSession *currentMKSession, HTSQLPage *page);

/*
		This is the lowest level function 
		-	sends the content of the responseBuffer to the client if it is not empty.
*/
void	_responseFlush(SodiumSession *currentMKSession, HTSQLPage *page, void *buffer, size_t size);

