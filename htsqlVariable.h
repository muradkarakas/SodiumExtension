/** \file   htsqlVariable.h
    \brief  Finds and returns all type of variables' values
*/

#pragma once

#include "htsql.h"


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

