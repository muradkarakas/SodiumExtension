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

#include "mkutils.h"

#include "response.h"


void mkPrint(SodiumSession *session, const char *first, ...) {
	size_t lSize;
	HTSQLPage *page = GetCurrentPage(session);
	if (!page) {
		return;
	}
	va_list vl;

	char *str = (char*) first;
	va_start(vl, first);

	while (str) {
		if (page->isJavaScriptEscapeEnabled) {
			char *quoteStr = mkEscapeJavaScriptQuoteCharacter(session, str);
			lSize = strnlen_s(quoteStr, 1024000);
			responseWriteByte(session, page, quoteStr, lSize);
			mkFree(session->heapHandle, quoteStr);
		}
		else {
			lSize = strnlen_s(str, 1024000);
			responseWriteByte(session, page, str, lSize);
		}
		str = va_arg(vl, char*);
	};

	va_end(vl);
}

void mkDebug(SodiumSession *session, SESSION_LOG_DETAIL pEventType, const char *first, ...) {

	DEBUG_SECTION_START

		if (session) {
			if (!(session->SessionLogDetail & pEventType)) {
				return;
			}
		}
		if (mkLogFile == NULL) {
			return;
		}
		va_list vl;

		EnterCriticalSection(&htsqlCriticalSection);
	
		const char *str = first;
		va_start(vl, first);

		if (str[0] == '\n' && strlen(str) == sizeof(char) && session != NULL) {
			fwrite("\n", sizeof(char), 1, mkLogFile);
			char space[250] = "                                                                                                                               ";
			fwrite(space, sizeof(char) * 2, session->currentFunctionCallDepth + session->debugIndentionCount, mkLogFile);
			str = va_arg(vl, char*);
		}

		while (str) {
			if (str[0] != '\0') {
				fwrite(str, strlen(str), 1, mkLogFile);
			}
			else {
				// Nothing written to the log file for empty string but continuing in loop
			}		
			str = va_arg(vl, char*);
		};

		int logFileHandle = _fileno(mkLogFile);
		fflush(mkLogFile);
		_commit(logFileHandle);
	
		LeaveCriticalSection(&htsqlCriticalSection);

	DEBUG_SECTION_END
}



char *mkUTF8index(SodiumSession *session, char *source, size_t pos) {
    ++pos;
    for (; *source; ++source) {
        if ((*source & 0xC0) != 0x80)
            --pos;
        if (pos == 0)
            return source;
    }
    return NULL;
}



char* getItemNameOfExpression(SodiumSession *session, const char* exp) {
    char *pointPos = strchr(exp, '.');
    char *itemname = mkStrdup(session->heapHandle, &pointPos[1], __FILE__, __LINE__);
    return itemname;
}

char* getDataBlockNameOfExpression(SodiumSession *session, const char* exp) {
    char *pointPos = strchr(exp, '.');

    int ind = (int) ( (pointPos - exp) * sizeof(char));

    char *blockname = mkMalloc(session->heapHandle, ind, __FILE__, __LINE__);
    strncpy_s(blockname, ind, &exp[1], ind-1);
    blockname[ind-1] = '\0';

    return blockname;
}

