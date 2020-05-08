#pragma once



void                destroyPreObjectGeneric(SodiumSession *currentMKSession, PreObjectGeneric *object);
PreObjectGeneric    *getCurrentPreGenericObject(SodiumSession *currentMKSession, HTSQLPage *page);
PreObjectGeneric    *findPreGenericObjectByName(SodiumSession *currentMKSession, HTSQLPage *page, const char *preGenericObjectName);
PreObjectGeneric	*createPreObjectGeneric(SodiumSession *currentMKSession, HTSQLPage *page, char *pObjectName, ObjectGenericType pObjectType, void *pPreObject);



