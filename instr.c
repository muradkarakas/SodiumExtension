#include "pch.h"

#include "instr.h"

char *mkInstr(SodiumSession *currentMKSession, const char *source, const char *searchStr) {
    char *posStr = mkMalloc(currentMKSession->heapHandle, 20, __FILE__, __LINE__);
    char *retval = NULL;
    char *found = strstr(source, searchStr);

    if (found) {
        size_t len = 0;
        for (; source < found; ++source) {
            if ((*source & 0xC0) != 0x80) {
                ++len;
            }
        }
        mkDoubleToChar(len + 1.0, posStr);
    } else {
        mkDoubleToChar(0, posStr);
    }

    retval = mkStrdup(currentMKSession->heapHandle, posStr, __FILE__, __LINE__);

    mkFree(currentMKSession->heapHandle, posStr);
    return retval;
}
