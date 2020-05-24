%include
{ 

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



#include "html.parser.imp.h"
#include "tagDataBlock.h"
#include "tagInput.h"
#include "tagDatalist.h"
#include "pre.parser.imp.h"
#include "htsqlFunction.imp.h"
#include "response.h"
#include "dispatcher.h"
#include "pageLoad.h"
#include "SodiumSession.h"
}


%name htmlParse

%extra_argument { SodiumSession *session }

%token_type { TokenHtml }

%syntax_error {
#ifndef NDEBUG
    HTSQLPage *page = GetCurrentPage(session);
	char lineStr[10];
	sprintf_s(lineStr, sizeof(lineStr), "%d", session->sessionDebugInfo->lineNumberOuter);
    char *errMsg = mkStrcat(session->heapHandle, __FILE__, __LINE__, "HTML Syntax Error on line ", lineStr, ". Possible Tokens : (", NULL);
    int n = sizeof(yyTokenName) / sizeof(yyTokenName[0]);
    for (int i = 0; i < n; ++i) {
            int a = yy_find_shift_action(yypParser, (YYCODETYPE)i);
            if (a < YYNSTATE + YYNRULE) {
                char *tmp = mkStrcat(session->heapHandle, __FILE__, __LINE__, errMsg, " ", yyTokenName[i], NULL);
                mkFree(session->heapHandle, errMsg);
                errMsg = tmp;
            }
    }
    char *tempMsg = mkStrcat(session->heapHandle, __FILE__, __LINE__, errMsg, ")", NULL);
    mkFree(session->heapHandle, errMsg);
    errMsg = tempMsg;
	__sendErrorResponse(session, page, errMsg, "");
	CLOG(ERR_SYNTAX_ERROR_IN_FRMX_FILE, errMsg);
#endif
}

%token_destructor {
    htmlTokenDestructor(session, $$);
}

start ::= expressions.

expressions ::= expressions expression.
expressions ::= expression.

expressions  ::= tagdoctype taghtml.
expressions  ::= taghtml.
expression   ::= space.
expression   ::= endoffile.

endoffile      ::= HTML_END_OF_FILE.

taghtmlcontents ::= tagbody.
taghtmlcontents ::= taghead tagbody.

tagheadcontents ::= tagheadcontents tagheadcontent.
tagheadcontents ::= tagheadcontent.

tagheadcontent  ::= tagtitle.
tagheadcontent  ::= tagscript.
tagheadcontent  ::= tagstyle.
tagheadcontent  ::= taglink.
tagheadcontent  ::= tagmeta.

tagbodycontents ::= tagbodycontents tagbodycontent.
tagbodycontents ::= tagbodycontent.

tagbodycontent  ::= htsqls.
tagbodycontent  ::= tagtable.
tagbodycontent  ::= space.
tagbodycontent  ::= htmltext.

htsqls ::= tagconnection.
htsqls ::= tagdatablock.
htsqls ::= tagcontrolblock.
htsqls ::= tagdatalist.

tagdoctype ::= TAG_DOCTYPE_HTSQL_DTD.
{
	session->whatIsPrinted = SESSION_PRINT_HTML_CONTENT;
}
tagdoctype ::= TAG_DOCTYPE_HTML.
{
	session->whatIsPrinted = SESSION_PRINT_HTML_CONTENT;
}

/** ############################################################################################
    HT/SQL Tree
    ############################################################################################
*/
tagtree                 ::= tagtreefullopen tagtreeblockclosefull.
tagtreefullopen         ::= tagtreeopen tagtreeproperties tagclosechar.
tagtreefullopen         ::= tagtreeopen tagtreeproperties tagclosechar spaces.

tagtreeopen             ::= TAG_TREE_OPEN(A).
{
    mkPrint(session, treeOuterDivOpen, NULL);
    htmlTokenDestructor(session, A);
}

tagtreeblockclosefull   ::= TAG_TREE_BLOCK_CLOSE(A).
{
    htmlTokenDestructor(session, A);
    mkPrint(session, treeOuterDivClose, NULL);
}

tagtreeproperties       ::= tagtreeproperties tagtreeproperty.
tagtreeproperties       ::= tagtreeproperty.

tagtreeproperty         ::= tagtreepropertyid PROPERTYDATA.

tagtreepropertyid       ::= SPACE PROP_TREE_NAME ASSIGMENT.

/** ############################################################################################
    HT/SQL Datalist
    ############################################################################################
*/
tagdatalist           ::= tagdatalistfullopen tagdatalistblockclosefull.
tagdatalistfullopen   ::= tagdatalistopen datalistproperties tagclosechar.
tagdatalistfullopen   ::= tagdatalistopen datalistproperties tagclosechar spaces.

tagdatalistopen       ::= TAG_DATALIST_OPEN.
{
    HTSQLPage *page = GetCurrentPage(session);
    createTAGDatalist(session, page);
}

tagdatalistblockclosefull ::= tagdatalistblockclose.

tagdatalistblockclose ::= TAG_DATALIST_BLOCK_CLOSE.
{
    HTSQLPage *page = GetCurrentPage(session);
    TAGDatalist *current = getCurrentTAGDatalist(session, page);

    TAGDatalist *found = findTAGDatalistByName(session, page, current->id);
    if (found != current) {
        stopHTMLParserWithMessage(session, page, ETEXT(ERR_NAME_ALREADY_IN_USE), current->id);
    }

    if (current->id == NULL || strlen(current->id)==0) {
		CLOG(ERR_PROPERTY_CANNOT_BE_EMPTY, "id");
    }
    if (current->connectionName == NULL || strlen(current->connectionName)==0) {
		CLOG(ERR_PROPERTY_CANNOT_BE_EMPTY, "connection-name");
    }
}
datalistproperties   ::= datalistproperties datalistproperty.
datalistproperties   ::= datalistproperty.

datalistproperty     ::= tagdatalistpropertyid PROPERTYDATA(A).
{
    HTSQLPage *page = GetCurrentPage(session);
	char *str = getCreateNTString(session, A);
    setTAGDatalistPropertyId(session, page, str);
	mkFree(session->heapHandle, str);
    htmlTokenDestructor(session, A);
}
tagdatalistpropertyid ::= SPACE PROP_DATALIST_DATALIST_NAME ASSIGMENT.

datalistproperty     ::= tagdatalistpropertyconnectionname PROPERTYDATA(A).
{
    HTSQLPage *page = GetCurrentPage(session);
	char *str = getCreateNTString(session, A);
    setTAGDatalistPropertyConnectionName(session, page, str);
	mkFree(session->heapHandle, str);
    htmlTokenDestructor(session, A);
}
tagdatalistpropertyconnectionname ::= SPACE PROP_DATALIST_CONNECTION_NAME ASSIGMENT.



/** ############################################################################################
    HT/SQL Connection
    ############################################################################################
*/
tagconnection           ::= tagconnectionfullopen tagconnectionblockclosefull.
{
    /*HTSQLPage *page = GetCurrentPage(session);
    openDatabaseConnection(session, page, getCurrentTAGConnection(session, page));*/
	
	// TODO: <connection>  tag'ýný sil 
}
tagconnectionfullopen   ::= tagconnectionopen connectionproperties tagclosechar.
tagconnectionfullopen   ::= tagconnectionopen connectionproperties tagclosechar spaces.

