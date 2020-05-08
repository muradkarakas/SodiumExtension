#include "pch.h"

#include "prompt.h"



void  __prompt(SodiumSession  *currentMKSession, char *label, char *defaultValue) {
    char *lastResponse = mkStrcat(currentMKSession->heapHandle, __FILE__, __LINE__,
                                  "action = { "
                                                "\"type\"            : \"prompt\", "
                                                "\"label\"           : \"", label, "\", "
                                                "\"defaultValue\"    : \"", defaultValue, "\" "
                                          " };",
                                  // standart part
                                  "addNewActionToTheResponseActionList(action);",
                                  NULL);
    mkPrint(currentMKSession, lastResponse, NULL);
    mkFree(currentMKSession->heapHandle, lastResponse);
}
