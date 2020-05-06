#include "stdafx.h"

#include "debugee.h"

#include "debugeePipeServer.h"


int 
IsBreakpointHit(
	SodiumSession *session, 
	const wchar_t *fileURI, 
	char *lineNumberStr
) 
{
	// Copy of fileURI to temp local variable in order to convert it into lowercase
	wchar_t fileURILowerCase[MAX_PATH];
	wcscpy_s(fileURILowerCase, MAX_PATH, fileURI);
	/* converting drive letter into lowercase */
	mkLowerCaseDriveLetterW(fileURILowerCase);

	/* convertion fileURILowerCase into multibyte version in order to print it log file*/
	char fUri[MAX_PATH] = "";
	size_t sizeOfFileUri = wcslen(fileURILowerCase);
	wcstombs_s(NULL, fUri, MAX_PATH, fileURILowerCase, sizeOfFileUri*sizeof(wchar_t));
	mkDebug(session, SESSION_LOG_DETAIL_DEBUGEE, "\nHit check: ", fUri, ":", lineNumberStr, NULL);

	// Extraction file name part from full path
	//const wchar_t* fName = GetFileName(fileURILowerCase);

	if (session == NULL || lineNumberStr == NULL) {
		return FALSE;
	}
	
	for (int i = 0; i < MAX_BREAKPOINT_COUNT; i++) {
		if (session->breakpointsRoot[i].breakpoint.breakpointState == BREAKPOINT_STATE_ACTIVE) {
			if (wcsncmp(session->breakpointsRoot[i].breakpoint.fileURI, fileURILowerCase, MAX_PATH) == 0) {
				if (strncmp(session->breakpointsRoot[i].breakpoint.lineNumber, lineNumberStr, MAX_PATH) == 0) {
					return session->breakpointsRoot[i].breakpoint.id;
				}
			}
		}
	}

	return -1;
}

void setBreakpointType(SodiumSession *currentMKSession, int pBreakpointId, BreakpointType pBreakpointType) {
	for (int i = 0; i < MAX_BREAKPOINT_COUNT; i++) {
		if (currentMKSession->breakpointsRoot[i].breakpoint.breakpointState == BREAKPOINT_STATE_ACTIVE) {
			if (currentMKSession->breakpointsRoot[i].breakpoint.id == pBreakpointId) {
				currentMKSession->breakpointsRoot[i].breakpoint.breakpointType = pBreakpointType;
			}
		}
	}
}

void 
DeleteBreakpoint(
	SodiumSession *session, 
	int pBreakpointId
) 
{
	for (int i = 0; i < MAX_BREAKPOINT_COUNT; i++) {
		//if (session->breakpointsRoot[i].breakpoint.breakpointState == BREAKPOINT_STATE_ACTIVE) {
			if (session->breakpointsRoot[i].breakpoint.id == pBreakpointId) {
				session->breakpointsRoot[i].breakpoint.breakpointState = BREAKPOINT_STATE_DELETED;

				char pBreakpointIdStr[10] = "";
				sprintf_s(pBreakpointIdStr, 10, "%d", pBreakpointId);
				mkDebug(session, SESSION_LOG_DETAIL_DEBUGEE, "\nBreakpoint deleted: ", pBreakpointIdStr, NULL);
			}
		//}
	}
}


BreakpointType getBreakpointType(SodiumSession *currentMKSession, int pBreakpointId) {
	for (int i = 0; i < MAX_BREAKPOINT_COUNT; i++) {

		if (currentMKSession->breakpointsRoot[i].breakpoint.breakpointState == BREAKPOINT_STATE_ACTIVE) {
		
			if (currentMKSession->breakpointsRoot[i].breakpoint.id == pBreakpointId) {
				return currentMKSession->breakpointsRoot[i].breakpoint.breakpointType;
			}

		}
	}
	return -1;
}

void setAllPreviousCallstackFunctionsDebugStepType(SodiumSession *currentMKSession, CallStack *cls, Debug_Step_Type stepType) {
	
	while (cls) {

		cls->functionDebugInfo->functionDebugStepType = stepType;

		if (cls->prev) {
			cls = cls->prev;
		}
		else {
			cls = NULL;
		}
		
	}
}

