/** \file   preInsert.h

    \brief  Contains required c functions for implementation of a data block's "pre_insert" trigger.
*/

#pragma once

#include "htsql.h"


bool runPreInsertHTSQLTriggerFunction(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock);


