#include "pch.h"

#include "showPage.h"


void  __sendShowPageResponse(SodiumSession *currentMKSession, HTSQLPage *page, const char *pageURL) {
   char *lastResponse = mkStrcat(currentMKSession->heapHandle, __FILE__, __LINE__,
                                  "action = { "
                                                "\"type\"       : \"showPage\", "
                                                "\"pageURL\"    : \"", pageURL, "\", "
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
