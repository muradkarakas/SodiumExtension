/** \file   createConnection.h

    \brief  Contains c implementation of built-in "create_connection" HT/SQL function.
*/
#pragma once


/** \brief  C implementation of the "create_connection" built-in HT/SQL function.
 *
 * \param currentMKSession MKSession*
 * \param pConnName const char*
 * \param pInstanceName const char*
 * \param pUserName const char*
 * \param pPassword const char*
 * \return bool
 *
 *  Opens a new database connection by creating corresponding C structure (TAGConnection) and DBInt data structure.
 *  If pConnName is already created, recreates it.
 *  This function is especially designed to be run in "connection_not_found" trigger.  
 */
const char * 
CreateConnection(
    SodiumSession * session, 
    HTSQLPage * page, 
    const char * pConnName, 
    const char * dbType, 
    const char * hostName, 
    const char * pInstanceName, 
    const char * pUserName, 
    const char * pPassword);


