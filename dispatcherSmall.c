#include "pch.h"

#include "dispatcherSmall.h"

#include "header.h"
#include "ajax.h"
#include "ajaxAsGet.h"
#include "dispatcher.h"
#include "pre.parser.imp.h"
#include "html.parser.imp.h"
#include "pageAccess.h"
#include "isapi.h"

void 
dispatchSmallRequest(
    SodiumSession *session, 
    HTSQLPage *page
 )
{
    wchar_t* codeBehindFileName = CalculateCurrentSQLXFileName(session);

    if (page->currentRequestMethodType == REQUEST_TYPE_POST) {
        /* AJAX REQUEST */
        initDebugInfoFileProperty(session, codeBehindFileName);
        mkDebug(session, SESSION_LOG_DETAIL_ISAPI, "\n\n######   POST (", page->fileNameRequested,")  \n", NULL);
        if (session->sessionReadyState != SESSION_READY_STATE_READY) {
            mkDebug(session, SESSION_LOG_DETAIL_SESSION_EVENTS, "\nSession is dead\n", NULL);
        } else {
            SendAjaxResponseHeader(session);
            serveAsAjaxRequest(session, page);
        }
        if (session->sessionReadyState != SESSION_READY_STATE_KILLED) {
            mkDebug(session, SESSION_LOG_DETAIL_ISAPI, "\n\n######   POST DONE (", page->fileNameRequested,") \n", NULL);
        }
    } else if (page->currentRequestMethodType == REQUEST_TYPE_GET) {

        /* All GET requests are treated as html request */
        SendHtmlResponseHeader(session);

        /* GET REQUEST */
        const char *nativeproceduretorun  = GetQueryStringParam(session, page, "nativeproceduretorun");

        if (nativeproceduretorun == NULL ) {
			clock_t start;
			start = clock();

            /** File exists */
            mkDebug(session, SESSION_LOG_DETAIL_ISAPI, "\n\n######   GET FILE  (", page->fileNameRequested,") \n", NULL);
            destroySessionGETPage(session, page);
            destroySessionPOSTPage(session, page);

            /** FIRST: Parse PRE file */
            
            bool isFileExists = ParseSQLXFile(session, page, codeBehindFileName);
            mkFree(session->heapHandle, codeBehindFileName);

			/** SECOND: Parse MK file */
			bool isUserAuthorized = runPageAccessHTSQLTriggerFunction(session, page);

			if (isUserAuthorized) {
				/** THIRD: Parse MK file */
				bool isFileExists = ParseFRMXFile(session, page);
				printGetRequestTiming(session, page, start);
			}
			else {
				mkDebug(session, SESSION_LOG_DETAIL_ISAPI, "\n\nPage access refused\n\n", NULL);
			}

            mkDebug(session, SESSION_LOG_DETAIL_ISAPI, "\n\n######   GET FILE DONE (", page->fileNameRequested,") \n", NULL);

        } else {

            /** File does not exists */
            mkDebug(session, SESSION_LOG_DETAIL_ISAPI, "\n\n######   GET IMAGE (", page->fileNameRequested,")  \n", NULL);
			sendImageResponseHeader(session);
			serveAsGetRequest(session, page);
            mkDebug(session, SESSION_LOG_DETAIL_ISAPI, "\n\n######   GET IMAGE DONE (", page->fileNameRequested,") \n", NULL);
        }
    }

    if (session->sessionReadyState == SESSION_READY_STATE_KILLED) {
        mkDebug(session, SESSION_LOG_DETAIL_SESSION_EVENTS, "\n", "Session instance memory released.", NULL);
        HeapFree(mkApplicationHeapHandle, HEAP_ZERO_MEMORY, session);
        session = NULL;
    }
}

void printGetRequestTiming(SodiumSession *session, HTSQLPage *page, clock_t start) {
	
	clock_t end; 
	int pageCount = GetPageCount(session);
	char *pageCountStr = mkMalloc(session->heapHandle, 50, __FILE__, __LINE__);
	mkItoa(pageCount, pageCountStr);

	mkPrint(session, "<center><br><br><hr><b>Session Id: </b>", session->id, "<br><b>Page Count: </b>", pageCountStr, NULL);

	/*
	char addrOfsession[100];
	sprintf_s(addrOfsession, 100, "0x%llx", session);
	mkPrint(session, "<br><br>", addrOfsession, NULL);
	*/

	end = clock();
	session->pageProcessTime = (end - start) / CLOCKS_PER_SEC;
	
	PrintPageProccessTime(session);

	mkFree(session->heapHandle, pageCountStr);
}