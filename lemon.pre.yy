%include
{
#include "stdafx.h"

#include "htsql.h"

#include "mksession.h"
#include "pre.parser.imp.h"
#include "preObjectVariable.h"
#include "preObjectGeneric.h"
#include "preObjectFunction.h"
#include "pre.imp.h"
#include "SodiumSession.h"
}

%name preParse

%extra_argument { SodiumSession *session }

%type function_body_line    { char * }
%type function_body_lines   { char * }

%token_type { TokenPre }

%default_type { TokenPre }

%token_destructor {
    preTokenDestructor(session, $$);
}


%syntax_error {
#ifndef NDEBUG
    char *tempMsg;
    char *errMsg = mkStrcat(session->heapHandle, __FILE__, __LINE__, "PRE Syntax Error. Possible Tokens : (", NULL);
    int n = sizeof(yyTokenName) / sizeof(yyTokenName[0]);
    for (int i = 0; i < n; ++i) {
            int a = yy_find_shift_action(yypParser, (YYCODETYPE)i);
            if (a < YYNSTATE + YYNRULE) {
                tempMsg = mkStrcat(session->heapHandle, __FILE__, __LINE__, errMsg, " ", yyTokenName[i], NULL);
                mkFree(session->heapHandle, errMsg);
                errMsg = tempMsg;
            }
    }
    tempMsg = mkStrcat(session->heapHandle, __FILE__, __LINE__, errMsg, ")", NULL);
    mkFree(session->heapHandle, errMsg);
    errMsg = tempMsg;
    stopPreParserWithMessage(session, GetCurrentPage(session), errMsg, "");
#endif
}

start ::= expressions.

expressions ::= expressions expression.
expressions ::= expression.


expression  ::= comment.
expression  ::= globals.
expression  ::= PRE_END_OF_FILE.

comment     ::= PRE_COMMENT_START   PRE_COMMENT_END.

/**  GLOBALS VARIABLES */
globals      ::= PRE_VARIABLE_TYPE_VARCHAR   identifier(AAA) PRE_SEMICOLON.
{
    HTSQLPage *page = GetCurrentPage(session);
    PreObjectGeneric *isExists = findPreGenericObjectByName(session, page, AAA.tokenStr);
    if (isExists) {
        stopPreParserWithMessage(session, page, ETEXT(ERR_NAME_ALREADY_IN_USE), AAA.tokenStr);
    } else {
        PreObjectVariable *obj = createPreObjectVariableVarchar(session, PRE_VARIABLE_TYPE_VARCHAR, AAA.tokenStr, NULL);
        createPreObjectGeneric(session, page, AAA.tokenStr, POST_OBJECT_GENERIC_TYPE_VARIABLE, obj);
        mkDebug(session, SESSION_LOG_DETAIL_HTSQL_VARIABLE, "\n", "Global variable created (char): '", AAA.tokenStr, "'", NULL);
    }
    preTokenDestructor(session, AAA);
}
globals      ::= PRE_VARIABLE_TYPE_BOOL   identifier(AAA) PRE_SEMICOLON.
{
    HTSQLPage *page = GetCurrentPage(session);
    PreObjectGeneric *isExists = findPreGenericObjectByName(session, page, AAA.tokenStr);
    if (isExists) {
        stopPreParserWithMessage(session, page, "Değişken daha önce tanımlandı : ", AAA.tokenStr);
    } else {
        PreObjectVariable *obj = createPreObjectVariableVarchar(session, PRE_VARIABLE_TYPE_BOOL, AAA.tokenStr, NULL);
        createPreObjectGeneric(session, page, AAA.tokenStr, POST_OBJECT_GENERIC_TYPE_VARIABLE, obj);
        mkDebug(session, SESSION_LOG_DETAIL_HTSQL_VARIABLE, "\n", "Global variable created (char): '", AAA.tokenStr, "'", NULL);
    }
    preTokenDestructor(session, AAA);
}
globals      ::= PRE_VARIABLE_TYPE_NUMBER    identifier(AAA) PRE_SEMICOLON.
{
    HTSQLPage *page = GetCurrentPage(session);
    PreObjectGeneric *isExists = findPreGenericObjectByName(session, page, AAA.tokenStr);
    if (isExists) {
        stopPreParserWithMessage(session, page, ETEXT(ERR_NAME_ALREADY_IN_USE), AAA.tokenStr);
    } else {
        PreObjectVariable *obj = createPreObjectVariableNumber(session, PRE_VARIABLE_TYPE_NUMBER, AAA.tokenStr, 0);
        createPreObjectGeneric(session, page, AAA.tokenStr, POST_OBJECT_GENERIC_TYPE_VARIABLE, obj);
        mkDebug(session, SESSION_LOG_DETAIL_HTSQL_VARIABLE, "\n", "Global variable created (int): '", AAA.tokenStr, "'", NULL);
    }
    preTokenDestructor(session, AAA);
}
globals      ::= PRE_VARIABLE_TYPE_DATE      identifier(AAA) PRE_SEMICOLON.
{
    HTSQLPage *page = GetCurrentPage(session);
    PreObjectGeneric *isExists = findPreGenericObjectByName(session, page, AAA.tokenStr);
    if (isExists) {
        stopPreParserWithMessage(session, page, ETEXT(ERR_NAME_ALREADY_IN_USE), AAA.tokenStr);
    } else {
        PreObjectVariable *obj = createPreObjectVariableDate(session, PRE_VARIABLE_TYPE_DATE, AAA.tokenStr, NULL);
        createPreObjectGeneric(session, page, AAA.tokenStr, POST_OBJECT_GENERIC_TYPE_VARIABLE, obj);
        mkDebug(session, SESSION_LOG_DETAIL_HTSQL_VARIABLE, "\n", "Global variable created (date): '", AAA.tokenStr, "'", NULL);
    }
    preTokenDestructor(session, AAA);
}
globals      ::= PRE_VARIABLE_TYPE_REDIS	identifier(AAA) PRE_SEMICOLON.
{
	HTSQLPage *page = GetCurrentPage(session);
	PreObjectGeneric *isExists = findPreGenericObjectByName(session, page, AAA.tokenStr);
	if (isExists) {
		stopPreParserWithMessage(session, page, ETEXT(ERR_NAME_ALREADY_IN_USE), AAA.tokenStr);
	}
	else {
		PreObjectVariable *obj = createPreObjectVariableRedis(session, AAA.tokenStr);
		createPreObjectGeneric(session, page, AAA.tokenStr, POST_OBJECT_GENERIC_TYPE_VARIABLE, obj);
		mkDebug(session, SESSION_LOG_DETAIL_HTSQL_VARIABLE, "\n", "Global variable created (redis): '", AAA.tokenStr, "'", NULL);
	}
	preTokenDestructor(session, AAA);
}


