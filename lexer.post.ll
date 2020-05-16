%{
#include "pch.h"

#include "lemon.post.h"
#include "debugee.h"
%}

%option extra-type="struct _SodiumSession *"
%option reentrant
%option noyywrap noinput nounput
%option never-interactive
%option nounistd

CRLF			"\n"
SPACE           [ \t]*
IDENTIFIER      [a-zA-Z_][a-zA-Z0-9_]*
NUMBER          [0-9]+("."[0-9]*)*

%x BLOCK_REFERENCES
%x LOOP
%x THEN
%x CONDITION_STRING
%x JOINCONDITIONSTRING
%x SC_PLSQL_BLOCK
%x SC_COMMENT
%x CASE_ELSE_END_BLOCK
%x SC_STRING
%x SC_DML

%%

{SPACE}"/**"{SPACE}                             {
                                                    BEGIN(SC_COMMENT);
                                                    return POST_COMMENT_START;
                                                }
{SPACE}"/*"{SPACE}                              {
                                                    BEGIN(SC_COMMENT);
                                                    return POST_COMMENT_START;
                                                }
{SPACE}false{SPACE}                             {
                                                    return POST_FALSE;
                                                }
{SPACE}"__LINE__"{SPACE}                        {
                                                    return POST_MACRO_LINE;
                                                }
{SPACE}true{SPACE}                              {
                                                    return POST_TRUE;
                                                }
{SPACE}char{SPACE}                              {
                                                    return POST_VARIABLE_TYPE_VARCHAR;
                                                }
{SPACE}redis{SPACE}                             {
                                                    return POST_VARIABLE_TYPE_REDIS;
                                                }
{SPACE}bool{SPACE}                              {
                                                    return POST_VARIABLE_TYPE_BOOL;
                                                }
{SPACE}int{SPACE}                               {
                                                    return POST_VARIABLE_TYPE_NUMBER;
                                                }
{SPACE}date{SPACE}                              {
                                                    return POST_VARIABLE_TYPE_DATE;
                                                }
{SPACE}"sizeof"{SPACE}"("{SPACE}                {
                                                    return POST_SIZEOF_PREFIX;
                                                }
{SPACE}"return"{SPACE}                          {
                                                    return POST_RETURN_PREFIX;
                                                }
{SPACE}"delete"{SPACE}";"                       {
                                                    return POST_DELETE;
                                                }
{SPACE}"rollback"{SPACE}"("{SPACE}              {
                                                    return POST_ROLLBACK_PREFIX;
                                                }
{SPACE}"commit"{SPACE}"("{SPACE}                {
                                                    return POST_COMMIT_PREFIX;
                                                }
{SPACE}"instr"{SPACE}"("{SPACE}                 {
                                                    return POST_INSTR_PREFIX;
                                                }
{SPACE}"replace"{SPACE}"("{SPACE}               {
                                                    return POST_REPLACE_PREFIX;
                                                }
{SPACE}"strlen"{SPACE}"("{SPACE}                {
                                                    return POST_STRLEN_PREFIX;
                                                }
{SPACE}"substr"{SPACE}"("{SPACE}                {
                                                    return POST_SUBSTR_PREFIX;
                                                }
{SPACE}"previous_record"{SPACE}"("{SPACE}       {
                                                    return POST_PREVIOUS_RECORD_PREFIX;
                                                }
{SPACE}"last_record"{SPACE}"("{SPACE}           {
                                                    return POST_LAST_RECORD_PREFIX;
                                                }
{SPACE}"next_record"{SPACE}"("{SPACE}           {
                                                    return POST_NEXT_RECORD_PREFIX;
                                                }
{SPACE}"first_record"{SPACE}"("{SPACE}          {
                                                    return POST_FIRST_RECORD_PREFIX;
                                                }
{SPACE}"message"{SPACE}"("{SPACE}               {
                                                    return POST_MESSAGE_PREFIX;
                                                }
{SPACE}"run_sql_file"{SPACE}"("{SPACE}			{
													return POST_RUN_SQL_FILE_PREFIX;
												}
{SPACE}"show_page"{SPACE}"("{SPACE}             {
                                                    return POST_SHOW_PAGE_PREFIX;
                                                }
{SPACE}"refresh_block"{SPACE}"("{SPACE}         {
                                                    return POST_REFRESH_BLOCK_PREFIX;
                                                }
{SPACE}"set_datablock_property"{SPACE}"("{SPACE}		{
															return POST_SET_DATABLOCK_PROPERTY_PREFIX;
														}
{SPACE}"create_oracle_connection"{SPACE}"("{SPACE}		{
															return POST_CREATE_ORACLE_CONNECTION_PREFIX;
														}
{SPACE}"create_mysql_connection"{SPACE}"("{SPACE}		{
															return POST_CREATE_MYSQL_CONNECTION_PREFIX;
														}
{SPACE}"create_postgresql_connection"{SPACE}"("{SPACE}	{
															return POST_CREATE_POSTGRESQL_CONNECTION_PREFIX;
														}
{SPACE}"create_sqlserver_connection"{SPACE}"("{SPACE}	{
															return POST_CREATE_SQLSERVER_CONNECTION_PREFIX;
														}
{SPACE}"create_redis_connection"{SPACE}"("{SPACE}		{
															return POST_CREATE_REDIS_CONNECTION_PREFIX;
														}
{SPACE}"close_redis_connection"{SPACE}"("{SPACE}		{
															return POST_CLOSE_REDIS_CONNECTION_PREFIX;
														}
{SPACE}"hide_block"{SPACE}"("{SPACE}            {
                                                    return POST_HIDE_BLOCK_PREFIX;
                                                }
{SPACE}"show_block"{SPACE}"("{SPACE}            {
                                                    return POST_SHOW_BLOCK_PREFIX;
                                                }
{SPACE}"show_column"{SPACE}"("{SPACE}           {
                                                    return POST_SHOW_COLUMN_PREFIX;
                                                }
{SPACE}"hide_column"{SPACE}"("{SPACE}           {
                                                    return POST_HIDE_COLUMN_PREFIX;
                                                }
{SPACE}"disable_column"{SPACE}"("{SPACE}        {
                                                    return POST_DISABLE_COLUMN_PREFIX;
                                                }
{SPACE}"enable_column"{SPACE}"("{SPACE}         {
                                                    return POST_ENABLE_COLUMN_PREFIX;
                                                }
{SPACE}"prompt"{SPACE}"("{SPACE}                {
                                                    return POST_PROMPT_PREFIX;
                                                }
{SPACE}"populate_datalist"{SPACE}"("{SPACE}     {
                                                    return POST_POPULATE_DATALIST_PREFIX;
                                                }
{SPACE}"populate_tree"{SPACE}"("{SPACE}         {
                                                    return POST_POPULATE_TREE_PREFIX;
                                                }
{SPACE}"refresh_tree_node"{SPACE}"("{SPACE}     {
                                                    return POST_REFRESH_TREE_NODE_PREFIX;
                                                }
{SPACE}"to_json"{SPACE}"("{SPACE}               {
                                                    return POST_TO_JSON_PREFIX;
                                                }
{SPACE}"get_database_name"{SPACE}"("{SPACE}		{
													return POST_GET_DATABASE_NAME_PREFIX;
												}
{SPACE}"get_database_type"{SPACE}"("{SPACE}		{
													return POST_GET_DATABASE_TYPE_PREFIX;
												}
{SPACE}".ping"{SPACE}"("{SPACE}					{
													return POST_REDIS_PING_PREFIX;
												}
{SPACE}".set"{SPACE}"("{SPACE}					{
													return POST_REDIS_SET_PREFIX;
												}
{SPACE}".append"{SPACE}"("{SPACE}				{
													return POST_REDIS_APPEND_PREFIX;
												}
{SPACE}".strlen"{SPACE}"("{SPACE}				{
													return POST_REDIS_STRLEN_PREFIX;
												}
{SPACE}".get"{SPACE}"("{SPACE}					{
													return POST_REDIS_GET_PREFIX;
												}
{SPACE}".del"{SPACE}"("{SPACE}					{
													return POST_REDIS_DEL_PREFIX;
												}
{SPACE}".incr"{SPACE}"("{SPACE}					{
													return POST_REDIS_INCR_PREFIX;
												}
{SPACE}".incrBy"{SPACE}"("{SPACE}				{
													return POST_REDIS_INCRBY_PREFIX;
												}
{SPACE}".decr"{SPACE}"("{SPACE}					{
													return POST_REDIS_DECR_PREFIX;
												}
{SPACE}".decrBy"{SPACE}"("{SPACE}				{
													return POST_REDIS_DECRBY_PREFIX;
												}
{SPACE}"null"{SPACE}                            {
                                                    return POST_NULL;
                                                }
