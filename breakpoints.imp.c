#include "pch.h"

#include "breakpoints.imp.h"
 
void clearAllBreakpoints(SodiumSession *currentMKSession) {
	if (currentMKSession) {
		for (int i = 0; i < MAX_BREAKPOINT_COUNT; i++) {
			/*currentMKSession->breakpointsRoot[i].breakpoint.fileURI[0] = '\0';
			currentMKSession->breakpointsRoot[i].breakpoint.id = 0;
			currentMKSession->breakpointsRoot[i].breakpoint.lineNumber[0] = '\0';*/
			currentMKSession->breakpointsRoot[i].breakpoint.breakpointState = BREAKPOINT_STATE_DELETED;
		}
		currentMKSession->breakpointsRootAddress = &(currentMKSession->breakpointsRoot[0]);
	}
}
