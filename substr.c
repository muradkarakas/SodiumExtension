#include "pch.h"

#include "substr.h"

char *mkSubstr(SodiumSession *currentMKSession, char *source, int posint, int length) {
    
	char *pos = mkUTF8index(currentMKSession, source, posint-1);

    if (length == 0) {
        return mkStrdup(currentMKSession->heapHandle, pos, __FILE__, __LINE__);

    } else if (length > 0) {

        char *start = mkStrdup(currentMKSession->heapHandle, pos, __FILE__, __LINE__);
        char *end2   = mkUTF8index(currentMKSession, start, length);
        *end2 = '\0';
        char *retval = mkStrdup(currentMKSession->heapHandle, start, __FILE__, __LINE__);
        mkFree(currentMKSession->heapHandle, start);
        return retval;

    } if (length < 0) {

    }
    return NULL;
}
