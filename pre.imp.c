#include "stdafx.h"

#include "pre.imp.h"


#include "preObjectGeneric.h"
#include "preObjectVariable.h"
#include "preObjectFunction.h"

void __setFunctionReturnVariable(SodiumSession *currentMKSession, HTSQLPage *page, PREOBJECTVARIABLETYPE pPreObjectVariableType) {

    PreObjectGeneric *currentGenericObject = getCurrentPreGenericObject(currentMKSession, page);
    switch(currentGenericObject->objectGenericType) {
        case POST_OBJECT_GENERIC_TYPE_FUNCTION: {
            PreObjectVariable *preObjectReturnVariable = NULL;
            switch(pPreObjectVariableType) {
                case PRE_VARIABLE_TYPE_VARCHAR : {
                    preObjectReturnVariable = createPreObjectVariableVarchar(currentMKSession, pPreObjectVariableType, "functionReturnValue", NULL);
                    break;
                }
                case PRE_VARIABLE_TYPE_NUMBER : {
                    preObjectReturnVariable = createPreObjectVariableNumber(currentMKSession, pPreObjectVariableType, "functionReturnValue", 0);
                    break;
                }
                case PRE_VARIABLE_TYPE_DATE : {
                    preObjectReturnVariable = createPreObjectVariableDate(currentMKSession, pPreObjectVariableType, "functionReturnValue", NULL);
                    break;
                }
                case PRE_VARIABLE_TYPE_BOOL : {
                    preObjectReturnVariable = createPreObjectVariableBool(currentMKSession, pPreObjectVariableType, "functionReturnValue", NULL);
                    break;
                }
                case PRE_VARIABLE_TYPE_VOID : {
                    preObjectReturnVariable = NULL;
                    break;
                }
                default: {
                    mkAssert("Gecersiz pPreObjectVariableType degeri.", NULL);
                }
            }
            setFunctionReturnVariable(currentMKSession, page, currentGenericObject, preObjectReturnVariable);
            break;
        }
        default: {
            mkAssert("currentGenericObject->objectGenericType", NULL);
        }
    }

}


/**
 *
 */
void __addFunctionParameterVariable(SodiumSession *currentMKSession, HTSQLPage *page, PREOBJECTVARIABLETYPE pPreObjectVariableType, char *pParameterName) {
    PreObjectGeneric *currentGenericObject = getCurrentPreGenericObject(currentMKSession, page);
    PreObjectVariable *preObjectVariable = NULL;
    switch(currentGenericObject->objectGenericType) {
        case POST_OBJECT_GENERIC_TYPE_FUNCTION: {
            preObjectVariable =__addFunctionParameterVariableVariant(currentMKSession, pPreObjectVariableType, pParameterName);
            addFunctionParameterVariable(currentMKSession, page, currentGenericObject, preObjectVariable);
            break;
        }
        default: {
            mkAssert("Gecersiz currentGenericObject->objectGenericType degeri.", NULL);
        }
    }
}

PreObjectVariable *__addFunctionParameterVariableVariant(SodiumSession *currentMKSession, PREOBJECTVARIABLETYPE pPreObjectVariableType, char *pParameterName) {
    PreObjectVariable *preObjectVariable = NULL;
    switch(pPreObjectVariableType) {
        case PREOBJECTTYPE_VARIABLE_VARCHAR: {
            preObjectVariable = createPreObjectVariableVarchar(currentMKSession, pPreObjectVariableType, pParameterName, NULL);
            break;
        }
        case PREOBJECTTYPE_VARIABLE_NUMBER: {
            preObjectVariable = __addFunctionParameterVariableNumber(currentMKSession, pPreObjectVariableType, pParameterName);
            break;
        }
        case PREOBJECTTYPE_VARIABLE_DATE: {
            preObjectVariable = __addFunctionParameterVariableDate(currentMKSession, pPreObjectVariableType, pParameterName);
            break;
        }
        default: {
            mkAssert("Gecersiz pPreObjectVariableType degeri.", NULL);
        }
    }
    return preObjectVariable;
}

/**
 *
 */
PreObjectVariable *__addFunctionParameterVariableDate(SodiumSession *currentMKSession, PREOBJECTVARIABLETYPE pPreObjectVariableType, char *pParameterName) {
    return createPreObjectVariableDate(currentMKSession, pPreObjectVariableType, pParameterName, NULL);
}

/**
 *
 */
PreObjectVariable *__addFunctionParameterVariableNumber(SodiumSession *currentMKSession, PREOBJECTVARIABLETYPE pPreObjectVariableType, char *pParameterName) {
    return createPreObjectVariableNumber(currentMKSession, pPreObjectVariableType, pParameterName, 0);
}

/**
 *
 */
PreObjectVariable *__addFunctionParameterVariableVarchar(SodiumSession *currentMKSession, PREOBJECTVARIABLETYPE pPreObjectVariableType, char *pParameterName) {
    return createPreObjectVariableVarchar(currentMKSession, pPreObjectVariableType, pParameterName, NULL);
}

/**
 *
 */
void __setFunctionBody(SodiumSession *currentMKSession, HTSQLPage *page, PreObjectGeneric *currentGenericObject, char *pFunctionBody) {
    switch(currentGenericObject->objectGenericType) {
        case POST_OBJECT_GENERIC_TYPE_FUNCTION: {
            currentGenericObject->value.preObjectFunction->preObjectFunctionBodyStrDeclaration = mkStrdup(currentMKSession->heapHandle, pFunctionBody, __FILE__, __LINE__);
            break;
        }
        default: {
            mkAssert("Gecersiz currentGenericObject->objectGenericType degeri.", NULL);
        }
    }
}
