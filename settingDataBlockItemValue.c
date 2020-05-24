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
