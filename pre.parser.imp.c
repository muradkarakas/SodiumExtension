#include "stdafx.h"

#include "pre.parser.imp.h"

 
bool 
ParseSQLXFile(
    SodiumSession *session, 
    HTSQLPage *page, 
    wchar_t *filePath
) 
{
	bool retVal;
    yyscan_t scanner;
   
	mkDebug(session, SESSION_LOG_DETAIL_SESSION_EVENTS, "\n", "\nSQLX file PRE parsing....", filePath, NULL);
	increaseDebugIndentionCount(session);
	
	initDebugInfoFileProperty(session, filePath);

    FILE *mkSourceFile = _wfsopen(filePath, L"r", _SH_DENYNO);

    if (mkSourceFile==NULL) {
        /** File does not exists */
        char path[MAX_PATH];
        wcstombs_s(NULL, path, MAX_PATH, filePath, MAX_PATH);
		CLOG(ERR_SQLX_FILE_NOT_FOUND, path);
		retVal = false;
    } else {

		prelex_init_extra(session, &scanner);
        preset_in(mkSourceFile, scanner);

        void *pParser = preParseAlloc (malloc);

        TokenPre token;
        page->stopParsing = false;

        do {
            token.tokenCode      = prelex(scanner);
            token.tokenStrLength = preget_leng(scanner);
            token.tokenStr       = mkMalloc(session->heapHandle, token.tokenStrLength+1, __FILE__, __LINE__);
            strncpy_s(token.tokenStr, token.tokenStrLength + 1, preget_text(scanner), token.tokenStrLength + 1);
            preParse(pParser, token.tokenCode, token, session);

        } while (token.tokenCode != PRE_END_OF_FILE && page->stopParsing == false);

        if (page->stopParsing == false) {
            preParse(pParser, 0, token, session);
        }

        preParseFree(pParser, free );
        prelex_destroy(scanner);
        fclose(mkSourceFile);

		retVal = true;
    }

	decreaseDebugIndentionCount(session);
    mkDebug(session, SESSION_LOG_DETAIL_SESSION_EVENTS, "\n", "SQLX file PRE parsing....done.\n" , NULL);
	
	return retVal;
}


void preTokenDestructor(SodiumSession *session, TokenPre t) {
	mkFree(session->heapHandle, t.tokenStr);
	t.tokenStr = NULL;
}

void stopPreParserWithMessage(SodiumSession *session, HTSQLPage *page, const char *message, const char *reason) {
    page->stopParsing = true;
    __sendErrorResponse(session, page, message, reason);
}

