/**
 * This file is part of Sodium Language project
 *
 * Copyright © 2020 Murad Karakaþ <muradkarakas@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License v3.0
 * as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 *	https://choosealicense.com/licenses/gpl-3.0/
 */

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
    const char* pDatabaseName,
    const char * pUserName, 
    const char * pPassword);


