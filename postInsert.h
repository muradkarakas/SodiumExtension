/** \file   postInsert.h

    \brief  Contains required c functions for implementation of a data block's "post_insert" trigger.
*/

#pragma once

#include "htsql.h"



/** \brief  Calls the post_insert HT/SQL function of the data block specified by pTAGDataBlock parameter.
 *
 * \param currentMKSession MKSession*
 * \param pTAGDataBlock TAGDataBlock*
 * \return bool     Returns true if trigger function found & executed.
 *
 */
bool runPostInsertHTSQLTriggerFunction(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock);



