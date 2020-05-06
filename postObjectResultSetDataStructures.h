#pragma once

#include "..\DBInt\dbIntefaceTypes.h"



struct _preObjectResultSet {
	char                *evaluatedSQL;
	DBInt_Statement		*ociStatement;
	TAGConnection		*resultsetTAGConnection;
};