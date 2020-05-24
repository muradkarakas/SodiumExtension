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

#include "dll.h"


#include "debugee.h"
#include "mkapplication.h"




#define DLL_EXPORT __declspec(dllexport)


DLL_EXPORT BOOL APIENTRY DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpvReserved) 
{
	//mkCoreDebug(__FILE__, __LINE__, "Sodium Extension is loading ...", NULL);
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH: {
			
			init();

            break;
		}
        case DLL_PROCESS_DETACH: {
			mkDebug(NULL, SESSION_LOG_DETAIL_DLL, "\nHeapDestroy(mkApplicationHeapHandle) successful", NULL);
			mkDebug(NULL, SESSION_LOG_DETAIL_DLL, "\nDeleteCriticalSection successful", NULL);
			destroyMKApplicationHeapHandle();
			DeleteCriticalSection(&htsqlCriticalSection);
			
			//	debug log
			if (mkLogFile) {
				fclose(mkLogFile);
				mkLogFile = NULL;
			}
			break;
		}
        case DLL_THREAD_ATTACH: {
            //mkDebug(NULL, "\nDLL_THREAD_ATTACH", NULL);
            break;
		}
        case DLL_THREAD_DETACH: {
            //mkDebug(NULL, "\nDLL_THREAD_DETACH", NULL);
            break;
		}
    }

    return TRUE; // succesful
}

/*
void setDebugLogFileFullPath(HMODULE hinstDLL) {

	// "wide char" variables for GetModuleFileName 
	char lpFullPath[MAX_PATH];

	// char variables for c routine
	char driveLetter[3], directory[MAX_PATH], lCoreLogFileFullPath[MAX_PATH] = "";

	// getting current dll's modele handþe
	HMODULE hModule = hinstDLL; // GetModuleHandle(L"htsql.dll");

	// getting current dll full path by using its own module handle
	DWORD lErr = GetModuleFileName(hModule, lpFullPath, sizeof(lpFullPath) / sizeof(lpFullPath[0]));

	// split current dll full path into chunks
	_splitpath_s(&lpFullPath[4], driveLetter, 3, directory, MAX_PATH, NULL, 0, NULL, 0);

	// construction log file full path 
	strcat_s(cDebugLogFileFullPath, MAX_PATH, driveLetter);
	strcat_s(cDebugLogFileFullPath, MAX_PATH, directory);
	strcat_s(cDebugLogFileFullPath, MAX_PATH, cLogFileName);
}*/

