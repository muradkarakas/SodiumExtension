%include
{
#include "pch.h"

#include "tagDataBlock.h"
#include "post.parser.imp.h"
#include "mkutils.h"
#include "preObjectVariable.h"
#include "postObjectGeneric.h"
#include "htsqlFunction.imp.h"
#include "htsqlVariable.h"
#include "postObjectResultSet.h"
#include "message.h"
#include "refreshBlock.h"
#include "showColumn.h"
#include "hideColumn.h"
#include "disableColumn.h"
#include "enableColumn.h"
#include "prompt.h"
#include "rollback.h"
#include "commit.h"
#include "delete.h"
#include "populateDatalist.h"
#include "hideBlock.h"
#include "showBlock.h"
#include "tagControlBlock.h"
#include "HTSQLFunction.h"
#include "showPage.h"
#include "createConnection.h"
#include "strlen.h"
#include "substr.h"
#include "instr.h"
#include "replace.h"
#include "like.h"
#include "tagInput.h"
#include "page.h"
#include "mkError.h"
#include "populateTree.h"
#include "addTreeNode.h"
#include "refreshTreeNode.h"
#include "getDatabaseName.h"
#include "getDatabaseType.h"
#include "runSQLFile.h"
#include "createRedisConnection.h"
#include "closeRedisConnection.h"
#include "debugee.h"
#include "preObjectGeneric.h"
#include "mkHiredis.h"
#include "tagConnection.h"
#include "userHTSQLFunction.h"
#include "SodiumSession.h"
}

%name postParse

%extra_argument { SodiumSession *session }

%token_type { TokenPost }

%default_type { TokenPost }

/** HT/SQL OPERATOR PRECENSE ORDER */
%right POST_ASSIGNMENT.
%left POST_CONDITION_OR.
%left POST_CONDITION_AND.
%left POST_NOT.
%left POST_CONDITION_EQ     POST_CONDITION_NEQ  POST_CONDITION_LT   POST_CONDITION_GT   POST_CONDITION_LTEQ POST_CONDITION_GTEQ     POST_CONDITION_IS_NULL.
%left POST_AMPERSAND        POST_PLUS           POST_SUBTRACT.
%left POST_MULTIPLY         POST_DIVISION       POST_MODULAR_DIVISION.

%type expressions               {char *}
%type expression                {char *}
%type sessionid                 {char *}
%type rowid                     {char *}
%type blockItemValue            {char *}
%type blockItemName             {char *}
%type assignment                {char *}
%type variablename              {char *}
%type variablevalue             {char *}
%type stringconstant            {char *}
%type numberconstant            {char *}
%type selectstatement           {char *}
%type resultsetcolumnvalue      {char *}
%type blockName                 {char *}
%type loopbodylines             {char *}
%type loopbody                  {char *}
%type ifbodylines               {char *}
%type ifbodyline                {char *}
%type rawconditionstr           {char *}
%type rawsqlstatements          {char *}
%type rawsqlstatementcomponent  {char *}
%type returnvalue               {char *}
%type userfunctioncall          {char *}
%type replace                   {char *}
%type tojson                    {char *}
%type getdatabasename			{char *}
%type getdatabasetype			{char *}
%type instr                     {char *}
%type strlen                    {char *}
%type substr                    {char *}
%type sizeof                    {char *}
%type postconditionstring       {char *}
%type postconditionchar         {char *}
%type posttrue                  {char *}
%type postfalse                 {char *}
%type postnull                  {char *}
%type createoracleconnection    {char *}
%type createmysqlconnection     {char *}
%type createredisconnection		{redisContext *}
%type createpostgresqlconnection{char *}
%type createsqlserverconnection {char *}
%type rediscommandPing			{char *}
%type rediscommandSet			{char *}
%type rediscommandGet			{char *}
%type rediscommandIncr			{char *}
%type rediscommandDecr			{char *}
%type rediscommandIncrBy		{char *}
%type rediscommandDecrBy		{char *}
%type rediscommandDel			{char *}
%type rediscommandAppend		{char *}
%type rediscommandStrlen		{char *}
%type rediscommands				{char *}
%type stringchars               {char *}
%type stringchar                {char *}
%type dmlcommand				{char *}
%type dmlchars					{char *}
%type dmlchar					{char *}
%type runsqlfile				{char *}
%type macroline					{char *}

%type condition_options         {bool }
%type condition_option          {bool }
%type condition_eq              {bool }
%type condition_neq             {bool }
%type condition_lt              {bool }
%type condition_gt              {bool }
%type condition_lteq            {bool }
%type condition_gteq            {bool }
%type condition_is_null         {bool }
%type condition_is_not_null     {bool }
%type condition_like            {bool }
%type condition_notlike         {bool }

%token_destructor {

}


%syntax_error {
#ifndef NDEBUG
    char *errMsg = mkStrcat(session->heapHandle, __FILE__, __LINE__, "POST Syntax Error. Possible Tokens : (", NULL);
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
    __sendErrorResponse(session, GetCurrentPage(session), errMsg, "");
#endif
}

start ::= procbody.
start ::= internals.

procbody ::= POST_BEGIN
                commands
             POST_END
             POST_END_OF_FILE.
procbody ::= POST_BEGIN

             POST_END
             POST_END_OF_FILE.

commands ::= commands command.
commands ::= command.

internals::= internals internal.
internals::= internal.

internal ::= phrase
             POST_END_OF_FILE.

phrase   ::= conditionEvaluationResult.
phrase   ::= joinEvaluationResult.
phrase   ::= whereCaluseEvaluationResult.
phrase   ::= resultsetsqlevaluation.

command  ::= localvariable POST_SEMI_COLON.
command  ::= built_in_procedures.
command  ::= built_in_macros.
command  ::= assignment.
command  ::= keywords.
command  ::= insertstatement.
command  ::= updatestatement.
command  ::= deletestatement.
command  ::= plsqlblock.
command  ::= userfunctioncall(A) POST_SEMI_COLON.
{
    if (A) {
        mkFree(session->heapHandle, A);
    }
}
command  ::= POST_EXECUTER.
command  ::= comment.

keywords ::= ifstatement.
keywords ::= loop1.
keywords ::= returnstatement.

comment  ::= POST_COMMENT_START   POST_COMMENT_END.

/** This rule is used to evaluate data blocks' join condition string.
    Before executing base select statement of the data block, its join condition string is evaluated to get actual values in it.
    That is, block/variable references in join condition statement are replaces with the actual values.
    syntax pattern example:

        #:employees>>dep_id = :deps.dep_id  -->  block reference example
        #:employess>>dep_id = :variable     -->  local/global reference variable example

    user will write "dep_id = :deps.dep_id" part into the html file. text coming before = character will be used to determmine where to store (in which data block)
    evaluated result string. Blocks have joinConditionEvaluated property to hold evaluated version of the join statement
    */
joinEvaluationResult ::=    POST_JOINCONDITION_START
                                blockName(A)
                            POST_JOINCONDITION_SEPARATOR
                                rawsqlstatements(B)
                            POST_JOINCONDITION_END.
{
    HTSQLPage *page = GetCurrentPage(session);
    char *joinStatementEvaulated = B;

    TAGDataBlock *tagDatablock = findTAGDataBlockByName(session, page, A);
    if (tagDatablock) {
        setTAGDataBlockPropertyJoinConditionEvaluated(session, tagDatablock, joinStatementEvaulated);
    }
    mkFree(session->heapHandle, A);
    mkFree(session->heapHandle, B);
}

/** This rule is used to evaluate data blocks' join condition string.
    Before executing base select statement of the data block, its join condition string is evaluated to get actual values in it.
    That is, block/variable references in join condition statement are replaces with the actual values.
    syntax pattern example:

        #:employees>>dep_id = :deps.dep_id  -->  block reference example
        #:employess>>dep_id = :variable     -->  local/global reference variable example

    user will write "dep_id = :deps.dep_id" part into the html file. text coming before = character will be used to determmine where to store (in which data block)
    evaluated result string. Blocks have joinConditionEvaluated property to hold evaluated version of the join statement
    */
whereCaluseEvaluationResult ::=     POST_WHERECLAUSEEVALUATION_START
                                        blockName(A)
                                    POST_JOINCONDITION_SEPARATOR
                                        rawsqlstatements(B)
                                    POST_WHERECLAUSEEVALUATION_END.
{
    HTSQLPage *page = GetCurrentPage(session);
    char *whereClauseEvaulated = B;

    TAGDataBlock *tagDatablock = findTAGDataBlockByName(session, page, A);
    if (tagDatablock) {
        setTAGDataBlockPropertyWhereClauseEvaluated(session, page, tagDatablock, whereClauseEvaulated);
    }
    mkFree(session->heapHandle, A);
    mkFree(session->heapHandle, B);
}

resultsetsqlevaluation ::=      POST_SQLEVALUATION_START
                                    variablename(RESULTSETNAME)
                                POST_JOINCONDITION_SEPARATOR
                                    rawsqlstatements(SQL)
                                POST_SQLEVALUATION_END.
{
    HTSQLPage *page = GetCurrentPage(session);
    char *resultSetName = RESULTSETNAME;
    char *sqlStatementEvaulated = SQL;

	PostObjectGeneric *postObjectGenericObject = findPostObjectGenericObject(session, page, resultSetName);
    if (postObjectGenericObject) {
        setPostObjectResultSetEvaluatedSQL(session, postObjectGenericObject, sqlStatementEvaulated);
    }

    mkFree(session->heapHandle, RESULTSETNAME);
    mkFree(session->heapHandle, SQL);
}


rawsqlstatements(RET) ::= rawsqlstatements(AAA) rawsqlstatementcomponent(BBB).
{
    char *p1 = AAA;
    char *p2 = BBB;
    char *ret = mkStrcat(session->heapHandle, __FILE__, __LINE__, p1, p2, NULL);
    RET = ret;
    mkFree(session->heapHandle, AAA);
    mkFree(session->heapHandle, BBB);
}
rawsqlstatements(RET)       ::= rawsqlstatementcomponent(A).
{
    char *value = A;
    RET = value;
}

rawsqlstatementcomponent(RET)        ::= blockItemValue(A).
{
	char *value;
	if (A) {
		value = mkStrcat(session->heapHandle, __FILE__, __LINE__, "'", A, "'", NULL);
	}
	else {
		value = mkStrdup(session->heapHandle, "", __FILE__, __LINE__);
	}
	RET = value;

	mkFree(session->heapHandle, A);
}
rawsqlstatementcomponent(RET)        ::= POST_IDENTIFIER(A).
{
	char *value = mkMalloc(session->heapHandle, A.tokenStrLength + 1, __FILE__, __LINE__);
	strncpy_s(value, A.tokenStrLength + 1, A.tokenStr, A.tokenStrLength);
	value[A.tokenStrLength] = '\0';
	RET = value;
}
rawsqlstatementcomponent(RET)        ::= POST_COLON POST_IDENTIFIER(BINDVARIABLENAME).
{
	HTSQLPage *page = GetCurrentPage(session);

	// BIND VARIABLE NAME
	char *value = mkMalloc(session->heapHandle, BINDVARIABLENAME.tokenStrLength + 1, __FILE__, __LINE__);
	strncpy_s(value, BINDVARIABLENAME.tokenStrLength + 1, BINDVARIABLENAME.tokenStr, BINDVARIABLENAME.tokenStrLength);
	value[BINDVARIABLENAME.tokenStrLength] = '\0';

	char *retval = findAndReturnValueOfAVariableInAllScopes(session, page, NULL, value);
	if (retval && retval[0] != '\0') {
		RET = mkStrcat(session->heapHandle, __FILE__, __LINE__, "'", retval, "'", NULL);
	}
	else {
		RET = mkStrdup(session->heapHandle, "''", __FILE__, __LINE__);
	}

	mkFree(session->heapHandle, retval);
	mkFree(session->heapHandle, value);
}
rawsqlstatementcomponent(RET)   ::= POST_JOIN_ANYCHARACTER(A).
{
	char *value = mkMalloc(session->heapHandle, A.tokenStrLength + 1, __FILE__, __LINE__);
	strncpy_s(value, A.tokenStrLength + 1, A.tokenStr, A.tokenStrLength);
	value[A.tokenStrLength] = '\0';
	RET = value;
}
rawsqlstatementcomponent(RET)   ::= POST_PLSQL_BLOCK_ANYCHARACTER(A).
{
	char *value = mkMalloc(session->heapHandle, A.tokenStrLength + 1, __FILE__, __LINE__);
	strncpy_s(value, A.tokenStrLength + 1, A.tokenStr, A.tokenStrLength);
	value[A.tokenStrLength] = '\0';
	RET = value;
}
rawsqlstatementcomponent(RET)   ::= POST_BLOCK_REFERENCE(A).
{
	char *value = mkMalloc(session->heapHandle, A.tokenStrLength + 1, __FILE__, __LINE__);
	strncpy_s(value, A.tokenStrLength + 1, A.tokenStr, A.tokenStrLength);
	value[A.tokenStrLength] = '\0';
	RET = value;
}




/**  WHILE CONDITION EVALUATION */
conditionEvaluationResult  ::=   POST_OPEN_PARANTHESIS
                                    variablename(A)
                                POST_CLOSE_PARANTHESIS.
{
    HTSQLPage *page = GetCurrentPage(session);
    PostObjectResultSet *postObjectResultSet = findPostObjectGenericPostResultSetVariable(session, page, A);
    if (postObjectResultSet) {
		bool isEof = DBInt_IsEof(session->rootTAGConnection->mkDBConnection, postObjectResultSet->ociStatement);
        page->conditionEvaluationResult   = (isEof == false);
    } else {
		CLOG(ERR_CORE_VARIABLE_NOT_FOUND, A);
    }
    mkFree(session->heapHandle, A);
}
/**  WHILE CONDITION EVALUATION */
conditionEvaluationResult  ::=   POST_OPEN_PARANTHESIS
                                    condition_options
                                 POST_CLOSE_PARANTHESIS.

condition_options(RET)   ::= condition_options(A) POST_CONDITION_AND condition_options(B).
{
    bool a = A;
    bool b = B;
	HTSQLPage *page = GetCurrentPage(session); 
	page->conditionEvaluationResult = (a && b);
    RET = page->conditionEvaluationResult;
}
condition_options(RET)   ::= condition_options(A) POST_CONDITION_OR  condition_options(B).
{
    bool a = A;
    bool b = B;
	HTSQLPage *page = GetCurrentPage(session); 
	page->conditionEvaluationResult = (a || b);
    RET = page->conditionEvaluationResult;
}
condition_options(RET)   ::= POST_OPEN_PARANTHESIS condition_options(A) POST_CLOSE_PARANTHESIS.
{
    bool a = A;
	HTSQLPage *page = GetCurrentPage(session);
    page->conditionEvaluationResult = a;
    RET = page->conditionEvaluationResult;
}
condition_options(RET)   ::= POST_NOT POST_OPEN_PARANTHESIS  condition_options  POST_CLOSE_PARANTHESIS.
{
	HTSQLPage *page = GetCurrentPage(session);
    page->conditionEvaluationResult = ! page->conditionEvaluationResult;
    RET = page->conditionEvaluationResult;
}

