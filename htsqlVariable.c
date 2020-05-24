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

#include "htsqlVariable.h"

#include "tagInput.h"
#include "tagControlBlock.h"
#include "tagControlBlock.h"
#include "tagDataBlock.h"
#include "tagDataBlockCurrentRow.h"
#include "preObjectVariable.h"
#include "preObjectGeneric.h"
#include "onConnectionNotFound.h"
#include "postObjectGeneric.h"
#include "mkHiredis.h"
#include "post.parser.imp.h"

PreObjectVariable *_findRedisPreObjectVariable(SodiumSession *session, HTSQLPage *page, const char *redisVariableName) {

	PreObjectVariable *redisPreObjectVariable = NULL;

	/** searching in local scope variables  */
	PreObjectVariable *postObjectVariable = findPostObjectGenericPostObjectVariable(session, page, redisVariableName);
	if (postObjectVariable) {
		if (postObjectVariable->preObjectVariableType == PREOBJECTTYPE_VARIABLE_REDIS) {
			redisPreObjectVariable = postObjectVariable;
		}
	}
	else {
		/** Searching in global scope variables  */
		PreObjectGeneric *preObjectGeneric = findPreGenericObjectByName(session, page, redisVariableName);

		if (preObjectGeneric) {
			if (preObjectGeneric->value.preObjectVariable->preObjectVariableType == PREOBJECTTYPE_VARIABLE_REDIS) {
				redisPreObjectVariable = preObjectGeneric->value.preObjectVariable;
			}
		}
	}
	return redisPreObjectVariable;  
} 

PreObjectVariable *findRedisPreObjectVariable(SodiumSession *session, HTSQLPage *page, const char *redisVariableName, bool isConnectionNotFoundTriggerBeExecuted) {
	
	PreObjectVariable *redisPreObjectVariable = NULL;

	PreObjectVariable *tmp = _findRedisPreObjectVariable(session, page, redisVariableName);
	
	if (tmp) {
		redisPreObjectVariable = tmp;  
	}

	// check whether the connection is alive
	if (tmp == NULL || tmp->value.redisContextValue == NULL) {
		
		if (isConnectionNotFoundTriggerBeExecuted) {
			// connection is not valid. execute "connection_not_valid" trigger
			runOnLogonHTSQLTriggerFunction(session, page, redisVariableName);
			tmp = _findRedisPreObjectVariable(session, page, redisVariableName);
			if (tmp && tmp->value.redisContextValue) {
				redisPreObjectVariable = tmp;
			}
		}
		
	}
	else {
		// check whether the connection is alive
		char *rep = redisCommandPing(session, page, redisPreObjectVariable->value.redisContextValue);
		if (rep) {
			if (isConnectionNotFoundTriggerBeExecuted && strcmp(rep, "PONG") != 0) {
				// connection is not valid. execute "connection_not_valid" trigger
				runOnLogonHTSQLTriggerFunction(session, page, redisVariableName);
				tmp = _findRedisPreObjectVariable(session, page, redisVariableName);
				if (tmp && tmp->value.redisContextValue) {
					redisPreObjectVariable = tmp;
				}
			} 
			mkFree(session->heapHandle, rep);
		}
	}

	return redisPreObjectVariable;
}

redisContext *findRedisContextVariable(SodiumSession *session, HTSQLPage *page, const char *redisValueName) {

	PreObjectVariable *preObjectVariable = findRedisPreObjectVariable(session, page, redisValueName, false);
	if (preObjectVariable && preObjectVariable->value.redisContextValue) {
		return preObjectVariable->value.redisContextValue;
	}
	return NULL;
}

char* findAndReturnValueOfAVariableInAllScopes(SodiumSession *session, HTSQLPage *page, const char *blockName, const char *itemName) {
    char *retValue = NULL;
	
	if (itemName == NULL) {
		return NULL;
	} else if (blockName == NULL) {
		retValue = getVariableValue(session, page, itemName);
	}
	else {
		retValue = getBlockVariableValue(session, page, blockName, itemName);
	}
    return retValue;
}


char *getVariableValue(SodiumSession *session, HTSQLPage *page, const char *variableName) {

	/** searching in local variables first */
	PreObjectVariable *preObjectVariable = findPostObjectGenericPostObjectVariable(session, page, variableName);
	if (preObjectVariable) {
		return mkStrdup(session->heapHandle, preObjectVariable->value.text, __FILE__, __LINE__);
	}
	else {
		/** Searching in global secondly */
		PreObjectGeneric *bObj = findPreGenericObjectByName(session, page, variableName);
		if (bObj && bObj->objectGenericType == POST_OBJECT_GENERIC_TYPE_VARIABLE) {
			return  mkStrdup(session->heapHandle, bObj->value.preObjectVariable->value.text, __FILE__, __LINE__);
		}
		else {
			/** not found */
			char *errMsg = mkStrcat(session->heapHandle, __FILE__, __LINE__, "Variable not found: '", variableName, "'", NULL);
			stopPostParserWithMessage(session, page, errMsg, "");
			mkFree(session->heapHandle, errMsg);
		}
	}
	return  mkStrdup(session->heapHandle, "", __FILE__, __LINE__);
}



