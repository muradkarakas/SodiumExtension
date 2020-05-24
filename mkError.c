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

#include "mkError.h"

void __sendErrorResponse(
	SodiumSession *session, 
	HTSQLPage *page, 
	const char *pErrorText, 
	const char *pAdditionalInfo
)
{
	char *lErrTxt = mkStrdup(session->heapHandle, pErrorText, __FILE__, __LINE__);
	char* lAddInfo = NULL;
	if (pAdditionalInfo && pAdditionalInfo[0] != '\0') {
		lAddInfo = mkStrdup(session->heapHandle, pAdditionalInfo, __FILE__, __LINE__);
	}
	else {
		lAddInfo = mkStrdup(session->heapHandle, "Not provided.", __FILE__, __LINE__);
	}

	mkStrReplaceEnter(lErrTxt);
	mkStrReplaceEnter(lAddInfo);

	// Check client side javascript library has been sent to the client browser or not 
	if (page && page->currentRequestMethodType == REQUEST_TYPE_GET && session->whatIsPrinted < SESSION_PRINT_CLIENT_LIBRARY) {
		// Not sent. This means we have a poor client 
		__sendErrorResponsePoor(session, page, lErrTxt, lAddInfo);
	}
	else {
		// Rich client active
		__sendErrorResponseRich(session, page, lErrTxt, lAddInfo);
	}
	mkFree(session->heapHandle, lErrTxt);
	mkFree(session->heapHandle, lAddInfo);
}

void __sendErrorResponseRich(
	SodiumSession * session, 
	HTSQLPage *page, 
	const char *pErrorText, 
	const char *pAdditionalInfo
)
{
	char *lReason = mkEscapeJavaScriptQuoteCharacter(session, pErrorText);
	char *lPosSol = mkEscapeJavaScriptQuoteCharacter(session, pAdditionalInfo);

	char *lastResponse = mkStrcat(session->heapHandle,
		__FILE__, __LINE__,
		"action = { "
		"\"type\"             : \"error\", "
		"\"reasonText\"       : \"", lReason, "\", "
		"\"possibleSolution\" : \"", lPosSol, "\" "
		" };",
		"addNewActionToTheResponseActionList(action);",
		NULL);

	if (page && page->currentRequestMethodType == REQUEST_TYPE_GET) {
		/** do not delete > character before <script.
		* If systax error occurs between < and > characters while get request, error message will be generated between these characters and will not be executed as
		* script in client side. For that reason, we are closing tag. These behavior should be solved using different solution.
		*/
		mkPrint(session, "><script type=\"text/javascript\">", NULL);
	}

	mkPrint(session, lastResponse, NULL);

	if (page && page->currentRequestMethodType == REQUEST_TYPE_GET) {
		mkPrint(session, "</script>", NULL);
	}
	mkFree(session->heapHandle, lReason);
	mkFree(session->heapHandle, lPosSol);
	mkFree(session->heapHandle, lastResponse);
}

void __sendErrorResponsePoor(SodiumSession *session, HTSQLPage *page, const char *pErrorText, const char *pAdditionalInfo) {

	char *lReason = mkEscapeJavaScriptQuoteCharacter(session, pErrorText);
	char *lPosSol = mkEscapeJavaScriptQuoteCharacter(session, pAdditionalInfo);

	char *lastResponse = mkStrcat(session->heapHandle,
		__FILE__, __LINE__,
		"var message = 'Error occured: ", lReason, ". Additional info : ", lPosSol, "'; ",
		"alert(message);",
		NULL);
	mkPrint(session, "<script type=\"text/javascript\">", NULL);
	mkPrint(session, lastResponse, NULL);
	mkPrint(session, "</script>", NULL);

	mkFree(session->heapHandle, lReason);
	mkFree(session->heapHandle, lPosSol);
	mkFree(session->heapHandle, lastResponse);
}

void __initializeClientJavaScriptReponse(SodiumSession *session) {
	mkPrint(session, "<script type=\"text/javascript\"> initResponse(); </script>", NULL);
}

