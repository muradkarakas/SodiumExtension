/**
    \file   mkUtils.h

    \brief  Utility functions.

*/

#pragma once


#include "SodiumSession.h"


void                      mkPrint(SodiumSession *currentMKSession, const char *first, ...);

/** \brief	Count how many '\n' are there in pStr
*
*/
int mkStrCountEnter(char *pStr);

/** \brief	It clears '\r', '\t', '\n', ' ', '(', '\)'
*/
void mkStrSanitize(char *pStr);

/** \brief It is used to get Physical Path of a file relative to htsql.dll
*
* \return char*    Return value should be released.
*
*  __CALLER IS RESPONSIBLE FOR RELEASING RETURN VALUE__
*/
wchar_t*
GetPhysicalPathForFile(
    SodiumSession* session,
    const char* fileRelativePath
);


/** \brief	It is used to generate null terminated "Session Id" string in length of strlength. 
 *
 * \return char*    strlength + 2 byte is allocated for the return string. Return value should be released. 
 *
 *  __CALLER IS RESPONSIBLE FOR RELEASING RETURN VALUE__
 */
char *  mkStrRandom(SodiumSession *currentMKSession, int strLength, const char *sourceFile, int sourceLine);

/** \brief It is the project's only function authorized to execute SQL statement.
 *
 * \param currentMKSession
 * \param pTAGDataBlock
 * \param ociStatement
 * \param pSql
 * \param bool isErrorMsgWillBeShown
 * \return char *                           Returns new row id if commandType is INSERT_COMMAND, otherwise is NULL. Return value
 *
 *  If an error is occurred:
 *      Creates a new log record,
 *      Set the clientMessage property of the currentSodiumSession instance. So, before calling, clientMessage should be clear by calling clearClientMessage(SodiumSession *currentMKSession).
 *      Sends the Oracle error message string to the browser as plain text.
 *  \n
 *
 *  Caller may use code block below to get error message.
 *
 >  clearClientMessage(currentMKSession);    <br>
 >  excute mkOCI_Execute_Stmt(....);
 >  char *msg = getClientMessage(currentMKSession);
 >  if (msg) {
 >      __sendErrorResponse(currentMKSession, "Veritabanýna yazýlamadý", msg);
 >      mkFree(currentMKSession->heapHandle, msg);
 >  }
 *
 */
//const char *mkOCI_ExecuteStmt(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTAGDataBlock, OCI_Statement *ociStatement, char *pSql, SQL_COMMAND_TYPE commandType, bool isErrorMsgWillBeShown);

void                           mkDebug(SodiumSession *currentMKSession, SESSION_LOG_DETAIL pEventType, const char *first, ...);
unsigned long                                              mkHash(unsigned char *str);

char *                                                mkItoa(size_t value, char* result);
/** \brief   Finds and replaces all instances of " character with ' character.
 *
 * \param pStr
 * \return void
 *
 *  Does not return char*. Instead, it modifies the original string.
 */
void                                                 mkStrReplaceQuotation(char *pStr);

/** \brief   Finds and replaces all instances of \t character with ' ' character.
*
* \param pStr
* \return void
*
*  Does not return char*. Instead, it modifies the original string.
*/
void													 mkStrReplaceTab(char *pStr);

/** \brief   Finds and replaces all instances of \n and \r character with ' ' character.
*
* \param pStr
* \return void
*
*  Does not return char*. Instead, it modifies the original string.
*/
void													 mkStrReplaceEnter(char *pStr);
void                                               mkAssert(char *first, va_list argp);
/** \brief Removes " characters at the beginning and end of the string.
 *
 * \param pPropertyValue
 * \return      Returns the address of pPropertyValue parameter.
 *
 *  Example:
 *  >   stripTAGPropertyValue("refresh_block")
 *  \n
 *  Output is:  refresh_block
 *  \n
 *  Does not allocate a new memory. It modifies orginal string.
 */
char *                                     
stripTAGPropertyValue(
    char *pPropertyValue
);