tagconnectionopen       ::= TAG_CONNECTION_OPEN.
{
   // createTAGConnection(session, GetCurrentPage(session));
}

tagconnectionblockclosefull ::= tagconnectionblockclose.

tagconnectionblockclose ::= TAG_CONNECTION_BLOCK_CLOSE.
{

}
connectionproperties   ::= connectionproperties connectionproperty.
connectionproperties   ::= connectionproperty.

connectionproperty     ::= connectionpropertyconnectionname PROPERTYDATA(A).
{
   /* HTSQLPage *page = GetCurrentPage(session);
	char *str = getCreateNTString(session, A);
    TAGConnection *current = getCurrentTAGConnection(session, page);
    TAGConnection *found = findTAGConnectionByName(session, page, stripTAGPropertyValue(str), true);
    if (found && found != current) {
        stopHTMLParserWithMessage(session, page, ETEXT(ERR_NAME_ALREADY_IN_USE), str);
    }
    setTAGConnectionPropertyConnectionName(session, current, str);
	mkFree(session->heapHandle, str);
    htmlTokenDestructor(session, A);*/
}
connectionpropertyconnectionname ::= SPACE PROP_CONNECTION_CONNECTION_NAME  ASSIGMENT.

connectionproperty          ::=  connectionpropertyusername PROPERTYDATA(A).
{
    /*HTSQLPage *page = GetCurrentPage(session);
    TAGConnection *current = getCurrentTAGConnection(session, page);
	char *str = getCreateNTString(session, A);
    setTAGConnectionPropertyUserName(session, current, str);
	mkFree(session->heapHandle, str);*/
}
connectionpropertyusername  ::= SPACE(A) PROP_CONNECTION_USER_NAME(B)  ASSIGMENT(C).
{
    /*mkFree(session->heapHandle, A.tokenStr);
    mkFree(session->heapHandle, B.tokenStr);
    mkFree(session->heapHandle, C.tokenStr);*/
}

connectionproperty          ::=  connectionpropertyuserpassword PROPERTYDATA(A).
{
    /*HTSQLPage *page = GetCurrentPage(session);
    TAGConnection *current = getCurrentTAGConnection(session, page);
	char *str = getCreateNTString(session, A);
    setTAGConnectionPropertyUserPassword(session, current, str);
	mkFree(session->heapHandle, str);*/
}
connectionpropertyuserpassword ::= SPACE(A) PROP_CONNECTION_USER_PASSWORD(B) ASSIGMENT(C).
{
    /*mkFree(session->heapHandle, A.tokenStr);
    mkFree(session->heapHandle, B.tokenStr);
    mkFree(session->heapHandle, C.tokenStr);*/
}

connectionproperty     ::= connectionpropertyinstancename PROPERTYDATA(A).
{
   /* HTSQLPage *page = GetCurrentPage(session);
    TAGConnection *current = getCurrentTAGConnection(session, page);
	char *str = getCreateNTString(session, A);
    setTAGConnectionPropertyInstanceName(session, current, str);
	mkFree(session->heapHandle, str);*/
}
connectionpropertyinstancename ::= SPACE PROP_CONNECTION_INSTANCE_NAME ASSIGMENT.


/** ############################################################################################
    HT/SQL Control Block
    ############################################################################################
*/
tagcontrolblock          ::= tagcontrolblockfullopen tagcontrolblockclosefull.
tagcontrolblock          ::= tagcontrolblockfullopen tagcontrolblockcontents tagcontrolblockclosefull.

tagcontrolblockfullopen  ::= tagcontrolblockopen tagcontrolblockproperties tagclosechar.

tagcontrolblockopen      ::= TAG_CONTROLBLOCK_OPEN.

tagcontrolblockclosefull ::= tagcontrolblockblockclose.
tagcontrolblockblockclose::= TAG_CONTROLBLOCK_CLOSE.

tagcontrolblockcontents  ::= tagcontrolblockcontents tagcontrolblockcontent.
tagcontrolblockcontents  ::= tagcontrolblockcontent.

tagcontrolblockcontent  ::= tagtree.
tagcontrolblockcontent  ::= space.
tagcontrolblockcontent  ::= htmltext.


tagcontrolblockproperties   ::= tagcontrolblockproperties tagcontrolblockproperty.

tagcontrolblockproperties   ::= tagcontrolblockproperty.

tagcontrolblockproperty     ::= SPACE(A) PROP_CONTROLBLOCK_BLOCK_NAME(B)  ASSIGMENT(C) PROPERTYDATA(NAME).
{
    HTSQLPage *page = GetCurrentPage(session);
	char *name = getCreateNTString(session, NAME);
    if (name == NULL) {
		CLOG(ERR_PROPERTY_CANNOT_BE_EMPTY, "control-block-name");
    }
    if (strlen(name) <= 2) {
		CLOG(ERR_PROPERTY_CANNOT_BE_EMPTY, "control-block-name");
    }
    htmlTokenDestructor(session, A);
    htmlTokenDestructor(session, B);
    htmlTokenDestructor(session, C);
	htmlTokenDestructor(session, NAME);
	if (name)
		mkFree(session->heapHandle, name);
}
tagcontrolblockproperty     ::= SPACE PROPERTYID                    ASSIGMENT PROPERTYDATA.



/** ############################################################################################
    HT/SQL Data Block
    ############################################################################################
*/
tagdatablock           ::= tagdatablockfullopen tagdatablockblockclosefull.
tagdatablock           ::= tagdatablockfullopen tagdatablockcontents tagdatablockblockclosefull.
tagdatablockfullopen   ::= tagdatablockopen tagdatablockproperties tagclosechar.
{
    HTSQLPage *page = GetCurrentPage(session);
    page->bInDataBlock++;
}
tagdatablockopen       ::= TAG_DATABLOCK_OPEN.
{
    HTSQLPage *page = GetCurrentPage(session);
    createTAGDataBlock(session, page);
	responseWriteByte(session, page, classDatablock, strlen(classDatablock));
}
tagdatablockblockclosefull ::= tagdatablockblockclose.

tagdatablockblockclose ::= TAG_DATABLOCK_BLOCK_CLOSE(A).
{
    HTSQLPage *page = GetCurrentPage(session);
    page->bInDataBlock--;

    htmlTokenDestructor(session, A);

    TAGDataBlock *tagDataBlock = getCurrentTAGDataBlock(session, page);
    if (tagDataBlock->dataBlockName == NULL || strlen(tagDataBlock->dataBlockName)==0) {
		CLOG(ERR_PROPERTY_CANNOT_BE_EMPTY, "data-block-name");
    }
    if (tagDataBlock->autoGeneratedColumns == NULL && (tagDataBlock->dataSourceName == NULL || strlen(tagDataBlock->dataSourceName)==0)) {
		CLOG(ERR_PROPERTY_CANNOT_BE_EMPTY, "data-source-name");
    }
    if (tagDataBlock->connectionName == NULL || strlen(tagDataBlock->connectionName)==0) {
		CLOG(ERR_PROPERTY_CANNOT_BE_EMPTY, "connection-name");
    }
}

