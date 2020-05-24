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


// Pipe Server Commands
#define			REPLY_BUFFER_SIZE 512
#define			PIPE_CMD_GET_SESSION_ROOT_INSTANCE_ADDRESS	"getSessionRootInstanceAddress"
#define			PIPE_CMD_CLEAR_ALL_BREAKPOINTS				"clearAllBreakpoints"
#define			PIPE_CMD_GET_PROCESS_ID						"getProcessId"

#define			DEBUG_SECTION_START						if (IsDebuggerPresent()) {
#define			DEBUG_SECTION_END						}

#define SUPPORTED_LANGUAGE_COUNT    2