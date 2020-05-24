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

#include "userHTSQLFunction.h"

#include "preObjectGeneric.h"
#include "htsqlFunction.imp.h"
#include "preObjectVariable.h"
 
PreObjectGeneric *callUserFunction(SodiumSession *session, HTSQLPage *page, const char *functionToRun, int parameterCount, ...) {
    va_list parameterList;
    va_start(parameterList, parameterCount);

    PreObjectGeneric *preObjectGeneric = findPreGenericObjectByName(session, page, functionToRun);
    if (preObjectGeneric) {

        // We have a user function. Lets check whether Parameter Count is correct or not
        if (preObjectGeneric->value.preObjectFunction->parameterCountDeclaration == parameterCount) {
            /** Step.1 */ 

            /** Step.2 */
			createANewFunctionCallStackStartPoint(session, page, functionToRun);

            /** Step.3 */
            for(int i = 0; i < parameterCount; i++) {
                char *parameter = va_arg(parameterList, char* );
                postFunctionAddArgumentsOrVariables(session,
                                                    page, 
													POST_OBJECT_GENERIC_TYPE_FUNCTION_ARGS,
													PREOBJECTTYPE_VARIABLE_VARCHAR,
                                                    preObjectGeneric->value.preObjectFunction->preObjectFunctionParametersDeclaration[i]->variableName,
                                                    parameter);
            }
			 
            /** Step.4 */

            /** Step.5 */
            runPostFunction(session, page, functionToRun, false);
			 
            /** Step.6 */

            /** Step.7 */
            cleanUpFunctionStack(session, page);

            /** Step.8 */

        } else {
            // Parameter count in function call is invalid
			CLOG(ERR_PARAMETER_COUNT_IN_FUNCTION_CALL_INVALID, functionToRun);
        }

    } else {
        // Function not found
		CLOG(ERR_FUNCTION_NOT_FOUND, functionToRun);
    }

    va_end(parameterList);
	return preObjectGeneric; 
} 

void setCurrentUserFunctionReturnValue(SodiumSession *session, HTSQLPage *page, const char *retValue) {
    if (page->currentCallStack->preObjectFunction->preObjectFunctionReturnValueDeclaration) {
		PreObjectVariable *variable = NULL;

		if (page->currentCallStack->preObjectFunctionReturnValue == NULL) {
			page->currentCallStack->preObjectFunctionReturnValue = createPreObjectVariable(session, PREOBJECTTYPE_VARIABLE_VARCHAR, "functionReturnValue", NULL);
		}
		  
		// I always set the current closure return value
		setPreObjectVariableValueVarchar(session, page->currentCallStack->preObjectFunctionReturnValue, retValue);

		if (page->currentCallStack->prev) {
			// Some HT/SQL functions are called by HT/SQL engine (like page.access, page.load trigger), not by another HT/SQL function. 
			// If that is the case, there is no previous Closure instance. 
			// Return value will be stored calling Closure's variable.
			if (page->currentCallStack->prev->preObjectFunctionReturnValue == NULL) {
				page->currentCallStack->prev->preObjectFunctionReturnValue = createPreObjectVariable(session, PREOBJECTTYPE_VARIABLE_VARCHAR, "functionReturnValue", NULL);
			}
			setPreObjectVariableValueVarchar(session, page->currentCallStack->prev->preObjectFunctionReturnValue, retValue);
		}

	} else {
		CLOG(ERR_FUNCTION_DECLARATION_DOES_NOT_RETURN_VALUE, page->currentCallStack->closureName);
    }
}

PreObjectVariable *getCurrentUserFunctionReturnValue(SodiumSession *session, HTSQLPage *page) {
	CallStack *cls = page->currentCallStack;
    return cls->preObjectFunctionReturnValue;
}





