#include "stdafx.h"

#include "disableColumn.h"
#include "tagDataBlock.h"
#include "tagInput.h"

void  __disableColumn(SodiumSession *currentMKSession, HTSQLPage *page, const char *pItemName) {
    char *lItemName = mkStrdup(currentMKSession->heapHandle, pItemName, __FILE__, __LINE__);
    char *point = strstr(lItemName, ".");
    if (point) {
        *point = '\0';
    }
    char *blockName = lItemName;
    char *itemName  = point + 1;
	TAGDataBlock *tagDataBlock = findTAGDataBlockByName(currentMKSession, page, blockName);
    TAGInput *input = findTAGInputByName(currentMKSession, page, tagDataBlock, itemName);

    if (input) {
        setTAGInputPropertyEnabled(currentMKSession, page, tagDataBlock, itemName, false);

        char *lastResponse = mkStrcat(currentMKSession->heapHandle,
                                      __FILE__, __LINE__,
                                      "action = { "
                                                    "\"type\"             : \"disableColumn\", "
                                                    "\"itemToDisable\"    : \"", pItemName, "\", "
                                              " };",
                                      /* standart part */
                                      "addNewActionToTheResponseActionList(action);",
                                      NULL);
        mkPrint(currentMKSession, lastResponse, NULL);
        mkFree(currentMKSession->heapHandle, lastResponse);
    }

    mkFree(currentMKSession->heapHandle, lItemName);
}
