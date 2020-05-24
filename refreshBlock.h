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





void __sendRefreshBlockHTSQLResponse(SodiumSession *currentMKSession, HTSQLPage *page, char *blockname);
void __sendRefreshBlockNativeResponse(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *dataBlockInstance, const char *cFunctionNameToBeCalled);
void __sendNextPageNativeResponse(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock, const char *cFunctionNameToBeCalled);
void __sendPrevPageNativeResponse(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock, const char *cFunctionNameToBeCalled);
void __sendFirstPageNativeResponse(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock, const char *cFunctionNameToBeCalled);
void __sendLastPageNativeResponse(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock, const char *cFunctionNameToBeCalled);
void __sendDeleteNativeResponse(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock, const char *cFunctionNameToBeCalled);
void __sendCreateRecordNativeResponse(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock, const char *cFunctionNameToBeCalled);
void __sendExecuteQueryNativeResponse(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *dataBlockInstance, const char *cFunctionNameToBeCalled);
bool runDatablockAccessHTSQLTriggerFunction(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *dataBlockInstance);
void __sendDatablockAccessRejectedResponse(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *dataBlockInstance);