void raiseBreakPointException(SodiumSession *currentMKSession) {
	HTSQLPage *page = currentMKSession->currentPage;
	char lineStr[10];
	char bpIdStr[10]; 
	char command[1000] = "";
	char cFileUri[MAX_PATH] = "";

	//	let check if we must raise a break point exception
	//	1.	Worker process has a debugger attached
	if (IsDebuggerPresent() == FALSE) {
		return;
	}

	int lineNo = getPageCurrentLineNumberToBeExecuted(currentMKSession, page);
	_itoa_s(lineNo, lineStr, 10, 10); 
	wcstombs_s(NULL, cFileUri, MAX_PATH, page->currentCallStack->functionDebugInfo->fileURI, MAX_PATH);
	 
	//	2.	Is there any breakpoint set on the current line
	int hit = IsBreakpointHit(currentMKSession, page->currentCallStack->functionDebugInfo->fileURI, lineStr);

	if (hit >= 0) {
		
		_itoa_s(hit, bpIdStr, 10, 10);

		//	Breakpoint 2, main (argc=1, argv=0x3c15a0) at C:\temp\test\main.c:24
		int breakpointType = getBreakpointType(currentMKSession, hit);
		if (breakpointType == BREAKPOINT_TYPE_PERMINENT) {
			strcpy_s(command, 1000, "Breakpoint ");
		}
		else if (breakpointType == BREAKPOINT_TYPE_TEMPORARY) {
			strcpy_s(command, 1000, "Temporary breakpoint ");
		}		
		
		strcat_s(command, 1000, bpIdStr);
		strcat_s(command, 1000, ", ");
		strcat_s(command, 1000, page->currentCallStack->functionDebugInfo->functionName);
		strcat_s(command, 1000, "() at ");
		strcat_s(command, 1000, cFileUri);
		strcat_s(command, 1000, ":");
		strcat_s(command, 1000, lineStr);
		strcat_s(command, 1000, "\r\n");

		setAllPreviousCallstackFunctionsDebugStepType(currentMKSession, page->currentCallStack, DEBUG_STEP_TYPE_STEP_OVER);
		if (breakpointType == BREAKPOINT_TYPE_TEMPORARY) {
			DeleteBreakpoint(currentMKSession, hit);
		}

		mkDebug(currentMKSession, SESSION_LOG_DETAIL_DEBUGEE, "\n", "**** Breakpoint Hit ****", NULL);
	} 
	BOOL isDebugStepActive = (page->currentCallStack->functionDebugInfo->functionDebugStepType == DEBUG_STEP_TYPE_STEP_OVER);
							  
	if (hit >= 0 || isDebugStepActive) {
		// "\32\32C:/projects/sodium/Sodium-Site/apps/deps\\logon.sqlx:5:1:beg:0x4015fd\r\n
		strcat_s(command, 1000, "\32\32");
		strcat_s(command, 1000, cFileUri);
		strcat_s(command, 1000, ":");
		strcat_s(command, 1000, lineStr);
		strcat_s(command, 1000, ":1:");
		strcat_s(command, 1000, "beg");
		strcat_s(command, 1000, ":0x0000");
		strcat_s(command, 1000, "\r\n");
		OutputDebugString(command);
	}

	if (currentMKSession->sessionDebugInfo->sessionDebugStepType == DEBUG_STEP_TYPE_STEP_CONTUNIE) {
		setAllPreviousCallstackFunctionsDebugStepType(currentMKSession, page->currentCallStack, DEBUG_STEP_TYPE_STEP_CONTUNIE);
	}
	else if (currentMKSession->sessionDebugInfo->sessionDebugStepType == DEBUG_STEP_TYPE_STEP_OUT) {
		page->currentCallStack->functionDebugInfo->functionDebugStepType = DEBUG_STEP_TYPE_STEP_CONTUNIE;
	}

	if (isDebugStepActive) {
		if (currentMKSession->sessionDebugInfo->sessionDebugStepType == DEBUG_STEP_TYPE_STEP_IN) 
			mkDebug(currentMKSession, SESSION_LOG_DETAIL_DEBUGEE, "\n", "**** STEP IN ****", NULL);
		else if (currentMKSession->sessionDebugInfo->sessionDebugStepType == DEBUG_STEP_TYPE_STEP_OUT)
			mkDebug(currentMKSession, SESSION_LOG_DETAIL_DEBUGEE, "\n", "**** STEP OUT ****", NULL);
		else if (currentMKSession->sessionDebugInfo->sessionDebugStepType == DEBUG_STEP_TYPE_STEP_OVER)
			mkDebug(currentMKSession, SESSION_LOG_DETAIL_DEBUGEE, "\n", "**** STEP OVER ****", NULL);
		else
			mkDebug(currentMKSession, SESSION_LOG_DETAIL_DEBUGEE, "\n", "**** UNKNOWN STEP TYPE ****", NULL);
	}

	
}

