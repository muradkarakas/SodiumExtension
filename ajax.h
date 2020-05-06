/** \file   ajax.h

    \brief  Deals with AJAX request.

    AJAX requests are categorized by their types which can be either __HT/SQL function Call__ or __Native C Code Call__. \n
    Type information is received as HTTP POST request form parameter. \n
    Data structure for both request type are as follows : \n

  	-	__HT/SQL function name should/will be in the form name value:__\n
  		htsqlproceduretorun=
    \n
    \n
  	-	__Native C function name should/will be in the form name value:__\n
  		nativeproceduretorun=
*/

#pragma once




/** \brief  Entry point for all AJAX requests.
 *
 * \param currentSodiumSession MKSession*
 * \return void
 *
 *  Detects what type of AJAX request (see above) is received and acts accordingly.
*   If the type of request is not specified or its value is invalid, client error message is raised.
 */
void serveAsAjaxRequest(SodiumSession *currentMKSession, HTSQLPage *page);


/** \brief If AJAX request calls a HT/SQL function, this function is called by serveAsAjaxRequest(SodiumSession *currentMKSession).
 *
 * \param currentSodiumSession              MKSession*
 * \param blockname                     const char*
 * \param htsqlProcedureNameToBeCalled  const char*
 * \param param                         YuarelParam*
 * \param totalHTTPFormVariableCount    int
 * \return void
 *
 */
void serveAsAjaxRequestTypeHTSQL(SodiumSession *currentMKSession, HTSQLPage *page, const char *blockname, const char *htsqlProcedureNameToBeCalled);


/** \brief  This function is called by serveAsAjaxRequest(SodiumSession *currentMKSession), If a NATIVE C function is called.
 *
 * \param currentSodiumSession              MKSession*
 * \param blockname                     const char*
 * \param cFunctionNameToBeCalled       const char*
 * \return void
 *
 */
void serveAsAjaxRequestTypeNative(SodiumSession *currentMKSession, HTSQLPage *page, const char *blockname, const char *cFunctionNameToBeCalled);


void __sendHTSQLResponse(SodiumSession *currentMKSession, HTSQLPage *page, const char *blockname, const char *functionNameToBeCalled);


