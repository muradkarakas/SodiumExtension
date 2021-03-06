/**
 * This file is part of Sodium Language project
 *
 * Copyright � 2020 Murad Karaka� <muradkarakas@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License v3.0
 * as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 *	https://choosealicense.com/licenses/gpl-3.0/
 */

 /** \file   html.imp.h

    \brief  Contains function and data structures needed for HTML (mk file) parse phase.
*/

#pragma once





#ifndef YY_EXTRA_TYPE
#define YY_EXTRA_TYPE struct _SodiumSession *
#endif

typedef void* yyscan_t;

int                         htmllex (yyscan_t yyscanner);
int                     htmlget_leng(yyscan_t yyscanner);
int                     htmllex_init (yyscan_t* scanner);
char                 *htmlget_text ( yyscan_t yyscanner);
int                htmllex_destroy ( yyscan_t yyscanner);
void         *htmlParseAlloc(void* (*allocProc)(size_t));
void       htmlset_in(FILE *_in_str, yyscan_t yyscanner);
void     htmlParseFree(void *p, void (*freeProc)(void*));
int		htmllex_init_extra(YY_EXTRA_TYPE user_defined, yyscan_t* scanner);;


/*	Returns false if frmx file does not exists */
bool              
ParseFRMXFile(
	SodiumSession *currentSession, 
	HTSQLPage *page
);

void           htmlTokenDestructor(SodiumSession *currentMKSession, TokenHtml token);
void    stopHTMLParserWithMessage(SodiumSession *currentMKSession, HTSQLPage *page, const char *message, const char *reason);

/**
	CALLER MUST FREE RETURN VALUE
*/
char *	       getCreateNTString(SodiumSession *currentMKSession, TokenHtml token);

wchar_t*
getCreateNTStringW(
	SodiumSession* session,
	TokenHtml token
);