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