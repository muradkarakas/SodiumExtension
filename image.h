#ifndef PUTIMAGE_H_INCLUDED
#define PUTIMAGE_H_INCLUDED



void    __sendPutImageResponse(SodiumSession *currentMKSession, HTSQLPage *page, const char *boundry);
void    __sendPutImageSuccessfulMessage(SodiumSession *currentMKSession, const char *fileName, size_t writenFileSize);

size_t
WritePostedBinaryDataToFile(
    SodiumSession* session,
    HTSQLPage* page,
    const char* boundry,
    const char* imageInputName,
    const char* datablockname,
    const char* inputName
);

void sendHTSQLNoImageFileContentWithErrorMessage(SodiumSession *currentMKSession, HTSQLPage *page, const char *errorMessage);


/** \brief  Execute select sql command in order to get image blob column value from database and send it to the client as "image content type"
 *
 * \param currentSodiumSession MKSession*
 * \param param YuarelParam*
 * \param totalHTTPFormVariableCount int
 * \return void
 *
 *  param[ INDEX ].key :
 *      1 ->    blockname
 *      2 ->    itemnane
 *      3 ->    rowid
 *
 */
void serveAsGetRequestGetImage(SodiumSession *currentMKSession, HTSQLPage *page);



#endif // PUTIMAGE_H_INCLUDED
