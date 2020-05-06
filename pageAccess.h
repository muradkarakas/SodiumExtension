/** \file   pageAccess.h

    \brief  Contains required c functions for implementation of the HT/SQL "page_access" trigger.
*/

#pragma once

#include "htsql.h"


/** \brief Runs the HT/SQL "page_access" function of the current *.frmx file.
*
* \param currentMKSession MKSession*   Current HTTP Session object instance
* \return bool                         Returns the HT/SQL "page_"access" function's return value.
*
*  This function enables HT/SQL programmer to control accessing to *.frmx file.
*  If return value is false, accessing to the page does not allowed, If true, page will be executed and response will be sent to the client.
*
*  If a *.frmx file does not have a "page_access" method, page access does not blocked. That is, it is a public page.
*  In order to be accessible, "Logon pages" must not have "page_access" function or the function must always return "true". Otherwise, nobody can authenticate themselves.
*
*/
bool runPageAccessHTSQLTriggerFunction(SodiumSession *currentMKSession, HTSQLPage *page);


