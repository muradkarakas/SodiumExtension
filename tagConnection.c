#include "pch.h"

#include "tagConnection.h"

#include "onConnectionNotFound.h"

DBInt_Connection *getRootDBIntConnection(SodiumSession *currentMKSession, HTSQLPage *page) {
	DBInt_Connection *retval = NULL;

	TAGConnection *tagConnection = getRootTAGConnection1(currentMKSession, page);
	if (tagConnection) {
		return tagConnection->mkDBConnection;
	}
	else {
		return NULL;
	}
}

TAGConnection *getRootTAGConnection1(SodiumSession *currentMKSession, HTSQLPage *page) {
	if (currentMKSession) {
		if (currentMKSession->rootTAGConnection == NULL) {
			runOnLogonHTSQLTriggerFunction(currentMKSession, page, "default");
		}
		return currentMKSession->rootTAGConnection;
	}
	else {
		return NULL;
	}
}

TAGConnection *getActiveTAGConnection(SodiumSession *currentMKSession, HTSQLPage *page) {
	if (currentMKSession && currentMKSession->activeTAGConnection) {
		return currentMKSession->activeTAGConnection;
	}
	else {
		return NULL;
	}
}

void setActiveTAGConnection(SodiumSession *currentMKSession, HTSQLPage *page, TAGConnection *tagConnection) {
	if (currentMKSession) {
		currentMKSession->activeTAGConnection = tagConnection;
	}
}

void setTAGConnectionPropertyConnectionName(SodiumSession *currentMKSession, TAGConnection *tagConnection, const char *pPropValue) {

    size_t len = strlen(pPropValue);
    if (len == 0) {
        if (tagConnection->connectionName != NULL) {
            mkFree(currentMKSession->heapHandle, tagConnection->connectionName);
            tagConnection->connectionName = NULL;
        }
        return;
    }
    if (tagConnection->connectionName) {
        tagConnection->connectionName = mkReAlloc(currentMKSession->heapHandle, tagConnection->connectionName, len + 1);
    } else {
        tagConnection->connectionName = mkMalloc(currentMKSession->heapHandle, len + 1, __FILE__, __LINE__);
    }
    strcpy_s( tagConnection->connectionName, len+1, pPropValue);
    tagConnection->connectionName = stripTAGPropertyValue(tagConnection->connectionName);
}




/**
 *
 */
void setTAGConnectionPropertyUserName(SodiumSession *currentMKSession, TAGConnection *tagConnection, const char *pPropValue) {

	size_t len = strlen(pPropValue);
    if (len == 0) {
        if (tagConnection->userName) {
            mkFree(currentMKSession->heapHandle, tagConnection->userName);
            tagConnection->userName = NULL;
        }
        return;
    }
    if (tagConnection->userName) {
        tagConnection->userName = mkReAlloc(currentMKSession->heapHandle, tagConnection->userName, len + 1);
    } else {
        tagConnection->userName = mkMalloc(currentMKSession->heapHandle, len + 1, __FILE__, __LINE__);
    }
    strcpy_s( tagConnection->userName, len+1, pPropValue);
    tagConnection->userName = stripTAGPropertyValue(tagConnection->userName);
}

void setTAGConnectionMKDBConnectionProperty(SodiumSession *currentMKSession, TAGConnection *tagConnection, DBInt_Connection *mkDBConnection) {
	tagConnection->mkDBConnection = mkDBConnection;
}

void setTAGConnectionPropertyUserPassword(SodiumSession *currentMKSession, TAGConnection *tagConnection, const char *pPropValue) {

	size_t len = strlen(pPropValue);
    if (len == 0) {
        if (tagConnection->userPassword) {
            mkFree(currentMKSession->heapHandle, tagConnection->userPassword);
            tagConnection->userPassword = NULL;
        }
        return;
    }
    if (tagConnection->userPassword) {
        tagConnection->userPassword = mkReAlloc(currentMKSession->heapHandle, tagConnection->userPassword, len + 1);
    } else {
        tagConnection->userPassword = mkMalloc(currentMKSession->heapHandle, len + 1, __FILE__, __LINE__);
    }
    strcpy_s(tagConnection->userPassword, len+1, pPropValue);
    tagConnection->userPassword = stripTAGPropertyValue(tagConnection->userPassword);
}