tagdatablockcontents ::= tagdatablockcontents tagdatablockcontent.
tagdatablockcontents ::= tagdatablockcontent.

tagdatablockcontent  ::= tagtable.
tagdatablockcontent  ::= space.
tagdatablockcontent  ::= htmltext.

tagdatablockproperties   ::= tagdatablockproperties tagdatablockproperty.

tagdatablockproperties   ::= tagdatablockproperty.

tagdatablockproperty     ::= SPACE PROPERTYID  ASSIGMENT PROPERTYDATA.

tagdatablockproperty     ::=  tagdatablockpropertyjoincondition PROPERTYDATA(B).
{
    HTSQLPage *page = GetCurrentPage(session);
	char *str = getCreateNTString(session, B);
    setCurrentTAGDataBlockPropertyJoinCondition(session, page, str);
	mkFree(session->heapHandle, str);
}
tagdatablockpropertyjoincondition ::= SPACE PROP_BLOCK_JOIN_CONDITION(A) ASSIGMENT(B).
{
	// cheating lemon 
	// A
	// B
}

tagdatablockproperty     ::=  tagdatablockpropertyconnectionname PROPERTYDATA(B).
{
    HTSQLPage *page = GetCurrentPage(session);
	char *str = getCreateNTString(session, B);
	char *strNoRelease = stripTAGPropertyValue(str);
    setCurrentTAGDataBlockPropertyConnectionName(session, page, strNoRelease);
	mkFree(session->heapHandle, str);
    htmlTokenDestructor(session, B);
}
tagdatablockpropertyconnectionname ::= SPACE PROP_BLOCK_CONNECTION_NAME(A) ASSIGMENT(B).
{
	// cheating lemon 
	// A
	// B
}

tagdatablockproperty     ::= tagdatablockpropertyautogeneratedcolumns PROPERTYDATA(B).
{
	HTSQLPage *page = GetCurrentPage(session);
	char *str = getCreateNTString(session, B);
	setCurrentTAGDataBlockPropertyAutoGeneratedColumns(session, page, str);
	mkFree(session->heapHandle, str);
	htmlTokenDestructor(session, B);
}
tagdatablockpropertyautogeneratedcolumns ::= SPACE PROP_BLOCK_AUTO_GENERATED_COLUMNS ASSIGMENT.

tagdatablockproperty     ::=  tagdatablockpropertyrecordcount PROPERTYDATA(B).
{
    HTSQLPage *page = GetCurrentPage(session);
	char *str = getCreateNTString(session, B);
    setCurrentTAGDataBlockPropertyRecordCount(session, page, str);
	mkFree(session->heapHandle, str);
    htmlTokenDestructor(session, B);
}
tagdatablockpropertyrecordcount ::= SPACE PROP_BLOCK_RECORD_COUNT ASSIGMENT.

tagdatablockproperty     ::=  tagdatablockpropertysourcename PROPERTYDATA(B).
{
    HTSQLPage *page = GetCurrentPage(session);
	char *str = getCreateNTString(session, B);
	setCurrentTAGDataBlockPropertyDataSourceName(session, page, str);
	mkFree(session->heapHandle, str);
}
tagdatablockpropertysourcename ::= SPACE PROP_BLOCK_DATA_SOURCE_NAME(A) ASSIGMENT(B).
{
	// cheating lemon 
	// A
	// B
}

tagdatablockproperty     ::= tagdatablockpropertyschemaname PROPERTYDATA(B).
{
	HTSQLPage *page = GetCurrentPage(session);
	char *str = getCreateNTString(session, B);
	setCurrentTAGDataBlockPropertyDataSchemaName(session, page, str);
	mkFree(session->heapHandle, str);
}
tagdatablockpropertyschemaname ::= SPACE PROP_BLOCK_DATA_SCHEMA_NAME(A) ASSIGMENT(B).
{
	// cheating lemon 
	// A
	// B
}

tagdatablockproperty     ::=  tagdatablockpropertyblockname PROPERTYDATA(B).
{
    HTSQLPage *page = GetCurrentPage(session);
	char *str = getCreateNTString(session, B);
    setCurrentTAGDataBlockPropertyBlockName(session, page, str);
	mkFree(session->heapHandle, str);
    htmlTokenDestructor(session, B);
}
tagdatablockpropertyblockname ::= SPACE PROP_BLOCK_BLOCK_NAME ASSIGMENT.

tagdatablockproperty     ::=  tagdatablockpropertykeycolumnname PROPERTYDATA(B).
{
    HTSQLPage *page = GetCurrentPage(session);
	char *str = getCreateNTString(session, B);
	setCurrentTAGDataBlockPropertyKeyColumnName(session, page, str);
	mkFree(session->heapHandle, str);
    htmlTokenDestructor(session, B);
}
tagdatablockpropertykeycolumnname ::= SPACE PROP_BLOCK_KEY_COLUMN_NAME ASSIGMENT.

tagdatablockproperty     ::=  tagdatablockpropertymasterblockname PROPERTYDATA(B).
{
    HTSQLPage *page = GetCurrentPage(session);
	char *str = getCreateNTString(session, B);
    setCurrentTAGDataBlockMasterPropertyBlockName(session, page, str);
	mkFree(session->heapHandle, str);
	htmlTokenDestructor(session, B);
}
tagdatablockpropertymasterblockname ::= SPACE PROP_BLOCK_MASTER_BLOCK_NAME ASSIGMENT.

tagdatablockproperty     ::= tagdatablockpropertyinsertallowed PROPERTYDATA(B).
{
    HTSQLPage *page = GetCurrentPage(session);
	char *str = getCreateNTString(session, B);
    setCurrentTAGDataBlockPropertyInsertAllowed(session, page, str);
	mkFree(session->heapHandle, str);
    htmlTokenDestructor(session, B);
}
tagdatablockpropertyinsertallowed ::= SPACE PROP_BLOCK_INSERT_ALLOWED ASSIGMENT.

tagdatablockproperty    ::= tagdatablockpropertydeleteallowed PROPERTYDATA(B).
{
    HTSQLPage *page = GetCurrentPage(session);
	char *str = getCreateNTString(session, B);
    setCurrentTAGDataBlockPropertyDeleteAllowed(session, page, str);
	mkFree(session->heapHandle, str);
    htmlTokenDestructor(session, B);
}
tagdatablockpropertydeleteallowed ::= SPACE PROP_BLOCK_DELETE_ALLOWED ASSIGMENT.

tagdatablockproperty     ::= tagdatablockpropertyupdateallowed PROPERTYDATA(B).
{
    HTSQLPage *page = GetCurrentPage(session);
	char *str = getCreateNTString(session, B);
    setCurrentTAGDataBlockPropertyUpdateAllowed(session, page, str);
	mkFree(session->heapHandle, str);
    htmlTokenDestructor(session, B);
}
tagdatablockpropertyupdateallowed ::= SPACE PROP_BLOCK_UPDATE_ALLOWED ASSIGMENT.


