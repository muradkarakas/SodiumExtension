%{
#include "stdafx.h"

#include "htsql.h"

#include "lemon.pre.h"
%}

%option extra-type="struct _SodiumSession *"
%option reentrant
%option noyywrap noinput nounput
%option never-interactive
%option nounistd

%x SC_FUNCTION_PARAMETERS
%x SC_FUNCTION_BODY
%x SC_COMMENT

CRLF			"\n"
SPACE           [ \t]*
NUMBER          [0-9]*
IDENTIFIER      [a-zA-Z_][a-zA-Z0-9_]*(\.?[a-zA-Z0-9_])*
DECLARE_BODY    [ \t.]*

BEGIN(INITIAL);
%%

{SPACE}char{SPACE}              {
                                    return PRE_VARIABLE_TYPE_VARCHAR;
                                }
{SPACE}int{SPACE}               {
                                    return PRE_VARIABLE_TYPE_NUMBER;
                                }
{SPACE}redis{SPACE}				{
									return PRE_VARIABLE_TYPE_REDIS;
								}
{SPACE}date{SPACE}              {
                                    return PRE_VARIABLE_TYPE_DATE;
                                }
{SPACE}void{SPACE}              {
                                    return PRE_VARIABLE_TYPE_VOID;
                                }
{SPACE}bool{SPACE}              {
                                    return PRE_VARIABLE_TYPE_BOOL;
                                }
{SPACE}";"{SPACE}               {
                                    return PRE_SEMICOLON;
                                }
{SPACE}"/**"{SPACE}             {
                                    BEGIN(SC_COMMENT);
                                    return PRE_COMMENT_START;
                                }
{SPACE}"/*"{SPACE}              {
                                    BEGIN(SC_COMMENT);
                                    return PRE_COMMENT_START;
                                }
{CRLF}							{
									SodiumSession *currentMKSession = yyextra;
									currentMKSession->sessionDebugInfo->lineNumberOuter++;
								}
[ \t]*							{
									
                                }
{SPACE}"\("{SPACE}              {
                                    BEGIN(SC_FUNCTION_PARAMETERS);
                                    return PRE_OPEN_PARAN;
                                }
{IDENTIFIER}	                {
                                    return PRE_IDENTIFIER;
                                }
<<EOF>>                         {
                                    return PRE_END_OF_FILE;
                                }

<SC_COMMENT>{
{CRLF}							{
									SodiumSession *currentMKSession = yyextra;
									currentMKSession->sessionDebugInfo->lineNumberOuter++;
								}
"*/"                            {
                                    BEGIN(INITIAL);
                                    return PRE_COMMENT_END;
                                }
.                               {

                                }
}


<SC_FUNCTION_PARAMETERS>{
{SPACE}"char"{SPACE}        {
                                return PRE_VARIABLE_TYPE_VARCHAR;
                            }
{SPACE}"int"{SPACE}         {
                                return PRE_VARIABLE_TYPE_NUMBER;
                            }
{SPACE}"date"{SPACE}        {
                                return PRE_VARIABLE_TYPE_DATE;
                            }
{SPACE}","{SPACE}           {
                                return PRE_COMMA;
                            }
{IDENTIFIER}	            {
                                return PRE_IDENTIFIER;
                            }
{SPACE}"\)"{SPACE}          {
                                BEGIN(SC_FUNCTION_BODY);
                                return PRE_CLOSE_PARAN;
                            }
}


<SC_FUNCTION_BODY>{
{SPACE}"\{"{SPACE}          {
                                return PRE_FUNCTION_BEGIN;
                            }
[ \t]*						{
                                return PRE_FUNCTION_BODY_LINE;
                            }
{SPACE}"} else {"{SPACE}    {
                                return PRE_FUNCTION_BODY_LINE;
                            }
"};"                        {
                                return PRE_FUNCTION_BODY_LINE;
                            }
"}"{SPACE}";"               {
                                return PRE_FUNCTION_BODY_LINE;
                            }
{SPACE}"}"{SPACE}			{
                                BEGIN(INITIAL);
                                return PRE_FUNCTION_END;
                            }
{CRLF}						{
								SodiumSession *currentMKSession = yyextra;
								currentMKSession->sessionDebugInfo->lineNumberOuter++;
								return PRE_FUNCTION_BODY_LINE;
							}
.*                          {
                                return PRE_FUNCTION_BODY_LINE;
                            }
}

.                           {
                                printf("Unrecognized character: %s\n", yytext);
                            }

%%


