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

#include "hideColumn.h"
#include "tagDataBlock.h"
#include "tagInput.h"


void  __hideColumn(SodiumSession *currentMKSession, HTSQLPage *page, const char *pItemName) {
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
        setTAGInputPropertyVisibility(currentMKSession, page, tagDataBlock, itemName, false);
        char *lastResponse = mkStrcat(currentMKSession->heapHandle,
                                      __FILE__, __LINE__,
                                      "action = { "
                                                    "\"type\"             : \"hideColumn\", "
                                                    "\"itemToHide\"       : \"", pItemName, "\", "
                                              " };",
                                      // standart part
                                      "addNewActionToTheResponseActionList(action);",
                                      NULL);
        mkPrint(currentMKSession, lastResponse, NULL);
        mkFree(currentMKSession->heapHandle, lastResponse);
    }

    mkFree(currentMKSession->heapHandle, lItemName);
}
