#pragma once




void  __sendPopulateDatalistResponse(SodiumSession *currentMKSession, HTSQLPage *page, char *dataListName, bool cascadeOption);
void  __sendPopulateDatalistWithRecordSetResponse(SodiumSession *currentMKSession, HTSQLPage *page, char *dataListName, char *recordSetName);

void  __populateDatalistWithRecordSet(SodiumSession *currentMKSession, HTSQLPage *page, TAGDatalist *tagDatalist, PostObjectResultSet *postObjectResultSet);
void  __populateDatalist(SodiumSession *currentMKSession, HTSQLPage *page, TAGDatalist *current, bool cascadeOption);

char *getDatalistOptionString(SodiumSession *currentMKSession, TAGDatalist *current, DBInt_Connection *mkDBConnection, DBInt_Statement *stm);


