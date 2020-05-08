/** \file   preUpdate.h

    \brief  Contains required c functions for implementation of a data block's "pre_update" trigger.
*/

#pragma once





bool runPreUpdateHTSQLTriggerFunction(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock);


