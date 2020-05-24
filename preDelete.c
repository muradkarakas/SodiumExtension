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

#include "preDelete.h"
#include "preObjectFunction.h"
#include "htsqlFunction.imp.h"



bool runPreDeleteHTSQLTriggerFunction(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGDataBlock)  {

    bool retval = false;
    char *functionToRun = mkStrcat(session->heapHandle, __FILE__, __LINE__,
                                   pTAGDataBlock->dataBlockName,
                                   trgName_DatablockPreDelete,
                                   NULL);

    /** Step.1 */


    /** Step.2 */
	createANewFunctionCallStackStartPoint(session, page, functionToRun);

    /** Step.3 */
    //postFunctionAddArgumentsOrVariables(session, variableName, parameter);

    /** Step.4 */

    /** Step.5 */
    PreObjectFunction *preObjectFunction = runPostFunction(session, page, functionToRun, true);

    /** Step.6 */
    if (preObjectFunction) {
        /** If trigger function is exists, we'll get its return value as the return value of this c function */
        if (page->currentCallStack->preObjectFunctionReturnValue) {
            if (page->currentCallStack->preObjectFunctionReturnValue->value.text) {
                retval  = (bool) (strcmp(page->currentCallStack->preObjectFunctionReturnValue->value.text, "true") == 0);
            } else {
				CLOG(ERR_FUNCTION_MUST_RETURN_BOOL, trgName_DatablockPreDelete);
            }
        } else {
            // Function declaration invalid
			CLOG(ERR_FUNCTION_MUST_RETURN_BOOL, trgName_DatablockPreDelete);
        }
    } else {
        /** No trigger function found.  */
        retval = true;
    }

    /** Step.7 */
    cleanUpFunctionStack(session, page);

    /** Step.8 */

    mkFree(session->heapHandle, functionToRun);
    return retval;
}
