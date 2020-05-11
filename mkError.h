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