\'          									{
													BEGIN(SC_STRING);
													return POST_STRING_CONSTANT_START;
												}
{NUMBER}                                    {
                                                return POST_NUMBER_CONSTANT;
                                            }
{SPACE}","{SPACE}                           {
                                                return POST_COMMA;
                                            }
"||"                                        {
                                                return POST_AMPERSAND;
                                            }
{SPACE}"="{SPACE}                           {
                                                return POST_ASSIGNMENT;
                                            }
"+"                                         {
                                                return POST_PLUS;
                                            }
"-"                                         {
                                                return POST_SUBTRACT;
                                            }
"*"                                         {
                                                return POST_MULTIPLY;
                                            }
"/"                                         {
                                                return POST_DIVISION;
                                            }
"%"                                         {
                                                return POST_MODULAR_DIVISION;
                                            }
{SPACE}";"{SPACE}                           {
                                                return POST_SEMI_COLON;
                                            }
{SPACE}"select"{SPACE}						{
												BEGIN(SC_DML);
                                                return POST_DML_START;
                                            }
{SPACE}"insert"{SPACE}"into"{SPACE}([^;])*{SPACE}   {
                                                        return POST_INSERT;
                                                    }
{SPACE}"delete"([^;])*{SPACE}               {
                                                return POST_DELETE;
                                            }
{SPACE}"update"([^;])*{SPACE}               {
                                                return POST_UPDATE;
                                            }
{SPACE}"begin"{SPACE}                       {
                                                BEGIN(SC_PLSQL_BLOCK);
                                                return POST_PLSQL_BLOCK_BEGIN;
                                            }
