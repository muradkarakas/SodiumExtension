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




typedef enum _PREOBJECTVARIABLETYPE {
	PREOBJECTTYPE_VARIABLE_VARCHAR = PRE_VARIABLE_TYPE_VARCHAR, 
	PREOBJECTTYPE_VARIABLE_BOOL = PRE_VARIABLE_TYPE_BOOL,
	PREOBJECTTYPE_VARIABLE_NUMBER = PRE_VARIABLE_TYPE_NUMBER,
	PREOBJECTTYPE_VARIABLE_DATE = PRE_VARIABLE_TYPE_DATE,
	PREOBJECTTYPE_VARIABLE_REDIS = PRE_VARIABLE_TYPE_REDIS, 
	PREOBJECTTYPE_VARIABLE_VOID = PRE_VARIABLE_TYPE_VOID,
	PREOBJECTTYPE_VARIABLE_RESULTSET
} PREOBJECTVARIABLETYPE;


typedef struct _PreObjectVariable {
	PREOBJECTVARIABLETYPE      preObjectVariableType;
	char                               *variableName;
	union {
		char                                  *text;
		int                                  number;
		char                                  *date;
		char							 *boolValue;
		redisContext             *redisContextValue;
		PostObjectResultSet	   *postObjectResultSet;
	} value;
} PreObjectVariable;


