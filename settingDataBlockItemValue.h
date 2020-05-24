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

 /*! \file settingDataBlockItemValue.h
    \brief Implementation of value assignment to a data block item value
*/

#pragma once



/** \brief Send "setDataBlockItemValue" response to the browser in order to set a block item new value.
 * This function is called in case of an HT/SQL function set a new value to the block item.
 *
 * \param currentMKSession MKSession*
 * \param dataBlockName const char*
 * \param rowid const char*
 * \param itemName const char*
 * \param itemValue const char*
 * \param simulateClientModification bool       If true, item status and css classes have been set as if it were modified by user.
 * \return void
 *
 */
void __sendSetDataBlockItemValueResponse(SodiumSession *currentMKSession, const char *dataBlockName, const char *rowid, const char *itemName, const char *itemValue, bool simulateClientModification);



