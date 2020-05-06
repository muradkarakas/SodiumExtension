// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#include "dll.h"


#include "debugee.h"
#include "mkapplication.h"

// Core log full file path
wchar_t cDebugLogFileFullPath[MAX_PATH] = L"";
FILE* mkLogFile = NULL;


void
init() {
	_wgetcwd(cDebugLogFileFullPath, MAX_PATH);
	wcscat_s(cDebugLogFileFullPath, MAX_PATH, L"\\");
	wcscat_s(cDebugLogFileFullPath, MAX_PATH, cLogFileName);

	mkLogFile = _wfsopen(cDebugLogFileFullPath, L"a+", _SH_DENYNO);

	// Log generation checking file path is set or not
	/*if (cDebugLogFileFullPath[0] == '\0') {
		setDebugLogFileFullPath(hModule);
	}*/
	//mkLogFile = _fsopen(cDebugLogFileFullPath, "a+", _SH_DENYNO);

	/*mkLogFile = CreateFile(cDebugLogFileFullPath,                // name of the write
		GENERIC_WRITE,          // open for writing
		FILE_SHARE_READ,        // share for reading
		NULL,                   // default security
		CREATE_NEW,             // Creates a new file, only if it does not already exist
		FILE_ATTRIBUTE_NORMAL,  // normal file
		NULL);*/

		// Initialize the critical section once only.
	bool crBool = InitializeCriticalSectionAndSpinCount(&htsqlCriticalSection, 1000);
	if (crBool == FALSE) {
		mkDebug(NULL, SESSION_LOG_DETAIL_DLL, "\nInitializeCriticalSectionAndSpinCount UNsuccessful", NULL);
	}
	else {
		mkDebug(NULL, SESSION_LOG_DETAIL_DLL, "\nInitializeCriticalSectionAndSpinCount successful", NULL);
	}
	setMKApplicationHeapHandle();

	debugeeCreatePipeServerThread();

	initializeRandomNumberGenerator();
}