#pragma once

#include "htsql.h"


typedef void* yyscan_t;

#ifndef YY_EXTRA_TYPE
#define YY_EXTRA_TYPE struct _SodiumSession *
#endif

//int                                     prelex(yyscan_t yyscanner);
int prelex_init_extra(YY_EXTRA_TYPE user_defined, yyscan_t* scanner);
int                                preget_leng(yyscan_t yyscanner);
int                                prelex_init (yyscan_t* scanner);
char                              *preget_text(yyscan_t yyscanner);
int                             prelex_destroy(yyscan_t yyscanner);
void preParse(
	void *yyp,                   /* The parser */
	int yymajor,                 /* The major token code number */
	TokenPre yyminor,       /* The value for the token */
	SodiumSession *               /* Optional %extra_argument parameter */
);
void                    *preParseAlloc(void* (*allocProc)(size_t));
void                  preset_in(FILE *_in_str, yyscan_t yyscanner);
void                preParseFree(void *p, void (*freeProc)(void*));
void   preTokenDestructor(SodiumSession *currentMKSession, TokenPre t);

/*	Returns false if preFile does not exists
*/
bool
ParseSQLXFile(
	SodiumSession* session,
	HTSQLPage* page,
	wchar_t* filePath
);

void stopPreParserWithMessage(SodiumSession *currentMKSession, HTSQLPage *page, const char *message, const char *reason);


