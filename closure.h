#pragma once

#include "htsql.h"

#include "mkSessionDataStructures.h"


void					destroyFunctionDebugInfo(SodiumSession *currentMKSession, HTSQLPage *page);

Function_Debug_Info*
CreateFunctionDebugInfo(
	SodiumSession* currentMKSession,
	HTSQLPage* page
);