void 
BindSQLVariables(
	SodiumSession * session, 
	HTSQLPage * page, 
	TAGDataBlock * pTAGDataBlock, 
	DBInt_Statement * stm, 
	SQL_COMMAND_TYPE commandType
)
{	
	size_t paramNumber = 1;
	char paramNumberStr[4];

	TAGInput *input = pTAGDataBlock->rootTAGInput;
	for (; input; input = input->next) {
        
		if (input->columnName == NULL ||		// if column name is empty, we are not adding the column into insert statement since it is a formula column. 
			(input->inputType != INPUT_TAG_TYPE_IMAGE && input->value == NULL) // All input items except images must have a  value 
			) {
			continue;
		}

        if (input->valueState == INPUT_TAG_VALUE_DIRTY ||
            (commandType == SQL_COMMAND_TYPE_INSERT && (input->masterItemName || input->sequenceName))
            ) {
			
			DBInt_Connection *conn = pTAGDataBlock->tagConnection->mkDBConnection;
			char *tmpBindVarName = NULL;

			switch (conn->dbType) {
				case SODIUM_ORACLE_SUPPORT: {
					tmpBindVarName = mkStrcat(session->heapHandle, __FILE__, __LINE__, ":", input->name, NULL); 
					break;
				}
				case SODIUM_POSTGRESQL_SUPPORT: {
					mkItoa(paramNumber, paramNumberStr);
					tmpBindVarName = mkStrcat(session->heapHandle, __FILE__, __LINE__, "$", paramNumberStr, NULL);					
					paramNumber++;
					break;
				}
				case SODIUM_MYSQL_SUPPORT: {
					mkItoa(paramNumber, paramNumberStr);
					tmpBindVarName = mkStrcat(session->heapHandle, __FILE__, __LINE__, "?", input->columnName, NULL);					
					paramNumber++;
					break;
				}
				case SODIUM_SQLSERVER_SUPPORT: {
					mkItoa(paramNumber, paramNumberStr);
					tmpBindVarName = mkStrcat(session->heapHandle, __FILE__, __LINE__, "?", NULL);					
					paramNumber++;
					break;
				}
			}

            switch(input->inputType) {
				case INPUT_TAG_TYPE_HIDDEN:
				case INPUT_TAG_TYPE_SELECT:
                case INPUT_TAG_TYPE_PASSWORD:
                case INPUT_TAG_TYPE_TEXT: {                        
                    switch(input->dataType) {
                        case INPUT_TAG_DATA_TYPE_DATE:
                        case INPUT_TAG_DATA_TYPE_CHAR: {
							//	ODBC C library requires parameter index to bind parameter value.
							//	That is, ODBC does not use named parameter variable for SELECT, INSERT and UPDATE statement, 
							//	so we send the index of parameter
							DBInt_BindString(	conn,
												stm, 
												(conn->dbType != SODIUM_SQLSERVER_SUPPORT) ? tmpBindVarName : paramNumberStr,
												input->value, 
												(input->value) ? strlen(input->value):0);
                            break;
                        }
						case INPUT_TAG_DATA_TYPE_NUMBER: {
							//	ODBC C library requires parameter index to bind parameter value.
							//	That is, ODBC does not use named parameter variable for SELECT, INSERT and UPDATE statement, 
							//	so we send the index of parameter
							DBInt_BindNumber(	conn,
												stm, 
												(conn->dbType != SODIUM_SQLSERVER_SUPPORT) ? tmpBindVarName : paramNumberStr,
												input->value, 
												(input->value) ? strlen(input->value):0);
                            break;
                        }
                    }
                    break;
                }
                case INPUT_TAG_TYPE_IMAGE: {
					char cFileName[MAX_PATH] = "";
					wcstombs_s(NULL, cFileName, MAX_PATH, input->imageFileName, wcslen(input->imageFileName));
					DBInt_BindLob(conn,
									stm,
									cFileName,
									tmpBindVarName);
                    break;						
				}
            }

			// getting error status
			if (conn->errText) {
				CLOG(ERR_DB_UNKNOWN_ERROR, conn->errText);
			}
			else {
				mkDebug(session, SESSION_LOG_DETAIL_DML, "\n", "Binded : '", tmpBindVarName, "' = '", input->value, "'", NULL);
			}

			// free resources
			if (tmpBindVarName) {
				mkFree(session->heapHandle, tmpBindVarName);
			}
        }
        
    }

}


