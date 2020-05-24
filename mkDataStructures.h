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

#pragma once

#include <Windows.h>

typedef struct _PreObjectVariable		PreObjectVariable;
typedef struct _PreObjectFunction		PreObjectFunction;
typedef struct _preObjectResultSet		PostObjectResultSet;

/*	_UserObjectGeneric data structure is used to hold all user sqlx pre and post data structures.	*/
typedef struct _UserObjectGeneric		PreObjectGeneric; 
typedef struct _UserObjectGeneric		PostObjectGeneric;


typedef struct _Callable				CallStack;
typedef struct _Callable				Closure;


typedef struct _Session_Debug_Info		Session_Debug_Info;
typedef struct _Function_Debug_Info		Function_Debug_Info;
typedef struct _HTSQLPage				HTSQLPage;
typedef enum _PREOBJECTVARIABLETYPE		PREOBJECTVARIABLETYPE;

typedef struct _HTSQLResponse			HTSQLResponse;
typedef struct _ControlBlockVariable	ControlBlockVariable;

typedef struct structDataBlock			TAGDataBlock;
typedef struct structConnection			TAGConnection;
typedef struct structTAGGeneric			TAGGeneric;
typedef struct structInput				TAGInput;



struct Token {
	int        tokenCode;
	char       *tokenStr;
	DWORD tokenStrLength;
};
typedef struct _TokenPost				TokenPost;
typedef struct Token					TokenHtml;
typedef struct Token					TokenPre;

typedef enum _ObjectGenericType			ObjectGenericType;

typedef enum _Error_Codes				ERROR_CODES;

//typedef struct _mkSession				MKSession;
typedef enum _sql_command_type			SQL_COMMAND_TYPE;

typedef struct _DBInt_Statement			DBInt_Statement;
typedef enum _sessionLogDetail			SESSION_LOG_DETAIL;

typedef struct structDatalist			TAGDatalist;
typedef struct redisContext				redisContext;

typedef struct _Breakpoint				Breakpoint;
typedef struct _Breakpoint_Node			Breakpoint_Node;

#include "mKConstants.h"

#include "mkDebugDataStructures.h"
#include "lemon.pre.h"
#include "breakpoint.ds.h"
#include "preObjectVariableDataStructures.h"
#include "preObjectFunctionDataStructures.h"

#include "page.ds.h"

#include "preObjectGenericDataStructures.h"
#include "mkSessionDataStructures.h"
#include "tagConnectionDataStructures.h"
#include "responseDataStructures.h"

#include "tagControlBlockDataStructures.h"
#include "tagDataBlockDataStructures.h"
#include "tagDatalistDataStructures.h"
#include "tagGenericDataStructures.h"
#include "tagInputDataStructures.h"

#include "postObjectGenericDataStructures.h"
#include "postObjectResultSetDataStructures.h"

#include "post.PaeserDataStructures.h"
#include "mkErrorCodesDataStructures.h"

#include "mkSqlDataStructures.h"
#include "closureDataStructures.h"