condition_options       ::= condition_is_null.
condition_options       ::= condition_is_not_null.
condition_options       ::= condition_eq.
condition_options       ::= condition_neq.
condition_options       ::= condition_lt.
condition_options       ::= condition_gt.
condition_options       ::= condition_lteq.
condition_options       ::= condition_gteq.
condition_options       ::= condition_like.
condition_options       ::= condition_notlike.
condition_options(RET)  ::= userfunctioncall(A).
{
	HTSQLPage *page = GetCurrentPage(session);
    char *a = A;
    if (a) {
        if (strcmp(a, "true") == 0) {
            page->conditionEvaluationResult = true;
        } else if (strcmp(a, "false") == 0) {
            page->conditionEvaluationResult = false;
        } else {
			CLOG(ERR_FUNCTION_MUST_RETURN_BOOL, "");
        }
    } else {
		CLOG(ERR_FUNCTION_MUST_RETURN_BOOL, "");
    }

    mkFree(session->heapHandle, a);
    RET = page->conditionEvaluationResult;
}
condition_options(RET)  ::= posttrue(A).        // posttrue is an "expression". must be released
{
    RET = true;
	HTSQLPage *page = GetCurrentPage(session);
    page->conditionEvaluationResult = true;
    mkFree(session->heapHandle, A);
}
condition_options(RET)  ::= postfalse(A).       // postfalse is an "expression". must be released
{
    RET = false;
	HTSQLPage *page = GetCurrentPage(session);
    page->conditionEvaluationResult = false;
    mkFree(session->heapHandle, A);
}

condition_is_null(RET)  ::= expression(A) POST_CONDITION_IS_NULL.
{
    char *a = A;
    HTSQLPage *page = GetCurrentPage(session);
    page->conditionEvaluationResult   = ( (a == NULL) || (a[0] == '\0') );
    mkFree(session->heapHandle, A);
    RET = page->conditionEvaluationResult;
}
condition_is_not_null(RET) ::= expression(A) POST_CONDITION_IS_NOT_NULL.
{
    char *a = A;
    HTSQLPage *page = GetCurrentPage(session);
    page->conditionEvaluationResult   = ( a != NULL );
    mkFree(session->heapHandle, A);
    RET = page->conditionEvaluationResult;
}
condition_eq(RET)   ::= expression(A) POST_CONDITION_EQ expression(B).
{
    char *a=A;
    char *b=B;
	HTSQLPage *page = GetCurrentPage(session);
    if (a && b) {
        page->conditionEvaluationResult   = (strcmp(a, b) == 0);
    } else if (!a && !b) {
        page->conditionEvaluationResult   = true;
    } else {
        page->conditionEvaluationResult   = false;
    }

	if (A)
		mkFree(session->heapHandle, A);
    if (B)
		mkFree(session->heapHandle, B);

    RET = page->conditionEvaluationResult;
}
condition_neq(RET)  ::= expression(A) POST_CONDITION_NEQ expression(B).
{
    char *a=A;
    char *b=B;
	HTSQLPage *page = GetCurrentPage(session);
    if (a && b) {
		page->conditionEvaluationResult   = (strcmp(a, b) != 0);
    } else if (!a && !b) {
		page->conditionEvaluationResult   = false;
    } else {
		page->conditionEvaluationResult   = true;
    }

    mkFree(session->heapHandle, A);
    mkFree(session->heapHandle, B);

    RET = page->conditionEvaluationResult;
}
condition_lt(RET)   ::= expression(A) POST_CONDITION_LT expression(B).
{
    char *a;
    char *b=B;

    HTSQLPage *page = GetCurrentPage(session);

    PreObjectGeneric *aObj = findPreGenericObjectByName(session, page, A);
    if (aObj) {
        a = aObj->value.preObjectVariable->value.text;
    } else {
        a = A;
    }

    if (a && b) {
		page->conditionEvaluationResult   = (atof(a) < atof(b));
    } else if (!a && !b) {
		page->conditionEvaluationResult   = false;
    } else {
		page->conditionEvaluationResult   = (b) ? true : false;
    }

    mkFree(session->heapHandle, A);
    mkFree(session->heapHandle, B);

    RET = page->conditionEvaluationResult;
}
/** > */
condition_gt(RET)   ::=  expression(A) POST_CONDITION_GT expression(B).
{
    char *a;
    char *b=B;
    HTSQLPage *page = GetCurrentPage(session);

    PreObjectGeneric *aObj = findPreGenericObjectByName(session, page, A);
    if (aObj) {
        a = aObj->value.preObjectVariable->value.text;
    } else {
        a = A;
    }

    if (a && b) {
		page->conditionEvaluationResult   = (atof(a) > atof(b));
    } else if (!a && !b) {
		page->conditionEvaluationResult   = false;
    } else {
		page->conditionEvaluationResult   = (a) ? true : false;
    }

    mkFree(session->heapHandle, A);
    mkFree(session->heapHandle, B);

    RET = page->conditionEvaluationResult;
}
condition_like(RET) ::=  expression(A) POST_CONDITION_LIKE expression(B).
{
    char *a;
    char *b=B;
    HTSQLPage *page = GetCurrentPage(session);

    PreObjectGeneric *aObj = findPreGenericObjectByName(session, page, A);
    if (aObj) {
        a = aObj->value.preObjectVariable->value.text;
    } else {
        a = A;
    }

    if (a && b) {
		page->conditionEvaluationResult   = mkLike(a, b, true, '\0');
    } else {
		page->conditionEvaluationResult   = false;
    }

    mkFree(session->heapHandle, A);
    mkFree(session->heapHandle, B);

    RET = page->conditionEvaluationResult;
}
condition_notlike(RET) ::=  expression(A) POST_CONDITION_NOT_LIKE expression(B).
{
    char *a;
    char *b=B;
    HTSQLPage *page = GetCurrentPage(session);
    PreObjectGeneric *aObj = findPreGenericObjectByName(session, page, A);
    if (aObj) {
        a = aObj->value.preObjectVariable->value.text;
    } else {
        a = A;
    }

    if (a && b) {
		page->conditionEvaluationResult   = ! mkLike(a, b, true, '\0');
    } else {
		page->conditionEvaluationResult   = false;
    }

    mkFree(session->heapHandle, A);
    mkFree(session->heapHandle, B);

    RET = page->conditionEvaluationResult;
}

/**  <= */
condition_lteq(RET) ::= expression(A) POST_CONDITION_LTEQ expression(B).
{
    char *a;
    char *b=B;
    HTSQLPage *page = GetCurrentPage(session);
    PreObjectGeneric *aObj = findPreGenericObjectByName(session, page, A);
    if (aObj) {
        a = aObj->value.preObjectVariable->value.text;
    } else {
        a = A;
    }

    if (a && b) {
		page->conditionEvaluationResult   = ((atof(a) < atof(b)) || (atof(a) == atof(b)));
    } else if (!a && !b) {
		page->conditionEvaluationResult   = true; // they are equal
    } else {
		page->conditionEvaluationResult   = (b) ? true : false;
    }

    mkFree(session->heapHandle, A);
    mkFree(session->heapHandle, B);

    RET = page->conditionEvaluationResult;
}
/**  >= */
condition_gteq(RET)  ::= expression(A) POST_CONDITION_GTEQ expression(B).
{
    char *a;
    char *b=B;
    HTSQLPage *page = GetCurrentPage(session);
    PreObjectGeneric *aObj = findPreGenericObjectByName(session, page, A);
    if (aObj) {
        a = aObj->value.preObjectVariable->value.text;
    } else {
        a = A;
    }

    if (a && b) {
		page->conditionEvaluationResult   = ((atof(a) > atof(b)) || (atof(a) == atof(b)));
    } else if (!a && !b) {
		page->conditionEvaluationResult   = true; // they are equal
    } else {
		page->conditionEvaluationResult   = (a) ? true : false;
    }

    mkFree(session->heapHandle, A);
    mkFree(session->heapHandle, B);

    RET = page->conditionEvaluationResult;
}




plsqlblock  ::=     POST_PLSQL_BLOCK_BEGIN
                        rawsqlstatements(PLSQLBLOCK)
                    POST_PLSQL_BLOCK_END.
{
    char *plSQL = PLSQLBLOCK;

    executePLSQLBlock(session, GetCurrentPage(session), plSQL);

    mkFree(session->heapHandle, plSQL);
}


insertstatement     ::= POST_INSERT(A) POST_SEMI_COLON.
{
    char *insertSQL = mkMalloc(session->heapHandle, A.tokenStrLength+1, __FILE__, __LINE__);
    strncpy_s(insertSQL, A.tokenStrLength + 1, A.tokenStr, A.tokenStrLength);
    insertSQL[A.tokenStrLength] = '\0';

    executeInsertStatement(session, GetCurrentPage(session), insertSQL);

    mkFree(session->heapHandle, insertSQL);
}

updatestatement     ::= POST_UPDATE(A) POST_SEMI_COLON.
{
    char *updateSQL = mkMalloc(session->heapHandle, A.tokenStrLength+1, __FILE__, __LINE__);
    strncpy_s(updateSQL, A.tokenStrLength + 1, A.tokenStr, A.tokenStrLength);
    updateSQL[A.tokenStrLength] = '\0';

    executeUpdateStatement(session, GetCurrentPage(session), updateSQL);

    mkFree(session->heapHandle, updateSQL);
}

deletestatement     ::= POST_DELETE(A) POST_SEMI_COLON.
{
    char *deleteSQL = mkMalloc(session->heapHandle, A.tokenStrLength+1, __FILE__, __LINE__);
    strncpy_s(deleteSQL, A.tokenStrLength + 1, A.tokenStr, A.tokenStrLength);
    deleteSQL[A.tokenStrLength] = '\0';

    executeDeleteStatement(session, GetCurrentPage(session), deleteSQL);

    mkFree(session->heapHandle, deleteSQL);
}



/**###########################################################################################
 *   BUILT-IN MACROS
 * ########################################################################################### */

built_in_macros	::= macroline.

macroline(RET)		::= POST_MACRO_LINE.
{
	HTSQLPage *page = GetCurrentPage(session);
	char *ret = mkMalloc(session->heapHandle, 10, __FILE__, __LINE__);
	mkItoa(getPageCurrentLineNumberToBeExecuted(session, page), ret);
	RET = ret;
}

/**###########################################################################################
 *   LOCAL VARIABLE DECLARATION VARIANTS
 * ########################################################################################### */


localvariable       ::= POST_VARIABLE_TYPE_VARCHAR  charvardecs.

charvardecs     ::= charvardecs POST_COMMA charvardec.
charvardecs     ::= charvardec.
charvardec      ::= POST_IDENTIFIER(A).
{
    char *variableName = mkMalloc(session->heapHandle, A.tokenStrLength+1, __FILE__, __LINE__);
    strncpy_s(variableName, A.tokenStrLength + 1, A.tokenStr, A.tokenStrLength);
    variableName[A.tokenStrLength] = '\0';

    postFunctionAddArgumentsOrVariables(session, GetCurrentPage(session), POST_OBJECT_GENERIC_TYPE_VARIABLE, PREOBJECTTYPE_VARIABLE_VARCHAR, variableName, NULL);

    mkDebug(session, SESSION_LOG_DETAIL_HTSQL_VARIABLE, "\n", "Local variable created : '", variableName, "'", NULL);
    mkFreeAdv(session->heapHandle, (void**) &variableName);
}
charvardec      ::= POST_IDENTIFIER(A) POST_ASSIGNMENT expressions(B).
{
    char *variableName = mkMalloc(session->heapHandle, A.tokenStrLength+1, __FILE__, __LINE__);
    strncpy_s(variableName, A.tokenStrLength + 1, A.tokenStr, A.tokenStrLength);
    variableName[A.tokenStrLength] = '\0';

    postFunctionAddArgumentsOrVariables(session, GetCurrentPage(session), POST_OBJECT_GENERIC_TYPE_VARIABLE, PREOBJECTTYPE_VARIABLE_VARCHAR, variableName, B);

    if (B) {
        mkDebug(session, SESSION_LOG_DETAIL_HTSQL_VARIABLE, "\n", "Local variable created : '", variableName, "' = '", B, "'", NULL);
    } else {
        mkDebug(session, SESSION_LOG_DETAIL_HTSQL_VARIABLE, "\n", "Local variable created : '", variableName, "' = null", NULL);
    }

    mkFree(session->heapHandle, B);
    mkFree(session->heapHandle, variableName);
}


localvariable   ::= POST_VARIABLE_TYPE_REDIS  redisvardecs.

redisvardecs    ::= redisvardecs POST_COMMA redisvardec.
redisvardecs    ::= redisvardec.
redisvardec		::= POST_IDENTIFIER(A).
{
	char *variableName = mkMalloc(session->heapHandle, A.tokenStrLength + 1, __FILE__, __LINE__);
	strncpy_s(variableName, A.tokenStrLength + 1, A.tokenStr, A.tokenStrLength);
	variableName[A.tokenStrLength] = '\0';

	postFunctionAddArgumentsOrVariables(session, GetCurrentPage(session), POST_OBJECT_GENERIC_TYPE_VARIABLE, PREOBJECTTYPE_VARIABLE_REDIS, variableName, NULL);

	mkDebug(session, SESSION_LOG_DETAIL_HTSQL_VARIABLE, "\n", "Local variable created : '", variableName, "'", NULL);
	mkFreeAdv(session->heapHandle, (void**)&variableName);
}
redisvardec      ::= POST_IDENTIFIER(A) POST_ASSIGNMENT expressions(B).
{
	char *variableName = mkMalloc(session->heapHandle, A.tokenStrLength + 1, __FILE__, __LINE__);
	strncpy_s(variableName, A.tokenStrLength + 1, A.tokenStr, A.tokenStrLength);
	variableName[A.tokenStrLength] = '\0';

	postFunctionAddArgumentsOrVariables(session, GetCurrentPage(session), POST_OBJECT_GENERIC_TYPE_VARIABLE, PREOBJECTTYPE_VARIABLE_REDIS, variableName, B);

	if (B) {
		mkDebug(session, SESSION_LOG_DETAIL_HTSQL_VARIABLE, "\n", "Local variable created : '", variableName, "' = '", B, "'", NULL);
	}
	else {
		mkDebug(session, SESSION_LOG_DETAIL_HTSQL_VARIABLE, "\n", "Local variable created : '", variableName, "' = null", NULL);
	}

	mkFree(session->heapHandle, B);
	mkFree(session->heapHandle, variableName);
}


