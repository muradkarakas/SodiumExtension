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

#include "postObjectGeneric.h"

#include "preObjectVariable.h"
#include "postObjectResultSet.h"


PostObjectGeneric     *createPostObjectGeneric(SodiumSession *currentMKSession, HTSQLPage *page, ObjectGenericType postObjectType, const char *pObjectName, void *pPostObject) {

	PostObjectGeneric *object = (PostObjectGeneric*) mkMalloc(currentMKSession->heapHandle, sizeof(PostObjectGeneric), __FILE__, __LINE__);
	object->value.preObjectVariable = NULL;
    if (pObjectName != NULL) {
        object->objectName  = mkStrdup(currentMKSession->heapHandle, pObjectName, __FILE__, __LINE__);
    }
    switch(postObjectType) {
		case POST_OBJECT_GENERIC_TYPE_FUNCTION: 
		case POST_OBJECT_GENERIC_TYPE_FUNCTION_ARGS:
		//case PREOBJECTTYPE_VARIABLE_RESULTSET:
        case POST_OBJECT_GENERIC_TYPE_VARIABLE: {
            object->value.preObjectVariable      = (PreObjectVariable*) pPostObject;
			//object->value.preObjectFunction->lineNumberOuter = currentMKSession->sessionDebugInfo->lineNumberOuter;
            break;
        }
        /*case POST_OBJECT_GENERIC_TYPE_RESULTSET: {
            object->value.preObjectVariable     = pPostObject;
            break;
        }*/
    }
	object->objectGenericType = postObjectType;
    object->next           = NULL;

    if (page->currentCallStack->rootPostObjectGeneric == NULL) {
        page->currentCallStack->rootPostObjectGeneric    = object;
        page->currentCallStack->currentPostObjectGeneric = object;
    } else {
        page->currentCallStack->currentPostObjectGeneric->next = object;
        page->currentCallStack->currentPostObjectGeneric       = object;
    }

    return object;
}

PostObjectGeneric *findPostObjectGenericObject(SodiumSession *currentMKSession, HTSQLPage *page, const char *pObjectName) {
	if (page) {
		if (page->currentCallStack) {
			PostObjectGeneric *current = page->currentCallStack->rootPostObjectGeneric;
			while (current != NULL) {
				if (strcmp(current->objectName, pObjectName) == 0) {
					return current;
				}
				current = current->next;
			}
		}
	}
    return NULL;
}

void setPostObjectGenericVariableValueVarchar(SodiumSession *currentMKSession, PreObjectVariable *postObjectVariable, const char *newStrValue) {
    if (postObjectVariable) {
        if (postObjectVariable->value.text) {
            mkFree(currentMKSession->heapHandle, postObjectVariable->value.text);
            postObjectVariable->value.text = NULL;
        }
        postObjectVariable->value.text = mkStrdup(currentMKSession->heapHandle, newStrValue, __FILE__, __LINE__);
    }
}



PreObjectVariable *findPostObjectGenericPostObjectVariable(SodiumSession *currentMKSession, HTSQLPage *page, const char *pObjectName) {
	if (page) {
		if (page->currentCallStack) {
			PostObjectGeneric *current = page->currentCallStack->rootPostObjectGeneric;
			while (current != NULL) {
				if (strcmp(current->objectName, pObjectName) == 0) {
					return current->value.preObjectVariable;
				}
				current = current->next;
			}
		}
	}
    return NULL;
}


PostObjectResultSet *findPostObjectGenericPostResultSetVariable(SodiumSession *currentMKSession, HTSQLPage *page, const char *pObjectName) {
	if (page) {
		if (page->currentCallStack) {
			PostObjectGeneric *current = page->currentCallStack->rootPostObjectGeneric;
			while (current != NULL) {
				if (strcmp(current->objectName, pObjectName) == 0) {
					return current->value.preObjectVariable->value.postObjectResultSet;
				}
				current = current->next;
			}
		}
	}
    return NULL;
}

/**
 *
 */
void  destroyPostObjectGeneric(SodiumSession *currentMKSession, PostObjectGeneric *object) {

    /*if (object->objectGenericType == POST_OBJECT_GENERIC_TYPE_VARIABLE) {
        mkDebug(currentMKSession, SESSION_LOG_DETAIL_HTSQL_VARIABLE, "\n", "Local variable destroyed : '", object->objectName, "'", NULL);
    }*/
    if (object->objectName) {
        mkFree(currentMKSession->heapHandle, object->objectName);
        object->objectName = NULL;
    }    
	if (object->value.preObjectVariable) {
		destroyPreObjectVariable(currentMKSession, object->value.preObjectVariable);
		object->value.preObjectVariable = NULL;
	}
}