char* mkEscapeJavaScriptQuoteCharacter(SodiumSession *session, const char *javaScriptCode) {
    if (javaScriptCode == NULL) {
        return NULL;
    }
    int destPos = 0;
    int sourcePos = 0;
	size_t len = strlen(javaScriptCode);
	char* newStr = NULL;
	if (len == 0) {
		newStr = mkMalloc(session->heapHandle, 2, __FILE__, __LINE__);
		newStr[0] = '\0';
	}
	else {
		newStr = mkMalloc(session->heapHandle, len * 2, __FILE__, __LINE__);
	}
	
    while(javaScriptCode[sourcePos] != '\0' ) {

        if (javaScriptCode[sourcePos] != '"' && javaScriptCode[sourcePos] != '\n' && javaScriptCode[sourcePos] != '\\') {
            newStr[destPos] = javaScriptCode[sourcePos];
            destPos++;
        } else {
            if (javaScriptCode[sourcePos] == '"') {
                newStr[destPos++] = '\\';
                newStr[destPos++] = '"';
            }
            if (javaScriptCode[sourcePos] == '\n') {
                newStr[destPos++] = '\\';
                newStr[destPos++] = 'n';
            }
			if (javaScriptCode[sourcePos] == '\\') {
				newStr[destPos++] = '\\';
				newStr[destPos++] = '\\';
			}
        }

        sourcePos++;
    }
    return newStr;
} 


/** \brief Convert int to char *
 *
 * \param value int
 * \param result char*
 * \return char*
 *
 *  Result will be put into result variable. size of the result variable should have enough to accommodate.
 *  No memory allocation is made in the mkItoa function.
 */
char* mkItoa(size_t value, char* result) {

    int base = 10;

    // check that the base if valid
    if (base < 2 || base > 36) { *result = '\0'; return result; }

    char* ptr = result, *ptr1 = result, tmp_char;
	size_t tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}

/** \brief Convert double to char *
 *
 * \param value int         Must be 50 byte long or bigger in size
 * \param result char*
 * \return char*
 *
 *  Result will be put into result variable. size of the result variable should have enough to accommodate.
 *  No memory allocation is made in the mkItoa function.
 */
char* mkDoubleToChar(double value, char* result) {
    int ret = snprintf(result, 50, "%g", value);
    if (ret < 0) {
        strcpy_s(result, 6, "ERROR");
    }
    if (ret > 50) {
        strcpy_s(result, 6, "ERROR");
    }
    return result;
}

void mkConvertFloatToChar(size_t pFloatValue, char *pStrBuffer, int pSizeOfStrBufer) {
    //int ret = snprintf(pStrBuffer, pSizeOfStrBufer, "%.4f", pFloatValue);
	int ret = snprintf(pStrBuffer, pSizeOfStrBufer, "%f", pFloatValue);
    if (ret < 0) {
        strcpy_s(pStrBuffer, 6, "ERROR");
    }
    if (ret > pSizeOfStrBufer) {
        strcpy_s(pStrBuffer, 6, "ERROR");
    }
}

void mkAssert(char *first, va_list argp) {
    mkDebug(NULL, SESSION_LOG_DETAIL_ASSERT, "File:", __FILE__, "Line:", __LINE__, first, argp, NULL);
    assert(1==0);
}

char *
GetCurrentCSSFileName(
	SodiumSession *session
) {
	size_t length = strlen(session->context->pRequest->pRawUrl);
	size_t memSize = (length + 1) * sizeof(char);
	char* cssFileName = mkMalloc(session->heapHandle, memSize, __FILE__, __LINE__);
	
	char *fileNamePos = strstr(session->context->pRequest->pRawUrl, ".frmx");
	if (fileNamePos) {
		size_t len = fileNamePos - session->context->pRequest->pRawUrl;
		memcpy_s(cssFileName, len, session->context->pRequest->pRawUrl, len);
		memcpy_s(cssFileName+len, 4, ".css", 4);
		char* restOfUrl = &(session->context->pRequest->pRawUrl[len + 5]);
		if (restOfUrl != NULL && restOfUrl[0] != '\0') {
			size_t sizeofRestOfUrl = strlen(restOfUrl);
			memcpy_s(cssFileName+len+4, sizeofRestOfUrl, restOfUrl, sizeofRestOfUrl);
		}
	}
	return cssFileName;
}

wchar_t *
CalculateCurrentSQLXFileName(
	SodiumSession *session
) 
{
    wchar_t *filePath = NULL;
    size_t size		= wcslen(session->context->Current_Full_Path);
	size_t memSize = (size + 1) * sizeof(wchar_t);
	filePath = mkMalloc(session->heapHandle, memSize, __FILE__, __LINE__);
	wcsncpy_s(filePath, size+1, session->context->Current_Full_Path, size);
    size -= 4;
	filePath[size++] = L's';
	filePath[size++] = L'q';
	filePath[size++] = L'l';
	filePath[size++] = L'x';
	
	return filePath;
}


