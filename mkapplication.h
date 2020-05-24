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


#include "SodiumSession.h"

wchar_t	 cDebugLogFileFullPath[MAX_PATH];
FILE* mkLogFile;

void                                             destroyMKApplicationHeapHandle();
void                                                 setMKApplicationHeapHandle();
int                                                           getMKSessionCount();
SodiumSession						*getMKSessionRootInstance();
SodiumSession									   * FindSodiumSessionById(char *pId);
void                            addMKSessionInstance(SodiumSession *currentMKSession);
void                        setMKSessionRootInstance(SodiumSession *currentMKSession);

void					init();