const char *
GetErrorText(
	SodiumSession *session, 
	ERROR_CODES errCode
)
{
	switch (errCode) {
		case ERR_NAME_ALREADY_IN_USE: {
			return "Name is already in use";
		}
		case ERR_SECURITY_ACCESS_REFUSED: {
			return "Access to that page refused";
		}
		case ERR_PROPERTY_CANNOT_BE_EMPTY : {
			return "Property cannot be empty";
		}
		case ERR_PROPERTY_INVALID : {
			return "Property invalid";
		}
		case ERR_FUNCTION_MUST_RETURN_BOOL : {
			return "Function must return a value of bool";
		}
		case ERR_FUNCTION_MUST_RETURN_VALUE : {
			return "Function must return a value";
		}
		case ERR_FUNCTION_MUST_RETURN_CHAR : {
			return "Function must return a value of char";
		}
		case ERR_FUNCTION_DECLARATION_DOES_NOT_RETURN_VALUE : {
			return "Function declaration does not have a return type";
		}
		case ERR_NAME_IS_NOT_FOUND : { 
			return "Name not found"; 
		}	
		case ERR_SESSION_IS_DEAD : { 
			return "Session is dead"; 
		}
		case ERR_FUNCTION_NOT_FOUND : { 
			return "Function not found"; 
		}
		case ERR_DATABLOCK_NOT_FOUND : { 
			return "Data block not found"; 
		}
		case ERR_DATALIST_NOT_FOUND : { 
			return "Data list not found"; 
		}
		case ERR_DATALIST_CONTENT_CANNOT_BE_REFRESHED : { 
			return "Data list cannot be refreshed"; 
		}
		case ERR_SYNTAX_ERROR_IN_SQLX_FILE : { 
			return "Syntax error in SQLX file"; 
		}
		case ERR_SYNTAX_ERROR_IN_FRMX_FILE: {
			return "Syntax error in FRMX file";
		}
		case ERR_SQLX_FILE_NOT_FOUND : { 
			return "SQLX file not found"; 
		}
		case ERR_FRMX_FILE_NOT_FOUND: {
			return "FRMX file not found";
		}
		case ERR_DATABLOCK_HAS_FK_BUT_NO_MASTER : { 
			return "Data block has foreign key columns but its master block name property is invalid"; 
		}
		case ERR_MASTER_DATABLOCK_NOT_FOUND : { 
			return "Master data block not found"; 
		}
		case ERR_TEXTAREA_CANNOT_BE_FK : { 
			return "Textarea element cannot be referenced as a foreign-key column"; 
		}
		case ERR_DATABLOCK_ITEM_NOT_FOUND : { 
			return "Data block item not found"; 
		}
		case ERR_PARAMETER_COUNT_IN_FUNCTION_CALL_INVALID : { 
			return "Parameter count in function call is invalid"; 
		}
		case ERR_INVALID_FUNCTION_CALL : { 
			return "Function cannot be called in this context"; 
		}
		case ERR_DB_UNKNOWN_ERROR : { 
			return "DB ERROR:"; 
		}
		case ERR_DB_INVALID_CONNECTION : { 
			return "DB ERROR: No valid database connection"; 
		}
		case ERR_DB_CONNECTION_CANNOT_BE_OPENED : { 
			return "DB ERROR: Database connection attempt failed"; 
		}
		case ERR_CORE_INVALID_REQUEST_TYPE_1 : { 
			return "CORE_ERROR: AJAX request must be either HTSQL or NATIVE, not both"; 
		}
		case ERR_CORE_INVALID_REQUEST_TYPE_2 : { 
			return "CORE_ERROR: AJAX request must be either HTSQL or NATIVE, but none of these specified"; 
		}
		case ERR_CORE_INVALID_HTTP_REQUEST : { 
			return "CORE_ERROR: Invalid HTTP request"; 
		}
		case ERR_CORE_INVALID_HTTP_REQUEST_PARAM : { 
			return "CORE_ERROR: Invalid HTTP request"; 
		}
		case ERR_CORE_IMAGE_CANNOT_BE_WRITEN_TO_DB : { 
			return "CORE_ERROR: Image file cannot be written to the database"; 
		}
		case ERR_CORE_IMAGE_CANNOT_BE_READ : { 
			return "CORE_ERROR: Image file cannot be read"; 
		}
		case ERR_CORE_VALUE_NOT_FOUND : { 
			return "CORE_ERROR: Value not found"; 
		}
		case ERR_NO_ACTIVE_CONNECTION: {
			return "No active connection exists";
		}
		case ERR_CORE_CONVERSATION_ERROR: {
			return "CORE conversation error occured";
		}
		case ERR_FILE_CANNOT_BE_OPENED: {
			return "File cannot be opened";
		}
		case ERR_FILE_CONTENT_CANNOT_BE_READ: {
			return "File contetnt cannot be read";
		}
		case ERR_DB_NO_CONNECTION_EXISTS: {
			return "No connection exists";
		}
		case ERR_CORE_VARIABLE_NOT_FOUND: {
			return "Core error: Internal variable not found";
		}
		case ERR_CORE_VARIABLE_ADDED_TWICE: {
			return "Control block variable cannot be added twice";
		}
		case ERR_CORE_DATABLOCK_CONTENT_CANNOT_BE_SHOWN: {
			return "For some reasons, datablock content cannot be shown.";
		}
		case ERR_DB_DBTYPE_NOT_SUPPORTED : {
			return "Database type not supported";
		}
		case ERR_REDIS_CONNECTION_FAILED: {
			return "Connection error";
		}
		case ERR_REDIS_INVALID_CONNECTION: {
			return "REDIS connection is not valid";
		}			
		case ERR_REDIS_NO_VALID_CONNECTION: {
			return "No valid REDIS connection exists";
		}		
		case ERR_REDIS_UNKNOWN: {
			return "Unknown error occured (REDIS)";
		}
		case ERR_REDIS_CANNOT_ASSIGN: {
			return "Cannot assign a redis variable to another variable";
		}
		case ERR_CORE_NOT_IMPLEMENTED: {
			return "Not implemented";
		}
		case ERR_CORE_RESPONSE_HEADER_NOT_PRINTED_YET: {
			return "No response header printed yet";
		}
		case ERR_CORE_INVALID_VALUE: {
			return "Invalid value";
		}
		case ERR_CORE_MEMORY_MANAGEMENT_FAILURE: {
			return "Error detected related memory management";
		}							 
		default: {
			return "No error description provided.";
		}
	}
    return "";
}

const char *
GetActionText(
	SodiumSession *session, 
	ERROR_CODES errCode
)
{
    return "";
}