tagdatablockproperty     ::= tagdatablockpropertywhereclause PROPERTYDATA(B).
{
    HTSQLPage *page = GetCurrentPage(session);
	char *str = getCreateNTString(session, B);
    setCurrentTAGDataBlockPropertyWhereClause(session, page, str);
    mkFree(session->heapHandle, str);
}
tagdatablockpropertywhereclause ::= SPACE PROP_BLOCK_WHERE_CLAUSE(A) ASSIGMENT(B).
{
	// cheating lemon 
	// A
	// B
}

tagdatablockproperty    ::=  tagdatablockpropertyorderbyclause PROPERTYDATA(B).
{
    HTSQLPage *page = GetCurrentPage(session);
	char *str = getCreateNTString(session, B);
    setCurrentTAGDataBlockPropertyOrderByClause(session, page, str);
    mkFree(session->heapHandle, str);
}
tagdatablockpropertyorderbyclause ::= SPACE PROP_BLOCK_OREDERBY_CLAUSE(A) ASSIGMENT(B).
{
	// cheating lemon 
	// A
	// B
}


/** ############################################################################################
    HT/SQL Input & Select
    ############################################################################################
*/
taginput           ::= taginputfullopen.
taginput           ::= taginputfullopen taginputblockclosefull.
taginputfullopen   ::= taginputopen taginputproperties tagclosechar.
taginputfullopen   ::= taginputopen taginputproperties taginputshortclose.


taginputshortclose ::= TAG_SHORT_CLOSE.
{
	HTSQLPage *page = GetCurrentPage(session);

	if (page->bInTableBody > 0) {
		/** SPECIAL CASE 1:

		HTML specification says that Radio items must have same name in order to group them together.
		However, this is not acceptable for C object instance. Object instances must have unique name
		Just for radio buttons, unique name will be generated by concatenation of name + checkedValue. */
		TAGInput *input = getCurrentTAGDataBlockTAGInput(session, page);
		if (input->inputType == INPUT_TAG_TYPE_RADIO) {
			setCurrentTAGInputPropertyNameWithNameAndValuePair(session, page, input);
		}
		tagInputValidityCheck(session, page, getCurrentTAGDataBlockTAGInput(session, page));
	}

	page->bInInput--;
}

taginputopen       ::= TAG_INPUT_OPEN(A).
{
    HTSQLPage *page = GetCurrentPage(session);
	//if (page->bInTableBody > 0) {
		createTAGInput(session, page, getCurrentTAGDataBlock(session, page), true);
	//}    
	page->bInInput++;
    htmlTokenDestructor(session, A);
}
taginputopen       ::= TAG_SELECT_OPEN(A).
{
    HTSQLPage *page = GetCurrentPage(session);
	//if (page->bInTableBody > 0) {
		TAGInput *input = createTAGInput(session, page, getCurrentTAGDataBlock(session, page), true);
		setCurrentTAGInputPropertyType(session, page, input, "select");
	//}
	page->bInInput++;
    htmlTokenDestructor(session, A);
}
taginputopen       ::= TAG_TEXTAREA_OPEN(A).
{
    HTSQLPage *page = GetCurrentPage(session);
	//if (page->bInTableBody > 0) {
		TAGInput *input = createTAGInput(session, page, getCurrentTAGDataBlock(session, page), true);
		setCurrentTAGInputPropertyType(session, page, input, "textarea");
	//}
	page->bInInput++;
    htmlTokenDestructor(session, A);
}
taginputblockclosefull ::= taginputblockclose.
{
    HTSQLPage *page = GetCurrentPage(session);
	if (page->bInTableBody > 0) {
		TAGInput *input = getCurrentTAGDataBlockTAGInput(session, page);
		if (!input->name) {
			CLOG(ERR_PROPERTY_CANNOT_BE_EMPTY, "input -> name");
		}
	}    
}
taginputblockclose ::= TAG_INPUT_BLOCK_CLOSE(A).
{
    HTSQLPage *page = GetCurrentPage(session);

	if (page->bInTableBody > 0) {	
		TAGInput *input = getCurrentTAGDataBlockTAGInput(session, page);
		tagInputValidityCheck(session, page, input);
	}
	page->bInInput--;
    htmlTokenDestructor(session, A);
}

taginputblockclose ::= TAG_SELECT_BLOCK_CLOSE(A).
{
    HTSQLPage *page = GetCurrentPage(session);
	
	if (page->bInTableBody > 0) {
		/** SPECIAL CASE 2:

		HTML "input", "select" and "textarea" tags are tracked by TAGInput data structure.
		For "select" element, call this method passing "select" value as second argument.
		*/		
		tagInputValidityCheck(session, page, getCurrentTAGDataBlockTAGInput(session, page));
	}
	page->bInInput--;
    htmlTokenDestructor(session, A);
}

taginputblockclose ::= TAG_TEXTAREA_BLOCK_CLOSE(A).
{
    HTSQLPage *page = GetCurrentPage(session);

	if (page->bInTableBody > 0) {
		/** SPECIAL CASE 2:

		HTML "input", "select" and "textarea" tags are tracked by TAGInput data structure.
		For "select" element, call this method passing "select" value as second argument.
		*/
		tagInputValidityCheck(session, page, getCurrentTAGDataBlockTAGInput(session, page));
	}
	page->bInInput--;
    htmlTokenDestructor(session, A);
}



taginputproperties    ::= taginputproperties taginputproperty.

taginputproperties    ::= taginputproperty.

taginputproperty      ::= taginputcheckedvalue PROPERTYDATA(B).
{
    HTSQLPage *page = GetCurrentPage(session);
	if (page->bInTableBody > 0) {
		char *str = getCreateNTString(session, B);
		setCurrentTAGInputCheckedValue(session, page, str);
		mkFree(session->heapHandle, str);
	}
    htmlTokenDestructor(session, B);
}
taginputcheckedvalue   ::= spaces PROP_INPUT_CHECKED_VALUE   ASSIGMENT.

taginputproperty      ::= taginputuncheckedvalue PROPERTYDATA(B).
{
    HTSQLPage *page = GetCurrentPage(session);
	if (page->bInTableBody > 0) {
		char *str = getCreateNTString(session, B);
		setCurrentTAGInputUncheckedValue(session, page, str);
		mkFree(session->heapHandle, str);
	}
    htmlTokenDestructor(session, B);
}
taginputuncheckedvalue   ::= spaces PROP_INPUT_UNCHECKED_VALUE   ASSIGMENT.

taginputproperty      ::= taginputpropertyinputname PROPERTYDATA(B).
{
	HTSQLPage *page = GetCurrentPage(session);
	if (page->bInTableBody > 0) {
		char *str = getCreateNTString(session, B);
		TAGInput *input = getCurrentTAGDataBlockTAGInput(session, page);
		setCurrentTAGInputPropertyName(session, page, input, str);
		mkFree(session->heapHandle, str);
	}
	htmlTokenDestructor(session, B);
}
taginputpropertyinputname   ::= spaces PROP_INPUT_NAME   ASSIGMENT.