void increaseInnerLineNumber(SodiumSession *currentMKSession) {
	HTSQLPage *page = GetCurrentPage(currentMKSession);
	if (page && page->currentCallStack && page->currentCallStack->functionDebugInfo) {
		page->currentCallStack->functionDebugInfo->lineNumberInner++;
		raiseBreakPointException(currentMKSession);
	}
}


void debugeeCreatePipeServerThread(void) {
	DWORD  dwThreadId = 0;
	HANDLE hThread = NULL;

	// Create a thread for this client. 
	hThread = CreateThread(
		NULL,              // no security attribute 
		0,                 // default stack size 
		debugeeCreatePipeServer,    // thread proc
		NULL,    // thread parameter 
		0,                 // not suspended 
		&dwThreadId);      // returns thread ID 
}

/*void debuggerEventOccured(SodiumSession *currentMKSession, HTSQLPage *page, const char *message) {
	
	int lSize = 0;

	int requiredSize = MultiByteToWideChar(
		CP_ACP,		//	CodePage
		MB_PRECOMPOSED, //	dwFlags,
		message,		//	LPCSTR lpMultiByteStr,
		-1,				//	cbMultiByte,
		NULL,		//	LPWSTR lpWideCharStr,
		lSize			//	cchWideChar
	);

	if (requiredSize > 0) {
		// allocation double for multibyte character array + 2 byte for null. precaution 
		requiredSize = (requiredSize*2)+2;
		wchar_t *lBuffer = mkMalloc(currentMKSession->heapHandle, requiredSize, __FILE__, __LINE__);

		int writtenSize = MultiByteToWideChar(
			CP_ACP,		//	CodePage
			MB_PRECOMPOSED, //	dwFlags,
			message,		//	LPCSTR lpMultiByteStr,
			-1,				//	cbMultiByte,
			lBuffer,			//	LPWSTR lpWideCharStr,
			requiredSize			//	cchWideChar
		);
		
		if (writtenSize > 0) {
			OutputDebugString(message);
			DebugBreak();
		}
		else {
			//error
		}

		mkFree(currentMKSession->heapHandle, lBuffer);		
	}
	
} */


int 
GetActiveBreakpointCount(
	SodiumSession *session
)
{
	int bpCount = 0;
	if (session == NULL) {
		return 0;
	}

	for (int i = 0; i < MAX_BREAKPOINT_COUNT; i++) 
	{
		char* extPos = NULL;
		char bpFileName[256];
		char currentFileName[256];

		// from wchar_t to char conversation
		sprintf_s(bpFileName, 256, "%ws", session->breakpointsRoot[i].breakpoint.fileURI);
		sprintf_s(currentFileName, 256, "%ws", session->currentPage->fileNameRequested);

		// removing file extension
		extPos = strstr(bpFileName, ".sqlx");
		if (extPos)
			*extPos = '\0';
		extPos = strstr(currentFileName, ".frmx");
		if (extPos)
			*extPos = '\0';
		//mkPrint(session, bpFileName, " ", currentFileName, NULL);

		if (session->breakpointsRoot[i].breakpoint.breakpointState == BREAKPOINT_STATE_ACTIVE) 
		{
			// comparing current filename with breakpoint file name without file extension
			if (_stricmp(bpFileName, currentFileName) == 0)
				bpCount++;
		}
	}
	return bpCount;
}