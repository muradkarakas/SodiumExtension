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