taginputproperty      ::= taginputpropertysequencename PROPERTYDATA(B).
{
    HTSQLPage *page = GetCurrentPage(session);
	if (page->bInTableBody > 0) {
		char *str = getCreateNTString(session, B);
		setCurrentTAGInputPropertySequenceName(session, page, str);
		mkFree(session->heapHandle, str);
	}
    htmlTokenDestructor(session, B);
}
taginputpropertysequencename   ::= spaces PROP_INPUT_SEQUENCE_NAME   ASSIGMENT.

taginputproperty      ::= taginputpropertysequenceschemaname PROPERTYDATA(B).
{
    HTSQLPage *page = GetCurrentPage(session);
	if (page->bInTableBody > 0) {
		char *str = getCreateNTString(session, B);
		setCurrentTAGInputPropertySequenceSchemaName(session, page, str);
		mkFree(session->heapHandle, str);
	}
    htmlTokenDestructor(session, B);
}
taginputpropertysequenceschemaname   ::= spaces PROP_INPUT_SEQUENCE_SCHEMA_NAME   ASSIGMENT.


taginputproperty      ::= taginputpropertymasteritemname PROPERTYDATA(B).
{
    HTSQLPage *page = GetCurrentPage(session);
	if (page->bInTableBody > 0) {
		char *str = getCreateNTString(session, B);
		setCurrentTAGInputPropertyMasterItemName(session, page, str);
		mkFree(session->heapHandle, str);
	}
	htmlTokenDestructor(session, B);
}
taginputpropertymasteritemname   ::= spaces PROP_INPUT_MASTER_ITEM_NAME ASSIGMENT.

taginputproperty            ::=  taginputpropertyinputtype PROPERTYDATA(B).
{
    HTSQLPage *page = GetCurrentPage(session);
	if (page->bInTableBody > 0) {
		char *str = getCreateNTString(session, B);
		TAGInput *input = getCurrentTAGDataBlockTAGInput(session, page);
		setCurrentTAGInputPropertyType(session, page, input, str);
		mkFree(session->heapHandle, str);
	}
	htmlTokenDestructor(session, B);
}
taginputpropertyinputtype   ::= spaces PROP_INPUT_TYPE  ASSIGMENT.

taginputproperty            ::= taginputpropertyinputvalue PROPERTYDATA(B).
{
    HTSQLPage *page = GetCurrentPage(session);
	if (page->bInTableBody > 0) {
		char *str = getCreateNTString(session, B);
		setCurrentTAGInputPropertyValue(session, page, str);
		mkFree(session->heapHandle, str);
	}
	htmlTokenDestructor(session, B);
}
taginputpropertyinputvalue  ::= spaces PROP_INPUT_VALUE ASSIGMENT.


taginputproperty            ::= taginputpropertydefaultvalue PROPERTYDATA(B).
{
    HTSQLPage *page = GetCurrentPage(session);
	if (page->bInTableBody > 0) {
		char *str = getCreateNTString(session, B);
		setCurrentTAGInputPropertyDefaultValue(session, page, str);
		mkFree(session->heapHandle, str);
	}
	htmlTokenDestructor(session, B);
}
taginputpropertydefaultvalue::= spaces PROP_INPUT_DEFAULT_VALUE ASSIGMENT.

taginputproperty       ::=  taginputpropertycolumnname PROPERTYDATA(B).
{
    HTSQLPage *page = GetCurrentPage(session);
	if (page->bInTableBody > 0) {
		char *str = getCreateNTString(session, B);
		TAGInput *tagInput = getCurrentTAGDataBlockTAGInput(session, page);
		setCurrentTAGInputPropertyColumnName(session, page, tagInput, str);
		mkFree(session->heapHandle, str);
	}
	htmlTokenDestructor(session, B);
}
taginputpropertycolumnname ::= spaces PROP_INPUT_COLUMN_NAME ASSIGMENT.

taginputproperty       ::= taginputpropertyformatmask PROPERTYDATA(B).
{
    HTSQLPage *page = GetCurrentPage(session);
	if (page->bInTableBody > 0) {
		char *str = getCreateNTString(session, B);
		setCurrentTAGInputPropertyFormatMask(session, page, str);
		mkFree(session->heapHandle, str);
	}
	htmlTokenDestructor(session, B);
}
taginputpropertyformatmask ::= spaces PROP_INPUT_FORMAT_MASK ASSIGMENT.

taginputproperty       ::= taginputpropertydatatype PROPERTYDATA(B).
{
    HTSQLPage *page = GetCurrentPage(session);
	if (page->bInTableBody > 0) {
		char *str = getCreateNTString(session, B);
		TAGInput *tagInput = getCurrentTAGDataBlockTAGInput(session, page);
		str = stripTAGPropertyValue(str);
		setCurrentTAGInputPropertyDataType(session, page, tagInput, str);
		mkFree(session->heapHandle, str);
	}
	htmlTokenDestructor(session, B);
}
taginputpropertydatatype ::= spaces PROP_INPUT_DATA_TYPE ASSIGMENT.

taginputproperty       ::= taginputpropertymaxlength PROPERTYDATA(B).
{
    HTSQLPage *page = GetCurrentPage(session);
	if (page->bInTableBody > 0) {
		char *str = getCreateNTString(session, B);
		TAGInput *input = getCurrentTAGDataBlockTAGInput(session, page);
		setCurrentTAGInputPropertyMaxLength(session, page, input, str);
		mkFree(session->heapHandle, str);
	}
	htmlTokenDestructor(session, B);
}
taginputpropertymaxlength ::= spaces PROP_INPUT_MAX_LENGTH ASSIGMENT.

taginputproperty       ::= taginputpropertyinsertallowed PROPERTYDATA(B).
{
    HTSQLPage *page = GetCurrentPage(session);
	if (page->bInTableBody > 0) {
		char *str = getCreateNTString(session, B);
		setCurrentTAGInputPropertyInsertAllowed(session, page, str);
		mkFree(session->heapHandle, str);
	}
	htmlTokenDestructor(session, B);
}
taginputpropertyinsertallowed ::= spaces PROP_INPUT_INSERT_ALLOWED   ASSIGMENT.

taginputproperty       ::= taginputpropertyupdateallowed PROPERTYDATA(B).
{
    HTSQLPage *page = GetCurrentPage(session);
	if (page->bInTableBody > 0) {
		char *str = getCreateNTString(session, B);
		setCurrentTAGInputPropertyUpdateAllowed(session, page, str);
		mkFree(session->heapHandle, str);
	}
	htmlTokenDestructor(session, B);
}
taginputpropertyupdateallowed ::= spaces PROP_INPUT_UPDATE_ALLOWED ASSIGMENT.

taginputproperty       ::= taginputpropertydatalistname PROPERTYDATA(B).
{
    HTSQLPage *page = GetCurrentPage(session);
	if (page->bInTableBody > 0) {
		char *str = getCreateNTString(session, B);
		setCurrentTAGInputPropertyDatalistName(session, page, str);
		mkFree(session->heapHandle, str);
	}
	htmlTokenDestructor(session, B);
}
taginputpropertydatalistname ::= spaces PROP_INPUT_DATALIST_NAME ASSIGMENT.

