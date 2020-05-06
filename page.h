#pragma once


#include "htsql.h"




int                
GetPageCount(
    SodiumSession *session
);

HTSQLPage * 
GetCurrentPage(
    SodiumSession *session
);


HTSQLPage * 
CreateHTSQLPage(
    SodiumSession *session, 
    wchar_t* pageUri
);

HTSQLPage *
FindHTSQLPageByName(
    SodiumSession *session, 
    wchar_t* pageURI
);


/*	Cannot be process more than one request from the same session */
void
InitCurrentPage(
    SodiumSession* session,
    HTSQLPage* page
);

/*  Returns the current page's code behind file line number which is about to be executed */
int getPageCurrentLineNumberToBeExecuted(SodiumSession *currentMKSession, HTSQLPage *page);

void
AddHTSQLPage(
    SodiumSession* session,
    HTSQLPage* page
);