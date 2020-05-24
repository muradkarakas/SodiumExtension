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

 /** \file   postObjectGeneric.h

    \brief  Contains functions and data structures for creating, finding, destroying of post phase variables.

    Post phase variables are the local variable.
    They are created before executing HT/SQL or Native function call from the browser and destroyed after HT/SQL or Native function execution ended.
*/

#pragma once



/*	Caller is responsible to release memory which pointed by object. Function do not release object. Just release properties of object	*/
void				destroyPostObjectGeneric(SodiumSession *currentMKSession, PostObjectGeneric *object);
PostObjectGeneric	*getCurrentPostObjectGeneric(SodiumSession *currentMKSession);
PreObjectVariable	*findPostObjectGenericPostObjectVariable(SodiumSession *currentMKSession, HTSQLPage *page, const char *pObjectName);
PostObjectResultSet *findPostObjectGenericPostResultSetVariable(SodiumSession *currentMKSession, HTSQLPage *page, const char *pObjectName);
PostObjectGeneric	*findPostObjectGenericObject(SodiumSession *currentMKSession, HTSQLPage *page, const char *pObjectName);
PostObjectGeneric	*createPostObjectGeneric(SodiumSession *currentMKSession, HTSQLPage *page, ObjectGenericType postObjectType, const char *pObjectName, void *pPreObject);
void				setPostObjectGenericVariableValueVarchar(SodiumSession *currentMKSession, PreObjectVariable *postObjectVariable, const char *newStrValue);
