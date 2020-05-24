/**
 * This file is part of Sodium Language project
 *
 * Copyright © 2020 Murad Karakaş <muradkarakas@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License v3.0
 * as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 *	https://choosealicense.com/licenses/gpl-3.0/
 */

#include "pch.h"

#include "message.h"



void __sendMessage3Response(SodiumSession *session, HTSQLPage *page, char *pTitle, char *pMessage, char *pType) {

    if (session->sessionReadyState == SESSION_READY_STATE_BEING_KILLED || session->sessionReadyState == SESSION_READY_STATE_KILLED) {

        // 'message' function can only be used when the http session is raady.
        mkDebug(session,
                SESSION_LOG_DETAIL_HTSQL_TRIGGER | SESSION_LOG_DETAIL_SESSION_EVENTS,
                "\n",
                ETEXT(ERR_INVALID_FUNCTION_CALL),
                ". ",
                "'message' function cannot be called from dead session.",
                NULL);

    } else {

        char *lastResponse = NULL;

        // If this function is executed in GET request,
        if (page->currentRequestMethodType == REQUEST_TYPE_GET) {
            mkPrint(session, "\n<script type=\"text/javascript\">\n", NULL);
        }

		char *lTitle = mkEscapeJavaScriptQuoteCharacter(session, pTitle);
		char *lType = mkEscapeJavaScriptQuoteCharacter(session, pType);
        
        if (pMessage) {
			// We must replace \n character with one space. Otherwise, we get java script 'unterminated string constant' error. 
			char *lMessage = mkEscapeJavaScriptQuoteCharacter(session, pMessage);
            lastResponse = mkStrcat(session->heapHandle, __FILE__, __LINE__,
                                        "action = { "
                                        "   \"type\" : \"message\", "
                                        "   \"messageText\" : \"", lMessage, "\", "
                                        "   \"messageTitle\" : \"", lTitle, "\", "
                                        "   \"messageType\" : \"", lType, "\" "
                                        "};" // "ı"
                                        // standart part
                                        "addNewActionToTheResponseActionList(action);",
                                      NULL);
			mkFree(session->heapHandle, lMessage);
        } else {
            lastResponse = mkStrcat(session->heapHandle, __FILE__, __LINE__,
                                        "action = { "
                                        "   \"type\" : \"message\", "
                                        "   \"messageText\" : \"\", "
                                        "   \"messageTitle\" : \"", lTitle, "\", "
                                        "   \"messageType\" : \"", lType, "\" "
                                        "};"
                                      // standart part
                                      "addNewActionToTheResponseActionList(action);",
                                      NULL);
        }
        mkPrint(session, lastResponse, NULL);

        if (page->currentRequestMethodType == REQUEST_TYPE_GET) {
            mkPrint(session, "\n</script>\n", NULL);
        }
        mkFree(session->heapHandle, lastResponse);
    }

}

void __sendMessageResponse(SodiumSession *session, HTSQLPage *page, char *pMessage) {

    if (session->sessionReadyState == SESSION_READY_STATE_BEING_KILLED || session->sessionReadyState == SESSION_READY_STATE_KILLED) {

        // 'message' function can only be used when the http session is raady.
        mkDebug(session,
                SESSION_LOG_DETAIL_HTSQL_TRIGGER | SESSION_LOG_DETAIL_SESSION_EVENTS,
                "\n",
                ETEXT(ERR_INVALID_FUNCTION_CALL),
                ". ",
                "'message' function cannot be called from dead session.",
                NULL);

    } else {

        char *lastResponse = NULL;

        // If this function is executed in GET request,
        if (page->currentRequestMethodType == REQUEST_TYPE_GET) {
            mkPrint(session, "\n<script type=\"text/javascript\">\n", NULL);
        }

        if (pMessage) {
			// We must replace \n character with one space. Otherwise, we get java script 'unterminated string constant' error. 
			char *lMessage = mkEscapeJavaScriptQuoteCharacter(session, pMessage);
            lastResponse = mkStrcat(session->heapHandle, __FILE__, __LINE__,
                                        "action = { "
                                        "   \"type\" : \"message\", "
                                        "   \"messageText\" : \"", lMessage, "\", "
                                        "   \"messageType\" : \"info\" "
                                        "};" 
                                        // standart part
                                        "addNewActionToTheResponseActionList(action);",
                                        NULL);
			mkFree(session->heapHandle, lMessage);
        } else {
            lastResponse = mkStrcat(session->heapHandle, __FILE__, __LINE__,
                                        "action = { "
                                        "   \"type\" : \"message\", "
                                        "   \"messageText\" : \"\", "
                                        "   \"messageType\" : \"info\" "
                                        "};"
                                        // standart part
                                        "addNewActionToTheResponseActionList(action);",
                                        NULL);
        }
        mkPrint(session, lastResponse, NULL);

        if (page->currentRequestMethodType == REQUEST_TYPE_GET) {
            mkPrint(session, "\n</script>\n", NULL);
        }
        mkFree(session->heapHandle, lastResponse);
    }

}
