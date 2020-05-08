/** \file   pageLoad.h

    \brief  Contains function and data structures for page_load trigger
*/
#pragma once



/** \brief  This function is called by html parser just before </body> tag sending to the browser.
 *
 * \param currentMKSession
 * \return void
 *
 *  This function calls HT/SQL "page_load" function during GET request.
 *
 *  Note that: before calling any native or HT/SQL function, client side response action library should be initialized by calling __initializeClientJavaScriptReponse();.

 */
void         __runHTSQLPageLoadFunction(SodiumSession *currentMKSession, HTSQLPage *page);

