/** \file   htsqlFunction.imp.h

    \brief  Deals with calling HT/SQL functions.

 *  STEPS FOR HOW TO RUN an HT/SQL FUNCTION:
 *  ---------------------
 *  ObjectGeneric data type is defined as a storage for local variables and function arguments of the HTSQL function currently running or to be called.
 *  Example usage can be found in ajax.c file
 *  \n
 *  - Step.1:\n
 *  >   Before a new function call is made, Caller is to backup the current value of the rootPostObjectGeneric and currentPreObjectGeneric variables defined 
 *	>	in SodiumSession object.  \n
 *  >   Then MUST always set currentMKSession->rootPostObjectGeneric and currentMKSession->currentPostObjectGeneric to NULL since they are used in 
 *	>	createANewFunctionCallStackStartPoint function. \n
 *	>
 *  >         ObjectGeneric *prevRootPostObjectGeneric    = page->rootPostObjectGeneric;    \n
 *  >         ObjectGeneric *prevCurrentPostObjectGeneric = page->currentPostObjectGeneric; \n
 *  >         page->rootPostObjectGeneric         = NULL;                                       \n
 *  >         page->currentPostObjectGeneric      = NULL;                                       \n
 *  \n
 *  - Step.2:\n
 *      Then, C Programmer must call createANewFunctionCallStackStartPoint(..) function in order to create rootPostObjectGeneric object.
 *      The object created newly will be the start point for local variables and function arguments. 
 *		This value should be assigned currentMKSession->currentPostObjectGeneric as well.\n
 *
 *  >         page->rootPostObjectGeneric         = createANewFunctionCallStackStartPoint(currentMKSession, page, functionNameToBeCalled);\n
 *  \n
 *  - Step.3\n
 *     Add block name of the caller's block name as variable named "datablockname" to the stack by using following c function. (This not optional!)\n
 *	>			
 *  >         postFunctionAddArgumentsOrVariables(currentMKSession, page, POST_OBJECT_GENERIC_TYPE_FUNCTION_ARGS or POST_OBEJCT_TYPE_FUNCTION_VARIABLE, "datablockname", name-of-the-datablock);
 *  \n
 *  - Step.4\n
 *      Add HT/SQL function arguments and control block items's names/values to the stack by using following c function.\n
 *  >         postFunctionAddArgumentsOrVariables(..);
 *  \n
 *  - Step.5\n
 *      Run the HTSQL function by calling the c function below.
 *      If return value is NULL, it means "functionNameToBeCalled" HT/SQL function was not found and not executed. Otherwise, it is found and executed.
 *		In that case return value points the function pre generic object structure. DO NOT DESTROY RETURN VALUE.
 *  \n
 *  >         PreObjectFunction *functionExecuted = runPostFunction(currentMKSession, page, functionNameToBeCalled, TRUE or FALSE);
 *  \n
 *  - Step.6.\n
 *      After execution the HTSQL function ends, the "function return value" will be in the object instance 
 *		(functionExecuted->preObjectFunctionReturnValue->varcharValue) which was created in Step.2.\n
 *	>
 *  >       functionExecuted->preObjectFunctionReturnValue->varcharValue
 *
 *  \n
 *  - Step.7:\n
 *      After execution of the HT/SQL function ends, caller is responsible of destroying all the linked list. 
 *		If this is ignored, the start point of the linked list will be lost and memory leak is inevitable.\n
 *	>
 *  >         cleanUpFunctionStack(currentMKSession, page);
 *  \n
 *  - Step.8:\n
 *      After execution of the HTSQL function and destroying linked list, Caller, as a last step, MUST then restore the orjinal value of the
 *      currentMKSession->rootPostObjectGeneric and currentMKSession->currentPostObjectGeneric variable.\n
 *
 *  >         page->rootPostObjectGeneric     = prevRootPostObjectGeneric;\n
 *  >         page->currentPostObjectGeneric  = prevCurrentPostObjectGeneric;
 *  \n
 *  Initially, rootPostObjectGeneric variable is assigned NULL. Procedure can call another procedure or itself again and again.
 *  IF all steps above is followed properly, rootPostObjectGeneric variable should be NULL since all callers are responsible to store old values back.
 *
 */

#pragma once


#include "htsql.h"