char *getBlockVariableValue(SodiumSession *session, HTSQLPage *page, const char *blockName, const char *itemName) {

	char *retvalue = NULL;
	char *variableName = mkStrcat(session->heapHandle, __FILE__, __LINE__, ":", blockName, ".", itemName, NULL);

	/** Searching in Local stack */
	PreObjectVariable *object = findPostObjectGenericPostObjectVariable(session, page, variableName);

	if (object) {
		retvalue = mkStrdup(session->heapHandle, object->value.text, __FILE__, __LINE__);
	}
	else {
		/** Searching in data blocks */
		TAGDataBlock *pTAGDataBlock = findTAGDataBlockByName(session, page, blockName);
		TAGInput *input = findTAGDataBlockCurrentRowTAGInputItem(session, pTAGDataBlock, itemName);
		if (input == NULL) {

			/** Searching in Control block variables */
			ControlBlockVariable *cbObject = findControlBlockVariableByName(session, page, variableName);
			if (cbObject) {
				retvalue = mkStrdup(session->heapHandle, cbObject->cbValue, __FILE__, __LINE__);
			}
			else {
				/** Searching in session variables */
				PreObjectGeneric *sessionObject = findSessionGenericObject(session, variableName);
				if (sessionObject) {
					retvalue = mkStrdup(session->heapHandle, sessionObject->value.preObjectVariable->value.text, __FILE__, __LINE__);
				}
				else {
					CLOG(ERR_NAME_IS_NOT_FOUND, variableName);
					retvalue = mkStrdup(session->heapHandle, "", __FILE__, __LINE__);
				}
			}
		}
		else {
			retvalue = mkStrdup(session->heapHandle, input->value, __FILE__, __LINE__);
		}
	}

	mkFree(session->heapHandle, variableName);

	return retvalue;
}


PreObjectGeneric *createSessionGenericObject(SodiumSession *session, const char *pObjectName, ObjectGenericType pObjectType, void *pPreObject) {

	PreObjectGeneric *object = (PreObjectGeneric*)mkMalloc(session->heapHandle, sizeof(PreObjectGeneric), __FILE__, __LINE__);
	if (pObjectName != NULL) {
		object->objectName = mkStrdup(session->heapHandle, pObjectName, __FILE__, __LINE__);
	}
	object->objectGenericType = pObjectType;
	switch (pObjectType) {
	case POST_OBJECT_GENERIC_TYPE_VARIABLE: {
		object->value.preObjectVariable = pPreObject;
		break;
	}
	case POST_OBJECT_GENERIC_TYPE_FUNCTION: {
		object->value.preObjectFunction = pPreObject;
		break;
	}
	default: {
		mkAssert("pObjectType gecersiz bir deger sahip", NULL);
	}
	}

	object->next = NULL;

	if (session->rootSessionObjectGeneric == NULL) {
		session->rootSessionObjectGeneric = object;
		session->currentSessionObjectGeneric = object;
	}
	else {
		session->currentSessionObjectGeneric->next = object;
		session->currentSessionObjectGeneric = object;
	}

	return object;
}


PreObjectGeneric *findSessionGenericObject(SodiumSession *session, const char *sessionVariableName) {
	if (sessionVariableName == NULL || strlen(sessionVariableName) == 0) {
		return NULL;
	}

	PreObjectGeneric *current = session->rootSessionObjectGeneric;

	while (current != NULL) {
		if (strcmp(current->objectName, sessionVariableName) == 0) {
			return current;
		}
		current = current->next;
	}
	return NULL;
}

void setBlockVariableValue(SodiumSession *session, HTSQLPage *page, const char *blockName, const char *itemName, const char *newValue) {

	char *variableName = mkStrcat(session->heapHandle, __FILE__, __LINE__, ":", blockName, ".", itemName, NULL);

	/** searching for data block */
	TAGDataBlock *tagDataBlock = findTAGDataBlockByName(session, page, blockName);
	if (tagDataBlock) {
		TAGInput *input = findTAGInputByName(session, page, tagDataBlock, itemName);
		if (input) {
			if (input->inputType == INPUT_TAG_TYPE_IMAGE) {
				setTAGInputPropertySrc(session, page, tagDataBlock, itemName, newValue);
			}
			else {
				modifyTAGDatablockCurrentRowItemValue(session, page, blockName, itemName, newValue);
				/*
				Burayý comment'ledim. Client'a block deðiþkeninin deðerini göndermeden de ayný sonucu alýyoruz. Zaten block görüntüleniyorsa atanan deðer de görüntüleniyor.
				if (session->currentRequestMethodType == REQUEST_TYPE_POST) {
				__sendSetDataBlockItemValueResponse(session, blockName, tagDataBlock->currentRowRowIdTAGInput->value, itemName, newValue, true);
				}*/
			} 
		}
		else {
			CLOG(ERR_DATABLOCK_ITEM_NOT_FOUND, itemName);
		}
	}
	else {
		/** searching in control blocks */
		ControlBlockVariable *cbObject = findControlBlockVariableByName(session, page, variableName);
		if (cbObject) {
			// setting controlblock instance
			setControlBlockVariableValue(session, page, cbObject, newValue);
		}
		else {
			/** Searching in session scope variables */
			PreObjectGeneric *sessionGenericObject = findSessionGenericObject(session, variableName);
			if (sessionGenericObject && sessionGenericObject->value.preObjectVariable) {
				setPreObjectVariableValueVarchar(session, sessionGenericObject->value.preObjectVariable, newValue);
			}
			else {
				/** session scope variable do not need to be created explicitly. They are created when they are assigned */
				PreObjectVariable *sesobj = createPreObjectVariableVarchar(session, PREOBJECTTYPE_VARIABLE_VARCHAR, variableName, newValue);
				createSessionGenericObject(session, variableName, POST_OBJECT_GENERIC_TYPE_VARIABLE, sesobj);
			}
		}
	}
	mkFree(session->heapHandle, variableName);
}