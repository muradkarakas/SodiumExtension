/** \file   mkSession.h
    \brief  HTTP Session management functions, types and variables.
*/   
  
#pragma once 
      
  
#include "SodiumSession.h"  

void		clearClientMessage(SodiumSession *currentMKSession, HTSQLPage *page);
const char*	getClientMessage(SodiumSession *currentMKSession, HTSQLPage *page);
void		setClientMessage(SodiumSession *currentMKSession, HTSQLPage *page, const char *message);
void		initUserSessionDefaults(SodiumSession *currentMKSession);


 
/** \brief Returns the session cookie value (which is named "mksession").
 *         CALLER MUST RELEASE THE RETURN VALUE WITH __FREE__ FUNCTION
 *
 * \param lpECB LPEXTENSION_CONTROL_BLOCK
 * \param const char *  cookieName
 * \return char *
 *
 *  !!! MKIMPLEMENT this function behaves as if only one cookie name is used. It does not check the cookie name.
 *     This function is not reentrant. It must be called in single threaded environment !!!
 */
char *  findHTTPCookieByName(LPEXTENSION_CONTROL_BLOCK lpECB, const char *cookieName);


/** \brief Before processing http requests, does all jobs for Session and related memory objects management.
 *
 * \param clpECB LPEXTENSION_CONTROL_BLOCK
 * \return SodiumSession *      Session instance for Current request.
 *
 *  This function does;\n
    - if the current request is first one, creates a new SodiumSession instance by calling createMKSession(LPEXTENSION_CONTROL_BLOCK clpECB).\n
    - if the current request is a subsequent request of the previous one, then returns the SodiumSession instance previously created.\n
      But, before processing further, all memory objects (except SodiumSession instance), are destroyed/released. This situation is called as __Session cleaning__.
      ( calls cleanSession(SodiumSession *session) ) \n
      This is required in order to reflects the current contents of files to be read. May be they are modified.
    - if the current request is a subsequent request of the previous one, but server is restarted between two requests, creates an new SodiumSession instance with new Session Id.
      In another world, If server goes down but browser has not been closed, we get the same cookie. However, All objects created for these session is not in memory anymore.
      So, creates a new session with a new session id. This situation is called as <b>"Http request from dead session"</b> and logged to log file.
 */
SodiumSession *                     getHTTPSessionInstance(LPEXTENSION_CONTROL_BLOCK clpECB);

SodiumSession   *findSessionById();

void		__sendKillSessionNativeResponse(SodiumSession *currentMKSession);


void		initDebugInfoFileProperty(SodiumSession *currentMKSession, wchar_t *fileURI);

void                 destroyIdleSessions();
/** \brief Destorys/Releases all objects created by previous for a specific session.
 *
 * \param session   MKSession*
 * \return void
 *
 */
SodiumSession *               destroySession(SodiumSession *session);
void               destroySessionGETPage(SodiumSession *currentMKSession, HTSQLPage *page);
void              destroySessionPOSTPage(SodiumSession *currentMKSession, HTSQLPage *page);

void
DestroySessionPages(
    SodiumSession *session
);

void                  LoadControllerFile(SodiumSession *currentMKSession);

void        setTheme(SodiumSession *currentMKSession, const char *theme);

/*	Caller must free return value*/
char		*getThemeHeader(SodiumSession *currentMKSession);

void		decreaseDebugIndentionCount(SodiumSession *currentMKSession);
void		increaseDebugIndentionCount(SodiumSession *currentMKSession);

void
ResetDebugData(
    SodiumSession* session
);