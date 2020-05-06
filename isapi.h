/*! \file   isapi.h
    \brief  Functions and data definitions for all incoming HTTP requests.

    Whether or not it is a AJAX or PAGE request, all incoming requests are firstly processed by the functions defined here, then dispatched to other functions.
*/

#pragma once


#include "htsql.h"



void
PrintPageProccessTime(
    SodiumSession* session
);

/** \brief IIS ISAPI extension spesification dictates that all ISAPI extensions should have this function in a dll file and export it.
 *
 * \param   dwFlags DWORD
 * \return __declspec(dllexport) BOOL WINAPI
 *
 */
//DLL_EXPORT  BOOL WINAPI                                   TerminateExtension(DWORD dwFlags);



/** \brief  __Entry point of all http__ request. IIS ISAPI extension spesification dictates that all ISAPI extensions should have this function in a dll file and export it.
 *
 * \param
 * \param
 * \return
 *
 */
//DLL_EXPORT DWORD WINAPI        HttpExtensionProc(LPEXTENSION_CONTROL_BLOCK clpECB);

