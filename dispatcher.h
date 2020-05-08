#pragma once


void            dispatchRequest(SodiumSession * session);
void            setRequestedFileName(SodiumSession *currentMKSession, HTSQLPage *page);

void
SetRequestDispatcherType(
    SodiumSession* session,
    HTSQLPage* page
);


const char *    
GetQueryStringParam(
    SodiumSession *session, 
    HTSQLPage *page, 
    const char *paramName
);


void
SetHttpVerb(
    SodiumSession* session,
    HTSQLPage* page
);

void
ParseQueryStringParams(
    SodiumSession* session,
    HTSQLPage* page
);

void
FreeQueryStringParams(
    SodiumSession* session,
    HTSQLPage* page
);