taginputproperty       ::= taginputpropertylookupitemname PROPERTYDATA(B).
{
    HTSQLPage *page = GetCurrentPage(session);
	if (page->bInTableBody > 0) {
		char *str = getCreateNTString(session, B);
		setCurrentTAGInputPropertyLookupItemName(session, page, str);
		mkFree(session->heapHandle, str);
	}
	htmlTokenDestructor(session, B);
}
taginputpropertylookupitemname ::= spaces PROP_INPUT_LOOKUP_ITEM_NAME ASSIGMENT.

taginputproperty       ::= taginputpropertylookupitemblockname PROPERTYDATA(B).
{
	HTSQLPage *page = GetCurrentPage(session);
	if (page->bInTableBody > 0) {
		char *str = getCreateNTString(session, B);
		setCurrentTAGInputPropertyLookupItemBlockName(session, page, str);
		mkFree(session->heapHandle, str);
	}
	htmlTokenDestructor(session, B);
}
taginputpropertylookupitemblockname ::= spaces PROP_INPUT_LOOKUP_ITEM_BLOCK_NAME ASSIGMENT.

taginputproperty              ::= spaces PROPERTYID(A) ASSIGMENT(B) PROPERTYDATA(C).
{
    HTSQLPage *page = GetCurrentPage(session);
	//if (page->bInTableBody > 0) {
		char *str1 = getCreateNTString(session, A);
		char *str2 = getCreateNTString(session, B);
		char *str3 = getCreateNTString(session, C);

		char *str = mkStrcat(session->heapHandle, __FILE__, __LINE__, str1, str2, str3, NULL);
		setCurrentTAGInputPropertyAppendHTMLProperty(session, page, str);

		htmlTokenDestructor(session, A);
		htmlTokenDestructor(session, B);
		htmlTokenDestructor(session, C);
		mkFree(session->heapHandle, str);
		mkFree(session->heapHandle, str1);
		mkFree(session->heapHandle, str2);
		mkFree(session->heapHandle, str3);
	//}
}

/** ############################################################################################
    <table> ... </table>
    ############################################################################################
*/
tagtable         ::= tagtablefullopen tagtableblockclosefull.
tagtable         ::= tagtablefullopen tablecontent tagtableblockclosefull.

tagtablefullopen ::= tagtableopen tagclosechar.
tagtablefullopen ::= tagtableopen tagclosechar spaces.
tagtablefullopen ::= tagtableopen tagproperties tagclosechar.
tagtablefullopen ::= tagtableopen tagproperties tagclosechar spaces.

tagtableopen        ::= TAG_TABLE_OPEN.
{
    GetCurrentPage(session)->bInTable++;
}
tagtableblockclosefull ::= tagtableblockclose.

tagtableblockclose  ::= TAG_TABLE_BLOCK_CLOSE.
{
    GetCurrentPage(session)->bInTable--;
}




tablecontent  ::= tagthead tagtbody tagtfoot.
tablecontent  ::= tagthead tagtfoot tagtbody.
tablecontent  ::= tagthead tagtbody.
tablecontent  ::= tagtbody tagtfoot.
tablecontent  ::= tagtbody.


/** ############################################################################################
    <thead> ... </thead>
    ############################################################################################
*/
tagthead            ::= tagtheadfullopen tagtheadblockclosefull.
tagthead            ::= tagtheadfullopen tagtablerows tagtheadblockclosefull.
tagtheadfullopen    ::= tagtheadopen tagclosechar.
tagtheadfullopen    ::= tagtheadopen tagclosechar spaces.
tagtheadfullopen    ::= tagtheadopen tagpropertieswithreturnvaluesall tagclosechar.
tagtheadfullopen    ::= tagtheadopen tagpropertieswithreturnvaluesall tagclosechar spaces.
tagtheadopen        ::= TAG_THEAD_OPEN.
{
	HTSQLPage *page = GetCurrentPage(session);
	if (page->bInTableBody == 0) {
		page->bInTableHead++;
	}	
}
tagtheadblockclosefull ::= tagtheadblockclose.
tagtheadblockclosefull ::= tagtheadblockclose spaces.

tagtheadblockclose  ::= TAG_THEAD_BLOCK_CLOSE.
{
	HTSQLPage *page = GetCurrentPage(session);
	if (page->bInTableBody == 0) {
		page->bInTableHead--;
	}
}


/** ############################################################################################
    <tfoot> ... </tfoot>
    ############################################################################################
*/
tagtfoot            ::= tagtfootfullopen tagtablerows tagtfootblockclosefull.
tagtfootfullopen    ::= tagtfootopen tagclosechar.
tagtfootfullopen    ::= tagtfootopen tagclosechar spaces.
tagtfootfullopen    ::= tagtfootopen tagproperties tagclosechar.
tagtfootfullopen    ::= tagtfootopen tagproperties tagclosechar spaces.
tagtfootopen        ::= TAG_TFOOT_OPEN.
{
	HTSQLPage *page = GetCurrentPage(session);
	if (page->bInTableBody == 0) {
		page->bInTableFoot++;
	}
}
tagtfootblockclosefull ::= tagtfootblockclose.
tagtfootblockclosefull ::= tagtfootblockclose spaces.

tagtfootblockclose  ::= TAG_TFOOT_BLOCK_CLOSE.
{
	HTSQLPage *page = GetCurrentPage(session);
	if (page->bInTableBody == 0) {
		page->bInTableFoot--;
	}
}


/** ############################################################################################
    <tbody> ... </tbody>
    ############################################################################################
*/
tagtbody            ::= tagtbodyfullopen tagtablerows tagtbodyblockclosefull.
tagtbodyfullopen    ::= tagtbodyopen tagclosechar.
tagtbodyfullopen    ::= tagtbodyopen tagclosechar spaces.
tagtbodyfullopen    ::= tagtbodyopen tagproperties tagclosechar.
tagtbodyfullopen    ::= tagtbodyopen tagproperties tagclosechar spaces.
tagtbodyopen        ::= TAG_TBODY_OPEN.
{
	HTSQLPage *page = GetCurrentPage(session); 
	if (page->bInTableHead == 0 && page->bInTableFoot == 0) {
		page->bInTableBody++;
	}
}
tagtbodyblockclosefull ::= tagtbodyblockclose.
tagtbodyblockclosefull ::= tagtbodyblockclose spaces.

tagtbodyblockclose  ::= TAG_TBODY_BLOCK_CLOSE.
{
	HTSQLPage *page = GetCurrentPage(session);
	if (page->bInTableHead == 0 && page->bInTableFoot == 0) {
		page->bInTableBody--;
	}
}


/** ############################################################################################
    <tr> ... </tr>
    ############################################################################################
*/
tagtablerows ::= tagtablerows tagtablerow.
tagtablerows ::= tagtablerow.

tagtablerow         ::= tagtablerowfullopen tagtablecols tagtablerowblockclosefull.
tagtablerowfullopen ::= tagtablerowopen tagclosechar.
tagtablerowfullopen ::= tagtablerowopen tagclosechar spaces.
tagtablerowfullopen ::= tagtablerowopen tagpropertieswithreturnvaluesall tagclosechar.

