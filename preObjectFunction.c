#include "pch.h"

#include "preObjectFunction.h"

#include "preObjectVariable.h"

PreObjectFunction  *createPreObjectFunction(SodiumSession *currentMKSession) {

    PreObjectFunction *object = mkMalloc(currentMKSession->heapHandle, sizeof(PreObjectFunction), __FILE__, __LINE__);

    object->parameterCountDeclaration                  = 0;
    object->preObjectFunctionBodyStrDeclaration        = NULL;
    object->preObjectFunctionReturnValueDeclaration    = NULL;

    return object;
}


/**
 *
 */
void addFunctionParameterVariable(SodiumSession *currentMKSession, HTSQLPage *page, PreObjectGeneric *currentGenericObject, PreObjectVariable *preObjectVariable) {
	
	currentGenericObject->value.preObjectFunction->preObjectFunctionParametersDeclaration[currentGenericObject->value.preObjectFunction->parameterCountDeclaration] = preObjectVariable;
	currentGenericObject->value.preObjectFunction->parameterCountDeclaration++;
}

/**
 *
 */
void setFunctionReturnVariable(SodiumSession *currentMKSession, HTSQLPage *page, PreObjectGeneric *currentGenericObject, PreObjectVariable *preObjectVariable) {
	currentGenericObject->value.preObjectFunction->preObjectFunctionReturnValueDeclaration = preObjectVariable;
}



/**
 *
 */
void destroyPreObjectFunction(SodiumSession *currentMKSession, PreObjectFunction  *object) {

    for (int paramCountTemp = 0; paramCountTemp < object->parameterCountDeclaration; paramCountTemp++) {
        mkDebug(currentMKSession, SESSION_LOG_DETAIL_HTSQL_VARIABLE, "\n", "Function parameter destroyed: ", object->preObjectFunctionParametersDeclaration[paramCountTemp]->variableName, NULL);
        destroyPreObjectVariable(currentMKSession, object->preObjectFunctionParametersDeclaration[paramCountTemp]);
        object->preObjectFunctionParametersDeclaration[paramCountTemp] = NULL;
    }
    object->parameterCountDeclaration = 0;
    if (object->preObjectFunctionReturnValueDeclaration != NULL) {
        destroyPreObjectVariable(currentMKSession, object->preObjectFunctionReturnValueDeclaration);
        object->preObjectFunctionReturnValueDeclaration = NULL;
    };
    if (object->preObjectFunctionBodyStrDeclaration != NULL) {
        mkFree(currentMKSession->heapHandle, object->preObjectFunctionBodyStrDeclaration);
        object->preObjectFunctionBodyStrDeclaration = NULL;
    };
    mkFree(currentMKSession->heapHandle, object);
}