localvariable       ::= POST_VARIABLE_TYPE_BOOL  boolvardecs.

boolvardecs     ::= boolvardecs POST_COMMA boolvardec.
boolvardecs     ::= boolvardec.
boolvardec      ::= POST_IDENTIFIER(A).
{
    char *variableName = mkMalloc(session->heapHandle, A.tokenStrLength+1, __FILE__, __LINE__);
    strncpy_s(variableName, A.tokenStrLength + 1, A.tokenStr, A.tokenStrLength);
    variableName[A.tokenStrLength] = '\0';

    postFunctionAddArgumentsOrVariables(session, GetCurrentPage(session), POST_OBJECT_GENERIC_TYPE_VARIABLE, PREOBJECTTYPE_VARIABLE_BOOL, variableName, NULL);

    mkDebug(session, SESSION_LOG_DETAIL_HTSQL_VARIABLE, "\n", "Local variable created : '", variableName, "'", NULL);
    mkFreeAdv(session->heapHandle, (void**) &variableName);
} 
boolvardec      ::= POST_IDENTIFIER(A) POST_ASSIGNMENT expressions(B).
{
    char *variableName = mkMalloc(session->heapHandle, A.tokenStrLength+1, __FILE__, __LINE__);
    strncpy_s(variableName, A.tokenStrLength+1, A.tokenStr, A.tokenStrLength);
    variableName[A.tokenStrLength] = '\0';

    postFunctionAddArgumentsOrVariables(session, GetCurrentPage(session), POST_OBJECT_GENERIC_TYPE_VARIABLE, PREOBJECTTYPE_VARIABLE_BOOL, variableName, B);

    mkDebug(session, SESSION_LOG_DETAIL_HTSQL_VARIABLE, "\n", "Local variable created : '", variableName, "' = '", B, "'", NULL);

    if (B) {
        mkDebug(session, SESSION_LOG_DETAIL_HTSQL_VARIABLE, "\n", "Local variable created : '", variableName, "' = '", B, "'", NULL);
        mkFreeAdv(session->heapHandle, (void**) &B);
    } else {
        mkDebug(session, SESSION_LOG_DETAIL_HTSQL_VARIABLE, "\n", "Local variable created : '", variableName, "' = null", NULL);
    }

    mkFreeAdv(session->heapHandle, (void**) &variableName);
}

localvariable  ::= POST_VARIABLE_TYPE_NUMBER intvardecs.
intvardecs     ::= intvardecs POST_COMMA intvardec.
intvardecs     ::= intvardec.
intvardec      ::= POST_IDENTIFIER(A).
{
    char *variableName = mkMalloc(session->heapHandle, A.tokenStrLength+1, __FILE__, __LINE__);
    strncpy_s(variableName, A.tokenStrLength + 1, A.tokenStr, A.tokenStrLength);
    variableName[A.tokenStrLength] = '\0';

    postFunctionAddArgumentsOrVariables(session, GetCurrentPage(session), POST_OBJECT_GENERIC_TYPE_VARIABLE, PREOBJECTTYPE_VARIABLE_NUMBER, variableName, 0);

    mkDebug(session, SESSION_LOG_DETAIL_HTSQL_VARIABLE, "\n", "Local variable created : '", variableName, "'", NULL);
    mkFreeAdv(session->heapHandle, (void**) &variableName);
}
intvardec      ::= POST_IDENTIFIER(A) POST_ASSIGNMENT expressions(B).
{
    char *variableName = mkMalloc(session->heapHandle, A.tokenStrLength+1, __FILE__, __LINE__);
    strncpy_s(variableName, A.tokenStrLength + 1, A.tokenStr, A.tokenStrLength);
    variableName[A.tokenStrLength] = '\0';

    postFunctionAddArgumentsOrVariables(session, GetCurrentPage(session), POST_OBJECT_GENERIC_TYPE_VARIABLE, PREOBJECTTYPE_VARIABLE_NUMBER, variableName, B);

    mkDebug(session, SESSION_LOG_DETAIL_HTSQL_VARIABLE, "\n", "Local variable created : '", variableName, "' = '", B, "'", NULL);
    mkFreeAdv(session->heapHandle, (void**) &variableName);
    mkFreeAdv(session->heapHandle, (void**) &B);
}


localvariable   ::= POST_VARIABLE_TYPE_DATE datevardecs.
datevardecs     ::= datevardecs POST_COMMA datevardec.
datevardecs     ::= datevardec.
datevardec      ::= POST_IDENTIFIER(A).
{
    char *variableName = mkMalloc(session->heapHandle, A.tokenStrLength+1, __FILE__, __LINE__);
    strncpy_s(variableName, A.tokenStrLength + 1, A.tokenStr, A.tokenStrLength);
    variableName[A.tokenStrLength] = '\0';

    postFunctionAddArgumentsOrVariables(session, GetCurrentPage(session), POST_OBJECT_GENERIC_TYPE_VARIABLE, PREOBJECTTYPE_VARIABLE_DATE, variableName, NULL);

    mkDebug(session, SESSION_LOG_DETAIL_HTSQL_VARIABLE, "\n", "Local variable created : '", variableName, "'", NULL);
    mkFreeAdv(session->heapHandle, (void**) &variableName);
}
datevardec      ::= POST_IDENTIFIER(A) POST_ASSIGNMENT expressions(B).
{
    char *variableName = mkMalloc(session->heapHandle, A.tokenStrLength+1, __FILE__, __LINE__);
    strncpy_s(variableName, A.tokenStrLength + 1, A.tokenStr, A.tokenStrLength);
    variableName[A.tokenStrLength] = '\0';

    postFunctionAddArgumentsOrVariables(session, GetCurrentPage(session), POST_OBJECT_GENERIC_TYPE_VARIABLE, PREOBJECTTYPE_VARIABLE_DATE, variableName, B);

    mkDebug(session, SESSION_LOG_DETAIL_HTSQL_VARIABLE, "\n", "Local variable created : '", variableName, "' = '", B, "'", NULL);
    mkFreeAdv(session->heapHandle, (void**) &variableName);
    mkFreeAdv(session->heapHandle, (void**) &B);
}
/** _________________________________________________________________________________________ */





/**###########################################################################################
 *   RETURN STATEMENT VARIANTS
 * ########################################################################################### */
returnstatement   ::= POST_RETURN_PREFIX POST_SEMI_COLON.
{
    HTSQLPage *page = GetCurrentPage(session);
    page->currentCallStack->returnStatementActive = true;

    // checking the function prototype whether it must return a value or not
    if (page->currentCallStack->preObjectFunction->preObjectFunctionReturnValueDeclaration) {
		CLOG(ERR_FUNCTION_MUST_RETURN_VALUE, page->currentCallStack->closureName);
	}

    mkDebug(session, SESSION_LOG_DETAIL_HTSQL_VARIABLE, "\n", "Function returned nothing", NULL);
}
returnstatement   ::= POST_RETURN_PREFIX returnvalue POST_SEMI_COLON.

returnvalue ::= expressions(RETURN_VALUE).
{
	HTSQLPage *page = GetCurrentPage(session);
    char *retval = RETURN_VALUE;
    page->currentCallStack->returnStatementActive = true;

    if (retval) {
		if (page->currentCallStack->preObjectFunction->preObjectFunctionReturnValueDeclaration) {
			setCurrentUserFunctionReturnValue(session, page, retval);
			mkDebug(session, SESSION_LOG_DETAIL_HTSQL_VARIABLE, "\n", "Function returned a value : '", page->currentCallStack->preObjectFunctionReturnValue->value.text, "'", NULL);
		}
		else {
			CLOG(ERR_FUNCTION_DECLARATION_DOES_NOT_RETURN_VALUE, page->currentCallStack->closureName);
		}
        
    } else {
        mkDebug(session, SESSION_LOG_DETAIL_HTSQL_VARIABLE, "\n", "Function is expected to return a value but nothing returned", NULL);
    }

    if (retval) {
        mkFreeAdv(session->heapHandle, (void**) &RETURN_VALUE);
    }
}
/** _________________________________________________________________________________________ */


/**###########################################################################################
 *   USER FUNCTION CALL VARIANTS
 * ########################################################################################### */
userfunctioncall(RET)    ::= POST_IDENTIFIER(A) POST_OPEN_PARANTHESIS POST_CLOSE_PARANTHESIS.
{
    HTSQLPage *page = GetCurrentPage(session);

    char *functionToRun = mkMalloc(session->heapHandle, A.tokenStrLength+1, __FILE__, __LINE__);
    strncpy_s(functionToRun, A.tokenStrLength + 1, A.tokenStr, A.tokenStrLength);
    functionToRun[A.tokenStrLength] = '\0';

    /** running user function */
	PreObjectGeneric *preObjectGeneric = callUserFunction(session, page, functionToRun, 0);

    /** returning user function return value as expression value */
	if (page->currentCallStack && page->currentCallStack->preObjectFunctionReturnValue) {
		RET = mkStrdup(session->heapHandle, page->currentCallStack->preObjectFunctionReturnValue->value.text, __FILE__, __LINE__);
	}
	else {
		//RET = mkStrdup(session->heapHandle, "", __FILE__, __LINE__);
		RET = NULL;
	}

    mkFree(session->heapHandle, functionToRun);
}

userfunctioncall(RET)    ::= POST_IDENTIFIER(A) POST_OPEN_PARANTHESIS expressions(PARAM1VALUE) POST_CLOSE_PARANTHESIS.
{
    HTSQLPage *page = GetCurrentPage(session);
	char *paramValue = PARAM1VALUE;

    char *functionToRun = mkMalloc(session->heapHandle, A.tokenStrLength+1, __FILE__, __LINE__);
    strncpy_s(functionToRun, A.tokenStrLength + 1, A.tokenStr, A.tokenStrLength);
    functionToRun[A.tokenStrLength] = '\0';

    /** running user function */
	PreObjectGeneric *preObjectGeneric = callUserFunction(session, page, functionToRun, 1, paramValue);

    /** returning user function return value as expression value */
    if (page->currentCallStack && page->currentCallStack->preObjectFunctionReturnValue) {
		RET = mkStrdup(session->heapHandle, page->currentCallStack->preObjectFunctionReturnValue->value.text, __FILE__, __LINE__);
	}
	else {
		RET = mkStrdup(session->heapHandle, "", __FILE__, __LINE__);
	}

    mkFreeAdv(session->heapHandle, (void**)&functionToRun);
    mkFreeAdv(session->heapHandle, (void**)&PARAM1VALUE);
}

userfunctioncall(RET)    ::= POST_IDENTIFIER(A) POST_OPEN_PARANTHESIS expressions(PARAM1VALUE) POST_COMMA expressions(PARAM2VALUE) POST_CLOSE_PARANTHESIS.
{
    HTSQLPage *page = GetCurrentPage(session);

    char *functionToRun = mkMalloc(session->heapHandle, A.tokenStrLength+1, __FILE__, __LINE__);
    strncpy_s(functionToRun, A.tokenStrLength + 1, A.tokenStr, A.tokenStrLength);
    functionToRun[A.tokenStrLength] = '\0';

    /** running user function */
    callUserFunction(session, page, functionToRun, 2, PARAM1VALUE, PARAM2VALUE);

    /** returning user function return value as expression value */
	if (page->currentCallStack && page->currentCallStack->preObjectFunctionReturnValue) {
		RET = mkStrdup(session->heapHandle, page->currentCallStack->preObjectFunctionReturnValue->value.text, __FILE__, __LINE__);
	}
	else {
		RET = mkStrdup(session->heapHandle, "", __FILE__, __LINE__);
	}

    mkFreeAdv(session->heapHandle, (void**)&functionToRun);
    mkFreeAdv(session->heapHandle, (void**)&PARAM1VALUE);
    mkFreeAdv(session->heapHandle, (void**)&PARAM2VALUE);
}

userfunctioncall(RET)    ::= POST_IDENTIFIER(A) POST_OPEN_PARANTHESIS expressions(PARAM1VALUE) POST_COMMA expressions(PARAM2VALUE) POST_COMMA expressions(PARAM3VALUE) POST_CLOSE_PARANTHESIS.
{
    HTSQLPage *page = GetCurrentPage(session);

    char *functionToRun = mkMalloc(session->heapHandle, A.tokenStrLength+1, __FILE__, __LINE__);
    strncpy_s(functionToRun, A.tokenStrLength + 1, A.tokenStr, A.tokenStrLength);
    functionToRun[A.tokenStrLength] = '\0';

    /** running user function */
    callUserFunction(session, GetCurrentPage(session), functionToRun, 3, PARAM1VALUE, PARAM2VALUE, PARAM3VALUE);

    /** returning user function return value as expression value */
	if (page->currentCallStack && page->currentCallStack->preObjectFunctionReturnValue) {
		RET = mkStrdup(session->heapHandle, page->currentCallStack->preObjectFunctionReturnValue->value.text, __FILE__, __LINE__);
	}
	else {
		RET = mkStrdup(session->heapHandle, "", __FILE__, __LINE__);
	}

    mkFreeAdv(session->heapHandle, (void**)&functionToRun);
    mkFreeAdv(session->heapHandle, (void**)&PARAM1VALUE);
    mkFreeAdv(session->heapHandle, (void**)&PARAM2VALUE);
    mkFreeAdv(session->heapHandle, (void**)&PARAM3VALUE);
}
/** _________________________________________________________________________________________ */




/**###########################################################################################
 *   WHILE CONDITION IMPLEMENTATION
 * ########################################################################################### */
loop1    ::=    POST_WHILE
                    rawconditionstr(AAA)
                POST_LOOP_1
                    loopbodylines(BODYLINES)
                POST_LOOP_END_1.
{
    HTSQLPage *page = GetCurrentPage(session);
    char *postWhileConditionStr = AAA;

    // If we encounter "return" statement, we are not executing loop.
    if (page->currentCallStack && page->currentCallStack->returnStatementActive) {


    } else {

        while (page->conditionEvaluationResult) {

            if (page->currentCallStack->returnStatementActive) {
                break;
            }
            /** evaluation results in true. So executing loop contents once again */
            char *bodyContents = mkStrcat(session->heapHandle, __FILE__, __LINE__, " { ", BODYLINES, " } ", NULL);
            parseAndExecutePostString(session, page, bodyContents);
            mkFree(session->heapHandle, bodyContents);

            /** reevaluate the condition string before loop */
            parseAndExecutePostString(session, page, postWhileConditionStr);
        }
    }
    mkFree(session->heapHandle, BODYLINES);
    mkFree(session->heapHandle, AAA);
}

