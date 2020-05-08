#include "pch.h"

#include "ajaxAsGet.h"

#include "dispatcher.h"
#include "image.h"


void serveAsGetRequest(SodiumSession *currentMKSession, HTSQLPage *page) {
    const char *nativeproceduretorun = GetQueryStringParam(currentMKSession, page, "nativeproceduretorun");

    if (nativeproceduretorun) {
        if (strcmp(nativeproceduretorun, "getimage") == 0) {
            serveAsGetRequestGetImage(currentMKSession, page);
        }
    }
}








