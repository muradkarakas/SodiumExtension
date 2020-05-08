#pragma once




PreObjectVariable    *createPreObjectVariable(SodiumSession *currentMKSession, PREOBJECTVARIABLETYPE pPreObjectVariableType, const char *pVariableName, const char *pPreObjectVariableValue);

PreObjectVariable    *createPreObjectVariableNumber(SodiumSession *currentMKSession, PREOBJECTVARIABLETYPE pPreObjectVariableType, const  char *pVariableName, int pPreObjectVariableValue);
PreObjectVariable    *createPreObjectVariableVarchar(SodiumSession *currentMKSession, PREOBJECTVARIABLETYPE pPreObjectVariableType, const  char *pVariableName, const char *pPreObjectVariableValue);
PreObjectVariable    *createPreObjectVariableDate(SodiumSession *currentMKSession, PREOBJECTVARIABLETYPE pPreObjectVariableType, const  char *pVariableName, const char *pPreObjectVariableValue);
PreObjectVariable    *createPreObjectVariableBool(SodiumSession *currentMKSession, PREOBJECTVARIABLETYPE pPreObjectVariableType, const char *pVariableName, const char *pPreObjectVariableValue);
PreObjectVariable    *createPreObjectVariableRedis(SodiumSession *currentMKSession, const char *pVariableName);
PreObjectVariable    *createPreObjectVariableResultSet(SodiumSession *currentMKSession, const char *pVariableName);

void				setPreObjectVariableValueVarchar(SodiumSession *currentMKSession, PreObjectVariable *preObjectVariable, const char *newStrValue);
void				setPreObjectVariableValueRedis(SodiumSession *currentMKSession, PreObjectVariable *preObjectVariable, redisContext *newConnectionInstance);
void				setPreObjectVariableValueRecordSet(SodiumSession *currentMKSession, PreObjectVariable *preObjectVariable, PostObjectResultSet *newRecordsetInstance);

void                  destroyPreObjectVariable(SodiumSession *currentMKSession, PreObjectVariable *object);



