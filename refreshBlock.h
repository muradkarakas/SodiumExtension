#pragma once


#include "htsql.h"


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

