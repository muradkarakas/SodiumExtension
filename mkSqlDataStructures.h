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

typedef enum _sql_command_type {
	SQL_COMMAND_TYPE_SELECT_NO_SCROLL,
	SQL_COMMAND_TYPE_SELECT_SCROLLABLE,
	SQL_COMMAND_TYPE_UPDATE,
	SQL_COMMAND_TYPE_INSERT,
	SQL_COMMAND_TYPE_DELETE,
	SQL_COMMAND_TYPE_PLSQL
} SQL_COMMAND_TYPE;