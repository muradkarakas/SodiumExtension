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

HTSQLPage * 
CreateHTSQLPage(
    SodiumSession *session, 
    wchar_t *pageUri
) 
{
    HTSQLPage *currentPage = mkMalloc(session->heapHandle, sizeof(HTSQLPage), __FILE__, __LINE__);

    currentPage->prev                  = NULL;
    currentPage->next                  = NULL;

    currentPage->fileNameRequested     = mkStrdupW(session->heapHandle, pageUri, __FILE__, __LINE__);

    currentPage->bInTable              = 0;
    currentPage->bInTableHead          = 0;
    currentPage->bInTableBody          = 0;
    currentPage->bInTableFoot          = 0;
    currentPage->bInTableTr            = 0;
    currentPage->bInDataBlock          = 0;
    currentPage->bInTableTd            = 0;
    currentPage->bInTableTh            = 0;
    currentPage->bInInput              = 0;

    currentPage->rootCBVariable        = NULL;
    currentPage->currentCBVariable     = NULL;

    currentPage->rootTAGDatalist       = NULL;
    currentPage->currentTAGDatalist    = NULL;

    currentPage->rootTAGDataBlock      = NULL;
    currentPage->currentTAGDataBlock   = NULL;

    currentPage->rootPreObjectGeneric  = NULL;
    currentPage->lastPreObjectGeneric  = NULL;

    currentPage->currentCallStack		= NULL;

    currentPage->currentScriptType     = NULL;
    currentPage->currentScriptFile     = NULL;

    currentPage->stopParsing           = false;

    currentPage->currentDatablockName  = NULL;
    currentPage->currentRowId          = NULL;

    currentPage->clientMessage         = NULL;

    currentPage->ifCounter             = 0;

    currentPage->isJavaScriptEscapeEnabled = false;

    currentPage->formDataStructureSmall     = NULL;
    currentPage->totalHTTPFormVariableCount = 0;
    currentPage->formDataBufferSmallGET     = NULL;

	strncpy_s(currentPage->nextCommandStrToBeExecuted, MAX_FUNCTION_NAME_LENGTH, "...", MAX_FUNCTION_NAME_LENGTH);
    return currentPage;
}

void 
AddHTSQLPage(
    SodiumSession *session, 
    HTSQLPage *page
) 
{
    if (session->rootPage == NULL) {
        session->rootPage  = page;
    } else {
        page->prev         = session->lastPage;
        session->lastPage->next = page;
    }
    session->lastPage  = page;
    session->currentPage = page;
}

int
GetPageCount(
    SodiumSession* session
) 
{
    int i = 0;
    HTSQLPage *page = session->rootPage;
    while(page) {
        i++;
        page = page->next;
    }
    return i;
}

int getPageCurrentLineNumberToBeExecuted(SodiumSession *currentMKSession, HTSQLPage *page) {
	return page->currentCallStack->preObjectFunction->lineNumberOuter + page->currentCallStack->functionDebugInfo->lineNumberInner;
}

HTSQLPage *
FindHTSQLPageByName(
    SodiumSession *session, 
    wchar_t* pageURI
) 
{
    if (!session) {
        return NULL;
    }
    HTSQLPage *page = session->rootPage;
    while(page) {
        if (wcscmp(page->fileNameRequested, pageURI) == 0) {
            return page;
        }
        page = page->next;
    }
    return NULL;
}

HTSQLPage *
GetCurrentPage(
    SodiumSession *session
) 
{
	if (session->currentPage) {
		return session->currentPage;
	}
    HTSQLPage *page = FindHTSQLPageByName(session, session->context->Current_Full_Path);
    return page;
}

void 
InitCurrentPage(
    SodiumSession *session, 
    HTSQLPage *page
) 
{
    session->currentPage = page;

	/** Reset HTSQL user function environment before processing each POST request */
	if (page->currentCallStack) {
		CLOG(ERR_CORE_MEMORY_MANAGEMENT_FAILURE, "page->currentCallStack must be empty");
	}
	page->currentCallStack = NULL;
}