":"                                         {
                                                BEGIN(BLOCK_REFERENCES);
                                                return POST_COLON;
                                            }
"."                                         {
                                                return POST_PERIOD;
                                            }
{IDENTIFIER}                                {
                                                return POST_IDENTIFIER;
                                            }


{SPACE}"{"{SPACE}                   {
                                        return POST_BEGIN;
                                    }
{SPACE}"}"{SPACE}                   {
                                        return POST_END;
                                    }
{SPACE}"("{SPACE}                   {
                                        return POST_OPEN_PARANTHESIS;
                                    }
{SPACE}")"{SPACE}                   {
                                        return POST_CLOSE_PARANTHESIS;
                                    }
{SPACE}"=="{SPACE}                  {
                                        return POST_CONDITION_EQ;
                                    }
{SPACE}"is"{SPACE}"null"{SPACE}     {
                                        return POST_CONDITION_IS_NULL;
                                    }
{SPACE}"is"{SPACE}"not"{SPACE}"null"{SPACE}     {
                                                    return POST_CONDITION_IS_NOT_NULL;
                                                }
{SPACE}"!="{SPACE}                  {
                                        return POST_CONDITION_NEQ;
                                    }
{SPACE}"!"{SPACE}                   {
                                        return POST_NOT;
                                    }
{SPACE}"<>"{SPACE}                  {
                                        return POST_CONDITION_NEQ;
                                    }
{SPACE}"<"{SPACE}                   {
                                        return POST_CONDITION_LT;
                                    }
{SPACE}"<="{SPACE}                  {
                                        return POST_CONDITION_LTEQ;
                                    }
{SPACE}">"{SPACE}                   {
                                        return POST_CONDITION_GT;
                                    }
{SPACE}">="{SPACE}                  {
                                        return POST_CONDITION_GTEQ;
                                    }
{SPACE}"like"{SPACE}                {
                                        return POST_CONDITION_LIKE;
                                    }
{SPACE}"not"{SPACE}"like"{SPACE}    {
                                        return POST_CONDITION_NOT_LIKE;
                                    }
{SPACE}"and"{SPACE}                 {
                                        return POST_CONDITION_AND;
                                    }
{SPACE}"or"{SPACE}                  {
                                        return POST_CONDITION_OR;
                                    }
{SPACE}"while"{SPACE}"("            {
                                        BEGIN(CONDITION_STRING);
                                        return POST_WHILE;
                                    }
{SPACE}"if"{SPACE}"("               {
										SodiumSession *currentMKSession = yyextra;
										HTSQLPage *page = GetCurrentPage(currentMKSession);
										page->ifCounter++;
                                        BEGIN(CONDITION_STRING);
                                        return POST_IF;
                                    }
