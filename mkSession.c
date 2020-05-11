#include "pch.h"

#include "mkSession.h"


#include "mkapplication.h"
#include "tagDataBlock.h"
#include "tagDatalist.h"
#include "dispatcher.h"
#include "pre.parser.imp.h"
#include "preObjectVariable.h"
#include "preObjectGeneric.h"
#include "htsqlVariable.h"
#include "htsqlFunction.imp.h"
#include "sessionEnd.h"
#include "yuarel.h"
#include "tagConnection.h"
#include "response.h"
#include "showPage.h"
#include "breakpoints.imp.h"

SodiumSession* 
GetSodiumSession(
    HttpContext* context,
    HTTP_KNOWN_HEADER cookieHeader
)
{
    SodiumSession* session = NULL;
    char* sid = cookieHeader.pRawValue;
    char* cmpResult = strstr(sid, "SodiumSessionCookie=");
    if (cmpResult != NULL) {
        sid = &sid[20];
        // SID must be 5 characters long
        sid[5] = '\0';
        session = FindSodiumSessionById(sid);
        if (session == NULL) {
            session = CreateSodiumSession(context, sid);
        }
    }
    return session;
}


SodiumSession *
CreateSodiumSession(
    HttpContext *context,
    char *sid
) 
{
    SodiumSession* session = HeapAlloc(mkApplicationHeapHandle, HEAP_ZERO_MEMORY, sizeof(SodiumSession));
    session->context = context;
    session->heapHandle = HeapCreate(HEAP_ZERO_MEMORY, 2048, 0);

    session->id = mkStrRandom(session, SESSION_ID_LENGTH, __FILE__, __LINE__);
    if (sid != NULL)
        strcpy_s(session->id, SESSION_ID_LENGTH+2, sid);

    /* Initializing session variables */
    session->sessionReadyState = SESSION_READY_STATE_INIT;
    session->whatIsPrinted = SESSION_PRINT_NOTHING;
    session->sessionCreationTime = clock();
    session->debugIndentionCount = 0;
    session->languageCode = LANGUAGE_CODE_EN;
    session->SessionLogDetail = SESSION_LOG_DETAIL_ALL; //SESSION_LOG_DETAIL_HTSQL_FUNCTION_CALL | SESSION_LOG_DETAIL_HTSQL_VARIABLE;
    session->next = NULL;
    session->prev = NULL;
    session->rootTAGConnection = NULL;
    session->currentTAGConnection = NULL;
    session->activeTAGConnection = NULL;
    session->rootPage = NULL;
    session->lastPage = NULL;
    session->theme = NULL;
    session->rootSessionObjectGeneric = NULL;
    session->currentSessionObjectGeneric = NULL;
    session->pageProcessTime = 0;
    session->sessionDebugInfo = NULL;
    ResetDebugData(session);
    clearAllBreakpoints(session);
    if (getMKSessionRootInstance() == NULL) {
        setMKSessionRootInstance(session);
    }
    else {
        addMKSessionInstance(session);
    }
    mkDebug(session, SESSION_LOG_DETAIL_SESSION_EVENTS, "\n", "Session created with id: ", session->id, NULL);
    initUserSessionDefaults(session);

    // We are loading controller.sqlx file here because 
    //	1.	for each session we must load it once
    //  2.	Instead of checking if it is loaded or not for each request, just load it here and get rid of check 
    LoadControllerFile(session);

    return session;
}

void 
ResetDebugData(
    SodiumSession *session
) 
{
	if (session) { 
		if (session->sessionDebugInfo == NULL) {
			session->sessionDebugInfo = mkMalloc(session->heapHandle, sizeof(Session_Debug_Info), __FILE__, __LINE__);
			session->sessionDebugInfo->lineNumberOuter = 1;
			session->sessionDebugInfo->fileURI[0] = L'\0';
			session->sessionDebugInfo->sessionDebugStepType = DEBUG_STEP_TYPE_STEP_CONTUNIE;
		}		
	} 
}

void initDebugInfoFileProperty(SodiumSession *session, wchar_t *fileURI) {
	ResetDebugData(session);

	if (session) {
		wcsncpy_s(session->sessionDebugInfo->fileURI, MAX_PATH, fileURI, MAX_PATH);    // strcpy_s(session->sessionDebugInfo->fileURI, MAX_PATH, fileURI);
		session->sessionDebugInfo->lineNumberOuter = 1;
		session->sessionDebugInfo->sessionDebugStepType = DEBUG_STEP_TYPE_STEP_CONTUNIE;
	}
}

