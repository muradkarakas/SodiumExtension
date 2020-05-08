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



