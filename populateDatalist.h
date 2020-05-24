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




void  __sendPopulateDatalistResponse(SodiumSession *currentMKSession, HTSQLPage *page, char *dataListName, bool cascadeOption);
void  __sendPopulateDatalistWithRecordSetResponse(SodiumSession *currentMKSession, HTSQLPage *page, char *dataListName, char *recordSetName);

void  __populateDatalistWithRecordSet(SodiumSession *currentMKSession, HTSQLPage *page, TAGDatalist *tagDatalist, PostObjectResultSet *postObjectResultSet);
void  __populateDatalist(SodiumSession *currentMKSession, HTSQLPage *page, TAGDatalist *current, bool cascadeOption);

char *getDatalistOptionString(SodiumSession *currentMKSession, TAGDatalist *current, DBInt_Connection *mkDBConnection, DBInt_Statement *stm);


