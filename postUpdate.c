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

#include "postUpdate.h"
#include "tagDataBlock.h"
#include "htsqlFunction.imp.h"
#include "postObjectGeneric.h"


bool runPostUpdateHTSQLTriggerFunction(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock) {
    bool retval = false;
    char *functionToRun = mkStrcat(currentMKSession->heapHandle, __FILE__, __LINE__,
                                   pTAGDataBlock->dataBlockName,
                                   trgName_DatablockPostUpdate,
                                   NULL);

    /** Step.1 */

    /** Step.2 */
	createANewFunctionCallStackStartPoint(currentMKSession, page, functionToRun);

    /** Step.3 */
    //postFunctionAddArgumentsOrVariables(currentMKSession, variableName, parameter);

    /** Step.4 */

    /** Step.5 */
    PreObjectFunction *preObjectFunction = runPostFunction(currentMKSession, page, functionToRun, true);

    /** Step.6 */
    if (preObjectFunction) {
        /** data block has "post_update" trigger function. */
        retval = true;
    } else {
        /** data block does not have "post_update" trigger function. */
        retval = false;
    }

    /** Step.7 */
    cleanUpFunctionStack(currentMKSession, page);

    /** Step.8 */

    mkFree(currentMKSession->heapHandle, functionToRun);
    return retval;
}



