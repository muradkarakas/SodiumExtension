#include "pch.h"

#include "tagControlBlock.h"

#include "settingControlBlockItemValue.h"

void    createControlBlockVariable(SodiumSession *session, HTSQLPage *page, const char *cbName, const char *cbValue) {

	const char *cbValueLocal = (cbValue) ? cbValue : "";

    ControlBlockVariable *found = findControlBlockVariableByName(session, page, cbName);
    if (found) {
		CLOG(ERR_CORE_VARIABLE_ADDED_TWICE, cbName);
        return;
    }
    ControlBlockVariable *object = mkMalloc(session->heapHandle, sizeof(ControlBlockVariable), __FILE__, __LINE__);
    object->cbName  = mkStrdup(session->heapHandle, cbName, __FILE__, __LINE__);
    object->cbValue = mkStrdup(session->heapHandle, cbValueLocal, __FILE__, __LINE__);
    object->next    = NULL;

    if (page->rootCBVariable == NULL) {
        page->rootCBVariable    = object;
    } else {
        page->currentCBVariable->next = object;
    }
    page->currentCBVariable = object;

    mkDebug(session, SESSION_LOG_DETAIL_HTSQL_VARIABLE, "\n", "Control block variable added: ", cbName, " = '", cbValueLocal, "'", NULL);
}

void setControlBlockVariableValue(SodiumSession *session, HTSQLPage *page, ControlBlockVariable *cbVariableInstance, const char *value) {
	if (cbVariableInstance) {
		if (cbVariableInstance->cbValue) {
			mkFree(session->heapHandle, cbVariableInstance->cbValue);
			cbVariableInstance->cbValue = NULL;
		}
		cbVariableInstance->cbValue = mkStrdup(session->heapHandle, value, __FILE__, __LINE__);

		__sendSetControlBlockItemValueResponse(session, cbVariableInstance->cbName, cbVariableInstance->cbValue);
	}
}

ControlBlockVariable *findControlBlockVariableByName(SodiumSession *session, HTSQLPage *page, const char *cbName) {
    ControlBlockVariable *object = page->rootCBVariable;
    while(object) {
        if (strcmp(object->cbName, cbName) == 0) {
            return object;
        }
        object = object->next;

    }
    return NULL;
}

void   destoryControlBlockVariables(SodiumSession *session, HTSQLPage *page) {
    ControlBlockVariable *object = page->rootCBVariable;
    while(object) {
        if (object->cbName) {
            mkDebug(session, SESSION_LOG_DETAIL_HTSQL_VARIABLE, "\n", "Control block variable destroyed: ", object->cbName, " = '", object->cbValue, "'", NULL);
            mkFree(session->heapHandle, object->cbName);
            object->cbName = NULL;
        }
        if (object->cbValue) {
            mkFree(session->heapHandle, object->cbValue);
            object->cbValue = NULL;
        }
        ControlBlockVariable *backupToFree = object;
        object = object->next;
        mkFree(session->heapHandle, backupToFree);
    }
    page->rootCBVariable    = NULL;
    page->currentCBVariable = NULL;
}


