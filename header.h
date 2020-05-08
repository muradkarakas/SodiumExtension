#pragma once




void addHeaderEx(SodiumSession *currentMKSession, LPEXTENSION_CONTROL_BLOCK lpECB, char *header);

void 
SendAjaxResponseHeader(
	SodiumSession *session
);

void sendImageResponseHeader(SodiumSession *currentMKSession);

void
SendHtmlResponseHeader(
	SodiumSession* session
);
