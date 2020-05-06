#ifndef PRE_IMP_H_INCLUDED
#define PRE_IMP_H_INCLUDED


#include "mkSession.h"
#include "mkutils.h"
#include "lemon.html.h"
#include "lemon.pre.h"
#include "page.h"

void    __setFunctionBody(SodiumSession *currentMKSession, HTSQLPage *page, PreObjectGeneric *currentGenericObject, char *pFunctionBody);
void    __setFunctionReturnVariable(SodiumSession *currentMKSession, HTSQLPage *page, PREOBJECTVARIABLETYPE pPreObjectVariableType);
void    __addFunctionParameterVariable(SodiumSession *currentMKSession, HTSQLPage *page, PREOBJECTVARIABLETYPE pPreObjectVariableType, char *pParameterName);


PreObjectVariable           *__addFunctionParameterVariableVariant(SodiumSession *currentMKSession, PREOBJECTVARIABLETYPE pPreObjectVariableType, char *pParameterName);
PreObjectVariable              *__addFunctionParameterVariableDate(SodiumSession *currentMKSession, PREOBJECTVARIABLETYPE pPreObjectVariableType, char *pParameterName);
PreObjectVariable           *__addFunctionParameterVariableVarchar(SodiumSession *currentMKSession, PREOBJECTVARIABLETYPE pPreObjectVariableType, char *pParameterName);
PreObjectVariable            *__addFunctionParameterVariableNumber(SodiumSession *currentMKSession, PREOBJECTVARIABLETYPE pPreObjectVariableType, char *pParameterName);


#endif // PRE_IMP_H_INCLUDED