rawconditionstr(RET)   ::= postconditionstring(AAA).
{
    HTSQLPage *page = GetCurrentPage(session);
    char *value = mkStrcat(session->heapHandle, __FILE__, __LINE__, "(", AAA, ")", NULL);

    /** evaluate the condition string before loop */
    parseAndExecutePostString(session, page, value);

    mkFree(session->heapHandle, AAA);
    RET = value;
}
loopbodylines(RET)  ::= loopbodylines(A) loopbody(B).
{
    char *pp1 = A;
    char *pp2 = B;
    char *retval = mkStrcat(session->heapHandle, __FILE__, __LINE__, pp1, pp2, NULL);
    RET = retval;
    mkFree(session->heapHandle, pp1);
    mkFree(session->heapHandle, pp2);
}
loopbodylines  ::= loopbody.

loopbody(RET)       ::= POST_LOOP_1_LINE_CHARS(A).
{
    char *value = mkMalloc(session->heapHandle, A.tokenStrLength+1, __FILE__, __LINE__);
    strncpy_s(value, A.tokenStrLength + 1, A.tokenStr, A.tokenStrLength);
    value[A.tokenStrLength] = '\0';
    RET = value;
}
/** _________________________________________________________________________________________ */



postconditionstring(RET) ::= postconditionstring(A) postconditionchar(B).
{
    char *p1 = A;
    char *p2 = B;

    char *retval = mkStrcat(session->heapHandle, __FILE__, __LINE__, p1, p2, NULL);
    RET = retval;
    mkFree(session->heapHandle, p1);
    mkFree(session->heapHandle, p2);
}
postconditionstring     ::= postconditionchar.

postconditionchar(RET)  ::= POST_CONDITION_CHAR(A).
{
    char *value = mkMalloc(session->heapHandle, A.tokenStrLength+1, __FILE__, __LINE__);
    strncpy_s(value, A.tokenStrLength + 1, A.tokenStr, A.tokenStrLength);
    value[A.tokenStrLength] = '\0';
    RET = value;
}


/**###########################################################################################
 *    IF CONDITION IMPLEMENTATION
 * ########################################################################################### */
ifstatement  ::=	POST_IF rawconditionstr POST_THEN
						/* empty if block */
					POST_END_IF_WITH_SEMICOLON.

ifstatement  ::=    POST_IF rawconditionstr(AAA) POST_THEN
						ifbodylines(TRUE_BODY_LINES)
					POST_END_IF_WITH_SEMICOLON
					POST_SEMI_COLON.
{
	HTSQLPage *page = GetCurrentPage(session);
	char *trueBodyLines = TRUE_BODY_LINES;
	
	if (page->conditionEvaluationResult == TRUE) {
		reduceifTrueAndFalseBlock(session, page, trueBodyLines);
	}
	else {		
		page->currentCallStack->functionDebugInfo->lineNumberInner += mkStrCountEnter(trueBodyLines);
		raiseBreakPointException(session);
	}

	mkFree(session->heapHandle, AAA); /** ONCE FREE CALL is ENOUGH */
	mkFree(session->heapHandle, TRUE_BODY_LINES);
}

ifstatement  ::=    POST_IF rawconditionstr(AAA) POST_THEN
						ifbodylines(TRUE_BODY_LINES)
					POST_ELSE
						ifbodylines(FALSE_BODY_LINES)
					POST_END_IF_WITH_SEMICOLON
					POST_SEMI_COLON.
{
	HTSQLPage *page = GetCurrentPage(session);
	char *trueBodyLines = TRUE_BODY_LINES;
	char *falseBodyLines = FALSE_BODY_LINES;

	int backupInner = page->currentCallStack->functionDebugInfo->lineNumberInner;
	int backupTotalLineCount = mkStrCountEnter(trueBodyLines) + mkStrCountEnter(falseBodyLines); //page->currenHTSQLFunctionGenericInstance->functionDebugInfo->ifTotalLineCount;

	if (page->conditionEvaluationResult == TRUE) {
		reduceifTrueAndFalseBlock(session, page, trueBodyLines);
		page->currentCallStack->functionDebugInfo->lineNumberInner = backupInner + backupTotalLineCount;
		raiseBreakPointException(session);
	}
	else {
		page->currentCallStack->functionDebugInfo->lineNumberInner += mkStrCountEnter(trueBodyLines);
		reduceifTrueAndFalseBlock(session, page, falseBodyLines);
	}

	mkFree(session->heapHandle, AAA); /** ONCE FREE CALL is ENOUGH */
	mkFree(session->heapHandle, TRUE_BODY_LINES);
	mkFree(session->heapHandle, FALSE_BODY_LINES);
}

ifbodylines(RET)  ::= ifbodylines(A) ifbodyline(B).
{
    char *p1 = A;
    char *p2 = B;
    char *retval = mkStrcat(session->heapHandle, __FILE__, __LINE__, p1, p2, NULL);
    RET = retval;
    mkFree(session->heapHandle, p1);
    mkFree(session->heapHandle, p2);
}
ifbodylines  ::= ifbodyline.


ifbodyline(RET)       ::= POST_IF_LINE(A).
{
    char *value = mkMalloc(session->heapHandle, A.tokenStrLength+1, __FILE__, __LINE__);
    strncpy_s(value, A.tokenStrLength + 1, A.tokenStr, A.tokenStrLength);
    value[A.tokenStrLength] = '\0';
    RET = value;
}
/** _________________________________________________________________________________________ */


/** assignment a value to a formula column */
assignment ::= blockItemName(A) POST_ASSIGNMENT expressions(B) POST_SEMI_COLON.
{
    char *blockName = mkStrdup(session->heapHandle, A, __FILE__, __LINE__);
    size_t point = (strstr(blockName, ".") - blockName);
    blockName[point] = '\0';

    char *itemName = &blockName[point + 1];

    setBlockVariableValue(session, GetCurrentPage(session), &blockName[1], itemName, B);

    mkFree(session->heapHandle, blockName);
    mkFree(session->heapHandle, A);
	if (B) {
		mkFree(session->heapHandle, B);
	}
}

assignment ::= variablename(A) POST_ASSIGNMENT createredisconnection(B) POST_SEMI_COLON.
{
	char *leftSide = A;
	redisContext *rightSide = B;

	HTSQLPage *page = GetCurrentPage(session);

	PreObjectVariable *preObjectVariable = findRedisPreObjectVariable(session, page, leftSide, false);
	if (preObjectVariable) {
		setPreObjectVariableValueRedis(session, preObjectVariable, rightSide);
	}
	else {
		// Variable is not valid. So, we must close redis connection
		closeRedisConnection(session, rightSide);
	}

	mkFree(session->heapHandle, A);
}

/** assignment a value to a variable */
assignment ::= variablename(A) POST_ASSIGNMENT expressions(B) POST_SEMI_COLON.
{
	char *leftSide = A;
	char *rightSide = B;

    HTSQLPage *page = GetCurrentPage(session);

    /** searching in local scope variables  */
    PreObjectVariable *postObjectVariable = findPostObjectGenericPostObjectVariable(session, page, leftSide);
    if (postObjectVariable) {
        setPostObjectGenericVariableValueVarchar(session, postObjectVariable, rightSide);
    } else {
        /** Searching in global scope variables  */
        PreObjectGeneric *preObjectGeneric = findPreGenericObjectByName(session, page, leftSide);

        if (preObjectGeneric) {

			if (preObjectGeneric->value.preObjectVariable->preObjectVariableType == PREOBJECTTYPE_VARIABLE_REDIS) {
				CLOG(ERR_REDIS_CANNOT_ASSIGN, "");
			}
			else {
				setPreObjectVariableValueVarchar(session, preObjectGeneric->value.preObjectVariable, rightSide);
			}
            
        } else {
             /** not found */
			CLOG(ERR_NAME_IS_NOT_FOUND, leftSide);
        }
    }
    mkFree(session->heapHandle, A);
    if (B) {
        mkFree(session->heapHandle, B);
    }
}


/** result set creation (implicit assignment */
assignment ::= variablename(VARIABLENAME) POST_ASSIGNMENT selectstatement(SQL).
{
    HTSQLPage *page = GetCurrentPage(session);
	char *sql = SQL;

    PostObjectResultSet *postObjectResultSet;

    /** we need a connection. we'll get it from session->rootTAGConnection */
    TAGConnection *resultsetTAGConnection = getActiveTAGConnection(session, page);

    if (resultsetTAGConnection != NULL && resultsetTAGConnection->mkDBConnection != NULL) {
        /**  Creating a PostObjectGeneric object in order to put ResultSet object inside it. */
		postObjectResultSet = createPostObjectResultSet(session, resultsetTAGConnection);		
		PreObjectVariable *preObjectVariable = createPreObjectVariableResultSet(session, VARIABLENAME);
		setPreObjectVariableValueRecordSet(session, preObjectVariable, postObjectResultSet);
        PostObjectGeneric* postObjectGeneric = createPostObjectGeneric(session, page, POST_OBJECT_GENERIC_TYPE_VARIABLE, VARIABLENAME, preObjectVariable);

        /** Result set object (postObjectResultSet) ready now */

        /** Let's get PreObjectGeneric object which is created for the VARIABLENAME variable before and put the
            postObjectResultSet object into it */
            setPostObjectResultSetResultSet(session, page, postObjectGeneric, resultsetTAGConnection, sql);
    } else {
		CLOG(ERR_NO_ACTIVE_CONNECTION, "");
    }

    mkFree(session->heapHandle, VARIABLENAME);
    mkFree(session->heapHandle, SQL);
}


/**###########################################################################################
*    BUILT-IN FUNCTION IMPLEMENTATION
* ########################################################################################### */
built_in_procedures     ::= message.
built_in_procedures     ::= showcolumn.
built_in_procedures     ::= hidecolumn.
built_in_procedures     ::= disablecolumn.
built_in_procedures     ::= enablecolumn.
built_in_procedures     ::= prompt.
built_in_procedures     ::= pupulatedatalist.
built_in_procedures     ::= pupulatetree.
built_in_procedures     ::= rollback.
built_in_procedures     ::= commit.
built_in_procedures     ::= delete.
built_in_procedures     ::= refreshblock.
built_in_procedures     ::= setdatablockproperty.
built_in_procedures     ::= hideblock.
built_in_procedures     ::= showblock.
built_in_procedures     ::= showpage.
built_in_procedures     ::= nextrecord.
built_in_procedures     ::= firstrecord.
built_in_procedures     ::= lastrecord.
built_in_procedures     ::= previousrecord.
built_in_procedures     ::= sizeof.
built_in_procedures     ::= refreshtreenode.
built_in_procedures     ::= closeredisconnection.

/**
 * SIZEOF  PROCEDURE VARIANTS
 */
sizeof(RET)  ::= POST_SIZEOF_PREFIX stringconstant(PARAM) POST_CLOSE_PARANTHESIS.
{
    char *source    = PARAM;

    if (source) {
        char *target = mkStrlen(session, source);
        RET = target;
    } else {
        RET = mkStrdup(session->heapHandle, "0", __FILE__, __LINE__);
    }
    mkFree(session->heapHandle, PARAM);
}
sizeof(RET)  ::= POST_SIZEOF_PREFIX numberconstant(PARAM) POST_CLOSE_PARANTHESIS.
{
    char *buf    = mkMalloc(session->heapHandle, 50, __FILE__, __LINE__);
    int s = sizeof(double);
    mkDoubleToChar(s, buf);
    RET = mkStrdup(session->heapHandle, buf, __FILE__, __LINE__);
    mkFree(session->heapHandle, PARAM);
    mkFree(session->heapHandle, buf);
}
sizeof(RET)  ::= POST_SIZEOF_PREFIX posttrue(PARAM) POST_CLOSE_PARANTHESIS.
{
    char *buf    = mkMalloc(session->heapHandle, 50, __FILE__, __LINE__);
    int s = sizeof(int);
    mkDoubleToChar(s, buf);
    RET = mkStrdup(session->heapHandle, buf, __FILE__, __LINE__);
    mkFree(session->heapHandle, PARAM);
    mkFree(session->heapHandle, buf);
}
sizeof(RET)  ::= POST_SIZEOF_PREFIX postfalse(PARAM) POST_CLOSE_PARANTHESIS.
{
    char *buf    = mkMalloc(session->heapHandle, 50, __FILE__, __LINE__);
    int s = sizeof(int);
    mkDoubleToChar(s, buf);
    RET = mkStrdup(session->heapHandle, buf, __FILE__, __LINE__);
    mkFree(session->heapHandle, PARAM);
    mkFree(session->heapHandle, buf);
}
sizeof(RET)  ::= POST_SIZEOF_PREFIX postnull(PARAM) POST_CLOSE_PARANTHESIS.
{
    char *buf    = mkMalloc(session->heapHandle, 50, __FILE__, __LINE__);
    int s = sizeof(int);
    mkDoubleToChar(s, buf);
    RET = mkStrdup(session->heapHandle, buf, __FILE__, __LINE__);
    mkFree(session->heapHandle, PARAM);
    mkFree(session->heapHandle, buf);
}


/**
 * STRLEN  PROCEDURE VARIANTS
 */
strlen(RET) ::= POST_STRLEN_PREFIX expressions(SOURCE) POST_CLOSE_PARANTHESIS.
{
    char *source    = SOURCE;

    if (source) {
        char *target = mkStrlen(session, source);
        RET = target;
    } else {
        RET = mkStrdup(session->heapHandle, "0", __FILE__, __LINE__);
    }

    mkFree(session->heapHandle, SOURCE);
}

/**
 * SUBSTR  PROCEDURE VARIANTS
 */
substr(RET) ::= POST_SUBSTR_PREFIX expressions(SOURCE) POST_COMMA expressions(POS) POST_CLOSE_PARANTHESIS.
{
    char *source    = SOURCE;
    char *pos       = POS;
    int  posint;

    if (source) {
        posint = atoi(pos);
        char *target = mkSubstr(session, source, posint, 0);
        RET = target;
    } else {
        RET = mkStrdup(session->heapHandle, "0", __FILE__, __LINE__);
    }
    mkFree(session->heapHandle, SOURCE);
    mkFree(session->heapHandle, POS);
}
substr(RET) ::= POST_SUBSTR_PREFIX expressions(SOURCE) POST_COMMA expressions(POS) POST_COMMA expressions(LENGTH) POST_CLOSE_PARANTHESIS.
{
    char *source    = SOURCE;
    char *pos       = POS;
    char *lengthStr = LENGTH;
    int  posint;
    int  length;

    if (source) {
        posint = atoi(pos);
        length = atoi(lengthStr);
        char *target = mkSubstr(session, source, posint, length);
        RET = target;
    } else {
        RET = mkStrdup(session->heapHandle, "0", __FILE__, __LINE__);
    }
    mkFree(session->heapHandle, SOURCE);
    mkFree(session->heapHandle, POS);
    mkFree(session->heapHandle, LENGTH);
}

/**
 * CREATE ORACLE CONNECTION PROCEDURE VARIANTS
 */
