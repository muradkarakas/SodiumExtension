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

#include "debugeePipeServer.h"

#include "breakpoints.imp.h"

void debugeeGetAnswerToRequest(char *pchRequest, char *pchReply, DWORD pchBytes) {

	mkMKSessionInstanceDebugged = findSessionById();
	//
	//	CMD_GET_SESSION_ROOT_INSTANCE_ADDRESS Command
	//
	if (strncmp(pchRequest, PIPE_CMD_GET_SESSION_ROOT_INSTANCE_ADDRESS, strlen(PIPE_CMD_GET_SESSION_ROOT_INSTANCE_ADDRESS)) == 0) {
		char sessionId[20] = "";		
		if (mkMKSessionInstanceDebugged) {
			sprintf_s(sessionId, 20, "%p", mkMKSessionInstanceDebugged);
			strcat_s(sessionId, 20, "\r\n");
		}
		else {
			strcpy_s(sessionId, 20, "0\r\n");
		}
		strcpy_s(pchReply, REPLY_BUFFER_SIZE, sessionId);
	
	} else if (strncmp(pchRequest, PIPE_CMD_GET_PROCESS_ID, strlen(PIPE_CMD_GET_PROCESS_ID)) == 0) {
		char processId[20] = "";
		if (mkMKSessionInstanceDebugged) {
			sprintf_s(processId, 20, "%lu", GetCurrentProcessId());
			strcat_s(processId, 20, "\r\n");
		}
		else {
			strcpy_s(processId, 20, "0\r\n");
		}
		strcpy_s(pchReply, REPLY_BUFFER_SIZE, processId);
	} else if (strncmp(pchRequest, PIPE_CMD_CLEAR_ALL_BREAKPOINTS, strlen(PIPE_CMD_CLEAR_ALL_BREAKPOINTS)) == 0) {
		//
		//	CMD_CLEAR_ALL_BREAKPOINTS Command
		//
		if (mkMKSessionInstanceDebugged) {
			clearAllBreakpoints(mkMKSessionInstanceDebugged);
			strcpy_s(pchReply, REPLY_BUFFER_SIZE, "OK\r\n");
		}
		else {
			mkCoreDebug(__FILE__, __LINE__, "\nCommand received but no session attached. Command: ", pchRequest, "\n", NULL);
		}
	} 

	else {
		mkCoreDebug(__FILE__, __LINE__, "\nInvalid command received. Command: ", pchRequest, "\n", NULL);
	}
}


DWORD WINAPI debugeeCreatePipeServer(void *param) {

	BOOL   fConnected = FALSE;
	DWORD  dwThreadId = 0;
	HANDLE hPipe = INVALID_HANDLE_VALUE, hThread = NULL;
	LPTSTR lpszPipename = "\\\\.\\pipe\\nextDebuggerServer";

	// The main loop creates an instance of the named pipe and 
	// then waits for a client to connect to it. When the client 
	// connects, a thread is created to handle communications 
	// with that client, and this loop is free to wait for the
	// next client connect request. It is an infinite loop.

	for (;;) {

		hPipe = CreateNamedPipe(
			lpszPipename,             // pipe name 
			PIPE_ACCESS_DUPLEX,       // read/write access 
			PIPE_TYPE_MESSAGE |       // message type pipe 
			PIPE_READMODE_MESSAGE |   // message-read mode 
			PIPE_WAIT,                // blocking mode 
			PIPE_UNLIMITED_INSTANCES, // max. instances  
			REPLY_BUFFER_SIZE,                  // output buffer size 
			REPLY_BUFFER_SIZE,                  // input buffer size 
			0,                        // client time-out 
			NULL);                    // default security attribute 

		if (hPipe == INVALID_HANDLE_VALUE) {
			mkCoreDebug(__FILE__, __LINE__, "\nCreateNamedPipe failed", NULL);
			return FALSE;
		}

		//mkCoreDebug(__FILE__, __LINE__, "\nHTSQL server is ready to accept debugger commands.", NULL);

		fConnected = ConnectNamedPipe(hPipe, NULL) ?
			TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
		
		//mkCoreDebug(__FILE__, __LINE__, "\nDebugger connected.", NULL);

		if (fConnected) {
			
			// Create a thread for this client. 
			hThread = CreateThread(
				NULL,              // no security attribute 
				0,                 // default stack size 
				debugeeServerThreadInstanceFunction,    // thread proc
				(LPVOID)hPipe,    // thread parameter 
				0,                 // not suspended 
				&dwThreadId);      // returns thread ID 

			if (hThread == NULL) {
				//_tprintf(TEXT("CreateThread failed, GLE=%d.\n"), GetLastError());
				mkCoreDebug(__FILE__, __LINE__, "\nCreateThread failed.", NULL);
				return FALSE;
			}
			else 
				CloseHandle(hThread);
		}		
	}

	CloseHandle(hPipe);
	
	// success
	return TRUE;
}


