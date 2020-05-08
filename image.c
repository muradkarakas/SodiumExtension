#include "pch.h"

#include "image.h"

#include "dispatcherBig.h"
#include "tagConnection.h"
#include "tagInput.h"
#include "tagDataBlock.h"
#include "memmem.h"
#include "dispatcher.h"
#include "response.h"
#include "tagDataBlockCurrentRow.h"

void __sendPutImageResponse(SodiumSession *session, HTSQLPage *page, const char *boundry) {

	char *datablockname = getFormDataBig(session, boundry, "datablockname");
	if (datablockname == NULL) {
		return;
	}

	TAGDataBlock *dataBlockInstance = findTAGDataBlockByName(session, page, datablockname);
	if (dataBlockInstance == NULL) {
		CLOG(ERR_DATABLOCK_NOT_FOUND, datablockname);
	}
	else { 
		mkDebug(session,
			SESSION_LOG_DETAIL_HTSQL_ITEM,
			"\n",
			"Image item modified... (", dataBlockInstance->keyColumnValue, ") '",
			NULL);

		char *inputName = getFormDataBig(session, boundry, "input-name");
		char *rowid = getFormDataBig(session, boundry, "rowid");
		char *imageFileName = getBinaryDataFileName(session, boundry, "filename");

		if (imageFileName && imageFileName[0] != '\0') {
			char *imageContentType = getBinaryDataContentType(session, boundry, "filename");

			/** Write blob stream to file */
			int writenFileSize = WritePostedBinaryDataToFile(session, page, boundry, "filename", datablockname, inputName);
			if (writenFileSize > 0) {
				char *numBuffer = mkMalloc(session->heapHandle, 50, __FILE__, __LINE__);
				mkItoa(writenFileSize, numBuffer);
				modifyTAGDatablockCurrentRowItemValue(session, page, datablockname, inputName, numBuffer);
				mkFree(session->heapHandle, numBuffer);
				
				/** send everything is ok message */
				__sendPutImageSuccessfulMessage(session, imageFileName, writenFileSize);
			}

			/** clean up */
			mkFree(session->heapHandle, imageContentType);
		}

		/** clean up */
		mkFree(session->heapHandle, datablockname);
		mkFree(session->heapHandle, inputName);
		mkFree(session->heapHandle, rowid);
		mkFree(session->heapHandle, imageFileName);
	
		mkDebug(session, SESSION_LOG_DETAIL_HTSQL_ITEM, "\n", "Done.", NULL);
	}

}





int 
WritePostedBinaryDataToFile(
	SodiumSession *session, 
	HTSQLPage *page, 
	const char *boundry, 
	const char *imageInputName, 
	const char *datablockname, 
	const char *inputName
) 
{
	ULONG result = NO_ERROR;
	ULONG BytesReturned = 0;
    int  writenFileSize = 0;
	
	TAGDataBlock *tagDataBlock = findTAGDataBlockByName(session, page, datablockname);
	TAGInput *tagInput = findTAGInputByName(session, page, tagDataBlock, inputName);
	
	if (tagDataBlock && tagInput) {
		
		/** File opening */
		FILE* imgFile = NULL; // _wfopen(tagInput->imageFileName, L"wb");
		errno_t err = _wfopen_s(&imgFile, tagInput->imageFileName, L"wb");
		char* pEntityBuffer = getBinaryDataStartPosition(session, boundry, imageInputName);
		/** First chunk locating ...*/
		char* start = session->context->pRequest->pEntityChunks->FromMemory.pBuffer;
		int  headSize = ((int)(pEntityBuffer - start));
		int dataSize = session->context->pRequest->pEntityChunks->FromMemory.BufferLength - headSize;

		char *boundryPtr = mkMemmem(pEntityBuffer, dataSize, boundry, strlen(boundry));


		/* writing first chunk */
		fwrite(pEntityBuffer, dataSize, 1, imgFile);
			   
		if (imgFile) {
			/** Allocating temp buffer */
			DWORD EntityBufferLength = 1024 * 1024;
			pEntityBuffer = mkMalloc(session->heapHandle, EntityBufferLength, __FILE__, __LINE__);

			if (pEntityBuffer != NULL) {

				/* reading data  */
				while (result == NO_ERROR) {

					// Read Http Server buffer
					result = HttpReceiveRequestEntityBody(
						session->context->hReqQueue,
						session->context->pRequest->RequestId,
						HTTP_RECEIVE_REQUEST_ENTITY_BODY_FLAG_FILL_BUFFER,
						pEntityBuffer,
						EntityBufferLength,
						&BytesReturned,
						NULL
					);

					if (BytesReturned > 0 && result != ERROR_HANDLE_EOF) {
						/* writing buffer to file */
						fwrite(pEntityBuffer, BytesReturned, 1, imgFile);
						writenFileSize += BytesReturned;
					}
				}

				fflush(imgFile);

				/* releasing buffer */
				mkFree(session->heapHandle, pEntityBuffer);
			}
		}

		/* closing file handle*/
        fclose(imgFile);

        tagInput->imageFileSize = writenFileSize;
    }

    return writenFileSize;
}