tagtablerowopen        ::= TAG_TR_OPEN(A).
{
    GetCurrentPage(session)->bInTableTr++;
    htmlTokenDestructor(session, A);
}
tagtablerowblockclosefull ::= tagtablerowblockclose.
tagtablerowblockclosefull ::= tagtablerowblockclose spaces.

tagtablerowblockclose  ::= TAG_TR_BLOCK_CLOSE.
{
    GetCurrentPage(session)->bInTableTr--;
}


tagtablecols ::= tagtablecols tagtablecol.
tagtablecols ::= tagtablecol.

tagtablecol ::= tagtablecoltd.
tagtablecol ::= tagtablecolth.

/** ############################################################################################
    <td> ... </td>
    ############################################################################################
*/
tagtablecoltd         ::= tagtablecoltdfullopen tagtablecoltdblockclosefull.
tagtablecoltd         ::= tagtablecoltdfullopen spaces tagtablecoltdblockclosefull.
tagtablecoltd         ::= tagtablecoltdfullopen spaces tagtable spaces tagtablecoltdblockclosefull.
tagtablecoltd         ::= tagtablecoltdfullopen tagtablecolcontents tagtablecoltdblockclosefull.
tagtablecoltd         ::= tagtablecoltdfullopen spaces tagtablecolcontents tagtablecoltdblockclosefull.
tagtablecoltdfullopen ::= tagtablecoltdopen tagclosechar.

tagtablecoltdfullopen ::= tagtablecoltdopen tagproperties tagclosechar.

tagtablecoltdopen        ::= TAG_TD_OPEN.
{
    GetCurrentPage(session)->bInTableTd++;
}
tagtablecoltdblockclosefull ::= tagtablecoltdblockclose.
tagtablecoltdblockclosefull ::= tagtablecoltdblockclose spaces.

tagtablecoltdblockclose  ::= TAG_TD_BLOCK_CLOSE.
{
    GetCurrentPage(session)->bInTableTd--;
}

tagtablecolcontents ::= tagtablecolcontents tagtablecolcontent.
tagtablecolcontents ::= tagtablecolcontent.

tagtablecolcontent  ::= taginput.
tagtablecolcontent  ::= taginput spaces.
tagtablecolcontent  ::= htmltext.
tagtablecolcontent  ::= htmltext spaces.
tagtablecolcontent  ::= tagtable.

/** ############################################################################################
    <th> ... </th>
    ############################################################################################
*/
tagtablecolth         ::= tagtablecolthfullopen tagtablecolthblockclosefull.
tagtablecolth         ::= tagtablecolthfullopen tagtablecolcontents tagtablecolthblockclosefull.
tagtablecolthfullopen ::= tagtablecolthopen tagclosechar.

tagtablecolthfullopen ::= tagtablecolthopen tagpropertieswithreturnvaluesall tagclosechar.

tagtablecolthopen        ::= TAG_TH_OPEN.
{
    GetCurrentPage(session)->bInTableTh++;
}
tagtablecolthblockclosefull ::= tagtablecolthblockclose.
tagtablecolthblockclosefull ::= tagtablecolthblockclose spaces.

tagtablecolthblockclose  ::= TAG_TH_BLOCK_CLOSE.
{
    GetCurrentPage(session)->bInTableTh--;
}


/** ############################################################################################
    <title> ... </title>
    ############################################################################################
*/
tagtitle            ::= tagtitlefullopen tagtitleblockclosefull.
tagtitle            ::= tagtitlefullopen htmlandspaces tagtitleblockclosefull.
tagtitlefullopen    ::= tagtitleopen tagclosechar.
tagtitlefullopen    ::= tagtitleopen tagproperties tagclosechar.
tagtitleblockclosefull  ::= tagtitleblockclose.
tagtitleblockclosefull  ::= tagtitleblockclose spaces.
tagtitleopen        ::= TAG_TITLE_OPEN.
tagtitleblockclose  ::= TAG_TITLE_BLOCK_CLOSE.

/** ############################################################################################
    <link></link>
    ############################################################################################
*/
taglink         ::= taglinkfullopen.
taglink         ::= taglinkfullopen taglinkblockclosefull.
taglinkfullopen ::= taglinkopen tagproperties tagclosechar.
taglinkfullopen ::= taglinkopen tagproperties tagshortclose.

taglinkblockclosefull ::= taglinkblockclose.
taglinkblockclosefull ::= taglinkblockclose spaces.

taglinkopen           ::= TAG_LINK_OPEN.
taglinkblockclose     ::= TAG_LINK_BLOCK_CLOSE.


/** ############################################################################################
    <meta></meta>
    ############################################################################################
*/
tagmeta         ::= tagmetafullopen.
tagmeta         ::= tagmetafullopen tagmetablockclosefull.
tagmetafullopen ::= tagmetaopen tagproperties tagclosechar.
tagmetafullopen ::= tagmetaopen tagproperties tagshortclose.

tagmetablockclosefull ::= tagmetablockclose.
tagmetablockclosefull ::= tagmetablockclose spaces.

tagmetaopen           ::= TAG_META_OPEN.
tagmetablockclose     ::= TAG_META_BLOCK_CLOSE.


/** ############################################################################################
    <style></style>
    ############################################################################################
*/
tagstyle         ::= tagstylefullopen tagstyleblockclosefull.
tagstyle         ::= tagstylefullopen htmlandspaces tagstyleblockclosefull.
tagstylefullopen ::= tagstyleopen tagclosechar.
tagstylefullopen ::= tagstyleopen tagstyleproperties tagclosechar.

tagstyleblockclosefull ::= tagstyleblockclose.

tagstyleblockclosefull ::= tagstyleblockclose spaces.
tagstyleopen           ::= TAG_STYLE_OPEN.
tagstyleblockclose     ::= TAG_STYLE_BLOCK_CLOSE.

/*******  style tag properties *************************/
tagstyleproperties  ::= tagstyleproperties tagstyleproperty.
tagstyleproperties  ::= tagstyleproperty.

tagstyleproperty    ::= space PROPERTYID_SCRIPT_SRC ASSIGMENT PROPERTYDATA.
tagstyleproperty    ::= space PROPERTYID_SCRIPT_TYPE ASSIGMENT PROPERTYDATA.
tagstyleproperty    ::= space PROPERTYID ASSIGMENT PROPERTYDATA.


/** ############################################################################################
    <script></script>
    ############################################################################################
*/
tagscript         ::= tagscriptfullopen tagscriptblockclosefull.
tagscript         ::= tagscriptfullopen htmlandspaces tagscriptblockclosefull.
tagscriptfullopen ::= tagscriptopen tagclosechar.
tagscriptfullopen ::= tagscriptopen tagscriptproperties tagclosechar.

