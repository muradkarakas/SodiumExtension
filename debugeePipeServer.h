#pragma once

#include "htsql.h"



DWORD WINAPI	debugeeCreatePipeServer(void *param);
DWORD WINAPI	debugeeServerThreadInstanceFunction(LPVOID param);
void			debugeeGetAnswerToRequest(char *pchRequest, char *pchReply, DWORD pchBytes);
 