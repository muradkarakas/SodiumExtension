#include "stdafx.h"

#include "preObjectVariable.h"


#include "mkHiredis.h"


PreObjectVariable    *createPreObjectVariable(SodiumSession *currentMKSession, PREOBJECTVARIABLETYPE pPreObjectVariableType, const char *pVariableName, const char *pPreObjectVariableValue) {
	PreObjectVariable *object = NULL;
	switch (pPreObjectVariableType) {
		case PREOBJECTTYPE_VARIABLE_NUMBER:
		case PREOBJECTTYPE_VARIABLE_VARCHAR:
		case PREOBJECTTYPE_VARIABLE_DATE:
		case PREOBJECTTYPE_VARIABLE_VOID:
		case PREOBJECTTYPE_VARIABLE_BOOL: {
			object = createPreObjectVariableVarchar(currentMKSession, pPreObjectVariableType, pVariableName, pPreObjectVariableValue);
			break;
		}
		case PREOBJECTTYPE_VARIABLE_REDIS: {
			object = createPreObjectVariableRedis(currentMKSession, pVariableName);
			break;
		}
		case PREOBJECTTYPE_VARIABLE_RESULTSET: {
			object = createPreObjectVariableResultSet(currentMKSession, pVariableName);
			break;
		}
	}
	return object;
}

PreObjectVariable *createPreObjectVariableResultSet(SodiumSession *currentMKSession, const char *pVariableName) {
	PreObjectVariable *object = mkMalloc(currentMKSession->heapHandle, sizeof(PreObjectVariable), __FILE__, __LINE__);
	object->preObjectVariableType = PREOBJECTTYPE_VARIABLE_RESULTSET;

	object->value.postObjectResultSet = NULL; 

	object->variableName = mkStrdup(currentMKSession->heapHandle, pVariableName, __FILE__, __LINE__);
	return object;
}

void setPreObjectVariableValueRecordSet(SodiumSession *currentMKSession, PreObjectVariable *preObjectVariable, PostObjectResultSet *newRecordsetInstance) {
	if (preObjectVariable) {
		if (preObjectVariable->value.postObjectResultSet) {
#pragma message("recordset'i nasýl yok edeceðiz")
			preObjectVariable->value.postObjectResultSet = NULL;
		}
		if (newRecordsetInstance) {
			preObjectVariable->value.postObjectResultSet = newRecordsetInstance;
		}
	}
}

PreObjectVariable *createPreObjectVariableNumber(SodiumSession *currentMKSession, PREOBJECTVARIABLETYPE pPreObjectVariableType, const char *pVariableName, int pPreObjectVariableValue) {
    PreObjectVariable *object = mkMalloc(currentMKSession->heapHandle, sizeof(PreObjectVariable), __FILE__, __LINE__);
	
    object->preObjectVariableType  = pPreObjectVariableType;

    object->value.number = pPreObjectVariableValue;

    object->variableName   = mkStrdup(currentMKSession->heapHandle, pVariableName, __FILE__, __LINE__);
    return object;
}


void setPreObjectVariableValueVarchar(SodiumSession *currentMKSession, PreObjectVariable *preObjectVariable, const char *newStrValue) {
    if (preObjectVariable) {
        if (preObjectVariable->value.text) {
            mkFree(currentMKSession->heapHandle, preObjectVariable->value.text);
			preObjectVariable->value.text = NULL;
        }
        preObjectVariable->value.text = mkStrdup(currentMKSession->heapHandle, newStrValue, __FILE__, __LINE__);
    }
}  


PreObjectVariable    *createPreObjectVariableVarchar(SodiumSession *currentMKSession, PREOBJECTVARIABLETYPE pPreObjectVariableType, const char *pVariableName, const char *pPreObjectVariableValue) {
	PreObjectVariable *object = mkMalloc(currentMKSession->heapHandle, sizeof(PreObjectVariable), __FILE__, __LINE__);
	object->preObjectVariableType = pPreObjectVariableType;

	if (pPreObjectVariableValue) {
		object->value.text = mkStrdup(currentMKSession->heapHandle, pPreObjectVariableValue, __FILE__, __LINE__);
	}
	else {
		object->value.text = NULL;
	}

	object->variableName = mkStrdup(currentMKSession->heapHandle, pVariableName, __FILE__, __LINE__);
	return object;
}

