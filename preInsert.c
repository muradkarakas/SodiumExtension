#include "stdafx.h"

#include "preInsert.h"
#include "htsqlFunction.imp.h"



bool runPreInsertHTSQLTriggerFunction(SodiumSession *session, HTSQLPage *page, TAGDataBlock *pTAGDataBlock)  {

    bool retval = false;
    char *functionToRun = mkStrcat(session->heapHandle, __FILE__, __LINE__,
                                   pTAGDataBlock->dataBlockName,
                                   trgName_DatablockPreInsert,
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
				CLOG(ERR_FUNCTION_MUST_RETURN_BOOL, trgName_DatablockPreInsert);
            }
        } else {
            // function declaration invalid
			CLOG(ERR_FUNCTION_MUST_RETURN_BOOL, trgName_DatablockPreInsert);
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
