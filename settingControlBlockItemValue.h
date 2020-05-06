/*! \file settingControlBlockItemValue.h
    \brief Implementation of value assignment to a control block item value
*/

#pragma once

#include "htsql.h"




void __sendSetControlBlockItemValueResponse(SodiumSession *currentMKSession, const char *itemName, const char *itemValue);


