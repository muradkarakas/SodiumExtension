#include "pch.h"

#include "settingDataBlockItemValue.h"


void __sendSetDataBlockItemValueResponse(SodiumSession *currentMKSession, const char *blockName, const char *rowid, const char *itemName, const char *itemValue, bool simulateClientModification) {
    char *lastResponse = mkStrcat(currentMKSession->heapHandle, __FILE__, __LINE__,
                                  "action = { ",
                                                "\"type\"            : \"setDataBlockItemValue\", ",
                                                "\"dataBlockName\"   : \"", blockName, "\", ",
                                                "\"rowid\"           : \"", rowid, "\", ",
                                                "\"itemName\"        : \"", itemName, "\", ",
                                                "\"itemValue\"       : \"", itemValue, "\", ",
                                                "\"simulateClientModification\" : ", (simulateClientModification) ? "\"Y\"": "\"N\"", ", ",
                                            " };",
                                  "addNewActionToTheResponseActionList(action);",
                                  NULL);
    mkPrint(currentMKSession, lastResponse, NULL);
    mkFree(currentMKSession->heapHandle, lastResponse);
}
