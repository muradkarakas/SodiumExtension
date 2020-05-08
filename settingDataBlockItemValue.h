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