void decreaseDebugIndentionCount(SodiumSession *session) {
	DEBUG_SECTION_START
		if (session) {
			if (session->debugIndentionCount > 0) {
				session->debugIndentionCount--;
			}
			else {
				assert(1 == 1);
			}
		}
	DEBUG_SECTION_END
}

void increaseDebugIndentionCount(SodiumSession *session) {
	DEBUG_SECTION_START
		if (session) {
			session->debugIndentionCount++;
		}
	DEBUG_SECTION_END
}

char *getThemeHeader(SodiumSession *session) {
	char *themeHeader = NULL;

	if (session->theme) {
		themeHeader = mkStrcat(session->heapHandle, __FILE__, __LINE__, stdHeadTAGContent,
			"<link rel=\"stylesheet\" type=\"text/css\" href=\"", RESOURCE_FOLDER, "css/next-level-", session->theme, ".css\"/>\n", NULL);
	}
	else {
		themeHeader = mkStrcat(session->heapHandle, __FILE__, __LINE__, stdHeadTAGContent,
			"<link rel=\"stylesheet\" type=\"text/css\" href=\"", RESOURCE_FOLDER, "css/next-level-default.css\"/>\n", NULL);
	}
		
	return themeHeader;
}

void setTheme(SodiumSession *session, const char *theme) {
	if (session->theme) { 
		mkFree(session->heapHandle, session->theme);
	}
	session->theme = mkStrdup(session->heapHandle, theme, __FILE__, __LINE__);
}

void
LoadControllerFile(
    SodiumSession *session
) 
{
    HTSQLPage *sessionRootPage = CreateHTSQLPage(session, L"controller.frmx");
    AddHTSQLPage(session, sessionRootPage);
	const wchar_t *curDir = getCurrentDirectory(session);
    wchar_t path[MAX_PATH] = L"";
    wcsncpy_s(path, MAX_PATH, curDir, MAX_PATH);
    wcscat_s(path, MAX_PATH, L"controller.sqlx");
	bool isFound = ParseSQLXFile(session, sessionRootPage, path);
	if (isFound == false) {

	}
}


void initUserSessionDefaults(SodiumSession *session) {
    const char *user            = ":Session.user";
    const char *password        = ":Session.password";
    const char *authenticated   = ":Session.authenticated";

    PreObjectVariable *userObject = createPreObjectVariableVarchar(session, PREOBJECTTYPE_VARIABLE_VARCHAR, user, NULL);
    createSessionGenericObject(session, user, POST_OBJECT_GENERIC_TYPE_VARIABLE, userObject);

    PreObjectVariable *passwordObject = createPreObjectVariableVarchar(session, PREOBJECTTYPE_VARIABLE_VARCHAR, password, NULL);
    createSessionGenericObject(session, password, POST_OBJECT_GENERIC_TYPE_VARIABLE, passwordObject);

    PreObjectVariable *userAuthenticated = createPreObjectVariableVarchar(session, PREOBJECTTYPE_VARIABLE_VARCHAR, authenticated, "false");
    createSessionGenericObject(session, authenticated, POST_OBJECT_GENERIC_TYPE_VARIABLE, userAuthenticated);
}



void
DestroySessionPages(
    SodiumSession* session
)
{
	increaseDebugIndentionCount(session);

    HTSQLPage *pageTmp;
    HTSQLPage *page = session->rootPage;
    while(page) {
        pageTmp = page->next;
        mkDebug(session,
                SESSION_LOG_DETAIL_HTSQL_VARIABLE,
                "\n",
                "File destroying : '",
                page->fileNameRequested,
                "'",
                NULL
                );

		increaseDebugIndentionCount(session);

        FreeQueryStringParams(session, page);
        destroySessionGETPage(session, page);
        destroySessionPOSTPage(session, page);

        if (page->fileNameRequested) {
            mkFree(session->heapHandle, page->fileNameRequested);
            page->fileNameRequested = NULL;
        }
        mkFree(session->heapHandle, page);

		decreaseDebugIndentionCount(session);

        mkDebug(session,
                SESSION_LOG_DETAIL_HTSQL_VARIABLE,
                "\n",
                "File destroying done.",
                "\n",
                NULL
                );

        page    = pageTmp;
    }

    session->rootPage = NULL;
    session->lastPage = NULL;

	decreaseDebugIndentionCount(session);
}

