/**
 * This file is part of Sodium Language project
 *
 * Copyright � 2020 Murad Karaka� <muradkarakas@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License v3.0
 * as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 *	https://choosealicense.com/licenses/gpl-3.0/
 */

#pragma once

#include "SodiumSession.h"

extern FILE				*mkLogFile;
CRITICAL_SECTION        htsqlCriticalSection;
extern const char *		stdHeadTAGContent;

extern const char		*stdHeaderForHtmlPageRequest;
extern const char		*stdHeaderForAjaxRequest;
extern HANDLE			mkApplicationHeapHandle;
extern SodiumSession	*mkRootSessionInstance;
extern SodiumSession	*mkCurrentMKSessionInstance;
extern SodiumSession	*mkMKSessionInstanceDebugged;
extern const char		*sessionEndReasonDescriptions[];