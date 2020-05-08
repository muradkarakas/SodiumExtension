/*! \file post.imp.h
    \brief Functions and data definitions for HT/SQL file parsing and execution.
*/



typedef struct yy_buffer_state *YY_BUFFER_STATE;


typedef void* yyscan_t;



int                                                   postlex(yyscan_t yyscanner);
int                                              postget_leng(yyscan_t yyscanner);
int                                               postlex_init(yyscan_t* scanner);
char                                            *postget_text(yyscan_t yyscanner);
int                                            postget_lineno(yyscan_t yyscanner);
int                                           postlex_destroy(yyscan_t yyscanner);
void                                  *postParseAlloc(void* (*allocProc)(size_t));
void                                postset_in(FILE *_in_str, yyscan_t yyscanner);
void                              postParseFree(void *p, void (*freeProc)(void*));
void                post_delete_buffer ( YY_BUFFER_STATE b , yyscan_t yyscanner );
YY_BUFFER_STATE      post_scan_string ( const char *yy_str , yyscan_t yyscanner );
void                                          postSyntaxError(SodiumSession *currentMKSession, TokenPost t);
void                                      postTokenDestructor(SodiumSession *currentMKSession, TokenPost t);

void executeUpdateStatement(SodiumSession *currentMKSession, HTSQLPage *page, const char *insertSQL);
void executeInsertStatement(SodiumSession *currentMKSession, HTSQLPage *page, const char *insertSQL);
void executeDeleteStatement(SodiumSession *currentMKSession, HTSQLPage *page, const char *deleteSQL);
void executeDMLStatement(SodiumSession *currentMKSession, HTSQLPage *page, SQL_COMMAND_TYPE commandType, const char *singleSQLCommand);
void executePLSQLBlock(SodiumSession *currentMKSession, HTSQLPage *page, const char *plSQL);


void reduceifTrueAndFalseBlock(SodiumSession *currentMKSession, HTSQLPage *page, const char *bodyLines);


/**
 * \brief   Parse and execute a HT/SQL source code.
 *
 * \param   Char pointer to HT/SQL source code.
 * \param   HTTP user session structure.
 * \return  Nothing
 *
 *  This function is used to run a sqlx function defined in *.sqlx file (codebehind file). Or
 *  Any string which is similar to sqlx function body syntax. That is, the string passed to that c function should
 *  start with '{' character (token is POST_BEGIN) and end with '}' character (token POST_END). after that <<EOF>> character should come (token POST_END_OF_FILE).
 *
 *
 *  Find "procbody" production rule in the lemon grammer file "lemon.post.yy" file for detail.
 */
void          parseAndExecutePostString(SodiumSession *currentMKSession, HTSQLPage *page, const char *preObjectFunctionBody);

/** \brief Returns current data block information.
 *
 * \param     Current session object instance.
 * \return    TAGDataBlock *
 *
 *  While reducing production rules in HT/SQL parsing phase, this function returns the data block information which is the requester of execution of the HT/SQL function.
 */
TAGDataBlock *  getPostCurrentTAGDataBlock(SodiumSession *currentMKSession, HTSQLPage *page);


char *convertToNullTerminatedChar(char *str, int sizeOfStr);

//void resetPostExecutionEnv(SodiumSession *currentMKSession, HTSQLPage *page);

void stopPostParserWithMessage(SodiumSession *currentMKSession, HTSQLPage *page, const char *message, const char *reason);

