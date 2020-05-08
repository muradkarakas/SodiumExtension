#pragma once




typedef enum _SESSION_END_REASON {
    SESSION_END_REASON_IDLE_TIMEOUT
} SESSION_END_REASON;

void        __sendSessionEndResponse(SodiumSession *currentMKSession, HTSQLPage *page, SESSION_END_REASON sessionEndReason);
const char *getSessionEndReasonDescrition(SodiumSession *currentMKSession, HTSQLPage *page, SESSION_END_REASON sessionEndReason);


