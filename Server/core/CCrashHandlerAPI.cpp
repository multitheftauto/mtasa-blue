/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CCrashHandlerAPI.cpp
 *  PURPOSE:     Crash handler API methods
 *
 *  "Debugging Applications" (Microsoft Press)
 *  Copyright (c) 1997-2000 John Robbins -- All rights reserved.
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

/*----------------------------------------------------------------------
CONDITIONAL COMPILATION :
    WORK_AROUND_SRCLINE_BUG - Define this symbol to work around the
                              SymGetLineFromAddr bug; this bug causes PDB
                              file lookups to fail after the first lookup.
                              This bug is fixed in DBGHELP.DLL, but I
                              still keep the workaround for users
                              who might need to use the old IMAGEHLP.DLL
                              versions.
----------------------------------------------------------------------*/

#include "StdInc.h"
#include "CCrashHandlerAPI.h"
#ifdef WIN32

#ifdef _M_IX86
    #include <SharedUtil.Detours.h>
#endif

/*//////////////////////////////////////////////////////////////////////
                      File Scope Global Variables
//////////////////////////////////////////////////////////////////////*/
// The custom unhandled exception filter (crash handler)
static PFNCHFILTFN g_pfnCallBack = NULL;

// The original unhandled exception filter
static LPTOP_LEVEL_EXCEPTION_FILTER g_pfnOrigFilt = NULL;

/*//////////////////////////////////////////////////////////////////////
                    File Scope Function Declarations
//////////////////////////////////////////////////////////////////////*/
// The exception handler
LONG __stdcall CrashHandlerExceptionFilter(EXCEPTION_POINTERS* pExPtrs);

/*//////////////////////////////////////////////////////////////////////
                            Destructor Class
//////////////////////////////////////////////////////////////////////*/
// See the note in MEMDUMPVALIDATOR.CPP about automatic classes.
// Turn off warning : initializers put in library initialization area
#pragma warning (disable : 4073)
#pragma init_seg(lib)
class CleanUpCrashHandler
{
public:
    CleanUpCrashHandler() {}
    ~CleanUpCrashHandler()
    {
        if (NULL != g_pfnOrigFilt)
        {
            // Restore the original unhandled exception filter.
            SetUnhandledExceptionFilter(g_pfnOrigFilt);
        }
    }
};

// The static class
static CleanUpCrashHandler g_cBeforeAndAfter;

/*//////////////////////////////////////////////////////////////////////
                 Crash Handler Function Implementation
//////////////////////////////////////////////////////////////////////*/

BOOL __stdcall SetCrashHandlerFilter(PFNCHFILTFN pFn)
{
    // A NULL parameter unhooks the callback.
    if (NULL == pFn)
    {
        if (NULL != g_pfnOrigFilt)
        {
            // Restore the original unhandled exception filter.
            SetUnhandledExceptionFilter(g_pfnOrigFilt);
            g_pfnOrigFilt = NULL;
            g_pfnCallBack = NULL;
        }
    }
    else
    {
        if (TRUE == IsBadCodePtr((FARPROC)pFn))
        {
            return (FALSE);
        }
        g_pfnCallBack = pFn;

        // If a custom crash handler isn't already in use, enable
        // CrashHandlerExceptionFilter and save the original unhandled
        // exception filter.
        if (NULL == g_pfnOrigFilt)
        {
            g_pfnOrigFilt = SetUnhandledExceptionFilter(CrashHandlerExceptionFilter);

#ifdef _M_IX86
            // Stop the OS from turning off our handler
            // Ref: https://www.codeproject.com/Articles/154686/SetUnhandledExceptionFilter-and-the-C-C-Runtime-Li
            LPTOP_LEVEL_EXCEPTION_FILTER(WINAPI * RedirectedSetUnhandledExceptionFilter)
            (LPTOP_LEVEL_EXCEPTION_FILTER) = [](LPTOP_LEVEL_EXCEPTION_FILTER /*ExceptionInfo*/) -> LPTOP_LEVEL_EXCEPTION_FILTER {
                // When the CRT calls SetUnhandledExceptionFilter with NULL parameter
                // our handler will not get removed.
                return 0;
            };
            static_assert(std::is_same_v<decltype(RedirectedSetUnhandledExceptionFilter), decltype(&SetUnhandledExceptionFilter)>,
                          "invalid type of RedirectedSetUnhandledExceptionFilter");

            DetourLibraryFunction("kernel32.dll", "SetUnhandledExceptionFilter", RedirectedSetUnhandledExceptionFilter);
#endif
        }
    }
    return (TRUE);
}

LONG __stdcall CrashHandlerExceptionFilter(EXCEPTION_POINTERS* pExPtrs)
{
    LONG lRet = EXCEPTION_CONTINUE_SEARCH;

    // If the exception is an EXCEPTION_STACK_OVERFLOW, there isn't much
    // you can do because the stack is blown. If you try to do anything,
    // the odds are great that you'll just double-fault and bomb right
    // out of your exception filter. Although I don't recommend doing so,
    // you could play some games with the stack register and
    // manipulate it so that you could regain enough space to run these
    // functions. Of course, if you did change the stack register, you'd
    // have problems walking the stack.
    // I take the safe route and make some calls to OutputDebugString here.
    // I still might double-fault, but because OutputDebugString does very
    // little on the stack (something like 8-16 bytes), it's worth a
    // shot. You can have your users download Mark Russinovich's
    // DebugView/Enterprise Edition (www.sysinternals.com) so they can
    // at least tell you what they see.
    // The only problem is that I can't even be sure there's enough
    // room on the stack to convert the instruction pointer.
    // Fortunately, EXCEPTION_STACK_OVERFLOW doesn't happen very often.

    // Note that I still call your crash handler. I'm doing the logging
    // work here in case the blown stack kills your crash handler.
    if (EXCEPTION_STACK_OVERFLOW == pExPtrs->ExceptionRecord->ExceptionCode)
    {
        OutputDebugString("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        OutputDebugString("EXCEPTION_STACK_OVERFLOW occurred\n");
        OutputDebugString("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    }

    __try
    {
        if (NULL != g_pfnCallBack)
        {
            // Check that the crash handler still exists in memory
            // before I call it. The user might have forgotten to
            // unregister, and the crash handler is invalid because
            // it got unloaded. If some other function loaded
            // back into the same address, however, there isn't much
            // I can do.
            if (FALSE == IsBadCodePtr((FARPROC)g_pfnCallBack))
            {
                lRet = g_pfnCallBack(pExPtrs);
            }
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        lRet = EXCEPTION_CONTINUE_SEARCH;
    }
    return (lRet);
}

#endif  // #ifdef WIN32