#include "tagDataBlock.h"
#include "tagConnection.h"


/** \brief Have look at "STEPS FOR CALLING HT/SQL FUNCTION:" section above.
 *
 * \param currentSodiumSession MKSession*
 * \return void
 *
 */
void      cleanUpFunctionStack(SodiumSession *currentMKSession, HTSQLPage *page);


/** \brief Finds HT/SQL function, then execute it.
 *
 * \param currentSodiumSession  MKSession*
 * \param htsqlFunctionName const char*\n     HT/SQL function name. Should not include return type, parameter list of the function to be called.
 * \param ignoreNotFound    bool\n            TRUE: "Function not found" error message does not be produced if it is not exists.
                                              This parameter is required for some optional HT/SQL functions (etc. "post_query" function of Data Block). If it is not exists, this does not mean an error. It is optional.
 * \return PreObjectFunction*                 PreObjectFunction pointer. It is the pointer to a HT/SQL function pre object instance which was executed.
 *                                            If function is optional and not found returns NULL.
 *
 */
PreObjectFunction		*runPostFunction(SodiumSession *currentMKSession, HTSQLPage *page, const char *htsqlFunctionName, bool ignoreNotFound);

void					runPostFunctionFast(SodiumSession *currentMKSession, HTSQLPage *page, PreObjectGeneric *htsqlFuncionInstance);

/** \brief	Before executing ant user HTSQL function, sets the active database connection for the SQL statements which are executed in that function 
*
*/
void					setActiveDatabaseConnectionForHTSQLFunction(SodiumSession *currentMKSession, HTSQLPage *page, const char *htsqlFunctionName);

/** \brief  Have look at "STEPS FOR CALLING HT/SQL FUNCTION:" section above.
 *
 * \param currentSodiumSession MKSession*
 * \param functionName const char*
 * \return ObjectGeneric*
 *
 */
void           createANewFunctionCallStackStartPoint(SodiumSession *currentMKSession, HTSQLPage *page, const char *functionName);
/** \brief Have look at "STEPS FOR CALLING HT/SQL FUNCTION:" section above.
 *
 * \param currentSodiumSession MKSession*
 * \param pVariableName const char*
 * \param pPreObjectVariableValue const char*
 * \return void
 *
 */
void                   postFunctionAddArgumentsOrVariables(SodiumSession *currentMKSession, HTSQLPage *page, ObjectGenericType genericObjectType, PREOBJECTVARIABLETYPE primitiveVariableType, const char *pVariableName, const char *pPreObjectVariableValue);

/** \brief Before calling any HT/SQL or native function in GET/Page parse phase, this function should be called in case of they maybe send java script
 *  response to the browser as response action.
 *
 * \param currentSodiumSession MKSession*
 * \return void
 *
 *  __This function should be called once for each GET/Page request__ and just after "<body>" tag.
 * \n
 *  If it is called more than once or called during different location other than specified above, client ajax actions most probably be broken.
 */
void                       __initializeClientJavaScriptReponse(SodiumSession *currentMKSession);


void    addDataBlockDataItemsToLocalStack(SodiumSession *currentMKSession, HTSQLPage *page, const char *blockname, const char *procedureName);
void addControlBlockDataItemsToLocalStack(SodiumSession *currentMKSession, HTSQLPage *page);

/** \brief	Returns the datablock name of a fully-qualified user function. 
			CALLER MUST RELEASE returned pointer if it is not NULL. 
			Does not check the datablock is exists or not.
			If htsqlFunctionName is NULL or does not contain datablock name, returns NULL.
			htsqlFunctionName value cannot have more than one "." character. If it is, returns NULL because more than 1 period means that
			this is a fully-qualified user function name for a trigger function of an element in a control block (Ex: imagine that 
			there is a <tree> element in a <controlblock> which has "tree_node_selected" trigger function)
*   Example:
*		Returns "deps" if "deps.getProductName" is passed as htsqlFunctionName. 
*	
*
* \param currentSodiumSession  MKSession*
* \param htsqlFunctionName const char*\n     HT/SQL function name. Should not include return type, parameter list of the function to be called.
* \return char*											
*
*/
char *getBlocknameFromHTSQLFunctionName(SodiumSession *currentMKSession, HTSQLPage *page, const char *htsqlFunctionName);

