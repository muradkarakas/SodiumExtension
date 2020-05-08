/** \file   preInsert.h

    \brief  Contains required c functions for implementation of a data block's "pre_insert" trigger.
*/

#pragma once




bool runPreInsertHTSQLTriggerFunction(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock);


