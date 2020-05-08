/** \file   preDelete.h

    \brief  Contains required c functions for implementation of a data block's "pre_delete" trigger.
*/
#pragma once




bool runPreDeleteHTSQLTriggerFunction(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock);