/**  GLOBALS FUNCTIONS */
globals      ::= PRE_VARIABLE_TYPE_VARCHAR funcdechead.
{
    HTSQLPage *page = GetCurrentPage(session);
    __setFunctionReturnVariable(session, page, PRE_VARIABLE_TYPE_VARCHAR);
}
globals      ::= PRE_VARIABLE_TYPE_NUMBER  funcdechead.
{
    HTSQLPage *page = GetCurrentPage(session);
    __setFunctionReturnVariable(session, page, PRE_VARIABLE_TYPE_NUMBER);
}
globals      ::= PRE_VARIABLE_TYPE_DATE    funcdechead.
{
    HTSQLPage *page = GetCurrentPage(session);
    __setFunctionReturnVariable(session, page, PRE_VARIABLE_TYPE_DATE);
}
globals      ::= PRE_VARIABLE_TYPE_VOID    funcdechead .
{
    HTSQLPage *page = GetCurrentPage(session);
    __setFunctionReturnVariable(session, page, PRE_VARIABLE_TYPE_VOID);
}
globals      ::= PRE_VARIABLE_TYPE_BOOL    funcdechead .
{
    HTSQLPage *page = GetCurrentPage(session);
    __setFunctionReturnVariable(session, page, PRE_VARIABLE_TYPE_BOOL);
}

funcdechead ::= funcdecid parameterlist htsqlfunctionbody.
{
	decreaseDebugIndentionCount(session); // increasing is in "funcdecid" rule reduction
	mkDebug(session, SESSION_LOG_DETAIL_HTSQL_VARIABLE, "\n", "done.", NULL);
}
funcdecid   ::= identifier(BBB).
{
    char *functionName = BBB.tokenStr;	
    HTSQLPage *page = GetCurrentPage(session);
    PreObjectGeneric* preObjectGenericObject = findPreGenericObjectByName(session, page, functionName);
    if (preObjectGenericObject) {
        char *errMsg = mkStrcat(session->heapHandle, __FILE__, __LINE__, "'", preObjectGenericObject->objectName, "' isimli fonksiyonu daha önce tanımlandı", NULL);
        stopPreParserWithMessage(session, GetCurrentPage(session), errMsg, "");
        mkFreeAdv(session->heapHandle, (void**) &errMsg);
    } else {
        PreObjectFunction *obj = createPreObjectFunction(session);
		
        increaseDebugIndentionCount(session); // decresing is in "htsqlfunctionbody" reduce
        createPreObjectGeneric(session, page, functionName, POST_OBJECT_GENERIC_TYPE_FUNCTION, obj);
    
        char ln[20];
        //mkItoa(session->sessionDebugInfo->lineNumberOuter, ln);
        mkItoa(obj->lineNumberOuter, ln);

        mkDebug(session, SESSION_LOG_DETAIL_HTSQL_VARIABLE, "\n", "Global function created: ", functionName, " (on line ", ln, ")...", NULL);
    }
    preTokenDestructor(session, BBB);
}



