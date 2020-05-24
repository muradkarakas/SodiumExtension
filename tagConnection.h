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



TAGConnection       *createTAGConnection(SodiumSession *currentMKSession, HTSQLPage *page);
TAGConnection       *getRootTAGConnection1(SodiumSession *currentMKSession, HTSQLPage *page);
TAGConnection		*getActiveTAGConnection(SodiumSession *currentMKSession, HTSQLPage *page);
void				setActiveTAGConnection(SodiumSession *currentMKSession, HTSQLPage *page, TAGConnection *tagConnection);
DBInt_Connection	*getRootDBIntConnection(SodiumSession *currentMKSession, HTSQLPage *page);
TAGConnection       *getCurrentTAGConnection(SodiumSession *currentMKSession, HTSQLPage *page);
void                pushTAGConnection(SodiumSession *currentMKSession, TAGConnection *pTAGConnection);
void				destroyTAGConnection(SodiumSession *currentMKSession, TAGConnection *tagConnection);
void				setTAGConnectionPropertyUserName(SodiumSession *currentMKSession, TAGConnection *tagConnection, const char *pPropValue);
void				setCurrentTAGConnection(SodiumSession *currentMKSession, TAGConnection *tagConnection);
void				setTAGConnectionPropertyUserPassword(SodiumSession *currentMKSession, TAGConnection *tagConnection, const char *pPropValue);
void				setTAGConnectionPropertyInstanceName(SodiumSession *currentMKSession, TAGConnection *tagConnection, const char *pInstanceName);
void				setTAGConnectionPropertyConnectionName(SodiumSession *currentMKSession, TAGConnection *tagConnection, const char *pPropValue);
TAGConnection *		findTAGConnectionByName(SodiumSession *currentMKSession, HTSQLPage *page, const char *pPropValue, bool callLogonTrigger);
void				setTAGConnectionMKDBConnectionProperty(SodiumSession *currentMKSession, TAGConnection *tagConnection, DBInt_Connection *mkDBConnection);

