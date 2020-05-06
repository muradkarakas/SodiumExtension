#include "stdafx.h"

#include "onConnectionNotFound.h"
#include "userHTSQLFunction.h"


void runOnLogonHTSQLTriggerFunction(SodiumSession *currentMKSession, HTSQLPage *page, const char *connectionName) {

    callUserFunction(currentMKSession, page, "connection_not_found", 1, connectionName);

}