htsqlfunctionbody   ::= PRE_FUNCTION_BEGIN(AAA) function_body_lines(BBB) PRE_FUNCTION_END(CCC).
{
    HTSQLPage *page = GetCurrentPage(session);
	PreObjectGeneric *currentGenericObject = getCurrentPreGenericObject(session, page);

	if (currentGenericObject) {
		char *bodyBegin = AAA.tokenStr;
		char *bodyItself = BBB;
		char *bodyEnd = CCC.tokenStr;
		char *body = mkStrcat(session->heapHandle, __FILE__, __LINE__, bodyBegin, bodyItself, bodyEnd, NULL);
		__setFunctionBody(session, page, currentGenericObject, body);
		mkFree(session->heapHandle, body);
	}

    preTokenDestructor(session, AAA);
    mkFree(session->heapHandle, BBB);
    preTokenDestructor(session, CCC);
}
htsqlfunctionbody   ::= PRE_FUNCTION_BEGIN(AAA) PRE_FUNCTION_END(CCC).
{
    HTSQLPage *page = GetCurrentPage(session);
	PreObjectGeneric *currentGenericObject = getCurrentPreGenericObject(session, page);

	if (currentGenericObject) {
		char *body = mkStrcat(session->heapHandle, __FILE__, __LINE__, AAA.tokenStr, CCC.tokenStr, NULL);
		__setFunctionBody(session, page, currentGenericObject, body);
		mkFree(session->heapHandle, body);
	}
    
    preTokenDestructor(session, AAA);
    preTokenDestructor(session, CCC);
}


function_body_lines(RET) ::= function_body_lines(AAA) function_body_line(BBB).
{
    char *p1  = AAA;
    char *p2  = BBB;
    char *ret = mkStrcat(session->heapHandle, __FILE__, __LINE__, p1, p2, NULL);
    RET = ret;
    mkFree(session->heapHandle, p1);
    mkFree(session->heapHandle, p2);
}
function_body_lines ::= function_body_line.

function_body_line(RET)  ::= PRE_FUNCTION_BODY_LINE(BBB).
{
    RET = BBB.tokenStr;
}






/**  Parameter list
*/
parameterlist   ::= openparenthesis parameters closeparenthesis.

parameters      ::= parameters comma parameter.
parameters      ::= parameter.


parameter       ::= PRE_VARIABLE_TYPE_VARCHAR identifier(AAA).
{
    HTSQLPage *page = GetCurrentPage(session);
    __addFunctionParameterVariable(session, page, PRE_VARIABLE_TYPE_VARCHAR, AAA.tokenStr);    
    mkDebug(session, SESSION_LOG_DETAIL_HTSQL_VARIABLE, "\n", "Function parameter created: ", AAA.tokenStr, NULL);
	preTokenDestructor(session, AAA);
}
parameter       ::= PRE_VARIABLE_TYPE_NUMBER  identifier(AAA).
{
    HTSQLPage *page = GetCurrentPage(session);
    __addFunctionParameterVariable(session, page, PRE_VARIABLE_TYPE_NUMBER, AAA.tokenStr);
    mkDebug(session, SESSION_LOG_DETAIL_HTSQL_VARIABLE, "\n", "Function parameter created: ", AAA.tokenStr, NULL);
	preTokenDestructor(session, AAA);
}
parameter       ::= PRE_VARIABLE_TYPE_DATE    identifier(AAA).
{
    HTSQLPage *page = GetCurrentPage(session);
    __addFunctionParameterVariable(session, page, PRE_VARIABLE_TYPE_DATE, AAA.tokenStr);
    mkDebug(session, SESSION_LOG_DETAIL_HTSQL_VARIABLE, "\n", "Function parameter created: ", AAA.tokenStr, NULL);
	preTokenDestructor(session, AAA);
}
parameter       ::= .




/**  identifier
*/
identifier(RET) ::= PRE_IDENTIFIER(A).
{
	TokenPre a = A;
	RET = a;
}


/**  parenthesis
*/
openparenthesis     ::= PRE_OPEN_PARAN.
closeparenthesis    ::= PRE_CLOSE_PARAN.

comma               ::= PRE_COMMA.
