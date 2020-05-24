/**
 * This file is part of Sodium Language project
 *
 * Copyright © 2020 Murad Karakaþ <muradkarakas@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License v3.0
 * as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 *	https://choosealicense.com/licenses/gpl-3.0/
 */

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
