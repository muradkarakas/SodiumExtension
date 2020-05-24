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




TAGDatalist *                         getCurrentTAGDatalist(SodiumSession *currentMKSession, HTSQLPage *page);
TAGDatalist *                             createTAGDatalist(SodiumSession *currentMKSession, HTSQLPage *page);
void                                        pushTAGDatalist(SodiumSession *currentMKSession, HTSQLPage *page, TAGDatalist *pTAGDatalist);
void                                  setCurrentTAGDatalist(SodiumSession *currentMKSession, HTSQLPage *page, TAGDatalist *tagDatalist);
void                                     destroyTAGDatalist(SodiumSession *currentMKSession, HTSQLPage *page, TAGDatalist *tagDatalist);
TAGDatalist *                         findTAGDatalistByName(SodiumSession *currentMKSession, HTSQLPage *page, const char *pDatalistName);

void                 setTAGDatalistPropertyId(SodiumSession *currentMKSession, HTSQLPage *page, char *pPropValue);

void     setTAGDatalistPropertyConnectionName(SodiumSession *currentMKSession, HTSQLPage *page, char *pPropValue);

