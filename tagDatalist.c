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

#include "pch.h"

#include "tagDatalist.h"

#include "tagConnection.h"

TAGDatalist *createTAGDatalist(SodiumSession *currentMKSession, HTSQLPage *page) {

    TAGDatalist *tagDatalist = mkMalloc(currentMKSession->heapHandle, sizeof(TAGDatalist), __FILE__, __LINE__ );
    if (!tagDatalist) {
        return NULL;
    }

    // Setting default values
    tagDatalist->next               = NULL;
    tagDatalist->id                 = NULL;
    tagDatalist->connectionName     = mkStrdup(currentMKSession->heapHandle, "default", __FILE__, __LINE__);;

    pushTAGDatalist(currentMKSession, page, tagDatalist);

    return tagDatalist;
}

TAGDatalist *findTAGDatalistByName(SodiumSession *currentMKSession, HTSQLPage *page, const char *pDatalistName) {
    TAGDatalist *obj = page->rootTAGDatalist;
    while (obj != NULL) {
        if (obj->id != NULL) {
            if ( strcmp(obj->id, pDatalistName) == 0 ) {
                return obj;
            }
        }
        obj = obj->next;
    }
    return NULL;
}

void destroyTAGDatalist(SodiumSession *currentMKSession, HTSQLPage *page, TAGDatalist *tagDatalist) {

    mkDebug(currentMKSession, SESSION_LOG_DETAIL_HTSQL_BUILTIN_CALL, "\n", "Datalist destroyed: '", tagDatalist->id, "'", NULL);

    if (tagDatalist->id != NULL) {
        mkFree(currentMKSession->heapHandle, tagDatalist->id);
        tagDatalist->id = NULL;
    }
    if (tagDatalist->connectionName != NULL) {
        mkFree(currentMKSession->heapHandle, tagDatalist->connectionName);
        tagDatalist->connectionName = NULL;
    }
    if (tagDatalist != NULL) {
        mkFree(currentMKSession->heapHandle, tagDatalist);
    }
}

void setTAGDatalistPropertyId(SodiumSession *currentMKSession, HTSQLPage *page, char *pPropValue) {
    TAGDatalist *current = getCurrentTAGDatalist(currentMKSession, page);
    size_t len = strlen(pPropValue);
    if (len == 0) {
        if (current->id != NULL) {
            mkFree(currentMKSession->heapHandle, current->id);
            current->id = NULL;
        }
        return;
    }
    if (current->id != NULL) {
		current->id = mkReAlloc(currentMKSession->heapHandle, current->id, len + 1);
    } else {
        current->id = mkMalloc(currentMKSession->heapHandle, len + 1, __FILE__, __LINE__);
    }
    strcpy_s( current->id, len + 1, pPropValue);
    current->id = stripTAGPropertyValue(current->id);
}

void setTAGDatalistPropertyConnectionName(SodiumSession *currentMKSession, HTSQLPage *page, char *pPropValue) {
    TAGDatalist *current = getCurrentTAGDatalist(currentMKSession, page);

    pPropValue = stripTAGPropertyValue(pPropValue);
	size_t len = strlen(pPropValue);
    if (current->connectionName == NULL) {
        current->connectionName = mkMalloc(currentMKSession->heapHandle, len + 1, __FILE__, __LINE__);
    } else {
        current->connectionName = mkReAlloc(currentMKSession->heapHandle, current->connectionName, len+1);
    }
    strcpy_s( current->connectionName, len+1, pPropValue);

    TAGConnection *tagConnection = findTAGConnectionByName(currentMKSession, page, pPropValue, true);
    if (!tagConnection) {
        mkPrint(currentMKSession, "Connection nesnesi bulunamadi.", 0);
    }
}

void pushTAGDatalist(SodiumSession *currentMKSession, HTSQLPage *page, TAGDatalist *pTAGDatalist) {
    if ( page->rootTAGDatalist == NULL) {
        page->rootTAGDatalist     = pTAGDatalist;
        page->currentTAGDatalist  = pTAGDatalist;
    } else {
        page->currentTAGDatalist->next = pTAGDatalist;
        page->currentTAGDatalist       = pTAGDatalist;
    }
}


TAGDatalist * getCurrentTAGDatalist(SodiumSession *currentMKSession, HTSQLPage *page) {
    return page->currentTAGDatalist;
}

void setCurrentTAGDatalist(SodiumSession *currentMKSession, HTSQLPage *page, TAGDatalist *tagDatalist) {
    page->currentTAGDatalist = tagDatalist;
}



