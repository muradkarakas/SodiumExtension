#include "pch.h"

#include "strlen.h"


char *mkStrlen(SodiumSession *currentMKSession, const char *source) {

    // This function takes care of UTF8 encoding
    char *retval = NULL;
    char *posStr = mkMalloc(currentMKSession->heapHandle, 20, __FILE__, __LINE__);
    size_t len = 0;

    for (; *source; ++source) {
        if ((*source & 0xC0) != 0x80) {
            ++len;
        }
    }

	mkItoa(len, posStr);

    retval = mkStrdup(currentMKSession->heapHandle, posStr, __FILE__, __LINE__);

    mkFree(currentMKSession->heapHandle, posStr);

    return retval;
}
