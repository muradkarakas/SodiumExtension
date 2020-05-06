#pragma once


typedef struct _HTSQLResponse {
	/* it is a pointer to buffer for accumulated response string / byte sequence*/
	void *buffer;

	/* Maximum size of the "responseBuffer" */
	size_t  allocatedSize;

	/* currently size of the used space from "responseBuffer"  */
	size_t  usedSize;
} HTSQLResponse;