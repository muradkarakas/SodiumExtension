#include "stdafx.h"

#include "pageLoad.h"
#include "htsqlFunction.imp.h"


void __runHTSQLPageLoadFunction(SodiumSession *currentMKSession, HTSQLPage *page) {

	/*clock_t start, end;
	start = clock();
	char cBuf[100];*/

    char blockname[]                = "";

    //mkDebug(currentMKSession, SESSION_LOG_DETAIL_HTSQL_TRIGGER, "\n", "function ", trgName_PageLoad, " is called if exists ...", NULL);

    /* ###################################################################################################### */
    /* #########    HTSQL FUNCTION CALL      ################################################################ */
    /* ###################################################################################################### */
    /* Preparing to make a function call. Steps are explained in postObjectGeneric.h file. */
    /* Step.1 */    

    /* Step.2 */	createANewFunctionCallStackStartPoint(currentMKSession, page, trgName_PageLoad);

    /* Step.3 */    /* adding blockname of the item triggered the event which cause to run this code  */
                    postFunctionAddArgumentsOrVariables(currentMKSession, page, POST_OBJECT_GENERIC_TYPE_FUNCTION_ARGS, PREOBJECTTYPE_VARIABLE_VARCHAR, "datablockname", blockname);

    /* Step.4 */    /* iterating again for ObjectGeneric creation with "datablockname" obtained abowe  */

    /* Step.5 */    runPostFunction(currentMKSession, page, trgName_PageLoad, true);

    /* Step.6       If the procedure is a function, result is ready in currentMKSession->rootPostObjectGeneric->postObjectVariable now. */

    /* Step.7 */    cleanUpFunctionStack(currentMKSession, page);

    /* Step.8 */    

    /* ###################################################################################################### */
    /* #########    HTSQL FUNCTION CALL      ################################################################ */
    /* ###################################################################################################### */

	/*end = clock();
	mkConvertFloatToChar((end - start) / CLOCKS_PER_SEC, cBuf, 100);
    mkDebug(currentMKSession, SESSION_LOG_DETAIL_HTSQL_TRIGGER, "\n", "Done in (", cBuf, ") msec. '", trgName_PageLoad, "'", NULL); */
}
