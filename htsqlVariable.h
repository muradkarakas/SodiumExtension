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

 /** \file   htsqlVariable.h
    \brief  Finds and returns all type of variables' values
*/

#pragma once




/** \brief     Caller is responsible for destroying return value.
 *
 * \param currentSodiumSession MKSession*
 * \param pVariableName const char*
 * \return char*
 * 
 *  __CAUTION : SOME PARTS OF THE FUNCTION CODE IS NOT IMPLEMENTED YET__
 *
 */
char* findAndReturnValueOfAVariableInAllScopes(SodiumSession *currentMKSession, HTSQLPage *page, const char *blockName, const char *itemName);


/** \brief __CALLER IS RESPONSIBLE TO RELEASE RETURN VALUE__
*
* \param currentSodiumSession MKSession*
* \param variableName const char*
* \return char*
*
*/ 
char *getVariableValue(SodiumSession *currentMKSession, HTSQLPage *page, const char *variableName);

/** \brief  __CALLER IS RESPONSIBLE TO RELEASE RETURN VALUE__
*
* \param currentSodiumSession MKSession*
* \param blockName const char*
* \param itemName const char*
* \return char*
*
*/
char *              getBlockVariableValue(SodiumSession *currentMKSession, HTSQLPage *page, const char *blockName, const char *itemName);
void                setBlockVariableValue(SodiumSession *currentMKSession, HTSQLPage *page, const char *blockName, const char *itemName, const char *newValue);


PreObjectVariable	*findRedisPreObjectVariable(SodiumSession *currentMKSession, HTSQLPage *page, const char *redisVariableName, bool isConnectionNotFoundTriggerBeExecuted);
redisContext		*findRedisContextVariable(SodiumSession *currentMKSession, HTSQLPage *page, const char *redisValueName);

PreObjectGeneric	*createSessionGenericObject(SodiumSession *currentMKSession, const char *pObjectName, ObjectGenericType pObjectType, void *pPreObject);
PreObjectGeneric	*findSessionGenericObject(SodiumSession *currentMKSession, const char *sessionVariableName);

