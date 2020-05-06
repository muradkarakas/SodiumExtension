#include "HTSQLFunction.h"


void callUserFunction(MKSession *currentMKSession, HTSQLPage *page, const char *functionToRun, int parameterCount, ...) {
    va_list parameterList;
    va_start(parameterList, parameterCount);

    PreObjectGeneric *preObjectGeneric = findPreGenericObjectByName(currentMKSession, page, functionToRun);
    if (preObjectGeneric) {

        // We have a user function. Lets check whether Parameter Count is correct or not
        if (preObjectGeneric->preObjectFunction->parameterCount == parameterCount) {
            /** Step.1 */
            PostObjectGeneric *prevRootPostObjectGeneric    = page->rootPostObjectGeneric;
            PostObjectGeneric *prevCurrentPostObjectGeneric = page->currentPostObjectGeneric;

            /** Step.2 */
            page->rootPostObjectGeneric         = createANewFunctionCallStackStartPoint(currentMKSession, page, functionToRun);

            /** Step.3 */
            for(int i = 0; i < parameterCount; i++) {
                char *parameter = va_arg(parameterList, char* );
                postFunctionAddArgumentsOrVariables(currentMKSession,
                                                    page,
                                                    preObjectGeneric->preObjectFunction->preObjectFunctionParameters[i]->variableName,
                                                    parameter);
            }

            /** Step.4 */

            /** Step.5 */
            runPostFunction(currentMKSession, page, functionToRun, false);
            page->lastHTSQLFunctionGenericInstanceExecuted = preObjectGeneric;

            /** Step.6 */

            /** Step.7 */
            cleanUpFunctionStack(currentMKSession, page);

            /** Step.8 */
            page->rootPostObjectGeneric     = prevRootPostObjectGeneric;
            page->currentPostObjectGeneric  = prevCurrentPostObjectGeneric;

        } else {
            // Parameter count in function call is invalid
            __sendErrorResponse(currentMKSession, page, ETEXT(ERR_PARAMETER_COUNT_IN_FUNCTION_CALL_INVALID), functionToRun);
        }

    } else {
        // Function not found
        __sendErrorResponse(currentMKSession, page, ETEXT(ERR_FUNCTION_NOT_FOUND), functionToRun);
    }

    va_end(parameterList);
}

void setCurrentUserFunctionReturnValue(MKSession *currentMKSession, HTSQLPage *page, const char *retValue) {

    PreObjectGeneric *ptr = page->currenHTSQLFunctionGenericInstance; //getCurrentUserFunctionInstance(currentMKSession);

    if (ptr->preObjectFunction->preObjectFunctionReturnValue) {
        setPreObjectVariableValueVarchar(currentMKSession, ptr->preObjectFunction->preObjectFunctionReturnValue, retValue);
    } else {
        __sendErrorResponse(currentMKSession, page, ETEXT(ERR_FUNCTION_DECLARATION_DOES_NOT_RETURN_VALUE), ptr->objectName);
    }
}

PreObjectVariable *getCurrentUserFunctionReturnValue(MKSession *currentMKSession, HTSQLPage *page) {
    PreObjectGeneric *ptr = page->currenHTSQLFunctionGenericInstance; //getCurrentUserFunctionInstance(currentMKSession);
    return ptr->preObjectFunction->preObjectFunctionReturnValue;
}





