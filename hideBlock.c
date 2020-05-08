#include "pch.h"

#include "hideBlock.h"
#include "tagDataBlock.h"

void __sendHideBlockResponse(SodiumSession *currentMKSession, HTSQLPage *page, char *pBlockName, bool cascade) {

    TAGDataBlock *dataBlock = findTAGDataBlockByName(currentMKSession, page, pBlockName);

    if (dataBlock) {
        setTAGDataBlockPropertyVisibility(currentMKSession, page, dataBlock, false);
    }

    char *lastResponse = mkStrcat(currentMKSession->heapHandle,
                                  __FILE__, __LINE__,
                                  "action = { "
                                                "\"type\"             : \"hideBlock\", "
                                                "\"dataBlockName\"    : \"", pBlockName, "\", "
                                                "\"cascade\"          : \"", (cascade) ? "true":"false", "\", "
                                          " };",
                                  // standart part
                                  "addNewActionToTheResponseActionList(action);",
                                  NULL);
    mkPrint(currentMKSession, lastResponse, NULL);
    mkFree(currentMKSession->heapHandle, lastResponse);
}
