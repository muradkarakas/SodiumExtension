// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#pragma comment(lib, "httpapi.lib")

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <http.h>
#include <winsock.h>

#include <windows.h>

#include <stdio.h>
#include <stdbool.h>
#include <heapapi.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <stdarg.h>
#include <httpext.h>
#include <ctype.h>
#include <malloc.h>
#include <sysinfoapi.h>
#include <errno.h>


#include "..\SodiumShared\SodiumShared.h"

#include "..\DBInt\db-interface.h"

#include "mkDataStructures.h"
#include "mkGlobals.h"
#include "mkMacros.h"
#include "mkConstants.h"
#include "mkFunctions.h"

#include "mkutils.h"
#include "mkError.h"

#include "mkSession.h"
#include "page.h"




#include "SodiumHttpServer.h"
#include "HttpServerToSodiumLangBridge.h"
#include "SodiumHttpServer.h"
#include "SodiumSession.h"


#include "mkapplication.h"

#include "breakpoints.imp.h"
#include "dispatcher.h"
#include "SodiumHttpServer.h"
#include "response.h"
#include "callStack.h"


SodiumSession*
CreateOrGetSession(
	HttpContext* context
	);