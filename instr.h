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




/** \brief  __CALLER IS RESPONSIBLE TO RELEASE RETURN VALUE__ \n
 *          Search the string in "searchStr" parameter in the string passed as "source" parameter.
 *
 * \param currentMKSession MKSession*
 * \param source const char*
 * \param searchStr const char*
 * \return char*            Returns the index (number) of the first character as char * if found. Otherwise, returns "0".
 *
 */
char *mkInstr(SodiumSession *currentMKSession, const char *source, const char *searchStr);



