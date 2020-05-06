#include "stdafx.h"

#include "mkapplication.h"


SodiumSession      *mkRootSessionInstance = NULL;
HANDLE        mkApplicationHeapHandle = NULL;
SodiumSession *mkCurrentMKSessionInstance = NULL;
SodiumSession *mkMKSessionInstanceDebugged = NULL;

void  destroyMKApplicationHeapHandle() {
    EnterCriticalSection(&htsqlCriticalSection);
    if (mkApplicationHeapHandle) {
        HeapDestroy(mkApplicationHeapHandle);
        mkApplicationHeapHandle = NULL;
    }
    LeaveCriticalSection(&htsqlCriticalSection);
}


void setMKApplicationHeapHandle() {
    EnterCriticalSection(&htsqlCriticalSection);
    if (mkApplicationHeapHandle == NULL) {
        mkApplicationHeapHandle = HeapCreate(HEAP_ZERO_MEMORY, 2048, 0);
    }
    LeaveCriticalSection(&htsqlCriticalSection);
}


SodiumSession *getMKSessionRootInstance() {
    return mkRootSessionInstance;
} 


SodiumSession *getCurrentMKSessionInstance() {
    return mkCurrentMKSessionInstance;
}

int getMKSessionCount() { 
	SodiumSession *session = mkRootSessionInstance;
	int        sessionCount = 0;
	while (session) {
		sessionCount++;
		session = session->next; 
	}
	return sessionCount;
}


SodiumSession *
FindSodiumSessionById(
    char *pMkSessionId
)
{

    SodiumSession *current = getMKSessionRootInstance();
    if (current == NULL) {
        return NULL;
    }
    do {
        if ( strcmp(current->id, pMkSessionId) == 0) {
            return current;
        }
        current = current->next;
    } while(current != NULL);

    return NULL;
}



