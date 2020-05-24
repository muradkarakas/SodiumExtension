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

#include "delete.h"
#include "tagDataBlock.h"
#include "tagDataBlockDatabaseOperations.h"

void __delete(SodiumSession* currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock) {
    mkDebug(currentMKSession, SESSION_LOG_DETAIL_HTSQL_FUNCTION_CALL, "\n", "Delete is executing...", NULL);
    
	if (pTAGDataBlock) {
        if (pTAGDataBlock->keyColumnValue) {
            deleteDatablockCurrentRow(currentMKSession, page, pTAGDataBlock);
        }
    }
    mkDebug(currentMKSession, SESSION_LOG_DETAIL_HTSQL_FUNCTION_CALL, "\n", "Done. (Delete)", NULL);
}



