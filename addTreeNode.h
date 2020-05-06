#pragma once

#include "htsql.h"



/** \brief __CALLERT MUST RELEASE RETURN VALUE__
 *
 * \param currentMKSession MKSession*
 * \param page HTSQLPage*
 * \param recordSetName const char*
 * \return char*
 *
 */
char *__sendAddTreeNodeResponse(SodiumSession *currentMKSession, HTSQLPage *page, const char *recordSetName);


