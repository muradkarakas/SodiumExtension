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



