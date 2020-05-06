/** \file   preUpdate.h

    \brief  Contains required c functions for implementation of a data block's "pre_update" trigger.
*/

#pragma once

#include "htsql.h"



bool runPreUpdateHTSQLTriggerFunction(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock);


