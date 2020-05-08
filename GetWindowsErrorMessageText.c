#include "pch.h"

void
GetWindowsErrorMessageText(
	wchar_t* buf,
	int bufSize
) {
	FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		buf, (bufSize / sizeof(wchar_t)), NULL);
}