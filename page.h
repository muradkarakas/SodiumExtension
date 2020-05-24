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