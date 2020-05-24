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

 /** \file   postUpdate.h

    \brief  Contains required c functions for implementation of a data block's "post_update" trigger.
*/

#pragma once






/** \brief  Calls the post_update HT/SQL function of the data block specified by pTAGDataBlock parameter.
 *
 * \param currentMKSession MKSession*
 * \param pTAGDataBlock TAGDataBlock*
 * \return bool     Returns true if trigger function found & executed.
 *
 */
bool runPostUpdateHTSQLTriggerFunction(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock);



