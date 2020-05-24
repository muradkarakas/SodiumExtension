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