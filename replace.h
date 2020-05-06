#pragma once

#include "htsql.h"

/** \brief __CALLER SHOULD FREE RETURN VALUE__
 *
 * \param currentMKSession MKSession*
 * \param source const char*
 * \param fromStr const char*
 * \param toStr const char*
 * \return char*
 *
 */
char *mkReplace(SodiumSession *currentMKSession, const char * source, const char * const oldW, const char * const newW);


