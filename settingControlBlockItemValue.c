#include "pch.h"

#include "settingControlBlockItemValue.h"



void __sendSetControlBlockItemValueResponse(SodiumSession *currentMKSession, const char *itemName, const char *itemValue) {
    char *lastResponse = mkStrcat(currentMKSession->heapHandle, __FILE__, __LINE__,
                                  "action = { ",
                                                "\"type\"            : \"setControlBlockItemValue\", ",
                                                "\"itemName\"        : \"", (itemName) ? itemName:"", "\", ",
                                                "\"itemValue\"       : \"", (itemValue) ? itemValue:"", "\" "
                                            " };",
                                  "addNewActionToTheResponseActionList(action);",
                                  NULL);
    mkPrint(currentMKSession, lastResponse, NULL);
    mkFree(currentMKSession->heapHandle, lastResponse);
}

