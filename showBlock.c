#include "pch.h"

#include "showBlock.h"
#include "tagDataBlock.h"


void __sendShowBlockResponse(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock, bool cascade) {

	if (pTAGDataBlock == NULL) {
		return;
	}
    
	setTAGDataBlockPropertyVisibility(currentMKSession, page, pTAGDataBlock, true);
    
    char *lastResponse = mkStrcat(currentMKSession->heapHandle,
                                  __FILE__, __LINE__,
                                  "action = { "
                                                "\"type\"             : \"showBlock\", "
                                                "\"dataBlockName\"    : \"", pTAGDataBlock->dataBlockName, "\", "
                                                "\"cascade\"          : \"", (cascade) ? "true":"false", "\", "
                                          " };",
                                  // standart part
                                  "addNewActionToTheResponseActionList(action);",
                                  NULL);
    mkPrint(currentMKSession, lastResponse, NULL);
    mkFree(currentMKSession->heapHandle, lastResponse);
}
