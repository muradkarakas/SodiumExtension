#pragma once

#include "htsql.h"



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