DWORD WINAPI debugeeServerThreadInstanceFunction(LPVOID lpvParam) {

	HANDLE hHeap = GetProcessHeap();
	char* pchRequest = (char*)HeapAlloc(hHeap, 0, REPLY_BUFFER_SIZE);
	char* pchReply = (char*)HeapAlloc(hHeap, 0, REPLY_BUFFER_SIZE);

	DWORD cbBytesRead = 0, cbReplyBytes = 0, cbWritten = 0;
	BOOL fSuccess = FALSE;
	HANDLE hPipe = NULL;

	if (lpvParam == NULL) {
		mkCoreDebug(__FILE__, __LINE__, "\nERROR - Pipe Server Failure:\nInstanceThread got an unexpected NULL value in lpvParam.\nDebugee functionality disabled.\n");
		if (pchReply != NULL)
			HeapFree(hHeap, 0, pchReply);
		if (pchRequest != NULL) 
			HeapFree(hHeap, 0, pchRequest);
		return (DWORD)-1;
	}

	if (pchRequest == NULL) {
		mkCoreDebug(__FILE__, __LINE__, "\nERROR - Pipe Server Failure:\nInstanceThread got an unexpected NULL heap allocation.\nDebugee functionality disabled.\n");
		if (pchReply != NULL) 
			HeapFree(hHeap, 0, pchReply);
		return (DWORD)-1;
	}

	if (pchReply == NULL) {
		mkCoreDebug(__FILE__, __LINE__, "\nERROR - Pipe Server Failure:\nInstanceThread got an unexpected NULL heap allocation.\nDebugee functionality disabled.\n", NULL); //GetLastError()  SESSION_LOG_DETAIL_DEBUGEE
		if (pchRequest != NULL) 
			HeapFree(hHeap, 0, pchRequest);
		return (DWORD)-1;
	}

	// The thread's parameter is a handle to a pipe object instance. 
	hPipe = (HANDLE)lpvParam;

	// get commands and reply in loop
	while (1) {

		// Read client command from the pipe. 
		fSuccess = ReadFile(
			hPipe,         
			pchRequest,    
			REPLY_BUFFER_SIZE, 
			&cbBytesRead, 
			NULL);        

		if (fSuccess == TRUE) {

			pchRequest[cbBytesRead] = '\0';
			//mkDebug(NULL, SESSION_LOG_DETAIL_DEBUGEE, "\nCommand received : ", pchRequest, NULL);

			// Process the incoming command.
			debugeeGetAnswerToRequest(pchRequest, pchReply, REPLY_BUFFER_SIZE);

			// Write the reply to the pipe. 
			fSuccess = WriteFile(
				hPipe,        
				pchReply,    
				(DWORD)strlen(pchReply), 
				&cbWritten,   
				NULL);       

			if (fSuccess == FALSE) {
				mkCoreDebug(__FILE__, __LINE__, "\nInstanceThread WriteFile failed. Request: ", pchRequest, "\n", NULL);//GetLastError()  
				break;
			}
		}
		else {
			if (GetLastError() == ERROR_BROKEN_PIPE) {
				//wchar_t errorText[256];
				//FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), errorText, sizeof(errorText), NULL);
				//mkCoreDebug(__FILE__, __LINE__, "\nInfo: Named pipe client disconnected.", NULL); //GetLastError()  SESSION_LOG_DETAIL_DEBUGEE
			}
			else {
				mkCoreDebug(__FILE__, __LINE__, "\nInstanceThread ReadFile failed.", NULL); // GetLastError()
			}
			break;
		}		
	}

	FlushFileBuffers(hPipe);
	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);

	HeapFree(hHeap, 0, pchRequest);
	HeapFree(hHeap, 0, pchReply);

	return 0;
}


