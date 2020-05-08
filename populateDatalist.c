#include "pch.h"

#include "populateDatalist.h"
#include "postObjectGeneric.h"
#include "postObjectResultSet.h"
#include "tagDatalist.h"
#include "tagConnection.h"

#include "..\DBInt\db-interface.h"

void  __sendPopulateDatalistResponse(SodiumSession *session, HTSQLPage *page, char *dataListName, bool cascadeOption) {
	TAGDatalist *current = findTAGDatalistByName(session, page, dataListName);
	if (current) {
		__populateDatalist(session, page, current, cascadeOption);
	}
	else {
		CLOG(ERR_DATALIST_NOT_FOUND, dataListName);
	}
}


char *getDatalistOptionString(SodiumSession *session, TAGDatalist *current, DBInt_Connection *mkDBConnection, DBInt_Statement *stm) {
	
	char *customPropertiesStr = mkStrdup(session->heapHandle, " ", __FILE__, __LINE__);
	char *opentag = "<option";
	char *closetag = "</option>";

	const char *valueTemp = DBInt_GetColumnValueByColumnName(mkDBConnection, stm, "value");
	const char *label = DBInt_GetColumnValueByColumnName(mkDBConnection, stm, "label");

	/*char *value = mkStrcat(session->heapHandle, __FILE__, __LINE__,
		" value=\\\"",
		valueTemp ? valueTemp : "",
		"\\\"",
		NULL);*/

	int colCount = DBInt_GetColumnCount(mkDBConnection, stm);
	for (int ind = 0; ind < colCount; ind++) {
		const char *colName = DBInt_GetColumnNameByIndex(mkDBConnection, stm, ind);
		if (colName) {
			const char *colValue = DBInt_GetColumnValueByColumnName(mkDBConnection, stm, colName);
			char *tmpstr = mkStrcat(session->heapHandle, __FILE__, __LINE__,
									customPropertiesStr, 
									" ", colName, "=\\\"", (colValue) ? (colValue) : "", "\\\"",
									NULL);
			mkFree(session->heapHandle, customPropertiesStr);
			customPropertiesStr = tmpstr;
		}
	}

	char *optionStr = mkStrcat(session->heapHandle, __FILE__, __LINE__,
		opentag,
		//value,
		customPropertiesStr,
		">",
		label,
		closetag, NULL);

	//mkFree(session->heapHandle, value);

	return optionStr;
}

void __sendPopulateDatalistWithRecordSetResponse(SodiumSession *session, HTSQLPage *page, char *dataListName, char *recordSetName) {

	if (dataListName == NULL) {
		CLOG(ERR_PROPERTY_CANNOT_BE_EMPTY, "datalist-name");
	}
	else if (recordSetName == NULL) {
		CLOG(ERR_PROPERTY_CANNOT_BE_EMPTY, "recordset name"); 
	}
	else {
		TAGDatalist *tagDatalist = findTAGDatalistByName(session, page, dataListName);
		if (tagDatalist == NULL) {
			CLOG(ERR_DATALIST_NOT_FOUND, dataListName);
		}
		else {
			// geting recordset object
			PostObjectResultSet *postObjectResultSet = findPostObjectGenericPostResultSetVariable(session, page, recordSetName);
			if (postObjectResultSet) {
				__populateDatalistWithRecordSet(session, page, tagDatalist, postObjectResultSet);
			}
			else {
				CLOG(ERR_NAME_IS_NOT_FOUND, recordSetName);
			}
		}
	}
}

void __populateDatalistWithRecordSet(SodiumSession *session, HTSQLPage *page, TAGDatalist *tagDatalist, PostObjectResultSet *postObjectResultSet) {
	char *tmpcat;
	int memReserved = 250;
	size_t memUsed = 0;
	bool cascadeOption = true;
	
	// getting root connection	
	DBInt_Connection *mkConnection = getRootDBIntConnection (session, page);
	if (mkConnection == NULL) {
		// No valid root connection exists
		CLOG(ERR_DB_NO_CONNECTION_EXISTS, "default");
		return;
	}
	char *dataListOptions = mkMalloc(session->heapHandle, memReserved, __FILE__, __LINE__);
	DBInt_Statement *stm = postObjectResultSet->ociStatement; 

	DBInt_First(mkConnection, stm);

	while (!DBInt_IsEof(mkConnection, postObjectResultSet->ociStatement)) {

		char *tmp = getDatalistOptionString(session, tagDatalist, mkConnection, stm);

		memUsed = memUsed + strlen(tmp);
		if (memUsed > (memReserved - 10)) {
			dataListOptions = mkReAlloc(session->heapHandle, dataListOptions, memReserved * 2);
			memReserved = memReserved * 2;
		}

		if (dataListOptions) {
			tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, dataListOptions, tmp, NULL);
			mkFree(session->heapHandle, dataListOptions);
			dataListOptions = tmpcat;
		}
		else {
			dataListOptions = mkStrdup(session->heapHandle, tmp, __FILE__, __LINE__);
		}
		mkFree(session->heapHandle, tmp);

		DBInt_Next(mkConnection, stm);
	}

	if (page->currentRequestMethodType == REQUEST_TYPE_GET) {
		mkPrint(session, "\n<script type=\"text/javascript\">", NULL);
	}
	char *lastResponse = mkStrcat(session->heapHandle, __FILE__, __LINE__,
		"action = { "
		"\"type\"            : \"populateDatalist\", "
		"\"dataListName\"    : \"", tagDatalist->id, "\", "
		"\"dataListOptions\" : \"", dataListOptions, "\", "
		"\"cascadeOption\"   : \"", (cascadeOption) ? "Y" : "N", "\", "
		" };",
		// standart part
		"addNewActionToTheResponseActionList(action);",
		NULL);
	mkPrint(session, lastResponse, NULL);
	if (page->currentRequestMethodType == REQUEST_TYPE_GET) {
		mkPrint(session, "\n</script>\n", NULL);
	}

	mkFree(session->heapHandle, lastResponse);
	mkFree(session->heapHandle, dataListOptions);
}

void  __populateDatalist(SodiumSession *session, HTSQLPage *page, TAGDatalist *tagDatalist, bool cascadeOption) {

	
}






