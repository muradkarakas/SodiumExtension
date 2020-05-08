#pragma once






void				destroyPreObjectFunction(SodiumSession *currentMKSession, PreObjectFunction  *pObject);
PreObjectFunction   *createPreObjectFunction(SodiumSession *currentMKSession);
void                setFunctionReturnVariable(SodiumSession *currentMKSession, HTSQLPage *page, PreObjectGeneric *currentGenericObject, PreObjectVariable *preObjectVariable);
void                addFunctionParameterVariable(SodiumSession *currentMKSession, HTSQLPage *page, PreObjectGeneric *currentGenericObject, PreObjectVariable *preObjectVariable);


