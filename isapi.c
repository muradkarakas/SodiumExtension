#include "pch.h"

#include "isapi.h"

#include "dispatcher.h"
#include "mkapplication.h"
#include "debugee.h"

/*
__declspec(dllexport) DWORD WINAPI   HttpExtensionProc(_In_ EXTENSION_CONTROL_BLOCK *clpECB)  {

    // Create or get the http session instance
    SodiumSession *currentMKSession         = getHTTPSessionInstance(clpECB);

    //* Destroy Idle sessions 
    destroyIdleSessions();

    // Before go, waiting for the termination of the previous http request of the same session  
    HANDLE currentRequestMutextHandle   = mkWaitTillPreviousRequestOfTheCurrentSessionEnds(currentMKSession);

    // Assign control block structure variable to that's of the current request 
    currentMKSession->lpECB = clpECB;

    // Determine the request type and dispatch it to the correct handler 
    if (currentMKSession->sessionReadyState != SESSION_READY_STATE_KILLED) {
        dispatchRequest(currentMKSession);
	}

    if (currentMKSession->sessionReadyState == SESSION_READY_STATE_INIT) {
        destroySession(currentMKSession);
        mkFree(mkApplicationHeapHandle, currentMKSession);
    } 
    // Request handling done 
    ReleaseMutex(currentRequestMutextHandle);

    return HSE_STATUS_SUCCESS;
}




__declspec(dllexport) BOOL WINAPI  GetExtensionVersion(HSE_VERSION_INFO* pVer) {
	pVer->dwExtensionVersion = HSE_VERSION;
	mkDebug(NULL, SESSION_LOG_DETAIL_ISAPI, "\nGetExtensionVersion run successfully", NULL);
	return TRUE;
}


__declspec(dllexport) BOOL  WINAPI   TerminateExtension(DWORD dwFlags) {
	mkDebug(NULL, SESSION_LOG_DETAIL_ISAPI, "\nTerminateExtension run successfully", NULL);
	return TRUE;
}
*/<


HANDLE mkWaitTillPreviousRequestOfTheCurrentSessionEnds(SodiumSession *currentMKSession) {
	// I do not converting session id from ansi to wchar_t here. Just bypassing warning by typecasting. It is working. 
    HANDLE currentRequestMutextHandle = CreateMutex(NULL, false, currentMKSession->id);
    /** before go, waiting for the termination of the previous http request of the same session  */
	if (currentRequestMutextHandle) {
		WaitForSingleObject(currentRequestMutextHandle, INFINITE);
	}
	else {

	}
	return currentRequestMutextHandle;
}


void 
PrintPageProccessTime(
    SodiumSession *session
)
{
    char cBuf[100] = "";
    mkConvertFloatToChar(session->pageProcessTime, cBuf, 100);
    mkPrint(session, "<br><b>Page proccess time: </b>", cBuf, " MSec", NULL);

    mkItoa(getMKSessionCount(), cBuf);
    mkPrint(session, "<br><b>Session Count: </b>", cBuf, NULL);

	if (IsDebuggerPresent()) {
		int bpCount = GetActiveBreakpointCount(session);
		_itoa_s(bpCount, cBuf, 100, 10);
		mkPrint(session, "<br><b>Being debugged. (Active Breakpoint Count: ", cBuf, ")</b>", NULL);
	}
	mkPrint(session, "</center>", NULL);
}




