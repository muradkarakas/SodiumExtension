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