void destroySessionPOSTPage(SodiumSession *session, HTSQLPage *page) {

    cleanUpFunctionStack(session, page);
}

void destroySessionGETPage(SodiumSession *session, HTSQLPage *page) {

    if (page->currentTAGDataBlock) {
        page->currentTAGDataBlock->rootTAGInput     = NULL;
        page->currentTAGDataBlock->currentTAGInput  = NULL;
    }
    // Destroy Datablocks
    TAGDataBlock *datablock = page->rootTAGDataBlock;// currentTAGDataBlock;
    while(datablock) {
        TAGDataBlock *tempTAGDatablock = datablock->next;
        destroyTAGDataBlock(session, page, datablock);
        mkFree(session->heapHandle, datablock);
        datablock = tempTAGDatablock;
    }
    page->currentTAGDataBlock = NULL;
    page->rootTAGDataBlock    = NULL;

    // Destroy Datalist
    TAGDatalist *datalist = page->rootTAGDatalist; //currentTAGDatalist;
    while(datalist != NULL) {
        TAGDatalist *tempTAGDatalist = datalist->next;
        destroyTAGDatalist(session, page, datalist);
        datalist = tempTAGDatalist;
    }
    page->currentTAGDatalist = NULL;
    page->rootTAGDatalist    = NULL;

    if (page->currentScriptType != NULL) {
        mkFree(session->heapHandle, page->currentScriptType);
        page->currentScriptType = NULL;
    }
    if (page->currentScriptFile != NULL) {
        mkFree(session->heapHandle, page->currentScriptFile);
        page->currentScriptFile = NULL;
    }

    if (page->clientMessage != NULL) {
        mkFree(session->heapHandle, page->clientMessage);
        page->clientMessage = NULL;
    }

	if (session->sessionDebugInfo) {
		mkFree(session->heapHandle, session->sessionDebugInfo);
		session->sessionDebugInfo = NULL;
	}

    // PreGenericObjects
    PreObjectGeneric *objectGeneric = page->rootPreObjectGeneric;
    while(objectGeneric != NULL) {
        PreObjectGeneric *objectTemp = objectGeneric->next;

        if (objectGeneric->objectGenericType == POST_OBJECT_GENERIC_TYPE_VARIABLE) {
            mkDebug(session, SESSION_LOG_DETAIL_HTSQL_VARIABLE, "\n", "Global variable destroyed: ", objectGeneric->objectName, NULL);
        } else if (objectGeneric->objectGenericType == POST_OBJECT_GENERIC_TYPE_FUNCTION) {
            mkDebug(session, SESSION_LOG_DETAIL_HTSQL_VARIABLE, "\n", "Global function destroyed: ", objectGeneric->objectName, NULL);
        } /*else if (objectGeneric->objectGenericType == POST_OBJECT_GENERIC_TYPE_RESULTSET) {
            mkDebug(session, SESSION_LOG_DETAIL_HTSQL_VARIABLE, "\n", "Global resultset destroyed: ", objectGeneric->objectName, NULL);
        }*/

        DestroyPreObjectGeneric(session, objectGeneric);
        objectGeneric = objectTemp;
    }
    page->lastPreObjectGeneric    = NULL;
    page->rootPreObjectGeneric    = NULL;

    if (page->currentDatablockName != NULL) {
        mkFree(session->heapHandle, page->currentDatablockName);
        page->currentDatablockName = NULL;
    }
    if (page->currentRowId != NULL) {
        mkFree(session->heapHandle, page->currentRowId);
        page->currentRowId = NULL;
    }
}