"#--"                               {
                                        BEGIN(JOINCONDITIONSTRING);
                                        return POST_JOINCONDITION_START;
                                    }
"?--"                               {
                                        BEGIN(JOINCONDITIONSTRING);
                                        return POST_SQLEVALUATION_START;
                                    }
"$--"                               {
                                        BEGIN(JOINCONDITIONSTRING);
                                        return POST_WHERECLAUSEEVALUATION_START;
                                    }
{CRLF}								{
										SodiumSession *currentMKSession = yyextra;
										HTSQLPage *page = GetCurrentPage(currentMKSession);
										increaseInnerLineNumber(currentMKSession);
									}
{SPACE}								{
										
									}
<<EOF>>                             {
                                        return POST_END_OF_FILE;
                                    }




<SC_DML>{
";"														{
															BEGIN(INITIAL);
															return POST_DML_END;
														}
{CRLF}													{
															increaseInnerLineNumber(yyextra);
															return POST_DML_CHAR;
														}
[.a-zA-Z#(),:!= ?"0-9\t\{\}\]\-\\[/@$*+_\. şığçöü']		{
															return POST_DML_CHAR;
														}
}






<SC_STRING>{
"\\'"													{
															return POST_STRING_CONSTANT_APOSTROPHE;
														}
\'														{
															BEGIN(INITIAL);
															return POST_STRING_CONSTANT_END;
														}
{CRLF}													{
															increaseInnerLineNumber(yyextra);
															return POST_STRING_CONSTANT_CHAR;
														}
[.a-zA-Z#(),:;!=?"0-9\t\{\}\]\-\\[/@$*+_\. şığçöü%<>]   {
															return POST_STRING_CONSTANT_CHAR;
														}
}




<SC_PLSQL_BLOCK>{
{IDENTIFIER}"."                     {
                                        return POST_BLOCK_REFERENCE;
                                    }
{IDENTIFIER}                        {
                                        return POST_IDENTIFIER;
                                    }
":"                                 {
                                        return POST_COLON;
                                    }
"end"{SPACE}";"                     {
                                        BEGIN(INITIAL);
                                        return POST_PLSQL_BLOCK_END;
                                    }
"\\"								{
										return POST_PLSQL_BLOCK_ANYCHARACTER;
									}
{CRLF}								{
										increaseInnerLineNumber(yyextra);
										return POST_PLSQL_BLOCK_ANYCHARACTER;
									}
[\r\t]*								{
										return POST_PLSQL_BLOCK_ANYCHARACTER;
									}
.			                        {
                                        return POST_PLSQL_BLOCK_ANYCHARACTER;
                                    }
}


<JOINCONDITIONSTRING>{
{CRLF}                                      {
                                                increaseInnerLineNumber(yyextra);
                                                return POST_PLSQL_BLOCK_ANYCHARACTER;
                                            }
{IDENTIFIER}"."                             {
                                                return POST_BLOCK_REFERENCE;
                                            }
{IDENTIFIER}                                {
                                                return POST_IDENTIFIER;
                                            }
">>"                                        {
                                                return POST_JOINCONDITION_SEPARATOR;
                                            }
":"                                         {
                                                return POST_COLON;
                                            }
"--#"                               {
                                        BEGIN(INITIAL);
                                        return POST_JOINCONDITION_END;
                                    }
"--?"                               {
                                        BEGIN(INITIAL);
                                        return POST_SQLEVALUATION_END;
                                    }
"--$"                               {
                                        BEGIN(INITIAL);
                                        return POST_WHERECLAUSEEVALUATION_END;
                                    }
.                                   {
                                        return POST_JOIN_ANYCHARACTER;
                                    }
}

<CONDITION_STRING>{
")"{SPACE}"then"{SPACE}				{
										SodiumSession *currentMKSession = yyextra;
										HTSQLPage *page = GetCurrentPage(currentMKSession);
										//page->currenHTSQLFunctionGenericInstance->functionDebugInfo->ifTotalLineCount = 0;										
                                        BEGIN(THEN);
                                        return POST_THEN;
                                    }
")"{SPACE}"loop"{SPACE}             {
                                        BEGIN(LOOP);
                                        return POST_LOOP_1;
                                    }
.                                   {
                                        return POST_CONDITION_CHAR;
                                    }
}

