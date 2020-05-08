#include "pch.h"

#include "refreshTreeNode.h"

void  __sendRefreshTreeNodeResponse(SodiumSession *currentMKSession, HTSQLPage *page, const char *treename, const char *nodeid) {
   char *lastResponse = mkStrcat(currentMKSession->heapHandle,
                                  __FILE__, __LINE__,
                                  "action = { "
                                                "\"type\"       : \"refreshTreeNode\", "
                                                "\"treename\"   : \"", treename, "\", "
                                                "\"nodeid\"     : \"", nodeid, "\", "
                                          " };",
                                  // standart part
                                  "addNewActionToTheResponseActionList(action);",
                                  NULL);
    mkPrint(currentMKSession, lastResponse, NULL);
    mkFree(currentMKSession->heapHandle, lastResponse);
}