createoracleconnection(RET) ::= POST_CREATE_ORACLE_CONNECTION_PREFIX expression(CONNECTION_NAME) POST_COMMA expression(HOST_NAME) POST_COMMA expression(INSTANCE_NAME) POST_COMMA expression(USER_NAME) POST_COMMA expression(PASSWORD) POST_CLOSE_PARANTHESIS.
{
    const char *retVal = CreateConnection(	
                                    session, 
									GetCurrentPage(session), 
									CONNECTION_NAME, 
									"oracle",
                                    HOST_NAME,
									INSTANCE_NAME, 
									USER_NAME, 
									PASSWORD);

    if (retVal) {
        RET = mkStrdup(session->heapHandle, retVal, __FILE__, __LINE__);
    } else {
        RET = mkStrdup(session->heapHandle, "", __FILE__, __LINE__);
    }

    mkFree(session->heapHandle, CONNECTION_NAME);
    mkFree(session->heapHandle, INSTANCE_NAME);
    mkFree(session->heapHandle, USER_NAME);
    mkFree(session->heapHandle, PASSWORD);
}

/**
 * CREATE ORACLE CONNECTION PROCEDURE VARIANTS
 */
createmysqlconnection(RET) ::= POST_CREATE_MYSQL_CONNECTION_PREFIX expression(CONNECTION_NAME) POST_COMMA expression(HOST_NAME) POST_COMMA expression(INSTANCE_NAME) POST_COMMA expression(USER_NAME) POST_COMMA expression(PASSWORD) POST_CLOSE_PARANTHESIS.
{
    const char *retVal = CreateConnection(
                                    session, 
									GetCurrentPage(session), 
									CONNECTION_NAME, 
									"mysql",
                                    HOST_NAME,
									INSTANCE_NAME, 
									USER_NAME, 
									PASSWORD);
    if (retVal) {
        RET = mkStrdup(session->heapHandle, retVal, __FILE__, __LINE__);
    } else {
        RET = mkStrdup(session->heapHandle, "", __FILE__, __LINE__);
    }
    mkFree(session->heapHandle, CONNECTION_NAME);
    mkFree(session->heapHandle, INSTANCE_NAME);
    mkFree(session->heapHandle, USER_NAME);
    mkFree(session->heapHandle, PASSWORD);
}

/**
* DESTROY REDIS CONNECTION PROCEDURE VARIANTS
*/
closeredisconnection ::= POST_CLOSE_REDIS_CONNECTION_PREFIX variablename(CONNECTION_NAME) POST_CLOSE_PARANTHESIS POST_SEMI_COLON.
{
	HTSQLPage *page = GetCurrentPage(session);
	__closeRedisConnection(session, page, CONNECTION_NAME);
	mkFree(session->heapHandle, CONNECTION_NAME);
}

/**
* CREATE REDIS CONNECTION PROCEDURE VARIANTS 
*/
createredisconnection(RET) ::= POST_CREATE_REDIS_CONNECTION_PREFIX expression(SERVER_NAME) POST_COMMA expression(PORT_NUMBER) POST_CLOSE_PARANTHESIS.
{
	HTSQLPage *page = GetCurrentPage(session);

	redisContext *retVal = __createRedisConnection(	session, page, SERVER_NAME, PORT_NUMBER);
	RET = retVal;

	if (SERVER_NAME)
		mkFree(session->heapHandle, SERVER_NAME);
	if (PORT_NUMBER)
		mkFree(session->heapHandle, PORT_NUMBER);
}

/**
* CREATE POSTGRESQL CONNECTION PROCEDURE VARIANTS
*/
createsqlserverconnection(RET) ::= POST_CREATE_SQLSERVER_CONNECTION_PREFIX expression(CONNECTION_NAME) POST_COMMA expression(HOST_NAME) POST_COMMA expression(INSTANCE_NAME) POST_COMMA expression(USER_NAME) POST_COMMA expression(PASSWORD) POST_CLOSE_PARANTHESIS.
{
	const char *retVal = CreateConnection(
                                    session,
									GetCurrentPage(session),
									CONNECTION_NAME,
									"sqlserver",
                                    HOST_NAME, 
                                    INSTANCE_NAME,
									USER_NAME,
									PASSWORD);
    
	if (retVal) {
		RET = mkStrdup(session->heapHandle, retVal, __FILE__, __LINE__);
	}
	else {
		RET = mkStrdup(session->heapHandle, "", __FILE__, __LINE__);
	}

	mkFree(session->heapHandle, CONNECTION_NAME);
	mkFree(session->heapHandle, INSTANCE_NAME);
	mkFree(session->heapHandle, USER_NAME);
	mkFree(session->heapHandle, PASSWORD);
}

/**
* CREATE POSTGRESQL CONNECTION PROCEDURE VARIANTS
*/
createpostgresqlconnection(RET) ::= POST_CREATE_POSTGRESQL_CONNECTION_PREFIX expression(CONNECTION_NAME) POST_COMMA expression(HOST_NAME) POST_COMMA expression(INSTANCE_NAME) POST_COMMA expression(USER_NAME) POST_COMMA expression(PASSWORD) POST_CLOSE_PARANTHESIS.
{
	const char *retVal = CreateConnection(
                                    session,
									GetCurrentPage(session),
									CONNECTION_NAME,
									"postgresql",
                                    HOST_NAME, 
                                    INSTANCE_NAME,
									USER_NAME,
									PASSWORD);

	if (retVal) {
		RET = mkStrdup(session->heapHandle, retVal, __FILE__, __LINE__);
	}
	else {
		RET = mkStrdup(session->heapHandle, "", __FILE__, __LINE__);
	}

	mkFree(session->heapHandle, CONNECTION_NAME);
	mkFree(session->heapHandle, INSTANCE_NAME);
	mkFree(session->heapHandle, USER_NAME);
	mkFree(session->heapHandle, PASSWORD);
}

/**
 *  POST_TO_JSON_PREFIX PROCEDURE VARIANTS
 */
tojson(RET)  ::= POST_TO_JSON_PREFIX variablename(RESULTSETNAME) POST_CLOSE_PARANTHESIS.
{
    HTSQLPage *page = GetCurrentPage(session);
    char *jsondata  = __sendAddTreeNodeResponse(session, page, RESULTSETNAME);
    RET             = jsondata;
    mkFree(session->heapHandle, RESULTSETNAME);
}

/**
*  RUN_DML_FILE PROCEDURE VARIANTS
*/
runsqlfile(RET) ::= POST_RUN_SQL_FILE_PREFIX expressions(A) POST_CLOSE_PARANTHESIS.
{
	HTSQLPage *page = GetCurrentPage(session);
	char *err = __runSQLFile(session, page, A);
	RET = err;
	if (A) {
		mkFree(session->heapHandle, A);
	}
}

/**
*  GET_DATABASE_INFO PROCEDURE VARIANTS
*/
getdatabasename(RET)  ::= POST_GET_DATABASE_NAME_PREFIX stringconstant(DATABASENAME) POST_CLOSE_PARANTHESIS.
{
	HTSQLPage *page = GetCurrentPage(session);
	char *dbInfo = __getDatabaseName(session, page, DATABASENAME);
	RET = dbInfo;
	mkFree(session->heapHandle, DATABASENAME);
}

/**
*  GET_DATABASE_INFO PROCEDURE VARIANTS
*/
getdatabasetype(RET)  ::= POST_GET_DATABASE_TYPE_PREFIX stringconstant(DATABASENAME) POST_CLOSE_PARANTHESIS.
{
	HTSQLPage *page = GetCurrentPage(session);
	char *dbInfo = __getDatabaseType(session, page, DATABASENAME);
	RET = dbInfo;
	mkFree(session->heapHandle, DATABASENAME);
}

/**
 * INSTR  PROCEDURE VARIANTS
 */
instr(RET) ::= POST_INSTR_PREFIX expressions(SOURCE) POST_COMMA expressions(SEARCH) POST_CLOSE_PARANTHESIS.
{
    char *source    = SOURCE;
    char *searchStr = SEARCH;

    if (source) {
        char *target = mkInstr(session, source, searchStr);
        RET = target;
    } else {
        RET = mkStrdup(session->heapHandle, "", __FILE__, __LINE__);
    }

    mkFree(session->heapHandle, SOURCE);
    mkFree(session->heapHandle, SEARCH);
}
/**
 * REPLACE  PROCEDURE VARIANTS
 */
replace(RET) ::= POST_REPLACE_PREFIX expressions(SOURCE) POST_COMMA expressions(FROM) POST_COMMA expressions(TO) POST_CLOSE_PARANTHESIS.
{
    char *source    = SOURCE;
    char *fromStr   = FROM;
    char *toStr     = TO;

    if (source) {
        char *target = mkReplace(session, source, fromStr, toStr);
        RET = target;
    } else {
        RET = mkStrdup(session->heapHandle, "", __FILE__, __LINE__);
    }

    mkFree(session->heapHandle, SOURCE);
    mkFree(session->heapHandle, FROM);
    mkFree(session->heapHandle, TO);
}
/**
 * POST_PREVIOUS_RECORD_PREFIX      PROCEDURE VARIANTS
 */
previousrecord::= POST_PREVIOUS_RECORD_PREFIX variablename(A) POST_CLOSE_PARANTHESIS POST_SEMI_COLON.
{
    HTSQLPage *page = GetCurrentPage(session);

    PostObjectResultSet *postObjectResultSet = findPostObjectGenericPostResultSetVariable(session, page, A);
    //OCI_FetchPrev(postObjectResultSet->ociResultSet);
	DBInt_Prev(session->rootTAGConnection->mkDBConnection, postObjectResultSet->ociStatement);
	mkFree(session->heapHandle, A);
}

/**
 * POST_LAST_RECORD_PREFIX      PROCEDURE VARIANTS
 */
lastrecord   ::= POST_LAST_RECORD_PREFIX variablename(A) POST_CLOSE_PARANTHESIS POST_SEMI_COLON.
{
	HTSQLPage *page = GetCurrentPage(session);
	CLOG(ERR_CORE_NOT_IMPLEMENTED, "");
    mkFree(session->heapHandle, A);
}

/**
 *  FIRST_RECORD                PROCEDURE VARIANTS
 */
firstrecord   ::= POST_FIRST_RECORD_PREFIX variablename(A) POST_CLOSE_PARANTHESIS POST_SEMI_COLON.
{
    HTSQLPage *page = GetCurrentPage(session);

    PostObjectResultSet *postObjectResultSet = findPostObjectGenericPostResultSetVariable(session, page, A);
	DBInt_Connection *mkDBConnection = session->rootTAGConnection->mkDBConnection;
	DBInt_First(mkDBConnection, postObjectResultSet->ociStatement);

    /*bool eof = OCI_FetchFirst(postObjectResultSet->ociResultSet);
    postObjectResultSet->eof = eof;*/
    mkFree(session->heapHandle, A);
}

/**
 *  NEXT_RECORD                 PROCEDURE VARIANTS
 */
nextrecord    ::= POST_NEXT_RECORD_PREFIX variablename(A) POST_CLOSE_PARANTHESIS POST_SEMI_COLON.
{
    HTSQLPage *page = GetCurrentPage(session);
    PostObjectResultSet *postObjectResultSet = findPostObjectGenericPostResultSetVariable(session, page, A);
#pragma message("builtin test safasında doğru çalışmayan işlevler var.")
	DBInt_Connection *mkDBConnection = session->rootTAGConnection->mkDBConnection;
	DBInt_Next(mkDBConnection, postObjectResultSet->ociStatement);

	/*bool eof = OCI_FetchNext(postObjectResultSet->ociResultSet);
    postObjectResultSet->eof = eof;*/
    mkFree(session->heapHandle, A);
}

/**
 *  HIDE_BLOCK PROCEDURE VARIANTS
 */
hideblock     ::= POST_HIDE_BLOCK_PREFIX expression(A) POST_CLOSE_PARANTHESIS POST_SEMI_COLON.
{
    HTSQLPage *page = GetCurrentPage(session);
    __sendHideBlockResponse(session, page, A, false);
    mkFree(session->heapHandle, A);
}
hideblock     ::= POST_HIDE_BLOCK_PREFIX expression(A) POST_COMMA expression(B) POST_CLOSE_PARANTHESIS POST_SEMI_COLON.
{
    HTSQLPage *page = GetCurrentPage(session);
    bool cascade = (strcmp("true", B) == 0);
    __sendHideBlockResponse(session, page, A, cascade);
    mkFree(session->heapHandle, A);
    mkFree(session->heapHandle, B);
}

/**
 *  SHOW_BLOCK PROCEDURE VARIANTS
 */
showblock     ::= POST_SHOW_BLOCK_PREFIX expression(A) POST_CLOSE_PARANTHESIS POST_SEMI_COLON.
{
    HTSQLPage *page = GetCurrentPage(session);
	TAGDataBlock *pTAGDataBlock = findTAGDataBlockByName(session, page, A);
    __sendShowBlockResponse(session, page, pTAGDataBlock, false);
    mkFree(session->heapHandle, A);
}
showblock     ::= POST_SHOW_BLOCK_PREFIX expression(A) POST_COMMA expression(B) POST_CLOSE_PARANTHESIS POST_SEMI_COLON.
{
    HTSQLPage *page = GetCurrentPage(session);
    bool cascade = (strcmp("true", B) == 0);
	TAGDataBlock *pTAGDataBlock = findTAGDataBlockByName(session, page, A);
    __sendShowBlockResponse(session, page, pTAGDataBlock, cascade);
    mkFree(session->heapHandle, A);
    mkFree(session->heapHandle, B);
}

/**
 *  SHOW_PAGE PROCEDURE VARIANTS
 */
showpage     ::= POST_SHOW_PAGE_PREFIX expression(A) POST_CLOSE_PARANTHESIS POST_SEMI_COLON.
{
    HTSQLPage *page = GetCurrentPage(session);
    __sendShowPageResponse(session, page, A);
    if (A) {
        mkFree(session->heapHandle, A);
    }
}


/**
 *  MESSAGE PROCEDURE VARIANTS
 */
message     ::= POST_MESSAGE_PREFIX expressions(A) POST_CLOSE_PARANTHESIS POST_SEMI_COLON.
{
	HTSQLPage *page = GetCurrentPage(session);
	__sendMessageResponse(session, page, A);
    if (A) {
        mkFree(session->heapHandle, A);
    }
}

/**
 *  MESSAGE PROCEDURE VARIANTS
 */
message     ::= POST_MESSAGE_PREFIX expressions(TITLE) POST_COMMA expressions(MESSAGE) POST_CLOSE_PARANTHESIS POST_SEMI_COLON.
{
	HTSQLPage *page = GetCurrentPage(session);
	__sendMessage3Response(session, page, TITLE, MESSAGE, "info");
    
    if (TITLE) {
        mkFree(session->heapHandle, TITLE);
    }
    if (MESSAGE) {
        mkFree(session->heapHandle, MESSAGE);
    }
}

