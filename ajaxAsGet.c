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








