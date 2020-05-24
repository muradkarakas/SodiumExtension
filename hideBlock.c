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