/**
 *  MESSAGE PROCEDURE VARIANTS
 */
message     ::= POST_MESSAGE_PREFIX expressions(TITLE) POST_COMMA expressions(MESSAGE) POST_COMMA expressions(TYPE) POST_CLOSE_PARANTHESIS POST_SEMI_COLON.
{
	HTSQLPage *page = GetCurrentPage(session);
	__sendMessage3Response(session, page, TITLE, MESSAGE, TYPE);
    
    if (TITLE) {
        mkFree(session->heapHandle, TITLE);
    }
    if (MESSAGE) {
        mkFree(session->heapHandle, MESSAGE);
    }
    if (TYPE) {
        mkFree(session->heapHandle, TYPE);
    }
}


refreshtreenode ::= POST_REFRESH_TREE_NODE_PREFIX stringconstant(TREENAME) POST_COMMA expressions(NODEID) POST_CLOSE_PARANTHESIS POST_SEMI_COLON.
{
    char *treename = TREENAME;
    char *nodeid   = NODEID;
    HTSQLPage *page = GetCurrentPage(session);
    __sendRefreshTreeNodeResponse(session, page, treename, nodeid);
    mkFree(session->heapHandle, treename);
    mkFree(session->heapHandle, nodeid);
}


/**
 *  REFRESH BLOCK PROCEDURE VARIANTS
 */
refreshblock ::= POST_REFRESH_BLOCK_PREFIX expression(BLOCK_NAME) POST_CLOSE_PARANTHESIS POST_SEMI_COLON.
{
    HTSQLPage *page = GetCurrentPage(session);
	TAGDataBlock *dataBlockInstance = findTAGDataBlockByName(session, page, BLOCK_NAME);

    if (page->currentRequestMethodType == REQUEST_TYPE_GET) {
        __sendRefreshBlockNativeResponse(session, page, dataBlockInstance, NULL);
    } else {
        __sendRefreshBlockHTSQLResponse(session, page, BLOCK_NAME);
    }
    mkFree(session->heapHandle, BLOCK_NAME);
}
refreshblock ::= POST_REFRESH_BLOCK_PREFIX expression(BLOCK_NAME) POST_COMMA expressions(WHERECLAUSE) POST_CLOSE_PARANTHESIS POST_SEMI_COLON.
{
    HTSQLPage *page = GetCurrentPage(session);
    TAGDataBlock *dataBlock = findTAGDataBlockByName(session, page, BLOCK_NAME);
	char *whereClause = WHERECLAUSE;

    if (dataBlock == NULL) {
        CLOG(ERR_NAME_IS_NOT_FOUND, BLOCK_NAME);
        return;
    }
    setTAGDataBlockPropertyWhereClause(session, page, dataBlock, whereClause);

    if (page->currentRequestMethodType == REQUEST_TYPE_GET) {
        __sendRefreshBlockNativeResponse(session, page, dataBlock, NULL);
    } else {
        __sendRefreshBlockHTSQLResponse(session, page, BLOCK_NAME);
    }
    mkFree(session->heapHandle, BLOCK_NAME);
    mkFree(session->heapHandle, whereClause);
}


/**
*  SET_DATABLOCK_PROPERTY  PROCEDURE VARIANTS
*/
setdatablockproperty ::= POST_SET_DATABLOCK_PROPERTY_PREFIX expression(BLOCK_NAME) POST_COMMA expressions(PROPERTY_NAME) POST_COMMA expressions(PROPERTY_VALUE_SCHEMA_NAME) POST_COMMA expressions(PROPERTY_VALUE_SOURCE_NAME) POST_CLOSE_PARANTHESIS POST_SEMI_COLON.
{
	HTSQLPage *page = GetCurrentPage(session);
	TAGDataBlock *dataBlockInstance = findTAGDataBlockByName(session, page, BLOCK_NAME);
	if (dataBlockInstance) {
		if (strcmp(PROPERTY_NAME, "data-source-name") == 0) {
			setCurrentTAGDataBlockPropertyDataSchemaSourceName(session, page, dataBlockInstance, PROPERTY_VALUE_SCHEMA_NAME, PROPERTY_VALUE_SOURCE_NAME);
		}
		else {
			CLOG(ERR_PROPERTY_INVALID, PROPERTY_NAME);
		}
	}
	else {
		CLOG(ERR_DATABLOCK_NOT_FOUND, BLOCK_NAME);
	}
	mkFree(session->heapHandle, BLOCK_NAME);
	mkFree(session->heapHandle, PROPERTY_NAME);
	mkFree(session->heapHandle, PROPERTY_VALUE_SCHEMA_NAME);
	mkFree(session->heapHandle, PROPERTY_VALUE_SOURCE_NAME);
}


/**
 *  SHOW ITEM PROCEDURE VARIANTS
 */
showcolumn    ::= POST_SHOW_COLUMN_PREFIX expression(A) POST_CLOSE_PARANTHESIS POST_SEMI_COLON.
{
    HTSQLPage *page = GetCurrentPage(session);
    __showColumn(session, page, A);
    if (A) {
        mkFree(session->heapHandle, A);
    }
}
/**
 *  HIDE ITEM PROCEDURE VARIANTS
 */
hidecolumn    ::= POST_HIDE_COLUMN_PREFIX expression(A) POST_CLOSE_PARANTHESIS POST_SEMI_COLON.
{
    HTSQLPage *page = GetCurrentPage(session);
    __hideColumn(session, page, A);
    if (A) {
        mkFree(session->heapHandle, A);
    }
}

/**
 *  DISABLE ITEM PROCEDURE VARIANTS
 */
disablecolumn   ::= POST_DISABLE_COLUMN_PREFIX expression(A) POST_CLOSE_PARANTHESIS POST_SEMI_COLON.
{
    HTSQLPage *page = GetCurrentPage(session);
    __disableColumn(session, page, A);
    if (A) {
        mkFree(session->heapHandle, A);
    }
}
/**
 *  ENABLE ITEM PROCEDURE VARIANTS
 */
enablecolumn   ::= POST_ENABLE_COLUMN_PREFIX expression(A) POST_CLOSE_PARANTHESIS POST_SEMI_COLON.
{
    HTSQLPage *page = GetCurrentPage(session);
    __enableColumn(session, page, A);
    mkFree(session->heapHandle, A);
}

/**
 *  PROMPT PROCEDURE VARIANTS
 */
prompt       ::= POST_PROMPT_PREFIX expressions(A) POST_COMMA expressions(B) POST_CLOSE_PARANTHESIS POST_SEMI_COLON.
{
    __prompt(session, A, B);
    mkFree(session->heapHandle, A);
    mkFree(session->heapHandle, B);
}

/**
 *  POPULATE_DATALIST PROCEDURE VARIANTS
 */
pupulatedatalist  ::= POST_POPULATE_DATALIST_PREFIX stringconstant(A) POST_CLOSE_PARANTHESIS POST_SEMI_COLON.
{
     __sendPopulateDatalistResponse(session, GetCurrentPage(session), A, false);
    mkFree(session->heapHandle, A);
}
pupulatedatalist  ::= POST_POPULATE_DATALIST_PREFIX stringconstant(A) POST_COMMA condition_options(CASCADE_OPTION) POST_CLOSE_PARANTHESIS POST_SEMI_COLON.
{
	__sendPopulateDatalistResponse(session, GetCurrentPage(session), A, CASCADE_OPTION);
	mkFree(session->heapHandle, A);
}
pupulatedatalist  ::= POST_POPULATE_DATALIST_PREFIX stringconstant(A) POST_COMMA variablename(RECORDSETNAME) POST_CLOSE_PARANTHESIS POST_SEMI_COLON.
{
	__sendPopulateDatalistWithRecordSetResponse(session, GetCurrentPage(session), A, RECORDSETNAME);
	mkFree(session->heapHandle, A);
}



/**
 *  POPULATE_TREE PROCEDURE VARIANTS
 */
pupulatetree  ::= POST_POPULATE_TREE_PREFIX stringconstant(A) POST_CLOSE_PARANTHESIS POST_SEMI_COLON.
{
     __sendPopulateTreeResponse(session, GetCurrentPage(session), A);
    mkFree(session->heapHandle, A);
}





/**
 *  ROLLBACK PROCEDURE VARIANTS
 */
rollback    ::= POST_ROLLBACK_PREFIX  POST_CLOSE_PARANTHESIS POST_SEMI_COLON.
{
    __rollback(session, GetCurrentPage(session), NULL);
}
rollback    ::= POST_ROLLBACK_PREFIX  expressions(A) POST_CLOSE_PARANTHESIS POST_SEMI_COLON.
{
    __rollback(session, GetCurrentPage(session), A);
    mkFree(session->heapHandle, A);
}

/**
 *  COMMIT PROCEDURE VARIANTS
 */
commit      ::= POST_COMMIT_PREFIX  POST_CLOSE_PARANTHESIS POST_SEMI_COLON.
{
    __commit(session, GetCurrentPage(session), NULL);
}
commit      ::= POST_COMMIT_PREFIX expressions(A) POST_CLOSE_PARANTHESIS POST_SEMI_COLON.
{
    __commit(session, GetCurrentPage(session), A);
    mkFree(session->heapHandle, A);
}

/**
 *  DELETE PROCEDURE VARIANTS
 */
delete      ::= POST_DELETE.
{
    HTSQLPage *page = GetCurrentPage(session);
    __delete(session,
             page,
             getPostCurrentTAGDataBlock(session, page));
}

/**  CALLER MUST FREE RESULT  */
selectstatement(RET) ::= dmlcommand(A).
{
    char *value = mkStrcat(session->heapHandle, __FILE__, __LINE__,
							"select ",
							A,
							NULL
						  );
    RET = value;
	mkFree(session->heapHandle, A);
}

/**
 *  IMPORTANT:
 *  1.  ALL expression SHOULD DUBLICATE THEIR VALUE IF THEIR VALUE IS NOT NULL.
 *  2.  ALL expression RESULT TYPE SHOULD BE char *.
 *  3.  All consumers of EXPRESSION should free return value. Return value can be NULL. So, test its value first, then free it.
 */
expressions(RET) ::= expressions(A) POST_AMPERSAND expression(B).
{
    char *temp = NULL;
    char *p1 = A;
    char *p2 = B;

    if (p1 && p2) {
        temp = mkStrcat(session->heapHandle, __FILE__, __LINE__, p1, p2, NULL);
    } else if (p1) {
        temp = mkStrdup(session->heapHandle, p1, __FILE__, __LINE__);
    } else if (p2) {
        temp = mkStrdup(session->heapHandle, p2, __FILE__, __LINE__);
    } else {
        // NULL concat with NULL
        temp = NULL;
    }

    RET = temp;
    if (p1) {
        mkFree(session->heapHandle, p1);
    }
    if (p2) {
        mkFree(session->heapHandle, p2);
    }
}
expressions(RET) ::= expressions(A) POST_PLUS expressions(B).
{
    char *p1 = A;
    char *p2 = B;
    char *a, *b;

    HTSQLPage *page = GetCurrentPage(session);

    PreObjectGeneric *aObj = findPreGenericObjectByName(session, page, p1);
    if (aObj) {
        a = aObj->value.preObjectVariable->value.text;
    } else {
        a = A;
    }
    PreObjectGeneric *bObj = findPreGenericObjectByName(session, page, p2);
    if (bObj) {
        b = bObj->value.preObjectVariable->value.text;
    } else {
        b = B;
    }
    double inta = atof(a);
    double intb = atof(b);

    char *retval = mkMalloc(session->heapHandle, 50, __FILE__, __LINE__);
    mkDoubleToChar(inta + intb, retval);
    RET = retval;

    mkFree(session->heapHandle, p1);
    if (p2) {
        mkFree(session->heapHandle, p2);
    }
}
expressions(RET) ::= expressions(A) POST_MODULAR_DIVISION expressions(B).
{
    char *p1 = A;
    char *p2 = B;
    char *a, *b;
    HTSQLPage *page = GetCurrentPage(session);

    PreObjectGeneric *aObj = findPreGenericObjectByName(session, page, p1);
    if (aObj) {
        a = aObj->value.preObjectVariable->value.text;
    } else {
        a = A;
    }
    PreObjectGeneric *bObj = findPreGenericObjectByName(session, page, p2);
    if (bObj) {
        b = bObj->value.preObjectVariable->value.text;
    } else {
        b = B;
    }
    double inta = atof(a);
	double intb = atof(b);

    char *retval = mkMalloc(session->heapHandle, 50, __FILE__, __LINE__);
    mkDoubleToChar( ((int)inta) % ((int)intb), retval);
    RET = retval;

    mkFree(session->heapHandle, p1);
    if (p2) {
        mkFree(session->heapHandle, p2);
    }
}
expressions(RET) ::= expressions(A) POST_MULTIPLY expressions(B).
{
    char *p1 = A;
    char *p2 = B;
    char *a, *b;
    HTSQLPage *page = GetCurrentPage(session);

    PreObjectGeneric *aObj = findPreGenericObjectByName(session, page, p1);
    if (aObj) {
        a = aObj->value.preObjectVariable->value.text;
    } else {
        a = p1;
    }
    PreObjectGeneric *bObj = findPreGenericObjectByName(session, page, p2);
    if (bObj) {
        b = bObj->value.preObjectVariable->value.text;
    } else {
        b = p2;
    }
    double inta = atof(a);
    double intb = atof(b);

    char *retval = mkMalloc(session->heapHandle, 50, __FILE__, __LINE__);
    mkDoubleToChar(inta * intb, retval);
    RET = retval;

    mkFree(session->heapHandle, p1);
    mkFree(session->heapHandle, p2);
}
expressions(RET) ::= expressions(A) POST_DIVISION expressions(B).
{
    char *p1 = A;
    char *p2 = B;
    char *a, *b;
    HTSQLPage *page = GetCurrentPage(session);
    PreObjectGeneric *aObj = findPreGenericObjectByName(session, page, p1);
    if (aObj) {
        a = aObj->value.preObjectVariable->value.text;
    } else {
        a = p1;
    }
    PreObjectGeneric *bObj = findPreGenericObjectByName(session, page, p2);
    if (bObj) {
        b = bObj->value.preObjectVariable->value.text;
    } else {
        b = p2;
    }
    double inta = atof(a);
    double intb = atof(b);

    char *retval = mkMalloc(session->heapHandle, 50, __FILE__, __LINE__);
    mkDoubleToChar(inta / intb, retval);
    RET = retval;

    mkFree(session->heapHandle, p1);
    mkFree(session->heapHandle, p2);
}
expressions(RET) ::= expressions(A) POST_SUBTRACT expressions(B).
{
    char *p1 = A;
    char *p2 = B;
    char *a, *b;
    HTSQLPage *page = GetCurrentPage(session);
    PreObjectGeneric *aObj = findPreGenericObjectByName(session, page, p1);
    if (aObj) {
        a = aObj->value.preObjectVariable->value.text;
    } else {
        a = p1;
    }
    PreObjectGeneric *bObj = findPreGenericObjectByName(session, page, p2);
    if (bObj) {
        b = bObj->value.preObjectVariable->value.text;
    } else {
        b = p2;
    }
    double  inta = atof(a);
    double intb = atof(b);

    char *retval = mkMalloc(session->heapHandle, 50, __FILE__, __LINE__);
    mkDoubleToChar(inta - intb, retval);
    RET = retval;

    mkFree(session->heapHandle, p1);
    mkFree(session->heapHandle, p2);
}
expressions(RET) ::= POST_OPEN_PARANTHESIS expressions(A) POST_CLOSE_PARANTHESIS.
{
    RET = A;
}