tagscriptblockclosefull ::= tagscriptblockclose.
{
	HTSQLPage *page = GetCurrentPage(session);

    const char *scriptType = page->currentScriptType;

    if (scriptType && strcmp(page->currentScriptType, "\"text/htsql\"") == 0) {
		ParseSQLXFile(session, page, stripTAGPropertyValueW(page->currentScriptFile));
    }
    if (page->currentScriptType) {
        mkFree(session->heapHandle, page->currentScriptType);
		page->currentScriptType = NULL;
    }
    if (page->currentScriptFile) {
        mkFree(session->heapHandle, page->currentScriptFile);
		page->currentScriptFile = NULL;
    }
}
tagscriptblockclosefull ::= tagscriptblockclose spaces.
{
	HTSQLPage *page = GetCurrentPage(session);
    const char *scriptType = GetCurrentPage(session)->currentScriptType;

    if (scriptType && strcmp(scriptType, "\"text/htsql\"") == 0) {
		ParseSQLXFile(session, page, stripTAGPropertyValueW(page->currentScriptFile));
    }
    if (page->currentScriptType) {
        mkFree(session->heapHandle, page->currentScriptType);
		page->currentScriptType = NULL;
    }
    if (page->currentScriptFile) {
        mkFree(session->heapHandle, page->currentScriptFile);
		page->currentScriptFile = NULL;
    }
}
tagscriptopen           ::= TAG_SCRIPT_OPEN.
tagscriptblockclose     ::= TAG_SCRIPT_BLOCK_CLOSE.


/*******  script tag properties *************************/
tagscriptproperties  ::= tagscriptproperties tagscriptproperty.
tagscriptproperties  ::= tagscriptproperty.

tagscriptproperty    ::=  tagscriptpropertyscripttype PROPERTYDATA(C).
{
	char *str = getCreateNTString(session, C);
    GetCurrentPage(session)->currentScriptType = mkStrdup(session->heapHandle, str, __FILE__, __LINE__);
	mkFree(session->heapHandle, str);
    htmlTokenDestructor(session, C);
}
tagscriptpropertyscripttype ::= SPACE PROPERTYID_SCRIPT_TYPE ASSIGMENT.

tagscriptproperty ::= tagscriptpropertysrc PROPERTYDATA(C).
{
	HTSQLPage *page = GetCurrentPage(session);
	wchar_t *str = getCreateNTStringW(session, C);
    page->currentScriptFile = mkStrdupW(session->heapHandle, str, __FILE__, __LINE__);
	mkFree(session->heapHandle, str);
    htmlTokenDestructor(session, C);
}
tagscriptpropertysrc ::= SPACE PROPERTYID_SCRIPT_SRC ASSIGMENT.

/** ############################################################################################
    <html> ... </html>
    ############################################################################################
*/
taghtml         ::= taghtmlfullopen taghtmlblockclosefull.
taghtml         ::= taghtmlfullopen taghtmlcontents taghtmlblockclosefull.
taghtmlfullopen ::= taghtmlopen tagclosechar.
taghtmlfullopen ::= taghtmlopen tagclosechar spaces.
taghtmlfullopen ::= taghtmlopen tagproperties tagclosechar.
taghtmlfullopen ::= taghtmlopen tagproperties tagclosechar spaces.

taghtmlblockclosefull ::= taghtmlblockclose.

taghtmlopen       ::= TAG_HTML_OPEN.
{
	session->whatIsPrinted = SESSION_PRINT_HTML_CONTENT;
}
taghtmlblockclose ::= TAG_HTML_BLOCK_CLOSE.


/** ############################################################################################
    <body> ... </body>
    ############################################################################################
*/

tagbody ::= tagbodyfullopen tagbodyblockclosefull.
tagbody ::= tagbodyfullopen tagbodycontents tagbodyblockclosefull.

tagbodyfullopen ::= tagbodyopen tagclosechar.
{
    /**
     *  During GET request phase, native or HT/SQL functions may need to send response actions to the client (browser), for that reason,
     *  client side java script variables must be initialized before.  __initializeClientJavaScriptReponse() function should be called once for each GET request
     *  after <body> tag open.
     */
    __initializeClientJavaScriptReponse(session);
}
tagbodyfullopen ::= tagbodyopen tagproperties tagclosechar.

tagbodyblockclosefull ::= tagbodyblockclose.
tagbodyblockclosefull ::= tagbodyblockclose spaces.

tagbodyopen         ::= TAG_BODY_OPEN.

tagbodyblockclose   ::= TAG_BODY_BLOCK_CLOSE(A).
{
    __runHTSQLPageLoadFunction(session, GetCurrentPage(session));
    htmlTokenDestructor(session, A);
}

/** ############################################################################################
    <head> ... </head>
    ############################################################################################
*/

taghead ::= tagheadfullopen tagheadblockclosefull.
taghead ::= tagheadfullopen tagheadcontents tagheadblockclosefull.

tagheadfullopen ::= tagheadopen.
{
	HTSQLPage *page = GetCurrentPage(session);
	const char *theme = GetQueryStringParam(session, page, "theme");
	if (theme) {
		setTheme(session, theme);
	}

	char *themeHeader = getThemeHeader(session);
	
	if (themeHeader) {
		char *cssFile = GetCurrentCSSFileName(session);
		mkPrint(session, themeHeader, NULL);
		mkPrint(session, "<link rel=\"stylesheet\" href=\"", cssFile, "\" />\n", NULL);
		mkFree(session->heapHandle, themeHeader);
		if (cssFile) {
			mkFree(session->heapHandle, cssFile);
		}
	}
	session->whatIsPrinted = SESSION_PRINT_CLIENT_LIBRARY;
}

tagheadblockclosefull ::= tagheadblockclose.
tagheadblockclosefull ::= tagheadblockclose space.

tagheadopen         ::= TAG_HEAD_OPEN.
tagheadopen         ::= TAG_HEAD_OPEN space.

tagheadblockclose   ::= TAG_HEAD_BLOCK_CLOSE.


/*******  generic tag properties WITH return value *************************
 *   Return values are REQUIRED.
 *   This definitions return all properties and their values within a single string.
 *   Return values should be destroyed by caller by calling htmlDestructor deliberately
 */
tagpropertieswithreturnvaluesall    ::= tagpropertieswithreturnvalues.

tagpropertieswithreturnvalues       ::= tagpropertieswithreturnvalues tagpropertywithreturnvalue.

tagpropertieswithreturnvalues       ::= tagpropertywithreturnvalue.

tagpropertywithreturnvalue          ::= PROPERTYID ASSIGMENT PROPERTYDATA.


/*******  generic tag properties WITHOUT return value*************************
 *   Return values are NOT REQUIRED.
 *   Return values will be destroyed by htmlDestructor after reduce. And automatically displayed in the client browser otherwise specified.
 */
tagproperties  ::= tagproperties tagproperty.
tagproperties  ::= tagproperty.

tagproperty    ::= space PROPERTYID ASSIGMENT PROPERTYDATA.


/*******  shared non-terminals **************************/
tagclosechar    ::=  TAG_CLOSE.
tagclosechar    ::=  SPACE TAG_CLOSE.

tagshortclose	::= TAG_SHORT_CLOSE.

/**
 *      Generic HTMLTEXTs and SPACEs
 *
 *      Result should be free.
 */
htmlandspaces ::= htmlandspaces htmlandspace.
htmlandspaces ::= htmlandspace.

htmlandspace  ::= HTMLTEXT.
htmlandspace  ::= SPACE.

htmltext ::= HTMLTEXT.

/**
 *      SPACE
 *
 */
spaces ::= spaces space.
spaces ::= space.
space  ::= SPACE.


