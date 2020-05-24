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

#pragma once


#include <stdio.h>

/**
*    CONSTANTS
*/
#define YES     'Y'
#define NO      'N'
#define RESOURCE_FOLDER								"/Sodium-Site/resources/"
/*	Excluding NULLs, \r or \n at the and of the string
DO NOT INCREASE SESSION_ID_LENGTH ; Reason;
In debug mode, it's value passed to the IDE as if it were process Id.
Some IDEs (such as Code::Blocks) does not work with big numbers since it expects process id and operating system has some constraints on it */
#define SESSION_ID_LENGTH							5		
/**
*   HT/SQL PREDEFINED LANGUAGE LIMITS
*/
#define MAX_PAGE_COUNT                              100
#define MAX_FUNCTION_PARAMETERS_COUNT               10
#define MAX_ACCEPTABLE_POSTED_FORM_ELEMENT_COUNT    50
#define MAX_BLOCKNAME_LENGTH                        100
#define MAX_FUNCTION_NAME_LENGTH                    100
#define MAX_VARIABLE_NAME_LENGTH                    100
#define MAX_VARIABLE_VALUE_LENGTH                   200		// maximum character count shown in debugger watch screen
#define MAX_IDLE_TIME_IN_MILISECONDS                600000
#define MAX_FORMULA_COLUMN_IMAGE_SIZE               10485760   /* 10 MB = (1024 * 1024 * 10) */
#define MAX_RESPONSE_BUFFER_SIZE					10485760   /* 10 MB = (1024 * 1024 * 10) */
#define INPUT_TEXT_WIDTH_PER_CHARACTER				8

#define MAX_BREAKPOINT_COUNT						50

#define HTTP_RESPONSE_BUFFER_CHUNK_SIZE				1024 * 4 // 4KB
enum htsql_errors {
	HTSQL_ERR_2
};



extern const char *mkSessionCookieName;

extern const char *trgName_PageAccess;
extern const char *trgName_PageLoad;

extern const char *trgName_DatablockRowSelected;
extern const char *trgName_DatablockPostQuery;
extern const char *trgName_DatablockPostInsert;
extern const char *trgName_DatablockPostUpdate;
extern const char *trgName_DatablockPostDelete;

extern const char *trgName_DatablockPreInsert;
extern const char *trgName_DatablockPreUpdate;
extern const char *trgName_DatablockPreDelete;

extern const char *trgName_DatablockAccess;


extern const char *trgName_TreeNodeExpanded;
extern const char *trgName_TreeNodeSelected;

extern const char *treeOuterDivOpen;
extern const char *treeOuterDivClose;

extern const char *resourseFolder;
extern const wchar_t *cLogFileName;


extern const char *classButton;
extern const char *classSelect;
extern const char *classText;
extern const char *classImage;
extern const char *classCheckBox;
extern const char *classTextArea;
extern char *classDatablock;



