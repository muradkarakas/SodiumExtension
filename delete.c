#include "pch.h"

#include "delete.h"
#include "tagDataBlock.h"
#include "tagDataBlockDatabaseOperations.h"

void __delete(SodiumSession* currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock) {
    mkDebug(currentMKSession, SESSION_LOG_DETAIL_HTSQL_FUNCTION_CALL, "\n", "Delete is executing...", NULL);
    
	if (pTAGDataBlock) {
        if (pTAGDataBlock->keyColumnValue) {
            deleteDatablockCurrentRow(currentMKSession, page, pTAGDataBlock);
        }
    }
    mkDebug(currentMKSession, SESSION_LOG_DETAIL_HTSQL_FUNCTION_CALL, "\n", "Done. (Delete)", NULL);
}