SodiumSession *destroySession(SodiumSession *session) {

    EnterCriticalSection(&htsqlCriticalSection);

    // Mark the session state to SESSION_READY_STATE_KILLED
    session->sessionReadyState = SESSION_READY_STATE_BEING_KILLED;

    __sendSessionEndResponse(session, NULL, SESSION_END_REASON_IDLE_TIMEOUT);

    increaseDebugIndentionCount(session);

    bool rootAndLeaf = false;
    SodiumSession* nextMKSessionToIterate = session->next;

    mkDebug(session,
        SESSION_LOG_DETAIL_SESSION_EVENTS,
        "\n",
        "CLEANING IDLE OR DEAD SESSION '", session->id, "'\n",
        NULL);

    if (session != mkRootSessionInstance && session != mkCurrentMKSessionInstance) {
        /** currentSodiumSession neither root nor leaf */
        session->prev->next = session->next;
        session->next->prev = session->prev;
    }
    else if (session == mkRootSessionInstance && session != mkCurrentMKSessionInstance) {
        /** currentSodiumSession is root but not leaf */
        mkRootSessionInstance->next->prev = NULL;
        mkRootSessionInstance = session->next;
    }
    else if (session != mkRootSessionInstance && session == mkCurrentMKSessionInstance) {
        /** currentSodiumSession is not root but leaf */
        mkCurrentMKSessionInstance->prev->next = NULL;
        mkCurrentMKSessionInstance = mkCurrentMKSessionInstance->prev;
    }
    else {
        /** currentSodiumSession is root and eaf. that is, it is the only session
            !!! this part must be implemented end of the function !!!
        */
        rootAndLeaf = true;
    }

    // clean all session object (includes PAGE and AJAX requests)
    DestroySessionPages(session);

    // Cleaning theme info
    if (session->theme) {
        mkFree(session->heapHandle, session->theme);
        session->theme = NULL;
    }

    /** Destroy Connections */
    TAGConnection* connection = session->rootTAGConnection; //currentTAGConnection;
    while (connection != NULL) {
        TAGConnection* tempTAGConnection = connection->next;
        destroyTAGConnection(session, connection);
        connection = tempTAGConnection;
    }
    session->currentTAGConnection = NULL;
    session->rootTAGConnection = NULL;

    /** Session variables destroyed */
    PreObjectGeneric* tempSessionObjectGeneric = session->rootSessionObjectGeneric;
    while (tempSessionObjectGeneric != NULL) {
        PreObjectGeneric* objectTemp = tempSessionObjectGeneric->next;
        DestroyPreObjectGeneric(session, tempSessionObjectGeneric);
        tempSessionObjectGeneric = objectTemp;
    }
    session->rootSessionObjectGeneric = NULL;
    session->currentSessionObjectGeneric = NULL;

    DestroyResponse(session);

    if (session->id) {
        mkFree(session->heapHandle, session->id);
        session->id = NULL;
    }

    decreaseDebugIndentionCount(session);

    mkDebug(session,
        SESSION_LOG_DETAIL_SESSION_EVENTS,
        "\n",
        "CLEANING IDLE OR DEAD SESSION DONE\n",
        NULL);

    // Mark the session state to SESSION_READY_STATE_KILLED
    session->sessionReadyState = SESSION_READY_STATE_KILLED;

    // Destroy the memory for http session objects
    HeapDestroy(session->heapHandle);
    session->heapHandle = NULL;

    if (rootAndLeaf) {
        mkCurrentMKSessionInstance = NULL;
        mkRootSessionInstance = NULL;
    }

    LeaveCriticalSection(&htsqlCriticalSection);

    return nextMKSessionToIterate;
}


void __sendKillSessionNativeResponse(SodiumSession *session) {
    mkDebug(session, SESSION_LOG_DETAIL_SESSION_EVENTS, "\n", "Session is signaled to be killed.", NULL);
    HTSQLPage *page = GetCurrentPage(session);

#ifndef  HTSQL_MEMORY_LEAK_DEBUG
    const char *url = GetQueryStringParam(session, page, "redirect-url");
    if (url) {
        __sendShowPageResponse(session, page, url);
    }
#endif // ! HTSQL_MEMORY_LEAK_DEBUG

    destroySession(session);
}


void clearClientMessage(SodiumSession *session, HTSQLPage *page) {
    if (page->clientMessage != NULL) {
        mkFree(session->heapHandle, page->clientMessage);
        page->clientMessage = NULL;
    }
}