wchar_t*
stripTAGPropertyValueW(
    wchar_t* pPropertyValue
);

void    mkConvertFloatToChar(size_t pFloatValue, char *pStrBuffer, int pSizeOfStrBufer);

char	* mkDoubleToChar(double value, char* result);


/** \brief  Escapes some characters with javascript equavalent.
 *
 * \param javaScriptCode const char*
 * \return  char *
 *
 *  __CALLER IS RESPONSIBLE FOR REALSING THE RETURN VALUE__
 * \n
 *  " escaped as \"
 * \n
 *  enter character escaped as "\n"
 *
 */
char *   mkEscapeJavaScriptQuoteCharacter(SodiumSession *currentMKSession, const char *javaScriptCode);

/** \brief Returns the "data block name" of a data block item reference string. Caller is responsible for the syntax of exp.
 *
 * \param exp char*
 * \return char*
 *
 *  example: returns "emps" for the string below
 >  :emps.emp_id
 *
 *  \n
 *  __CALLER SHOULD FREE RETURN VALUE__
 */
char*  getDataBlockNameOfExpression(SodiumSession *currentMKSession, const char* exp);

/** \brief Returns the "item name" of a data block item reference string. Caller is responsible for the syntax of exp.
 *
 * \param exp char*
 * \return char*
 *
 */
char*  getItemNameOfExpression(SodiumSession *currentMKSession, const char* exp);



/** \brief
 *
 * \param currentSodiumSession MKSession*
 * \param str char*
 * \return char*
 *  __CALLER SHOULD FREE RETURN VALUE__
 */
char* url_encode(SodiumSession *currentMKSession, const char *str);
/** \brief
 *
 * \param currentSodiumSession MKSession*
 * \param str char*
 * \return char*
 *  __CALLER SHOULD FREE RETURN VALUE__
 */
char* url_decode(SodiumSession *currentMKSession, const char *str);

char to_hex(char code);
char from_hex(char ch);



void
BindSQLVariables(
    SodiumSession* session,
    HTSQLPage* page,
    TAGDataBlock* pTAGDataBlock,
    DBInt_Statement* stm,
    SQL_COMMAND_TYPE commandType
);

/** \brief  Returns a pointer to the beginning of the pos'th utf8 codepoint in the buffer at "source"
 *
 * \param currentSodiumSession MKSession*
 * \param s const char*
 * \param pos size_t
 * \return char*
 *
 */
char *mkUTF8index(SodiumSession *currentMKSession, char *source, size_t pos);


/** \brief __CALLER IS RESPONSEIBLE TO RELEASE RETURN VALUE__
 *
 * \param currentSodiumSession MKSession*
 * \return char*
 *
 */
wchar_t * getCurrentDirectory(SodiumSession *currentMKSession);

/** \brief __CALLER IS RESPONSEIBLE TO RELEASE RETURN VALUE__
*
*       Returns CSS file URL for frmx file. Functio create a new string. Only difference is ".frmx" is replaced with ".css".
*       if current file URL does not contain ".frmx" returns a string but unknown content.
*
* \param currentSodiumSession MKSession*
* \return char*             Return value may be NULL.
*
*/
char * 
GetCurrentCSSFileName(
    SodiumSession *session
);

/** \brief __CALLER IS RESPONSEIBLE TO RELEASE RETURN VALUE__
 *
 * \param currentSodiumSession MKSession*
 * \return char*            Return value may be NULL.
 *
 */
wchar_t*
CalculateCurrentSQLXFileName(
    SodiumSession* session
);

/** \brief Send a file's content to the client as is. Content of the file can be either binary or text.
 *
 * \param currentSodiumSession MKSession*
 * \param fileName const char*
 * \return void
 *
 *  fileName parameter must not contain folder name. It will be translated to the real path.
 */
void
SendFileContent(
    SodiumSession* session,
    HTSQLPage* page,
    const wchar_t* fileName
);


void initializeRandomNumberGenerator();

