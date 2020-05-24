/**
 * This file is part of Sodium Language project
 *
 * Copyright � 2020 Murad Karaka� <muradkarakas@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License v3.0
 * as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 *	https://choosealicense.com/licenses/gpl-3.0/
 */

#pragma once


typedef struct _HTSQLResponse {
	/* it is a pointer to buffer for accumulated response string / byte sequence*/
	void *buffer;

	/* Maximum size of the "responseBuffer" */
	size_t  allocatedSize;

	/* currently size of the used space from "responseBuffer"  */
	size_t  usedSize;
} HTSQLResponse;