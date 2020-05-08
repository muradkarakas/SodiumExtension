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

