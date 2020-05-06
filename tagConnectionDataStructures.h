#pragma once

#include "..\DBInt\dbIntefaceTypes.h"

typedef struct structConnection {

	char *connectionName;
	char *userName;
	char *userPassword;
	char *instanceName;

	DBInt_Connection *mkDBConnection;

	struct structConnection *next;
} TAGConnection;