void setClientMessage(SodiumSession *session, HTSQLPage *page, const char *message) {
    clearClientMessage(session, page);
    if (message != NULL) {
        page->clientMessage = mkStrdup(session->heapHandle, message, __FILE__, __LINE__);
    }
    // "else" block is not required since session->clientMessage was nullified in clearClientMessage(..) call above.
}


const char *getClientMessage(SodiumSession *session, HTSQLPage *page) {
    return page->clientMessage;
}

SodiumSession *findSessionById() {	
	return mkRootSessionInstance;

}

SodiumSession *getHTTPSessionInstance(LPEXTENSION_CONTROL_BLOCK clpECB) {

    EnterCriticalSection(&htsqlCriticalSection);

    SodiumSession *session = NULL;
    /*
    char *sessionIdStr = findHTTPCookieByName(clpECB, mkSessionCookieName);

    if (sessionIdStr) {
        session = getMKSessionByMKSessionId(sessionIdStr);

        if (!session) {
            mkDebug(session, SESSION_LOG_DETAIL_SESSION_EVENTS, "\n\n#######   POST   #############\n", NULL);

            mkDebug(NULL, SESSION_LOG_DETAIL_SESSION_EVENTS, "\nHttp request from dead session: ", sessionIdStr, NULL);
            // If server goes down but browser has not been closed, we get the same cookie.
            // In that case, there is no session instance in memory anymore. So, we are creating a new session.
            session = CreateSodiumSession(mkApplicationHeapHandle, clpECB);
            mkDebug(session, SESSION_LOG_DETAIL_SESSION_EVENTS, "\n\n#######   POST DONE   #############\n", NULL);
        }
        free(sessionIdStr);
    } else {
        session = createMKSession(mkApplicationHeapHandle, clpECB);
    }

    LeaveCriticalSection(&htsqlCriticalSection);
    */
    return session;
}


char *findHTTPCookieByName(LPEXTENSION_CONTROL_BLOCK lpECB, const char *cookieName) {

    int cookieCount = 1;
    DWORD sizeOfBuffer = 50;
    char *retTemp = NULL;
    char *retval  = NULL;

    struct yuarel_param *param = malloc( cookieCount * sizeof(struct yuarel_param));
    char *cookie = malloc(sizeOfBuffer);

    BOOL opret = lpECB->GetServerVariable(lpECB->ConnID, "HTTP_COOKIE", cookie, &sizeOfBuffer);
    if (opret == TRUE) {

        int p = yuarel_parse_query(cookie, '&', param, cookieCount );
        for(int i=0; i < p; i++) {
            if (strcmp(param[i].key, cookieName) == 0) {
                retTemp = param[i].val;
                break;
            }
        }

    } else {
        DWORD errCode = GetLastError();
        switch(errCode) {
            case ERROR_INSUFFICIENT_BUFFER: {
                retTemp = NULL;
                break;
            }
            case ERROR_INVALID_INDEX: {
                retTemp = NULL;
                break;
            }
            case ERROR_INVALID_PARAMETER: {
                retTemp = NULL;
                break;
            }
        }
    }

    if (retTemp) {
		size_t size = strlen(retTemp) + 1;
        retval = malloc(size);
		if (retval)
			strcpy_s(retval, size, retTemp);
    }

    free(param);
    free(cookie);

    return retval;
}



void destroyIdleSessions() {

    // Do not get ISAPI control block pointer as argument since it will not be correct

	clock_t end;
    SodiumSession *current = getMKSessionRootInstance();
    if (current == NULL) {
        return;
    }
    do {
        if (current->sessionReadyState == SESSION_READY_STATE_READY) {
			end = clock();
            if (((end - current->lastActivityTime) / CLOCKS_PER_SEC) >= MAX_IDLE_TIME_IN_MILISECONDS) {
                current = destroySession(current);
                break;
            }
        }
        current = current->next;
    } while(current);

}




void setMKSessionRootInstance(SodiumSession* currentMKSession) {
    if (mkRootSessionInstance == NULL) {
        mkRootSessionInstance = currentMKSession;
        mkCurrentMKSessionInstance = currentMKSession;
    }
}


void addMKSessionInstance(SodiumSession* currentMKSession) {
    currentMKSession->prev = mkCurrentMKSessionInstance;
    mkCurrentMKSessionInstance->next = currentMKSession;
    mkCurrentMKSessionInstance = currentMKSession;
}