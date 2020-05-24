/**
 * This file is part of Sodium Language project
 *
 * Copyright © 2020 Murad Karakaþ <muradkarakas@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License v3.0
 * as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 *	https://choosealicense.com/licenses/gpl-3.0/
 */

#pragma once


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