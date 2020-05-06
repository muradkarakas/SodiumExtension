#pragma once

#include "mkConstants.h"

#include "tagControlBlockDataStructures.h"
#include "tagDatalistDataStructures.h"
#include "tagDataBlockDataStructures.h"
#include "postObjectGenericDataStructures.h"


typedef enum _REQUEST_TYPE {
	REQUEST_TYPE_UNKNOWN,
	REQUEST_TYPE_GET,
	REQUEST_TYPE_POST
} REQUEST_TYPE;

typedef enum _REQUEST_DISPATCHER_TYPE {
	REQUEST_DISPATCHER_TYPE_SMALL,
	REQUEST_DISPATCHER_TYPE_BIG
} REQUEST_DISPATCHER_TYPE;

typedef struct _HTSQLPage {

	wchar_t                 *fileNameRequested;
	char 					nextCommandStrToBeExecuted[MAX_FUNCTION_NAME_LENGTH];

	REQUEST_TYPE            currentRequestMethodType;
	REQUEST_DISPATCHER_TYPE currentRequestDispatcherType;

	struct yuarel_param *   formDataStructureSmall;
	//char *                  formDataBufferSmallPOST;
	char *                  formDataBufferSmallGET;
	int                     totalHTTPFormVariableCount;

	/**
	*  \brief  These variables are used to parse html contents of the data block element correctly.
	*
	*/
	int bInDataBlock;
	int bInTable;
	int bInTableHead;
	int bInTableFoot;
	int bInTableBody;
	int bInTableTr;
	int bInTableTd;
	int bInTableTh;
	int bInInput;

	/**
	*  \brief  These variables hold control block definitions in a linked list.
	*
	*          Current release parses and captures the control block definitions in *.frmx file but do not creates corresponding objects for them.
	*          Does some semantic and syntax check.
	*/
	ControlBlockVariable *rootCBVariable;
	ControlBlockVariable *currentCBVariable;

	/**
	*  \brief These variables hold the datalist definitions in a linked list
	*/
	TAGDatalist *rootTAGDatalist;
	TAGDatalist *currentTAGDatalist;

	/**
	*  \brief These variables hold the data block definitions in a linked list
	*/
	TAGDataBlock *rootTAGDataBlock;
	TAGDataBlock *currentTAGDataBlock;

	/**
	*  \brief These variables hold the global variables and functions in a linked list
	*/
	PreObjectGeneric *rootPreObjectGeneric;
	PreObjectGeneric *lastPreObjectGeneric;

	/**
	*  \brief These variable holds closure as a CallStack linked list.
	*/
	CallStack		  *currentCallStack;

	/**
	*  \brief  These variables are used for <script type="sqlx" src="*.sqlx></script>.
	*          After </script> tag, this variable contents has been released.
	*/
	char  *currentScriptType;
	wchar_t	*currentScriptFile;

	/** \brief Look at the detail below.
	*
	*   These variable holds; \n
	*   *    the user function instance which is being currently executed. \n
	*   *    the linked list for call stack. \n
	*
	*   In order to access caller of the current user function use code below.\n
	*
	*   >   PreObjectGeneric *callerUserFunction = currentMKSession->currenHTSQLFunctionGenericInstance->preObjectFunction->callerUserFunction;
	*
	*   And after that
	*   >   PreObjectGeneric *CallerOfCaller = callerUserFunction->preObjectFunction->callerUserFunction;
	*   can be used to access all caller functions. \n
	*   \n
	*   This example is given just for simplify how this structure works. Best way is "looping" or "iteration" until reaching the NULL value.
	*/
	//Closure *currentClosureBeingExecuted;

	/**  Used to stop parsing process (for html, pre and post) in case of error */
	BOOL             stopParsing;

	/** Used to escape \n and " character in java script which is produced actually for html output. Hard to explain :( */
	BOOL isJavaScriptEscapeEnabled;

	char *currentDatablockName;
	char *currentRowId;

	/** This variable is used to get database related error message which occurs in lower functions */
	char *clientMessage;

	/** Used to parse nested "if", "else" and "end if" keywords. Used in lexer not grammer file. */
	int                 ifCounter;

	BOOL conditionEvaluationResult;

	struct _HTSQLPage *next;
	struct _HTSQLPage *prev;

} HTSQLPage;
