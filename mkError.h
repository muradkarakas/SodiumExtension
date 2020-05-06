#pragma once

#include "htsql.h"
#include "SodiumSession.h"



const char *getErrorText(SodiumSession *currentMKSession, ERROR_CODES errCode);
const char *getActionText(SodiumSession *currentMKSession, ERROR_CODES errCode);

void	__sendErrorResponse(SodiumSession *currentMKSession, HTSQLPage *page, const char *pErrorText, const char *pAdditionalInfo);
void	__sendErrorResponseRich(SodiumSession *currentMKSession, HTSQLPage *page, const char *pErrorText, const char *pAdditionalInfo);
void	__sendErrorResponsePoor(SodiumSession *currentMKSession, HTSQLPage *page, const char *pErrorText, const char *pAdditionalInfo);



