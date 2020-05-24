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


#include "SodiumSession.h"



const char*
GetErrorText(
	SodiumSession* session,
	ERROR_CODES errCode);

const char*
GetActionText(
	SodiumSession* session,
	ERROR_CODES errCode);

void	__sendErrorResponse(SodiumSession *currentMKSession, HTSQLPage *page, const char *pErrorText, const char *pAdditionalInfo);
void	__sendErrorResponseRich(SodiumSession *currentMKSession, HTSQLPage *page, const char *pErrorText, const char *pAdditionalInfo);
void	__sendErrorResponsePoor(SodiumSession *currentMKSession, HTSQLPage *page, const char *pErrorText, const char *pAdditionalInfo);



