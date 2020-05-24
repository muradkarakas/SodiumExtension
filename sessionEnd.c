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

#include "sessionEnd.h"
#include "htsqlFunction.imp.h"

    const char *sessionEndReasonDescriptions[] = {
        "Max idle time reached",
        "Unknown Reason"
    };


void __sendSessionEndResponse(SodiumSession *currentMKSession, HTSQLPage *page, SESSION_END_REASON sessionEndReason) {

    if (page == NULL) {
        page = currentMKSession->lastPage;
    }
    const char *sessionEndReasonDesc = getSessionEndReasonDescrition(currentMKSession, page, sessionEndReason);
    const char *functionToRun = "user_session_end";

    /** Step.1 */
	
    /** Step.2 */
	createANewFunctionCallStackStartPoint(currentMKSession, page, functionToRun);

    /** Step.3 */
    postFunctionAddArgumentsOrVariables(currentMKSession, page, POST_OBJECT_GENERIC_TYPE_FUNCTION_ARGS, PREOBJECTTYPE_VARIABLE_VARCHAR,
                                        "reason",
                                        sessionEndReasonDesc);

    postFunctionAddArgumentsOrVariables(currentMKSession, page, POST_OBJECT_GENERIC_TYPE_FUNCTION_ARGS, PREOBJECTTYPE_VARIABLE_VARCHAR,
                                        "client_ip_address",
                                        "not implemented yet");

    postFunctionAddArgumentsOrVariables(currentMKSession, page, POST_OBJECT_GENERIC_TYPE_FUNCTION_ARGS, PREOBJECTTYPE_VARIABLE_VARCHAR,
                                        "client_domain",
                                        "not implemented yet");

    postFunctionAddArgumentsOrVariables(currentMKSession, page, POST_OBJECT_GENERIC_TYPE_FUNCTION_ARGS, PREOBJECTTYPE_VARIABLE_VARCHAR,
                                        "client_os_user",
                                        "not implemented yet");

    postFunctionAddArgumentsOrVariables(currentMKSession, page, POST_OBJECT_GENERIC_TYPE_FUNCTION_ARGS, PREOBJECTTYPE_VARIABLE_VARCHAR,
                                        "user_session_start_time",
                                        "not implemented yet");
    /** Step.4 */

    /** Step.5 */
    PreObjectFunction *preObjectFunction = runPostFunction(currentMKSession, page, functionToRun, true);

    /** Step.6 */
    if (preObjectFunction) {
        /** Page has "user_session_end" function */
    } else {
        /** page does not have "user_session_end" function. */
    }

    /** Step.7 */
    cleanUpFunctionStack(currentMKSession, page);

    /** Step.8 */
}


const char *getSessionEndReasonDescrition(SodiumSession *currentMKSession, HTSQLPage *page, SESSION_END_REASON sessionEndReason) {



    return sessionEndReasonDescriptions[sessionEndReason];
}
