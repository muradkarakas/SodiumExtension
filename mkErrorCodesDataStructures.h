/**
 * This file is part of Sodium Language project
 *
 * Copyright � 2020 Murad Karaka� <muradkarakas@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License v3.0
 * as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 *	https://choosealicense.com/licenses/gpl-3.0/
 */

#pragma once


#define ETEXT(ERRCODE)  GetErrorText(session, ERRCODE)
#define ATEXT(ERRCODE)  GetActionText(session, ERRCODE)

#define CLOG(ERRCODE, INFO)		{ __sendErrorResponse(session, page, ETEXT(ERRCODE), INFO); mkCoreDebug(__FILE__, __LINE__, ETEXT(ERRCODE), INFO, NULL);  }


typedef enum _Error_Codes {
	ERR_NAME_ALREADY_IN_USE = 0,
	ERR_PROPERTY_CANNOT_BE_EMPTY,
	ERR_PROPERTY_INVALID,
	ERR_FUNCTION_MUST_RETURN_BOOL,
	ERR_FUNCTION_MUST_RETURN_VALUE,
	ERR_FUNCTION_MUST_RETURN_CHAR,
	ERR_FUNCTION_DECLARATION_DOES_NOT_RETURN_VALUE,
	ERR_NAME_IS_NOT_FOUND,
	ERR_SESSION_IS_DEAD,
	ERR_FUNCTION_NOT_FOUND,
	ERR_DATABLOCK_NOT_FOUND,
	ERR_DATALIST_NOT_FOUND,
	ERR_DATALIST_CONTENT_CANNOT_BE_REFRESHED,
	ERR_SYNTAX_ERROR_IN_SQLX_FILE,
	ERR_SYNTAX_ERROR_IN_FRMX_FILE,
	ERR_SQLX_FILE_NOT_FOUND,
	ERR_FRMX_FILE_NOT_FOUND,
	ERR_DATABLOCK_HAS_FK_BUT_NO_MASTER,
	ERR_MASTER_DATABLOCK_NOT_FOUND,
	ERR_TEXTAREA_CANNOT_BE_FK,
	ERR_DATABLOCK_ITEM_NOT_FOUND,
	ERR_PARAMETER_COUNT_IN_FUNCTION_CALL_INVALID,
	ERR_INVALID_FUNCTION_CALL,
	ERR_NO_ACTIVE_CONNECTION,
	ERR_FUNCTION_PARAMETER_CANNOT_BE_NULL,
	ERR_FORMULA_COLUMN_VALUE_CANNOT_BE_UPDATED,

	/* REDIS  */
	ERR_REDIS_INVALID_CONNECTION,
	ERR_REDIS_CONNECTION_FAILED,
	ERR_REDIS_NO_VALID_CONNECTION,
	ERR_REDIS_UNKNOWN,
	ERR_REDIS_CANNOT_ASSIGN,

	/* FILE OPERATIONS */
	ERR_FILE_CANNOT_BE_OPENED,
	ERR_FILE_CONTENT_CANNOT_BE_READ,

	/* DB SPECIFIC ERRORS */
	ERR_DB_UNKNOWN_ERROR,
	ERR_DB_INVALID_CONNECTION,
	ERR_DB_CONNECTION_CANNOT_BE_OPENED,
	ERR_DB_NO_CONNECTION_EXISTS,
	ERR_DB_DBTYPE_NOT_SUPPORTED,

	/* CORE ERRORS */
	ERR_CORE_INVALID_REQUEST_TYPE_1,
	ERR_CORE_INVALID_REQUEST_TYPE_2,
	ERR_CORE_INVALID_HTTP_REQUEST,
	ERR_CORE_INVALID_HTTP_REQUEST_PARAM,
	ERR_CORE_IMAGE_CANNOT_BE_WRITEN_TO_DB,
	ERR_CORE_IMAGE_CANNOT_BE_READ,
	ERR_CORE_VALUE_NOT_FOUND,
	ERR_CORE_CONVERSATION_ERROR,
	ERR_CORE_VARIABLE_NOT_FOUND,
	ERR_CORE_VARIABLE_ADDED_TWICE,
	ERR_CORE_DATABLOCK_CONTENT_CANNOT_BE_SHOWN,
	ERR_CORE_NOT_IMPLEMENTED,
	ERR_CORE_RESPONSE_HEADER_NOT_PRINTED_YET,
	ERR_CORE_INVALID_VALUE,
	ERR_CORE_MEMORY_MANAGEMENT_FAILURE,

	/* ERRORS RELATED TO SECURITY */
	ERR_SECURITY_ACCESS_REFUSED

} ERROR_CODES;
