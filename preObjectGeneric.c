#include "stdafx.h"

#include "preObjectGeneric.h"

#include "preObjectVariable.h"
#include "preObjectFunction.h"


PreObjectGeneric   *createPreObjectGeneric(SodiumSession *currentMKSession, HTSQLPage *page, char *pObjectName, ObjectGenericType pObjectType, void *pPreObject) {

    PreObjectGeneric *object = (PreObjectGeneric*) mkMalloc(currentMKSession->heapHandle, sizeof(PreObjectGeneric), __FILE__, __LINE__);
    if (pObjectName != NULL) {
        object->objectName  = mkStrdup(currentMKSession->heapHandle, pObjectName, __FILE__, __LINE__);
    }
    object->objectGenericType = pObjectType;
    switch(pObjectType) {
        case POST_OBJECT_GENERIC_TYPE_VARIABLE: {
            object->value.preObjectVariable   = pPreObject;
            break;
        }
        case POST_OBJECT_GENERIC_TYPE_FUNCTION: {
            object->value.preObjectFunction   = pPreObject;
            object->value.preObjectFunction->lineNumberOuter = currentMKSession->sessionDebugInfo->lineNumberOuter;
            break;
        }
/*        case POST_OBJECT_GENERIC_TYPE_RESULTSET: {
            object->preObjectResultSet  = pPreObject;
            break;
        }*/
        default: {
            mkAssert("pObjectType gecersiz bir deger sahip", NULL);
        }
    }
	
    object->next                = NULL;

    if (page) {
        // Page object is exists.
        if (page->rootPreObjectGeneric == NULL) {
            page->rootPreObjectGeneric    = object;
            page->lastPreObjectGeneric    = object;
        } else {
            page->lastPreObjectGeneric->next = object;
            page->lastPreObjectGeneric       = object;
        }
    } else {
        // Page object is not exists. Adding newly created preObjectGeneric to the root page
        if (currentMKSession->rootPage->rootPreObjectGeneric == NULL) {
            currentMKSession->rootPage->rootPreObjectGeneric    = object;
            currentMKSession->rootPage->lastPreObjectGeneric    = object;
        } else {
            currentMKSession->rootPage->lastPreObjectGeneric->next = object;
            currentMKSession->rootPage->lastPreObjectGeneric       = object;
        }
    }
    return object;
}




PreObjectGeneric *findPreGenericObjectByName(SodiumSession *currentMKSession, HTSQLPage *page, const char *preGenericObjectName) {
    if (page == NULL || preGenericObjectName==NULL || strlen(preGenericObjectName)==0) {
        return NULL;
    }
    PreObjectGeneric *current = NULL;

    // Step.1: Looking for page object
    current = page->rootPreObjectGeneric;
    while(current != NULL) {
        if (strcmp(current->objectName, preGenericObjectName) == 0) {
            return current;
        }
        current = current->next;
    }

    // Step.2: Looking for root objects
    current = currentMKSession->rootPage->rootPreObjectGeneric;
    while(current != NULL) {
        if (strcmp(current->objectName, preGenericObjectName) == 0) {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

/**
 *
 */
PreObjectGeneric   *getCurrentPreGenericObject(SodiumSession *currentMKSession, HTSQLPage *page) {
    if (page) {
        return page->lastPreObjectGeneric;
    } else {
        return currentMKSession->rootPage->lastPreObjectGeneric;
    }
}





void  destroyPreObjectGeneric(SodiumSession *currentMKSession, PreObjectGeneric *object) {

    if (object->objectName != NULL) {
        mkFree(currentMKSession->heapHandle, object->objectName);
        object->objectName = NULL;
    }
    if(object->value.preObjectVariable != NULL) {
        destroyPreObjectVariable(currentMKSession, object->value.preObjectVariable);
        object->value.preObjectVariable = NULL;
    }
    if(object->value.preObjectFunction) {
        destroyPreObjectFunction(currentMKSession, object->value.preObjectFunction);
        object->value.preObjectFunction = NULL;
    }
    mkFree(currentMKSession->heapHandle, object);
}


