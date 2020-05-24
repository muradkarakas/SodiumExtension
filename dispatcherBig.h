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





void
DispatchBigRequest(
    SodiumSession* session,
    HTSQLPage* page
);


/** \brief  __NO RESPONSIBILITY__
 *
 * \param currentSodiumSession MKSession*
 * \return const char     *
 *
 */
const char      * getBoundry(SodiumSession *currentMKSession);

/** \brief  __CALLER IS RESPONSIBLE TO RELEASE RETURN VALUE__
 *
 * \param currentSodiumSession MKSession*
 * \param boundry const char*
 * \param formInputName const char*
 * \return char     *
 *
 */
char                    *getFormDataBig(SodiumSession *currentMKSession, const char *boundry, const char *formInputName);


void    *getBinaryDataStartPosition(SodiumSession *currentMKSession, const char *boundry, const char *name);


/** \brief __CALLER IS RESPONSIBLE TO RELEASE RETURN VALUE__
 *
 * \param currentSodiumSession MKSession*
 * \param boundry const char*
 * \param imageInputName const char*
 * \return char  *
 *
 */
char         *getBinaryDataFileName(SodiumSession *currentMKSession, const char *boundry, const char *imageInputName);


/** \brief __CALLER IS RESPONSIBLE TO RELEASE RETURN VALUE__
 *
 * \param currentSodiumSession MKSession*
 * \param boundry const char*
 * \param imageInputName const char*
 * \return char*
 *
 */
char      *getBinaryDataContentType(SodiumSession *currentMKSession, const char *boundry, const char *imageInputName);


