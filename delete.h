/** \file  delete.h

    \brief  Contains functions to implement built-in "delete" function implementation.
*/
#pragma once

#include "htsql.h"



void  __delete(SodiumSession*currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock);


