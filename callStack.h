#pragma once


#include "htsql.h"

int
GetCallstackDepth(
	SodiumSession* session,
	HTSQLPage* page
);

/*	TODO: this function should check if current function called is not called before in this callstack/sequence */
BOOL
PushCallStack(
	SodiumSession* session,
	HTSQLPage* page,
	PreObjectGeneric* htsqlFuncionInstance
);



BOOL
PopCallStack(
	SodiumSession* session,
	HTSQLPage* page,
	PreObjectGeneric* htsqlFuncionInstance
);

void
ReNumberCallstack(
	SodiumSession* session,
	HTSQLPage* page
);

void
InitCallstack(
	SodiumSession* session
);