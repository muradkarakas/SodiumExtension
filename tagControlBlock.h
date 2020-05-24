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




void                    destoryControlBlockVariables(SodiumSession *currentMKSession, HTSQLPage *page);
void                    createControlBlockVariable(SodiumSession *currentMKSession, HTSQLPage *page, const char *cbName, const char *cbValue);
ControlBlockVariable	*findControlBlockVariableByName(SodiumSession *currentMKSession, HTSQLPage *page, const char *cbName);
void					setControlBlockVariableValue(SodiumSession *currentMKSession, HTSQLPage *page, ControlBlockVariable *cbVariableInstance, const char *value);

