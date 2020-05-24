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

#ifndef LIKE_H_INCLUDED
#define LIKE_H_INCLUDED

#include "stdbool.h"
#include "stdio.h"

/** \brief This function compares text strings, one of which can have wildcards ('*').
 *
 * \param   pTameText       A string without wildcards
 * \param   pWildText       A (potentially) corresponding string with wildcards
 * \param   bCaseSensitive  By default, match on 'X' vs 'x'
 * \param   cAltTerminator  For function names, for example, you can stop at the first '('
 * \return
 *
 *
 *  Matching Wildcards: An Algorithm
 *  http://www.drdobbs.com/architecture-and-design/matching-wildcards-an-algorithm/210200888
 */
bool mkLike(char *pTameText, char *pWildText, bool bCaseSensitive, const char cAltTerminator);


#endif // LIKE_H_INCLUDED