void setTAGConnectionPropertyInstanceName(SodiumSession *currentMKSession, TAGConnection *tagConnection, const char *pInstanceName ) {
    
	size_t len = strlen(pInstanceName);
    if (len == 0) {
        if (tagConnection->instanceName != NULL) {
            mkFree(currentMKSession->heapHandle, tagConnection->instanceName);
            tagConnection->instanceName = NULL;
        }
        return;
    }
    if (tagConnection->instanceName) {
        tagConnection->instanceName = mkReAlloc(currentMKSession->heapHandle, tagConnection->instanceName, len + 1);
    } else {
        tagConnection->instanceName = mkMalloc(currentMKSession->heapHandle, len + 1, __FILE__, __LINE__);
    }
    strcpy_s( tagConnection->instanceName, len+1, pInstanceName);
    tagConnection->instanceName = stripTAGPropertyValue(tagConnection->instanceName);
}


TAGConnection *createTAGConnection(SodiumSession *currentMKSession, HTSQLPage *page) {

    TAGConnection *tagConnection = mkMalloc(currentMKSession->heapHandle, sizeof(TAGConnection), __FILE__, __LINE__ ); 

    if (!tagConnection) {
        return NULL;
    }

    // Setting default values
    tagConnection->next             = NULL;
    tagConnection->connectionName   = NULL;
    tagConnection->userName         = NULL;
    tagConnection->userPassword     = NULL;
    tagConnection->instanceName     = NULL;
    tagConnection->mkDBConnection	= NULL;

    pushTAGConnection(currentMKSession, tagConnection);

    return tagConnection;
}


void destroyTAGConnection(SodiumSession *currentMKSession, TAGConnection *tagConnection) {
    mkDebug(currentMKSession, SESSION_LOG_DETAIL_DB_EVENTS, "\n", "Connection destroyed: '", tagConnection->connectionName, "'", NULL);

    if (tagConnection->mkDBConnection != NULL) {
		DBInt_DestroyDBConnection(tagConnection->mkDBConnection);
        tagConnection->mkDBConnection = NULL;
    }

    if (tagConnection->connectionName != NULL) {
        mkFree(currentMKSession->heapHandle, tagConnection->connectionName);
        tagConnection->connectionName = NULL;
    }

    if (tagConnection->userName != NULL) {
        mkFree(currentMKSession->heapHandle, tagConnection->userName);
        tagConnection->userName = NULL;
    }

    if (tagConnection->userPassword != NULL) {
        mkFree(currentMKSession->heapHandle, tagConnection->userPassword);
        tagConnection->userPassword = NULL;
    }

    if (tagConnection->instanceName != NULL) {
        mkFree(currentMKSession->heapHandle, tagConnection->instanceName);
        tagConnection->instanceName = NULL;
    }

    if (tagConnection != NULL) {
        mkFree(currentMKSession->heapHandle, tagConnection);
    }
}

/**
 *  Put the object in linked list and set current TAG object to NULL.
 */
void pushTAGConnection(SodiumSession *currentMKSession, TAGConnection *pTAGConnection) {
    if ( currentMKSession->rootTAGConnection == NULL) {
        currentMKSession->rootTAGConnection     = pTAGConnection;
        currentMKSession->currentTAGConnection  = pTAGConnection;
		currentMKSession->activeTAGConnection   = pTAGConnection;
    } else {
        currentMKSession->currentTAGConnection->next = pTAGConnection;
        currentMKSession->currentTAGConnection       = pTAGConnection;
    }
}


/**
 *
 */
TAGConnection * getCurrentTAGConnection(SodiumSession *currentMKSession, HTSQLPage *page) {
    return currentMKSession->currentTAGConnection;
}

/**
 *  set current TAGConnection object to "tagConnection"
 */
void setCurrentTAGConnection(SodiumSession *currentMKSession, TAGConnection *tagConnection) {
    currentMKSession->currentTAGConnection = tagConnection;
}


/**
 *  If found return object, otherwise return NULL
 */
TAGConnection  *findTAGConnectionByName(SodiumSession *currentMKSession, HTSQLPage *page, const char *pPropValue, bool callLogonTrigger) {
    TAGConnection *obj = currentMKSession->rootTAGConnection;
    while (obj) {
        if (obj->connectionName) {
            if ( strcmp(obj->connectionName, pPropValue) == 0 ) {
                return obj;
            }
        }
        obj = obj->next;
    }

	// No connection created previously. I create it if requested to do so.
    if (callLogonTrigger) {
        runOnLogonHTSQLTriggerFunction(currentMKSession, page, pPropValue);
		// lets check one more time, connection has been createdd by user trigger function or not. If user created a named connection, its reference will be returned.
		// In order to prevent infinite loop, I call the function itself with "false" value as a last parameter.
        return findTAGConnectionByName(currentMKSession, page, pPropValue, false);
    } else {
        return NULL;
    }
}


