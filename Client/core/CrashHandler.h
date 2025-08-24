/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CrashHandler.h
 *  PURPOSE:     Header file for crash handling functions
 *
 *  THIS FILE IS PROPERTY OF
 *  "Debugging Applications" (Microsoft Press)
 *  Copyright (c) 1997-2000 John Robbins -- All rights reserved.
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

/*----------------------------------------------------------------------
"Debugging Applications" (Microsoft Press)
Copyright (c) 1997-2000 John Robbins -- All rights reserved.
----------------------------------------------------------------------*/

#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define BUGSUTIL_DLLINTERFACE

#ifdef __cplusplus
extern "C"
{
#endif  //__cplusplus

    /*//////////////////////////////////////////////////////////////////////
                                Type Definitions
    //////////////////////////////////////////////////////////////////////*/
    // The type for the filter function called by the Crash Handler API.
    typedef LONG(__stdcall* PFNCHFILTFN)(EXCEPTION_POINTERS* pExPtrs);

    /*//////////////////////////////////////////////////////////////////////
                       Crash Handler Function Definitions
    //////////////////////////////////////////////////////////////////////*/

    /*----------------------------------------------------------------------
    FUNCTION        :   SetCrashHandlerFilter
    DISCUSSION      :
        Sets the filter function that will be called when there is a fatal
    crash.  The passed in function will only be called if the crash is one
    of the modules passed to AddCrashHandlerLimitModule.  If no modules have
    been added to narrow down the interested modules then the callback
    filter function will always be called.
    PARAMETERS      :
        pFn - A valid code pointer.  If this is NULL, then the Crash Handler
              filter function is removed.
    RETURNS         :
        1 - The crash handler was properly set.
        0 - There was a problem.
    ----------------------------------------------------------------------*/
    BOOL BUGSUTIL_DLLINTERFACE __stdcall SetCrashHandlerFilter(PFNCHFILTFN pFn);

#ifdef __cplusplus
}
#endif  //__cplusplus
