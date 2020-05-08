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