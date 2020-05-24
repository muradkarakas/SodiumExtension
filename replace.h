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