void __sendPutImageSuccessfulMessage(SodiumSession *session, const char *fileName, int writenFileSize) {
    char *sizeBuf = mkMalloc(session->heapHandle, 50, __FILE__, __LINE__);
    mkItoa(writenFileSize, sizeBuf);
	char *lFileName = mkEscapeJavaScriptQuoteCharacter(session, fileName);
    char *lastResponse = mkStrcat(session->heapHandle,
                                  __FILE__, __LINE__,
                                  "action = { "
                                                "\"type\"       : \"putImageSuccessful\", "
                                                "\"fileName\"   : \"", (lFileName) ? lFileName :"", "\", "
                                                "\"fileSize\"   : \"", sizeBuf, "\" "
                                          " };",
                                  // standart part
                                  "addNewActionToTheResponseActionList(action);",
                                  NULL);
    mkPrint(session, lastResponse, NULL);
	if (lFileName) {
		mkFree(session->heapHandle, lFileName);
	}
    mkFree(session->heapHandle, lastResponse);
    mkFree(session->heapHandle, sizeBuf);
}

void serveAsGetRequestGetImage(SodiumSession *session, HTSQLPage *page) {
	const char *blockname            = GetQueryStringParam(session, page, "datablockname");
	const char *itemnane             = GetQueryStringParam(session, page, "itemname");
    const char *rowid                = GetQueryStringParam(session, page, "rowid");

    TAGDataBlock *tagDataBlock = findTAGDataBlockByName(session, page, blockname);
    if (tagDataBlock) {

        mkDebug(session, SESSION_LOG_DETAIL_DML, "\nImage is reading ", NULL);

        TAGInput     *tagInput     = findTAGInputByName(session, page, tagDataBlock, itemnane);
        if (tagInput) {
			
			char *sql = NULL;
			TAGConnection *tagConnection = findTAGConnectionByName(session, page, tagDataBlock->connectionName, true);
			
			switch (tagConnection->mkDBConnection->dbType) {
				case SODIUM_ORACLE_SUPPORT: {
					sql = mkStrcat(session->heapHandle, __FILE__, __LINE__,
						" select ",
						tagInput->columnName,
						" from ",
						(tagDataBlock->dataSchemaName) ? tagDataBlock->dataSchemaName : "",
						(tagDataBlock->dataSchemaName) ? "." : "",
						tagDataBlock->dataSourceName,
						" where "
						"rowid = '", rowid, "'",
						NULL
					);
					break;
				}
				case SODIUM_POSTGRESQL_SUPPORT: {
					sql = mkStrcat(session->heapHandle, __FILE__, __LINE__,
						" select ",
						tagInput->columnName,
						" from ",
						(tagDataBlock->dataSchemaName) ? tagDataBlock->dataSchemaName : "",
						(tagDataBlock->dataSchemaName) ? "." : "",
						tagDataBlock->dataSourceName,
						" where "
						"oid = ", rowid, " ",
						NULL
					);
					break;
				}
			}
           
			DBInt_Statement *stm = DBInt_CreateStatement(tagConnection->mkDBConnection);
			DBInt_Prepare(tagConnection->mkDBConnection, stm, sql);

			if (tagConnection->mkDBConnection->errText == NULL) {

				DBInt_ExecuteSelectStatement(tagConnection->mkDBConnection, stm, sql);

				if (tagConnection->mkDBConnection->errText == NULL) {

					if (!DBInt_IsEof(tagConnection->mkDBConnection, stm)) {
						DWORD numOfBytes;

						void *lobContent = DBInt_GetLob(tagConnection->mkDBConnection, stm, tagInput->columnName, &numOfBytes, false); // (page->currentRequestMethodType == REQUEST_TYPE_GET)
						if (tagConnection->mkDBConnection->errText) {
							sendHTSQLNoImageFileContentWithErrorMessage(session, page, tagConnection->mkDBConnection->errText);
						}
						if (numOfBytes > 0 && lobContent != NULL) {
							/*char *numOfBytesStr = mkMalloc(session->heapHandle, 20, __FILE__, __LINE__);
							mkItoa(numOfBytes, numOfBytesStr);
							ADD_KNOWN_HEADER(session->context->response, HttpHeaderContentLength, numOfBytesStr);
							ADD_KNOWN_HEADER(session->context->response, HttpHeaderContentEncoding, "gzip");*/
							responseWriteByte(session, page, lobContent, numOfBytes);
							mkFree(session->heapHandle, lobContent);
						}
						else {
							sendHTSQLNoImageFileContentWithErrorMessage(session, page, "");
						}
					}
				}
				else {
					sendHTSQLNoImageFileContentWithErrorMessage(session, page, tagConnection->mkDBConnection->errText);
				}
			}
			else {
				sendHTSQLNoImageFileContentWithErrorMessage(session, page, tagConnection->mkDBConnection->errText);
			}
            
			DBInt_FreeStatement(tagConnection->mkDBConnection, stm);
            mkFree(session->heapHandle, sql);
        }
        mkDebug(session, SESSION_LOG_DETAIL_DML, "\ndone.", NULL);
    }
}

void sendHTSQLNoImageFileContentWithErrorMessage(SodiumSession *session, HTSQLPage *page, const char *errorMessage) {

	wchar_t *imgPath = GetPhysicalPathForFile(session, "htsql_noimage.png");
	SendFileContent(session, page, imgPath);
	mkFree(session->heapHandle, imgPath);

	if (errorMessage && errorMessage[0] != '\0') {
		mkDebug(session, SESSION_LOG_DETAIL_ASSERT, errorMessage, NULL);
	}
}

