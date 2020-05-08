#pragma once

/*	Instead of deleting/deallocationg breakpoint linked list, we are setting breakpoint state to the BREAKPOINT_STATE_DELETED value. */	
void clearAllBreakpoints(SodiumSession *currentMKSession);