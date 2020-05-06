#pragma once

#include "stdafx.h"

#include "..\SodiumServer\LoadExtensions.h"


typedef enum _Language_Codes {
	LANGUAGE_CODE_EN = 0,
	LANGUAGE_CODE_TR
} LANGUAGE_CODES;

typedef enum _sessionLogDetail {
	SESSION_LOG_DETAIL_NONE = 0,
	SESSION_LOG_DETAIL_ASSERT = 1,
	SESSION_LOG_DETAIL_DLL = 2,
	SESSION_LOG_DETAIL_ISAPI = 4,
	SESSION_LOG_DETAIL_SESSION_EVENTS = 8,
	SESSION_LOG_DETAIL_DB_EVENTS = 16,
	SESSION_LOG_DETAIL_DML = 32,
	SESSION_LOG_DETAIL_HTSQL_BUILTIN_CALL = 64,
	SESSION_LOG_DETAIL_HTSQL_FUNCTION_CALL = 128,
	SESSION_LOG_DETAIL_HTSQL_TRIGGER = 256,
	SESSION_LOG_DETAIL_HTSQL_ITEM = 512,
	SESSION_LOG_DETAIL_HTSQL_VARIABLE = 1024,
	SESSION_LOG_DETAIL_DEBUGEE = 2048,
	SESSION_LOG_DETAIL_ALL = 0b111111111111  /** DO NOT FORGET TO SET THIS VALUE IF YOU ADD NEW OPTIONS ABOVE. Put X times 1 after 0b */
} SESSION_LOG_DETAIL;


typedef enum _sessionReadyStates {
	SESSION_READY_STATE_INIT,
	SESSION_READY_STATE_IN_PROGRESS,
	SESSION_READY_STATE_READY,
	SESSION_READY_STATE_BEING_KILLED,
	SESSION_READY_STATE_KILLED
} SESSION_READY_STATES;

typedef enum {
	SESSION_PRINT_NOTHING,
	SESSION_PRINT_RESPONSE_HEADER,
	SESSION_PRINT_HTML_CONTENT,
	SESSION_PRINT_CLIENT_LIBRARY
} SESSION_PRINT_STEP;

typedef struct _SodiumSession {

	HttpContext			* context;

	HANDLE              heapHandle;
	//HTTP_RESPONSE		response;
	HTSQLResponse		*response;

	LPEXTENSION_CONTROL_BLOCK lpECB;

	char* id;
	clock_t 				lastActivityTime;
	clock_t 				sessionCreationTime;
	SESSION_READY_STATES    sessionReadyState;
	int                     SessionLogDetail;
	long					pageProcessTime;
	SESSION_PRINT_STEP		whatIsPrinted;

	/* It is used in html parse phase */
	Function_Debug_Info		*functionDebugInfo;
	Session_Debug_Info		*sessionDebugInfo;
	Breakpoint_Node			breakpointsRoot[MAX_BREAKPOINT_COUNT];
	Breakpoint_Node*		breakpointsRootAddress;			/* "breakpointsRootAddress" is a pointer which points to the "breakpointsRoot" structure.
																	This kind of usage is the safest way for debugger since debugger must write only the
																	"breakpointsRoot" memory part of the session structure instead of
																	whole session structure.
																*/

	char* theme;

	/** \brief These variables hold the current count of the nested user function call and indention count.
	*
	*  Thery are used to generate formated debug information.
	*/
	int currentFunctionCallDepth;
	int debugIndentionCount;

	/**
	*  \brief These variables hold the connection definitions in a linked list
	*/
	TAGConnection* rootTAGConnection;
	TAGConnection* currentTAGConnection;
	TAGConnection* activeTAGConnection;	//	do not destroy this object. It is just a shortcut

	/**
	*  \brief These variables hold the session variables in a linked list
	*/
	PreObjectGeneric* rootSessionObjectGeneric;
	PreObjectGeneric* currentSessionObjectGeneric;

	/** \brief
	*
	*/
	HTSQLPage* rootPage;
	HTSQLPage* lastPage;
	HTSQLPage* currentPage;	//	Cannot be process more than one request from the same session. So, I set this variable for each request	
							//	this is required since we cannot access path info in lpECB structure

	int     languageCode;

	/** Going forward or backward in session linked list */
	struct _SodiumSession * prev;
	struct _SodiumSession* next;

} SodiumSession;


/** \brief Creates a new SodiumSession objects.
 *
 * \param clpECB LPEXTENSION_CONTROL_BLOCK
 * \return SodiumSession                        *
 *
 *  All initialized objects/variables should be free in destroySodiumSession function.
 */
SodiumSession* 
CreateSodiumSession(
	HttpContext* context,
	char* sid
);

SodiumSession*	
GetSodiumSession(
	HttpContext* context, 
	HTTP_KNOWN_HEADER cookieHeader
);

SodiumSession*
FindSodiumSessionBySessionId(
	const char* sessionId
);


/** \brief IIS ISAPI extension spesification dictates that all ISAPI extensions should have this function in a dll file and export it.
 *
 * \param pVer HSE_VERSION_INFO*
 * \return __declspec(dllexport) BOOL WINAPI
 *
 */
 //DLL_EXPORT  BOOL WINAPI                         GetExtensionVersion(HSE_VERSION_INFO* pVer);


HANDLE 
mkWaitTillPreviousRequestOfTheCurrentSessionEnds(
	SodiumSession* currentMKSession
);