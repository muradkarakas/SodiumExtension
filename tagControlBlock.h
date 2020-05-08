#pragma once




void                    destoryControlBlockVariables(SodiumSession *currentMKSession, HTSQLPage *page);
void                    createControlBlockVariable(SodiumSession *currentMKSession, HTSQLPage *page, const char *cbName, const char *cbValue);
ControlBlockVariable	*findControlBlockVariableByName(SodiumSession *currentMKSession, HTSQLPage *page, const char *cbName);
void					setControlBlockVariableValue(SodiumSession *currentMKSession, HTSQLPage *page, ControlBlockVariable *cbVariableInstance, const char *value);