void setPreObjectVariableValueRedis(SodiumSession *currentMKSession, PreObjectVariable *preObjectVariable, redisContext *newConnectionInstance) {
	if (preObjectVariable) {
		if (preObjectVariable->value.redisContextValue) {
			closeRedisConnection(currentMKSession, preObjectVariable->value.redisContextValue);
			preObjectVariable->value.redisContextValue = NULL;
		}
		if (newConnectionInstance) {
			preObjectVariable->value.redisContextValue = newConnectionInstance;
		}		
	}
}

PreObjectVariable    *createPreObjectVariableBool(SodiumSession *currentMKSession, PREOBJECTVARIABLETYPE pPreObjectVariableType, const char *pVariableName, const char *pPreObjectVariableValue) {
    PreObjectVariable *object = mkMalloc(currentMKSession->heapHandle, sizeof(PreObjectVariable), __FILE__, __LINE__);
    object->preObjectVariableType  = pPreObjectVariableType;

    if (pPreObjectVariableValue) {
        object->value.boolValue    = mkStrdup(currentMKSession->heapHandle, pPreObjectVariableValue, __FILE__, __LINE__);
    } else {
		object->value.boolValue = NULL; // raise compiler warning but do not modify, or if you do, make correction in setPreObjectVariableValueVarchar function
    }

    object->variableName    = mkStrdup(currentMKSession->heapHandle, pVariableName, __FILE__, __LINE__);
    return object;
}

PreObjectVariable    *createPreObjectVariableRedis(SodiumSession *currentMKSession, const char *pVariableName) {
	PreObjectVariable *object = mkMalloc(currentMKSession->heapHandle, sizeof(PreObjectVariable), __FILE__, __LINE__);
	object->preObjectVariableType = PRE_VARIABLE_TYPE_REDIS;

	object->value.redisContextValue = NULL;
	
	object->variableName = mkStrdup(currentMKSession->heapHandle, pVariableName, __FILE__, __LINE__);
	return object;
}

PreObjectVariable    *createPreObjectVariableDate(SodiumSession *currentMKSession, PREOBJECTVARIABLETYPE pPreObjectVariableType, const char *pVariableName, const char *pPreObjectVariableValue) {
    PreObjectVariable *object = mkMalloc(currentMKSession->heapHandle, sizeof(PreObjectVariable), __FILE__, __LINE__);
    object->preObjectVariableType  = pPreObjectVariableType;

    if (pPreObjectVariableValue != NULL) {
        object->value.date = mkStrdup(currentMKSession->heapHandle, pPreObjectVariableValue, __FILE__, __LINE__);
    } else {
        object->value.date = NULL;
    }

    object->variableName    = mkStrdup(currentMKSession->heapHandle, pVariableName, __FILE__, __LINE__);
    return object;
}


/**
 *
 */
void destroyPreObjectVariable(SodiumSession *currentMKSession, PreObjectVariable *object) {
	if (object == NULL)
		return;

    if (object->variableName != NULL) {
        mkFree(currentMKSession->heapHandle, object->variableName);
        object->variableName = NULL;
    }

	if (object->preObjectVariableType == PREOBJECTTYPE_VARIABLE_VARCHAR) {
		if (object->value.text) {
			mkFree(currentMKSession->heapHandle, object->value.text);
			object->value.text = NULL;
		}
	}
	else if (object->preObjectVariableType == PREOBJECTTYPE_VARIABLE_REDIS) {
		if (object->value.redisContextValue) {
			closeRedisConnection(currentMKSession, object->value.redisContextValue);
			object->value.redisContextValue = NULL;
		}
	} 
	else if (object->preObjectVariableType == PREOBJECTTYPE_VARIABLE_RESULTSET) {
#pragma message("recordset'i nasýl yok edeceðiz. bu if dogrumu")
		if (object->value.postObjectResultSet) {
			mkFree(currentMKSession->heapHandle, object->value.postObjectResultSet);
			object->value.postObjectResultSet = NULL;
		}
	}
    mkFree(currentMKSession->heapHandle, object);
}

