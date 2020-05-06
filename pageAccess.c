#include "stdafx.h"

#include "pageAccess.h"
#include "htsqlFunction.imp.h"


bool runPageAccessHTSQLTriggerFunction(SodiumSession *session, HTSQLPage *page) {

    bool retval = false;

    /** Step.1 */

    /** Step.2 */
	createANewFunctionCallStackStartPoint(session, page, trgName_PageAccess);

    /** Step.3 */

    /** Step.4 */

    /** Step.5 */
    PreObjectFunction *preObjectFunction = runPostFunction(session, page, trgName_PageAccess, true);

    /** Step.6 */
    if (preObjectFunction) {
        /** If page has "page_access" function, we'll get its return value as the return value of this c function */
        if (page->currentCallStack->preObjectFunctionReturnValue) {
            if (page->currentCallStack->preObjectFunctionReturnValue->value.text) {
                retval  = (bool) (strcmp(page->currentCallStack->preObjectFunctionReturnValue->value.text, "true") == 0);
            } else {
				CLOG(ERR_FUNCTION_MUST_RETURN_BOOL, trgName_PageAccess);
            }
        } else {
            // Function declaration is invalid
			CLOG(ERR_FUNCTION_MUST_RETURN_BOOL, trgName_PageAccess);
        }
    } else {
        /** page does not have "page_access" function. this means it is a public function */
        retval = true;
    }

    /** Step.7 */
    cleanUpFunctionStack(session, page);

    /** Step.8 */

    return retval;
}