wchar_t *
GetPhysicalPathForFile(
	SodiumSession *session, 
	const char *fileRelativePath
) 
{
	wchar_t	*physicalPath = NULL;
	wchar_t *currentPhysicalPath = getCurrentDirectory(session);
	if (currentPhysicalPath) {
		int memSize = MAX_PATH * sizeof(wchar_t);
		physicalPath = mkMalloc(session->heapHandle, memSize, __FILE__, __LINE__);
		
		wcsncpy_s(physicalPath, MAX_PATH, currentPhysicalPath, MAX_PATH);

		wchar_t temp[MAX_PATH] = L"";
		mbstowcs_s(NULL, temp, MAX_PATH, fileRelativePath, strlen(fileRelativePath));

		wcscat_s(physicalPath, MAX_PATH, temp);
	}
	return physicalPath;
}

wchar_t *getCurrentDirectory(SodiumSession *session) {
    /*char *logFileName   = NULL;
	size_t size            = strlen(session->lpECB->lpszPathTranslated);
    if (size > 0) {
        logFileName         = mkMalloc(session->heapHandle, size+3, __FILE__, __LINE__);
        strcpy_s(logFileName, size + 3, session->lpECB->lpszPathTranslated);
		size_t tmpFilePathSize = strlen(logFileName);
        while(logFileName[tmpFilePathSize] != '\\' && tmpFilePathSize >= 0) {
            tmpFilePathSize--;
        }
        logFileName[tmpFilePathSize] = '\0';
    }
    return logFileName;*/
	return session->context->Current_Directory;
}



void mkStrReplaceQuotation(char *pStr) {
	size_t strLen = strlen(pStr);
    for(int i=0; i<strLen; i++) {
        if (pStr[i] == '"') {
            pStr[i] = '\'';
        }
    }
}

void mkStrReplaceTab(char *pStr) {
	if (pStr == NULL) {
		return;
	}
	size_t strLen = strlen(pStr);
	for (int i = 0; i<strLen; i++) {
		if (pStr[i] == '\t') {
			pStr[i] = ' ';
		}
	}
}

int mkStrCountEnter(char *pStr) {
	int count = 0;
	size_t orjSize = strnlen_s(pStr, 1000);

	for (int i = 0; i < orjSize; i++) {
		if (pStr[i] == '\n') {
			count++;
		}
	}

	return count;
}

void mkStrSanitize(char *pStr) {
	int pos=0;
	size_t orjSize = strnlen_s(pStr, 1000);
	for (int i = 0; i < orjSize; i++) {
		switch(pStr[i]) {
			case '(':
			case ')':
			case ' ':
			case '\r':
			case '\n':
			case ':':
			case '\\':
			case '/':
			case '\t': {
				continue;
			}
			default: {
				pStr[pos] = pStr[i];
				pos++;
				continue;
			}
		}
	}
	pStr[pos] = '\0';
}

void mkStrReplaceEnter(char *pStr) {
	if (pStr == NULL) {
		return;
	}
	size_t strLen = strlen(pStr);
	for (int i = 0; i<strLen; i++) {
		if (pStr[i] == '\n') {
			pStr[i] = ' ';
		}
		if (pStr[i] == '\r') {
			pStr[i] = ' ';
		}
	}
}

unsigned long mkHash(unsigned char *str) {
    unsigned long hash = 5381;
    int c = *str;
    while (c) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
        str++;
        c = *str;
    }
    return hash;
}