<LOOP>{
{SPACE}"end"{SPACE}"loop"{SPACE}";"{SPACE}  {
                                                BEGIN(INITIAL);
                                                return POST_LOOP_END_1;
                                            }
.                                           {
                                                return POST_LOOP_1_LINE_CHARS;
                                            }
}

<THEN>{
{SPACE}"if"{SPACE}"("                       {
                                                SodiumSession *currentMKSession = yyextra;
												HTSQLPage *page = GetCurrentPage(currentMKSession);
                                                page->ifCounter++;
                                                return POST_IF_LINE;
                                            }
{SPACE}"case"{SPACE}                        {
                                                /* "else" part of a "case" block in sql command results in a syntax error if it is in "if" command.
                                                 *
                                                 * Example HT/SQL Code block in : "tree_node_expanded" trigger
                                                 *
                                          >>>>> if (node_id == '#') then
                                                    json = '[ { "id" : "0", "text" : "Ürün Kategorileri", "parent" : "#", "children" : true }  ]';
                                          >>>>> else
                                                    rs =	select
                                                                  id			id,
                                                                  kategori_adi 	text,
                                                                  parent_id		parent,
                                           >>>>>                  case (select count(*) from urun_kategorileri kk where kk.parent_id = k.id)
                                                                    when 0 then
                                                                        'false'
                                           >>>>>				  else
                                                                        'true'
                                                                    end children
                                                                from
                                                                  urun_kategorileri k
                                                                where
                                                                    k.parent_id = :node_id;

                                                    json = to_json('cb.data', rs);
                                                end if;
                                                */
                                                BEGIN(CASE_ELSE_END_BLOCK);
                                                return POST_IF_LINE;
                                            }
{SPACE}"else"{SPACE}                        {
                                                SodiumSession *currentMKSession = yyextra;
												HTSQLPage *page = GetCurrentPage(currentMKSession);
                                                if (page->ifCounter == 1) {
													return POST_ELSE;
                                                } else {
                                                    return POST_IF_LINE;
                                                }
                                            }
{SPACE}"end"{SPACE}"if"{SPACE}				{
												SodiumSession *currentMKSession = yyextra;
												HTSQLPage *page = GetCurrentPage(currentMKSession);												
												if (page->ifCounter-- == 1) {
													BEGIN(INITIAL);													
													return POST_END_IF_WITH_SEMICOLON;
												} else {
													//page->currenHTSQLFunctionGenericInstance->functionDebugInfo->ifTotalLineCount++;
													return POST_IF_LINE;
												}
											}
{SPACE}";"{SPACE}                           {
												SodiumSession *currentMKSession = yyextra;
												HTSQLPage *page = GetCurrentPage(currentMKSession);												
												if (page->ifCounter == 0) {											
													return POST_SEMI_COLON;
												} else {
													return POST_IF_LINE;
												}                                                
                                            }
{CRLF}										{												
												SodiumSession *currentMKSession = yyextra;
												HTSQLPage *page = GetCurrentPage(currentMKSession);
												//page->currenHTSQLFunctionGenericInstance->functionDebugInfo->ifTotalLineCount++;
												return POST_IF_LINE;
											}
.                                           {
                                                return POST_IF_LINE;
                                            }
}

<CASE_ELSE_END_BLOCK>{
{SPACE}"end"{SPACE}                         {
                                                BEGIN(THEN);
                                                return POST_IF_LINE;
                                            }
{CRLF}										{
												return POST_IF_LINE;
											}
.                                           {
                                                return POST_IF_LINE;
                                            }
}

<BLOCK_REFERENCES>{
"Session.Id"                                {
                                                BEGIN(INITIAL);
                                                return POST_SESSION_ID;
                                            }
"Row.Id"                                    {
                                                BEGIN(INITIAL);
                                                return POST_ROW_ID;
                                            }
{IDENTIFIER}"."                             {
                                                return POST_BLOCK_REFERENCE;
                                            }
{IDENTIFIER}                                {
                                                BEGIN(INITIAL);
                                                return POST_IDENTIFIER;
                                            }
}



<SC_COMMENT>{
"*/"                            {
                                    BEGIN(INITIAL);
                                    return POST_COMMENT_END;
                                }
[\t\n]                          {

                                }
.                               {
                                        
                                }
}

.                               {
                                        printf("Unrecognized character: %s\n", yytext);
                                }

%%




