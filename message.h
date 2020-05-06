#pragma once

#include "htsql.h"


void __sendMessageResponse(SodiumSession *currentMKSession, HTSQLPage *page, char *pMessage);

void __sendMessage3Response(SodiumSession* session, HTSQLPage* page, char* pTitle, char* pMessage, char* pType);


