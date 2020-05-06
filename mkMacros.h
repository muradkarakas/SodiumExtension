#pragma once


// Pipe Server Commands
#define			REPLY_BUFFER_SIZE 512
#define			PIPE_CMD_GET_SESSION_ROOT_INSTANCE_ADDRESS	"getSessionRootInstanceAddress"
#define			PIPE_CMD_CLEAR_ALL_BREAKPOINTS				"clearAllBreakpoints"
#define			PIPE_CMD_GET_PROCESS_ID						"getProcessId"

#define			DEBUG_SECTION_START						if (IsDebuggerPresent()) {
#define			DEBUG_SECTION_END						}

#define SUPPORTED_LANGUAGE_COUNT    2