void 
SendFileContent(
	SodiumSession *session, 
	HTSQLPage *page, 
	const wchar_t *fileName
) 
{
    FILE *file = NULL;
    //fopen_s(&file, fileName, "rb");
	errno_t err = _wfopen_s(&file, fileName, L"rb");
    if (file) {
        fseek(file, 0L, SEEK_END);
        size_t fileSize = ftell(file);
        void *fileBuf = mkMalloc(session->heapHandle, fileSize, __FILE__, __LINE__);
        if (fseek(file, 0L, SEEK_SET) == 0) {
            fileSize = fread(fileBuf, 1, fileSize, file);
			//writeRawData(session, (void*) fileBuf, fileSize);
			responseWriteByte(session, page, fileBuf, fileSize);
        }
        mkFree(session->heapHandle, fileBuf);
		fclose(file);
    }
}

char * 
stripTAGPropertyValue(
	char *pPropertyValue
) 
{
    if (pPropertyValue[strlen(pPropertyValue)-1] == '"') {
        pPropertyValue[strlen(pPropertyValue)-1] = '\0';
    }
    if (pPropertyValue[0] == '"') {
		size_t len = strlen(pPropertyValue);
        strcpy_s(pPropertyValue, len, (char*) &(pPropertyValue[1]));
    }
    return pPropertyValue;
}

wchar_t * 
stripTAGPropertyValueW(
	wchar_t *pPropertyValue
) 
{
    if (pPropertyValue[wcslen(pPropertyValue)-1] == L'"') {
        pPropertyValue[wcslen(pPropertyValue)-1] = L'\0';
    }
    if (pPropertyValue[0] == L'"') {
		size_t len = wcslen(pPropertyValue);
		wcsncpy_s(pPropertyValue, len, (wchar_t*) &(pPropertyValue[1]), len);
        //strcpy_s(pPropertyValue, len, (char*) &(pPropertyValue[1]));
    }
    return pPropertyValue;
}

void initializeRandomNumberGenerator() {
	time_t t;
	srand((unsigned)time(&t));
}

char *mkStrRandom(SodiumSession *session, int strLength, const char *sourceFile, int sourceLine) {
	size_t index;
	// session id should not contain 0 as a first character. so I deleted it from the character list. 
	// if you add 0 and coinsdentally it will be the first character, then debugger command's "attach" MAY NOT find the session 
    static const char charset[] =   "123456789";
    char *dest = mkMalloc(session->heapHandle, strLength+2, sourceFile, sourceLine);
    int pos = 0;
    while (strLength-- > 0) {
		index = (size_t) (rand() % (sizeof(charset) - 1));
        *(dest+pos) = charset[index];
        pos++;
    }
    *(dest+pos) = '\0';
    return dest;
}


/* Converts a hex character to its integer value */
char from_hex(char ch) {
  return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

/* Converts an integer value to its hex character*/
char to_hex(char code) {
  static char hex[] = "0123456789abcdef";
  return hex[code & 15];
}

/* Returns a url-encoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
char *url_encode(SodiumSession *session, const char *str) {

	const char  *pstr = str;
	char	*buf = mkMalloc(session->heapHandle, strlen(str) * 3 + 1, __FILE__, __LINE__),
			*pbuf = buf;

	while (*pstr) {
		if (isalnum(*pstr) || *pstr == '-' || *pstr == '_' || *pstr == '.' || *pstr == '~')
			*pbuf++ = *pstr;
		else if (*pstr == ' ')
			*pbuf++ = '+';
		else
			*pbuf++ = '%', *pbuf++ = to_hex(*pstr >> 4), *pbuf++ = to_hex(*pstr & 15);
		pstr++;
	}
	*pbuf = '\0';
	return buf;
}

/* Returns a url-decoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
char *url_decode(SodiumSession *session, const char *str) {

    if (str == NULL) {
        return NULL;
    }

    const char *pstr = str;
    char *buf = mkMalloc(session->heapHandle, strlen(str) + 1, __FILE__, __LINE__);
    char *pbuf = buf;

  while (*pstr) {
    if (*pstr == '%') {
      if (pstr[1] && pstr[2]) {
        *pbuf++ = from_hex(pstr[1]) << 4 | from_hex(pstr[2]);
        pstr += 2;
      }
    } else if (*pstr == '+') {
      *pbuf++ = ' ';
    } else {
      *pbuf++ = *pstr;
    }
    pstr++;
  }
  *pbuf = '\0';
  return buf;
}
