#include "stdafx.h"

#include "html.parser.imp.h"

#include "response.h"
#include "lemon.html.h"
#include "tagDataBlock.h"
#include "tagGeneric.h"

void htmlTokenDestructor(SodiumSession *session, TokenHtml token) {
    HTSQLPage *page = GetCurrentPage(session);
    if (page->bInDataBlock == 0) {
		if (token.tokenStrLength > 0 && token.tokenStr[0] != '\0') {
			responseWriteByte(session, page, token.tokenStr, token.tokenStrLength);
		}
    } else { 
        /*
         *  We have already created TAGInput object instance when we encounter TAG_INPUT_OPEN in production rule.
         *  Then creating new Generic objects for its properties, between ">" and "</input>" since
         *  these values will be generated and printed to the client while processing DataBlock dynamically.
         *  This rule only applies the input elements in tbody inside data block
         */
		if (page->bInTable > 0 || token.tokenCode == TAG_TABLE_OPEN) {

			if (
				token.tokenCode == TAG_INPUT_OPEN || token.tokenCode == TAG_SELECT_OPEN || token.tokenCode == TAG_TEXTAREA_OPEN || 
				// Input tags out of tbody can be parsed as a normal html text.
				(page->bInInput == 0 || (page->bInInput == 1 && page->bInTableBody == 0) ) ) {

				// we are creating a linked list which will be used printing of whole datablock content. 
				// Static parts will be merged into a single node in the linked list. However, each dynamic part will be a single node in the linked list.
				TAGDataBlock *tagDataBlock = getCurrentTAGDataBlock(session, page);
				if (tagDataBlock) {
					char *str = getCreateNTString(session, token);
					TAGGeneric *tmp = createOrMergeTAGGeneric(session, page, tagDataBlock, token.tokenCode, str);
					mkFree(session->heapHandle, str);
				}				
			}

		}
    }
}



bool 
ParseFRMXFile(
	SodiumSession *session, 
	HTSQLPage *page
) 
{
    yyscan_t scanner;

	//initDebugInfoFileProperty(session, session->lpECB->lpszPathTranslated);
	FILE* mkSourceFile = _wfsopen(session->context->Current_Full_Path, L"r", _SH_DENYNO);
	char fUri[MAX_PATH] = "";
	wcstombs_s(NULL, fUri, MAX_PATH, session->context->Current_Full_Path, MAX_PATH);

    if (mkSourceFile == NULL) {
		char fUri[MAX_PATH] = "";
		wcstombs_s(NULL, fUri, MAX_PATH, session->context->Current_Full_Path, MAX_PATH);
		CLOG(ERR_FRMX_FILE_NOT_FOUND, fUri);
		return false;
	}
	mkDebug(session, SESSION_LOG_DETAIL_SESSION_EVENTS, 
		"\n", 
		"\nFRMX file parsing....", 
		fUri,
		".", NULL);
	
	increaseDebugIndentionCount(session);

    session->sessionReadyState     = SESSION_READY_STATE_IN_PROGRESS;
	 
    //htmllex_init(&scanner);
	htmllex_init_extra(session, &scanner);
    htmlset_in(mkSourceFile, scanner); 

    void *pParser = htmlParseAlloc (malloc);
    TokenHtml token;
    
    do {
        token.tokenCode         = htmllex(scanner);
        token.tokenStrLength    = htmlget_leng(scanner);
		token.tokenStr = htmlget_text(scanner);
		htmlParse(pParser, token.tokenCode, token, session);
		if (token.tokenCode == TAG_HTML_BLOCK_CLOSE) { 
			TokenHtml tokenReducer;
			tokenReducer.tokenCode = SPACE;
			tokenReducer.tokenStr = "  ";
			tokenReducer.tokenStrLength = 2;
			htmlParse(pParser, tokenReducer.tokenCode, tokenReducer, session);
		}
    } while (token.tokenCode != HTML_END_OF_FILE && page->stopParsing == false);

    if (page->stopParsing == false) {
		token.tokenCode = 0;
		token.tokenStr = "";
		token.tokenStrLength = 0;
        htmlParse(pParser, 0, token, session);
    }
    page->stopParsing = false;

    htmlParseFree(pParser, free );
    htmllex_destroy(scanner);
    fclose(mkSourceFile);
    session->sessionReadyState     = SESSION_READY_STATE_READY;
	decreaseDebugIndentionCount(session);
	mkDebug(session, SESSION_LOG_DETAIL_SESSION_EVENTS, "\n", "FRMX file parsing....done\n", NULL);
	return true;
}

void stopHTMLParserWithMessage(SodiumSession *session, HTSQLPage *page, const char *message, const char *reason) {
    page->stopParsing = true;
    __sendErrorResponse(session, page, message, reason);
}

char *getCreateNTString(SodiumSession *session, TokenHtml token) {
	if (token.tokenStrLength > 0) {
		char *ntStr = mkMalloc(session->heapHandle, token.tokenStrLength + 1, __FILE__, __LINE__);
		strncpy_s(ntStr, token.tokenStrLength + 1, token.tokenStr, token.tokenStrLength);
		ntStr[token.tokenStrLength] = '\0';
		return ntStr;
	}
	return NULL;
}

wchar_t *
getCreateNTStringW(
	SodiumSession *session, 
	TokenHtml token
) 
{
	if (token.tokenStrLength > 0) {
		int memSize = (token.tokenStrLength + 1) * sizeof(wchar_t);
		wchar_t *ntStr = mkMalloc(session->heapHandle, memSize, __FILE__, __LINE__);
		mbstowcs_s(NULL, ntStr, token.tokenStrLength + 1, token.tokenStr, token.tokenStrLength);
		//wcsncpy_s(ntStr, token.tokenStrLength + 1, token.tokenStr, token.tokenStrLength);
		//strncpy_s(ntStr, token.tokenStrLength + 1, token.tokenStr, token.tokenStrLength);
		ntStr[token.tokenStrLength] = L'\0';
		return ntStr;
	}
	return NULL;
}
