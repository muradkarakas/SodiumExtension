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