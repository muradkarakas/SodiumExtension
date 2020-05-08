/** \file   userHTSQLFunction.h

    \brief  Used to call a HT/SQL function (let call it as "user function") from another HT/SQL function.
*/

#pragma once




/**
 * \brief Runs an HT/SQL (user function) function from another user function.
 *
 * \param currentMKSession MKSession*
 * \param functionToRun const char*
 * \param parameterCount int
 * \param ...
 * \return void
 *
 * This function actually calls the "runPostFunction" c function. Behavior is similar to "runPostFunction" if user function
 * which is called has no parameter (argument). If user function to be called has argument(s), they are added as argument to the local stack.
 *
 */
PreObjectGeneric		*callUserFunction(SodiumSession *currentMKSession, HTSQLPage *page, const char *functionToRun, int parameterCount, ...);


/**
 * \brief   Returns the current user function return value. Return value is set after "return ...;" statement is executed in user function.
 *
 * \param currentMKSession MKSession*
 * \return PreObjectVariable                                   *
 *
 */
PreObjectVariable *             getCurrentUserFunctionReturnValue(SodiumSession *currentMKSession, HTSQLPage *page);
void                            setCurrentUserFunctionReturnValue(SodiumSession *currentMKSession, HTSQLPage *page, const char *retValue);

