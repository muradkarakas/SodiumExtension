/** \file   postDelete.h

    \brief  Contains required c functions for implementation of a data block's "post_delete" trigger.
*/

#pragma once

#include "htsql.h"



/** \brief  Calls the post_delete HT/SQL function of the data block specified by pTAGDataBlock parameter.
 *
 * \param currentMKSession MKSession*
 * \param pTAGDataBlock TAGDataBlock*
 * \return bool     Returns true if trigger function found & executed.
 *
 */
bool runPostDeleteHTSQLTriggerFunction(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock);


