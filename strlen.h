#pragma once


#include "htsql.h"

/** \brief  __CALLER IS RESPONSIBLE TO RELEASE RETURN VALUE__ \n
 *          Count the characters in "source" pointer. This code takes care of UTF8 encoding
 *
 * \param currentMKSession MKSession*
 * \param source const char*
 * \return char*
 *
 */
char *mkStrlen(SodiumSession *currentMKSession, const char *source);



