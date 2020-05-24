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