expressions ::= expression.

expression  ::= stringconstant.         /** read only */
expression  ::= numberconstant.
expression  ::= sessionid.              /** read only */
expression  ::= rowid.                  /** read only */
expression  ::= blockItemValue.
expression  ::= resultsetcolumnvalue.   /** read only */
expression  ::= variablevalue.
expression  ::= replace.
expression  ::= instr.
expression  ::= tojson.
expression  ::= runsqlfile.
expression  ::= getdatabasename.
expression  ::= getdatabasetype.
expression  ::= strlen.
expression  ::= sizeof.
expression  ::= substr.
expression  ::= posttrue.
expression  ::= postfalse.
expression  ::= postnull.
expression  ::= createoracleconnection.
expression  ::= createpostgresqlconnection.
expression  ::= createmysqlconnection.
expression  ::= createsqlserverconnection.
expression  ::= rediscommands.
expression  ::= macroline.

expression(RET)  ::= userfunctioncall(A).
{
    char *retval = A;
	HTSQLPage *page = GetCurrentPage(session);

    if (page->currentCallStack == NULL) {
        // Fonksiyon yok veya calistirilamadığı için dönüş değer elde edilemedi
		CLOG(ERR_NAME_IS_NOT_FOUND, retval);
    } else if (retval == NULL || page->currentCallStack->preObjectFunction->preObjectFunctionReturnValueDeclaration && page->currentCallStack->preObjectFunctionReturnValue == NULL) {
		CLOG(ERR_FUNCTION_MUST_RETURN_VALUE, "");
    }

    RET = retval;
}

posttrue(RET)   ::= POST_TRUE(A).
{
    char *value = mkMalloc(session->heapHandle, A.tokenStrLength+1, __FILE__, __LINE__);
    strncpy_s(value, A.tokenStrLength + 1, A.tokenStr, A.tokenStrLength);
    value[A.tokenStrLength] = '\0';
    RET = value;
}
postfalse(RET)  ::= POST_FALSE(A).
{
    char *value = mkMalloc(session->heapHandle, A.tokenStrLength+1, __FILE__, __LINE__);
    strncpy_s(value, A.tokenStrLength + 1, A.tokenStr, A.tokenStrLength);
    value[A.tokenStrLength] = '\0';
    RET = value;
}

postnull(RET)   ::= POST_NULL.
{
    RET = NULL;
}
/**
 *  SHARED NON-TERMINALS
 */

/**  CALLER MUST FREE RESULT  */
variablename(RET) ::= POST_IDENTIFIER(A).
{
	char *a = A.tokenStr;
	int b = A.tokenStrLength;

    char *value = mkMalloc(session->heapHandle, A.tokenStrLength+1, __FILE__, __LINE__);
    strncpy_s(value, A.tokenStrLength + 1, A.tokenStr, A.tokenStrLength);
    value[A.tokenStrLength] = '\0';
    RET = value;
}

/**  CALLER MUST FREE RESULT  */
variablevalue(RET) ::= POST_IDENTIFIER(A).
{
	HTSQLPage *page = GetCurrentPage(session);

    char *value = mkMalloc(session->heapHandle, A.tokenStrLength+1, __FILE__, __LINE__);
    strncpy_s(value, A.tokenStrLength + 1, A.tokenStr, A.tokenStrLength);
    value[A.tokenStrLength] = '\0';

	char *retval = findAndReturnValueOfAVariableInAllScopes(session, page, NULL, value);
    if (retval) {
        RET = mkStrdup(session->heapHandle, retval, __FILE__, __LINE__);
    } else {
		RET = NULL;// mkStrdup(session->heapHandle, "", __FILE__, __LINE__);
    }
    mkFree(session->heapHandle, retval);
    mkFree(session->heapHandle, value);
}

/**  CALLER MUST FREE RESULT  */
/*
stringconstant(RET) ::= POST_STRING_CONSTANT(A).
{
    char *value = mkMalloc(session->heapHandle, A.tokenStrLength+1, __FILE__, __LINE__);
    strncpy(value, A.tokenStr, A.tokenStrLength);
    value[A.tokenStrLength] = '\0';

    if (value[strlen(value)-1] == '\'') {
        value[strlen(value)-1] = '\0';
    }
    if (value[0] == '\'') {
        strcpy(value, &value[1]);
    }

    RET = value;//stripTAGPropertyValue(value);
} */

dmlcommand(RET) ::= POST_DML_START dmlchars(A) POST_DML_END.
{
	char *a = A;
	RET = a;
}
dmlchars(RET)	::= dmlchars(A) dmlchar(B).
{
	char *a = A;
	char *b = B;
	char *tmp;

	if (a) {
		tmp = mkStrcat(session->heapHandle, __FILE__, __LINE__, a, b, NULL);
	}
	else {
		tmp = B;
	}
	RET = tmp;
	mkFree(session->heapHandle, a);
	mkFree(session->heapHandle, b);
}
dmlchars    ::= dmlchar.
dmlchar(RET)::= POST_DML_CHAR(C).
{
	TokenPost token = C;

	char *sp = mkStrdup(session->heapHandle, "  ", __FILE__, __LINE__);

	sp[0] = token.tokenStr[0];
	sp[1] = '\0';

	RET = sp;
}

stringconstant(RET) ::= POST_STRING_CONSTANT_START POST_STRING_CONSTANT_END.
{
	// empty string
	RET = mkStrdup(session->heapHandle, "", __FILE__, __LINE__);
}

stringconstant(RET) ::= POST_STRING_CONSTANT_START stringchars(A) POST_STRING_CONSTANT_END.
{
    char *a = A;
    RET     = a;
}
stringchars(RET)   ::= stringchars(A) stringchar(B).
{
    char *a = A;
    char *b = B;
    char *tmp;

    if (a) {
        tmp = mkStrcat(session->heapHandle, __FILE__, __LINE__,  a, b, NULL);
    } else {
        tmp = B;
    }
    RET = tmp;
    mkFree(session->heapHandle, a);
    mkFree(session->heapHandle, b);
}
stringchars    ::= stringchar.

stringchar(RET)::= POST_STRING_CONSTANT_APOSTROPHE.
{
    char *sp = mkStrdup(session->heapHandle, "'", __FILE__, __LINE__);
    RET = sp;
}
stringchar(RET)::= POST_STRING_CONSTANT_CHAR(C).
{
    TokenPost token = C;

    char *sp = mkStrdup(session->heapHandle, "  ", __FILE__, __LINE__);

    sp[0] = token.tokenStr[0];
    sp[1] = '\0';

    RET =  sp;
}


numberconstant(RET) ::= POST_NUMBER_CONSTANT(A).
{
    char *value = mkMalloc(session->heapHandle, A.tokenStrLength+1, __FILE__, __LINE__);
    strncpy_s(value, A.tokenStrLength + 1, A.tokenStr, A.tokenStrLength);
    value[A.tokenStrLength] = '\0';

    double dblval = atof(value);

    char *retval = mkMalloc(session->heapHandle, 50, __FILE__, __LINE__);
    mkDoubleToChar(dblval, retval);
    RET = retval;
    mkFree(session->heapHandle, value);
}
numberconstant(RET) ::= POST_SUBTRACT POST_NUMBER_CONSTANT(A).
{
    char *value = mkMalloc(session->heapHandle, A.tokenStrLength+1, __FILE__, __LINE__);
    strncpy_s(value, A.tokenStrLength + 1, A.tokenStr, A.tokenStrLength);
    value[A.tokenStrLength] = '\0';

    double dblval = atof(value);
    dblval = dblval * -1;

    char *retval = mkMalloc(session->heapHandle, 50, __FILE__, __LINE__);
    mkDoubleToChar(dblval, retval);
    RET = retval;
    mkFree(session->heapHandle, value);
}

/**  CALLER MUST FREE RESULT  */
sessionid(RET) ::= POST_COLON   POST_SESSION_ID.
{
    RET = mkStrdup(session->heapHandle, session->id, __FILE__, __LINE__);
}

rowid(ROWID)     ::= POST_COLON  POST_ROW_ID(A).
{
    HTSQLPage *page = GetCurrentPage(session);

    char *value = mkMalloc(session->heapHandle, A.tokenStrLength+1, __FILE__, __LINE__);
    strncpy_s(value, A.tokenStrLength + 1, A.tokenStr, A.tokenStrLength);
    value[A.tokenStrLength] = '\0';
    ROWID = stripTAGPropertyValue(value);

    PreObjectVariable *object = findPostObjectGenericPostObjectVariable (session, page, ROWID);
    mkFree(session->heapHandle, ROWID);
    if (object && object->value.text) {
        ROWID = mkStrdup(session->heapHandle, object->value.text, __FILE__, __LINE__);
    } else {
        ROWID = mkMalloc(session->heapHandle, 10, __FILE__, __LINE__);
    }
}

resultsetcolumnvalue(RESULTSET_COLUMN_VALUE) ::= POST_IDENTIFIER(RESULTSET_NAME) POST_PERIOD POST_IDENTIFIER(RESULTSET_COLUMN_NAME).
{
    HTSQLPage *page = GetCurrentPage(session);
    RESULTSET_COLUMN_VALUE = NULL;

    char *valueA = mkMalloc(session->heapHandle, RESULTSET_NAME.tokenStrLength+1, __FILE__, __LINE__);
    strncpy_s(valueA, RESULTSET_NAME.tokenStrLength + 1, RESULTSET_NAME.tokenStr, RESULTSET_NAME.tokenStrLength);
    valueA[RESULTSET_NAME.tokenStrLength] = '\0';

    char *valueB = mkMalloc(session->heapHandle, RESULTSET_COLUMN_NAME.tokenStrLength+1, __FILE__, __LINE__);
    strncpy_s(valueB, RESULTSET_COLUMN_NAME.tokenStrLength + 1, RESULTSET_COLUMN_NAME.tokenStr, RESULTSET_COLUMN_NAME.tokenStrLength);
    valueB[RESULTSET_COLUMN_NAME.tokenStrLength] = '\0';

    PostObjectResultSet *postObjectResultSet = findPostObjectGenericPostResultSetVariable(session, page, valueA);

    if (postObjectResultSet) {
		TAGConnection *rootConnection = getActiveTAGConnection(session, page);
        SODIUM_DATABASE_COLUMN_TYPE colType = DBInt_GetColumnType(rootConnection->mkDBConnection, postObjectResultSet->ociStatement, valueB);
		if (colType == HTSQL_COLUMN_TYPE_TEXT || colType == HTSQL_COLUMN_TYPE_NUMBER) {
			const char *retval = DBInt_GetColumnValueByColumnName(rootConnection->mkDBConnection, postObjectResultSet->ociStatement, valueB);
			RESULTSET_COLUMN_VALUE = mkStrdup(session->heapHandle, retval, __FILE__, __LINE__);
		}
		else {
			// Lob column is fetched
			DWORD sizeOfValue;
			void *retVal = DBInt_GetLob(rootConnection->mkDBConnection, postObjectResultSet->ociStatement, valueB, &sizeOfValue, true);
			RESULTSET_COLUMN_VALUE = retVal;
		}
		
    } else {
        RESULTSET_COLUMN_VALUE = mkMalloc(session->heapHandle, 1, __FILE__, __LINE__);
		CLOG(ERR_NAME_IS_NOT_FOUND, valueA);
    }
    mkFree(session->heapHandle, valueA);
    mkFree(session->heapHandle, valueB);
}

rediscommands ::= rediscommandPing.
rediscommands ::= rediscommandSet.
rediscommands ::= rediscommandGet.
rediscommands ::= rediscommandDel.
rediscommands ::= rediscommandIncr.
rediscommands ::= rediscommandDecr.
rediscommands ::= rediscommandIncrBy.
rediscommands ::= rediscommandDecrBy.
rediscommands ::= rediscommandAppend.
rediscommands ::= rediscommandStrlen.

rediscommandStrlen(RET)	::= POST_IDENTIFIER(REDIS_VARIABLE_NAME) POST_REDIS_STRLEN_PREFIX expressions(KEYNAME) POST_CLOSE_PARANTHESIS.
{
	RET = NULL;
	HTSQLPage *page = GetCurrentPage(session);

	char *redVarName = mkMalloc(session->heapHandle, REDIS_VARIABLE_NAME.tokenStrLength + 1, __FILE__, __LINE__);
	strncpy_s(redVarName, REDIS_VARIABLE_NAME.tokenStrLength + 1, REDIS_VARIABLE_NAME.tokenStr, REDIS_VARIABLE_NAME.tokenStrLength);
	redVarName[REDIS_VARIABLE_NAME.tokenStrLength] = '\0';

	char *keyName = KEYNAME;

	PreObjectVariable *preObjectVariable = findRedisPreObjectVariable(session, page, redVarName, true);
	if (preObjectVariable) {
		if (preObjectVariable->value.redisContextValue) {
			RET = redisStrlen(session, page, preObjectVariable->value.redisContextValue, keyName);
		}
	}
	else {
		CLOG(ERR_NAME_IS_NOT_FOUND, redVarName);
		RET = mkStrdup(session->heapHandle, "", __FILE__, __LINE__);

	}
	mkFree(session->heapHandle, redVarName);
	mkFree(session->heapHandle, keyName);
}


rediscommandAppend(RET) ::= POST_IDENTIFIER(REDIS_VARIABLE_NAME) POST_REDIS_APPEND_PREFIX expressions(KEYNAME) POST_COMMA expressions(KEYVALUE) POST_CLOSE_PARANTHESIS.
{
	RET = NULL;
	HTSQLPage *page = GetCurrentPage(session);

	char *valueA = mkMalloc(session->heapHandle, REDIS_VARIABLE_NAME.tokenStrLength + 1, __FILE__, __LINE__);
	strncpy_s(valueA, REDIS_VARIABLE_NAME.tokenStrLength + 1, REDIS_VARIABLE_NAME.tokenStr, REDIS_VARIABLE_NAME.tokenStrLength);
	valueA[REDIS_VARIABLE_NAME.tokenStrLength] = '\0';

	char *keyName = KEYNAME;
	char *keyValue = KEYVALUE;

	PreObjectVariable *preObjectVariable = findRedisPreObjectVariable(session, page, valueA, true);
	if (preObjectVariable) {
		if (preObjectVariable->value.redisContextValue) {
			RET = redisAppend(session, page, preObjectVariable->value.redisContextValue, keyName, keyValue);
		}
	}
	else {
		CLOG(ERR_NAME_IS_NOT_FOUND, valueA);
		RET = mkStrdup(session->heapHandle, "", __FILE__, __LINE__);

	}
	mkFree(session->heapHandle, valueA);
	mkFree(session->heapHandle, keyName);
	mkFree(session->heapHandle, keyValue);
}

