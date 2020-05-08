#include "pch.h"

#include "addTreeNode.h"
#include "postObjectGeneric.h"

char *__sendAddTreeNodeResponse(SodiumSession *currentMKSession, HTSQLPage *page, const char *recordSetName) {

    char *json = NULL;
    char *nodeStr = NULL,
         *iconStr = NULL,
         *childrenStr = NULL;

	DBInt_Connection *mkDBConnection = currentMKSession->rootTAGConnection->mkDBConnection;

    PostObjectResultSet *postObjectResultSet = findPostObjectGenericPostResultSetVariable(currentMKSession, page, recordSetName);

    if (postObjectResultSet && postObjectResultSet->ociStatement) {
		bool eof = DBInt_IsEof(mkDBConnection, postObjectResultSet->ociStatement);  //postObjectResultSet->eof;

        while(! eof) {
            const char *id      = DBInt_GetColumnValueByColumnName(mkDBConnection, postObjectResultSet->ociStatement, "ID");
            const char *text    = DBInt_GetColumnValueByColumnName(mkDBConnection, postObjectResultSet->ociStatement, "TEXT");
            const char *parentid= DBInt_GetColumnValueByColumnName(mkDBConnection, postObjectResultSet->ociStatement, "PARENT");
            const char *children= DBInt_GetColumnValueByColumnName(mkDBConnection, postObjectResultSet->ociStatement, "CHILDREN");
            const char *nodeType= DBInt_GetColumnValueByColumnName(mkDBConnection, postObjectResultSet->ociStatement, "TYPE");
            const char *icon    = DBInt_GetColumnValueByColumnName(mkDBConnection, postObjectResultSet->ociStatement, "ICON");

			if (id == NULL && text == NULL && parentid == NULL) {
				break;
			}

            if (nodeType) {
                nodeStr = mkStrcat(currentMKSession->heapHandle, __FILE__, __LINE__,
                                   ", \"data\"   : { \"node_type\" : \"", nodeType, "\" } ",
                                   NULL);
            }
            if (icon) {
                iconStr = mkStrcat(currentMKSession->heapHandle, __FILE__, __LINE__,
                                   ", \"icon\"   : \"", icon, "\" ",
                                   NULL);
            }
            if (children) {
                childrenStr = mkStrcat(currentMKSession->heapHandle, __FILE__, __LINE__,
                                   ", \"children\"   : ", children, " ",
                                   NULL);
            }
            char *recordStr = mkStrcat(currentMKSession->heapHandle, __FILE__, __LINE__,
                                        "{"
                                            "\"id\"         : \"", id, "\", "
                                            "\"text\"       : \"", text, "\", "
                                            "\"parent\"     : \"", parentid, "\", "
                                            "\"state\"      : { \"opened\" : false } ",
                                            (iconStr) ? iconStr: " ",
                                            (nodeStr) ? nodeStr: " ",
                                            (childrenStr) ? childrenStr: " ",
                                        "}",
                                        NULL
                                     );
            if (json) {
                char *tmp = json;
                json = mkStrcat(currentMKSession->heapHandle, __FILE__, __LINE__, json, ", ", recordStr, NULL);
                mkFree(currentMKSession->heapHandle, tmp);
            } else {
                json = mkStrdup(currentMKSession->heapHandle, recordStr, __FILE__, __LINE__);
            }

            if (nodeStr) {
                mkFree(currentMKSession->heapHandle, nodeStr);
            }
            if (iconStr) {
                mkFree(currentMKSession->heapHandle, iconStr);
            }
            if (childrenStr) {
                mkFree(currentMKSession->heapHandle, childrenStr);
            }
            mkFree(currentMKSession->heapHandle, recordStr);

			eof = DBInt_Next(mkDBConnection, postObjectResultSet->ociStatement);
        }
    }

    char *retVal =  mkStrcat(currentMKSession->heapHandle, __FILE__, __LINE__,
                             "[",
                             (json) ? json:"",
                             "]",
                             NULL);
    mkFree(currentMKSession->heapHandle, json);
    return retVal;
}


