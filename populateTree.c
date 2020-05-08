#include "pch.h"

#include "populateTree.h"

void  __sendPopulateTreeResponse(SodiumSession *currentMKSession, HTSQLPage *page, const char *treeName)  {
    char *lastResponse = mkStrcat(currentMKSession->heapHandle,
                                  __FILE__, __LINE__,
                                  "action = { "
                                                "\"type\"        : \"populateTree\", "
                                                "\"treename\"    : \"", treeName, "\", "
                                          " };",
                                  // standart part
                                  "addNewActionToTheResponseActionList(action);",
                                  NULL);
    if (page->currentRequestMethodType == REQUEST_TYPE_GET) {
        mkPrint(currentMKSession, "\n<script type=\"text/javascript\">\n", NULL);
    }
    mkPrint(currentMKSession, lastResponse, NULL);
    if (page->currentRequestMethodType == REQUEST_TYPE_GET) {
        mkPrint(currentMKSession, "\n</script>\n", NULL);
    }
    mkFree(currentMKSession->heapHandle, lastResponse);
}