rediscommandIncr(RET) ::= POST_IDENTIFIER(REDIS_VARIABLE_NAME) POST_REDIS_INCR_PREFIX expressions(KEYNAME) POST_CLOSE_PARANTHESIS.
{
	RET = NULL;
	HTSQLPage *page = GetCurrentPage(session);

	char *redVarName = mkMalloc(session->heapHandle, REDIS_VARIABLE_NAME.tokenStrLength + 1, __FILE__, __LINE__);
	strncpy_s(redVarName, REDIS_VARIABLE_NAME.tokenStrLength + 1, REDIS_VARIABLE_NAME.tokenStr, REDIS_VARIABLE_NAME.tokenStrLength);
	redVarName[REDIS_VARIABLE_NAME.tokenStrLength] = '\0';

	char *keyName = KEYNAME;

	PreObjectVariable *preObjectVariable = findRedisPreObjectVariable(session, page, redVarName, true);
	if (preObjectVariable) {
		if (preObjectVariable->value.redisContextValue) {
			RET = redisIncr(session, page, preObjectVariable->value.redisContextValue, keyName);
		}
	}
	else {
		CLOG(ERR_NAME_IS_NOT_FOUND, redVarName);
		RET = mkStrdup(session->heapHandle, "", __FILE__, __LINE__);

	}
	mkFree(session->heapHandle, redVarName);
	mkFree(session->heapHandle, keyName);
}

rediscommandIncrBy(RET) ::= POST_IDENTIFIER(REDIS_VARIABLE_NAME) POST_REDIS_INCRBY_PREFIX expressions(KEYNAME) POST_COMMA expressions(BYVALUE) POST_CLOSE_PARANTHESIS.
{
	RET = NULL;
	HTSQLPage *page = GetCurrentPage(session);

	char *redVarName = mkMalloc(session->heapHandle, REDIS_VARIABLE_NAME.tokenStrLength + 1, __FILE__, __LINE__);
	strncpy_s(redVarName, REDIS_VARIABLE_NAME.tokenStrLength + 1, REDIS_VARIABLE_NAME.tokenStr, REDIS_VARIABLE_NAME.tokenStrLength);
	redVarName[REDIS_VARIABLE_NAME.tokenStrLength] = '\0';

	char *keyName = KEYNAME;
	char *byValue = BYVALUE;

	PreObjectVariable *preObjectVariable = findRedisPreObjectVariable(session, page, redVarName, true);
	if (preObjectVariable) {
		if (preObjectVariable->value.redisContextValue) {
			RET = redisIncrBy(session, page, preObjectVariable->value.redisContextValue, keyName, byValue);
		}
	}
	else {
		CLOG(ERR_NAME_IS_NOT_FOUND, redVarName);
		RET = mkStrdup(session->heapHandle, "", __FILE__, __LINE__);

	}
	mkFree(session->heapHandle, redVarName);
	mkFree(session->heapHandle, keyName);
}

rediscommandDecr(RET) ::= POST_IDENTIFIER(REDIS_VARIABLE_NAME) POST_REDIS_DECR_PREFIX expressions(KEYNAME) POST_CLOSE_PARANTHESIS.
{
	RET = NULL;
	HTSQLPage *page = GetCurrentPage(session);

	char *redVarName = mkMalloc(session->heapHandle, REDIS_VARIABLE_NAME.tokenStrLength + 1, __FILE__, __LINE__);
	strncpy_s(redVarName, REDIS_VARIABLE_NAME.tokenStrLength + 1, REDIS_VARIABLE_NAME.tokenStr, REDIS_VARIABLE_NAME.tokenStrLength);
	redVarName[REDIS_VARIABLE_NAME.tokenStrLength] = '\0';

	char *keyName = KEYNAME;

	PreObjectVariable *preObjectVariable = findRedisPreObjectVariable(session, page, redVarName, true);
	if (preObjectVariable) {
		if (preObjectVariable->value.redisContextValue) {
			RET = redisDecr(session, page, preObjectVariable->value.redisContextValue, keyName);
		}
	}
	else {
		CLOG(ERR_NAME_IS_NOT_FOUND, redVarName);
		RET = mkStrdup(session->heapHandle, "", __FILE__, __LINE__);

	}
	mkFree(session->heapHandle, redVarName);
	mkFree(session->heapHandle, keyName);
}

rediscommandDecrBy(RET) ::= POST_IDENTIFIER(REDIS_VARIABLE_NAME) POST_REDIS_DECRBY_PREFIX expressions(KEYNAME) POST_COMMA expressions(BYVALUE) POST_CLOSE_PARANTHESIS.
{
	RET = NULL;
	HTSQLPage *page = GetCurrentPage(session);

	char *redVarName = mkMalloc(session->heapHandle, REDIS_VARIABLE_NAME.tokenStrLength + 1, __FILE__, __LINE__);
	strncpy_s(redVarName, REDIS_VARIABLE_NAME.tokenStrLength + 1, REDIS_VARIABLE_NAME.tokenStr, REDIS_VARIABLE_NAME.tokenStrLength);
	redVarName[REDIS_VARIABLE_NAME.tokenStrLength] = '\0';

	char *keyName = KEYNAME;
	char *byValue = BYVALUE;

	PreObjectVariable *preObjectVariable = findRedisPreObjectVariable(session, page, redVarName, true);
	if (preObjectVariable) {
		if (preObjectVariable->value.redisContextValue) {
			RET = redisDecrBy(session, page, preObjectVariable->value.redisContextValue, keyName, byValue);
		}
	}
	else {
		CLOG(ERR_NAME_IS_NOT_FOUND, redVarName);
		RET = mkStrdup(session->heapHandle, "", __FILE__, __LINE__);

	}
	mkFree(session->heapHandle, redVarName);
	mkFree(session->heapHandle, keyName);
}

rediscommandGet(RET) ::= POST_IDENTIFIER(REDIS_VARIABLE_NAME) POST_REDIS_GET_PREFIX expressions(KEYNAME) POST_CLOSE_PARANTHESIS.
{
	RET = NULL;
	HTSQLPage *page = GetCurrentPage(session);

	char *redVarName = mkMalloc(session->heapHandle, REDIS_VARIABLE_NAME.tokenStrLength + 1, __FILE__, __LINE__);
	strncpy_s(redVarName, REDIS_VARIABLE_NAME.tokenStrLength + 1, REDIS_VARIABLE_NAME.tokenStr, REDIS_VARIABLE_NAME.tokenStrLength);
	redVarName[REDIS_VARIABLE_NAME.tokenStrLength] = '\0';

	char *keyName = KEYNAME;

	PreObjectVariable *preObjectVariable = findRedisPreObjectVariable(session, page, redVarName, true);
	if (preObjectVariable) {
		if (preObjectVariable->value.redisContextValue) {
			RET = redisGet(session, page, preObjectVariable->value.redisContextValue, keyName);
		}
	}
	else {
		CLOG(ERR_NAME_IS_NOT_FOUND, redVarName);
		RET = mkStrdup(session->heapHandle, "", __FILE__, __LINE__);

	}
	mkFree(session->heapHandle, redVarName);
	mkFree(session->heapHandle, keyName);
}

/* redis command:  DEL */
rediscommandDel(RET) ::= POST_IDENTIFIER(REDIS_VARIABLE_NAME) POST_REDIS_DEL_PREFIX expressions(KEYNAME) POST_CLOSE_PARANTHESIS.
{
	RET = NULL;
	HTSQLPage *page = GetCurrentPage(session);

	char *redVarName = mkMalloc(session->heapHandle, REDIS_VARIABLE_NAME.tokenStrLength + 1, __FILE__, __LINE__);
	strncpy_s(redVarName, REDIS_VARIABLE_NAME.tokenStrLength + 1, REDIS_VARIABLE_NAME.tokenStr, REDIS_VARIABLE_NAME.tokenStrLength);
	redVarName[REDIS_VARIABLE_NAME.tokenStrLength] = '\0';

	char *keyName = KEYNAME;

	PreObjectVariable *preObjectVariable = findRedisPreObjectVariable(session, page, redVarName, true);
	if (preObjectVariable) {
		if (preObjectVariable->value.redisContextValue) {
			RET = redisDel(session, page, preObjectVariable->value.redisContextValue, keyName);
		}
	}
	else {
		CLOG(ERR_NAME_IS_NOT_FOUND, redVarName);
		RET = mkStrdup(session->heapHandle, "", __FILE__, __LINE__);

	}
	mkFree(session->heapHandle, redVarName);
	mkFree(session->heapHandle, keyName);
}

rediscommandSet(RET) ::= POST_IDENTIFIER(REDIS_VARIABLE_NAME) POST_REDIS_SET_PREFIX expressions(KEYNAME) POST_COMMA expressions(KEYVALUE) POST_CLOSE_PARANTHESIS.
{
	RET = NULL;
	HTSQLPage *page = GetCurrentPage(session);

	char *valueA = mkMalloc(session->heapHandle, REDIS_VARIABLE_NAME.tokenStrLength + 1, __FILE__, __LINE__);
	strncpy_s(valueA, REDIS_VARIABLE_NAME.tokenStrLength + 1, REDIS_VARIABLE_NAME.tokenStr, REDIS_VARIABLE_NAME.tokenStrLength);
	valueA[REDIS_VARIABLE_NAME.tokenStrLength] = '\0';

	char *keyName = KEYNAME;

	char *keyValue = KEYVALUE;

	PreObjectVariable *preObjectVariable = findRedisPreObjectVariable(session, page, valueA, true);
	if (preObjectVariable) {
		if (preObjectVariable->value.redisContextValue) {
			RET = redisSet(session, page, preObjectVariable->value.redisContextValue, keyName, keyValue);
		}		
	}
	else {
		CLOG(ERR_NAME_IS_NOT_FOUND, valueA);
		RET = mkStrdup(session->heapHandle, "", __FILE__, __LINE__);

	}
	mkFree(session->heapHandle, valueA);
	mkFree(session->heapHandle, keyName);
	mkFree(session->heapHandle, keyValue);
}


/* redis command:  ping */
rediscommandPing(RET) ::= POST_IDENTIFIER(REDIS_VARIABLE_NAME) POST_REDIS_PING_PREFIX POST_CLOSE_PARANTHESIS.
{
	RET = NULL;
	HTSQLPage *page = GetCurrentPage(session);
	
	char *valueA = mkMalloc(session->heapHandle, REDIS_VARIABLE_NAME.tokenStrLength + 1, __FILE__, __LINE__);
	strncpy_s(valueA, REDIS_VARIABLE_NAME.tokenStrLength + 1, REDIS_VARIABLE_NAME.tokenStr, REDIS_VARIABLE_NAME.tokenStrLength);
	valueA[REDIS_VARIABLE_NAME.tokenStrLength] = '\0';
						  
	PreObjectVariable *preObjectVariable = findRedisPreObjectVariable(session, page, valueA, true);
	if (preObjectVariable) {
		if (preObjectVariable->value.redisContextValue) {
			RET = redisCommandPing(session, page, preObjectVariable->value.redisContextValue);
		}		
	}
	else {
		CLOG(ERR_NAME_IS_NOT_FOUND, valueA);
		RET = mkStrdup(session->heapHandle, ETEXT(ERR_REDIS_NO_VALID_CONNECTION), __FILE__, __LINE__);
	}
	
	mkFree(session->heapHandle, valueA);
}

/**  CALLER MUST FREE RESULT  */
blockName(RET)     ::= POST_COLON POST_IDENTIFIER(A).
{
    char *blockName = mkMalloc(session->heapHandle, A.tokenStrLength+1, __FILE__, __LINE__);
    strncpy_s(blockName, A.tokenStrLength + 1, A.tokenStr, A.tokenStrLength);
    blockName[A.tokenStrLength] = '\0';
    RET = blockName;
}

/**  CALLER MUST FREE RESULT  */
blockItemValue(RET) ::= POST_COLON POST_BLOCK_REFERENCE(A) POST_IDENTIFIER(B).
{
	HTSQLPage *page = GetCurrentPage(session);

    char *blockName = mkMalloc(session->heapHandle, A.tokenStrLength+1, __FILE__, __LINE__);
    strncpy_s(blockName, A.tokenStrLength + 1, A.tokenStr, A.tokenStrLength);
    blockName[A.tokenStrLength-1] = '\0';

    char *itemName = mkMalloc(session->heapHandle, B.tokenStrLength+1, __FILE__, __LINE__);
    strncpy_s(itemName, B.tokenStrLength + 1, B.tokenStr, B.tokenStrLength);
    itemName[B.tokenStrLength] = '\0';

    //char *retval = getBlockVariableValue(session, GetCurrentPage(session), blockName, itemName);
	char *retval = findAndReturnValueOfAVariableInAllScopes(session, page, blockName, itemName);
    RET = retval;

    mkFree(session->heapHandle, itemName);
    mkFree(session->heapHandle, blockName);
}


/**  CALLER MUST FREE RESULT  */
blockItemName(RET) ::= POST_COLON POST_BLOCK_REFERENCE(A) POST_IDENTIFIER(B).
{
    char *blockName = mkMalloc(session->heapHandle, A.tokenStrLength+1, __FILE__, __LINE__);
    strncpy_s(blockName, A.tokenStrLength+1, A.tokenStr, A.tokenStrLength);
    blockName[A.tokenStrLength] = '\0';

    char *itemName = mkMalloc(session->heapHandle, B.tokenStrLength+1, __FILE__, __LINE__);
    strncpy_s(itemName, B.tokenStrLength+1, B.tokenStr, B.tokenStrLength);
    itemName[B.tokenStrLength] = '\0';

    int len = A.tokenStrLength + B.tokenStrLength + 2;
    char *variableName = mkMalloc(session->heapHandle, len, __FILE__, __LINE__);
    strcpy_s(variableName, len, ":");
    strcat_s(variableName, len, blockName);
    strcat_s(variableName, len, itemName);
    variableName[len-1] = '\0';

    RET = mkStrdup(session->heapHandle, variableName, __FILE__, __LINE__);

    mkFree(session->heapHandle, variableName);
    mkFree(session->heapHandle, blockName);
    mkFree(session->heapHandle, itemName);
}
