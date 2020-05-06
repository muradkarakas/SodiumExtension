/** \file   onLogon.h

    \brief  Contains required c functions for implementation of the HT/SQL "on_logon" trigger.
*/
#pragma once

#include "htsql.h"


/** \brief  This C function runs "on_logon" HT/SQL trigger function if exists.
 *
 * \param currentMKSession MKSession*
 * \return void
 *
 *  If an HT/SQL application needs a database connection (TAGConnection instance) but there is no one available, "on_logon" HT/SQL trigger is called to notify
 *  application to create a new one. HT/SQL programmer should create a new connection and corresponding TAGConnection instance by using
 *  the command below in that trigger :
 *
 * >    void on_logon() {   \n
 * >        create_connection("yugs.world", "ORCL.WORLD", "hr", "hr");  \n
 * >    }
 *
 *  createConnection(SodiumSession   *currentMKSession, const char *pConnName, const char *pInstanceName, const char *pUserName, const char *pPassword);
 */
void runOnLogonHTSQLTriggerFunction(SodiumSession *currentMKSession, HTSQLPage *page, const char *connectionName);



