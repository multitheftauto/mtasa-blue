/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CrashHandler.cpp
*  PURPOSE:     Crash handling functions
*
*  THIS FILE IS PROPERTY OF
*  "Debugging Applications" (Microsoft Press)
*  Copyright (c) 1997-2000 John Robbins -- All rights reserved.
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
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
#include "CrashHandler.h"
#include <dbghelp.h>
#include <TLHELP32.h>

using namespace std;

#define ASSERT
#define TRACE0
#define TRACE1
#define VERIFY

DWORD BUGSUTIL_DLLINTERFACE __stdcall
      BSUGetModuleBaseName ( HANDLE  hProcess   ,
                             HMODULE hModule    ,
                             LPTSTR  lpBaseName ,
                             DWORD   nSize       );



DWORD BUGSUTIL_DLLINTERFACE __stdcall
        BSUSymInitialize ( DWORD  dwPID          ,
                           HANDLE hProcess       ,
                           PSTR   UserSearchPath ,
                           BOOL   fInvadeProcess  ) ;


/*//////////////////////////////////////////////////////////////////////
                           File Scope Defines
//////////////////////////////////////////////////////////////////////*/
// The maximum symbol size handled in the module
#define MAX_SYM_SIZE  256
#define BUFF_SIZE 1024
#define SYM_BUFF_SIZE 512

/*//////////////////////////////////////////////////////////////////////
                      File Scope Global Variables
//////////////////////////////////////////////////////////////////////*/
// The custom unhandled exception filter (crash handler)
static PFNCHFILTFN g_pfnCallBack = NULL ;

// The original unhandled exception filter
static LPTOP_LEVEL_EXCEPTION_FILTER g_pfnOrigFilt = NULL ;

// The array of modules to limit crash handler to
static HMODULE * g_ahMod = NULL ;
// The size, in items, of g_ahMod
static UINT g_uiModCount = 0 ;

// The static buffer returned by various functions. This buffer
// allows data to be transferred without using the stack.
static TCHAR g_szBuff [ BUFF_SIZE ] ;

// The static symbol lookup buffer
static BYTE g_stSymbol [ SYM_BUFF_SIZE ] ;

// The static source file and line number structure
static IMAGEHLP_LINE g_stLine ;

// The stack frame used in walking the stack
static STACKFRAME g_stFrame ;

// The flag indicating that the symbol engine has been initialized
static BOOL g_bSymEngInit = FALSE ;

/*//////////////////////////////////////////////////////////////////////
                    File Scope Function Declarations
//////////////////////////////////////////////////////////////////////*/
// The exception handler
LONG __stdcall CrashHandlerExceptionFilter ( EXCEPTION_POINTERS *
                                             pExPtrs              ) ;

// Converts a simple exception to a string value
LPCTSTR ConvertSimpleException ( DWORD dwExcept ) ;

// The internal function that does all the stack walking
LPCTSTR __stdcall
            InternalGetStackTraceString ( DWORD                dwOpts  ,
                                          EXCEPTION_POINTERS * pExPtrs );

// The internal SymGetLineFromAddr function
BOOL InternalSymGetLineFromAddr ( IN  HANDLE          hProcess        ,
                                  IN  DWORD           dwAddr          ,
                                  OUT PDWORD          pdwDisplacement ,
                                  OUT PIMAGEHLP_LINE  Line            );

// Initializes the symbol engine if needed
void InitSymEng ( void ) ;

// Cleans up the symbol engine if needed
void CleanupSymEng ( void ) ;

/*//////////////////////////////////////////////////////////////////////
                            Destructor Class
//////////////////////////////////////////////////////////////////////*/
// See the note in MEMDUMPVALIDATOR.CPP about automatic classes.
// Turn off warning : initializers put in library initialization area
#pragma warning (disable : 4073)
#pragma init_seg(lib)
class CleanUpCrashHandler
{
public  :
    CleanUpCrashHandler ( void )
    {
    }
    ~CleanUpCrashHandler ( void )
    {
        // Are there any outstanding memory allocations?
        if ( NULL != g_ahMod )
        {
            VERIFY ( HeapFree ( GetProcessHeap ( ) ,
                                0                  ,
                                g_ahMod             ) ) ;
            g_ahMod = NULL ;
        }
        if ( NULL != g_pfnOrigFilt )
        {
            // Restore the original unhandled exception filter.
            SetUnhandledExceptionFilter ( g_pfnOrigFilt ) ;
        }
    }
} ;

// The static class
static CleanUpCrashHandler g_cBeforeAndAfter ;

/*//////////////////////////////////////////////////////////////////////
                 Crash Handler Function Implementation
//////////////////////////////////////////////////////////////////////*/

BOOL __stdcall SetCrashHandlerFilter ( PFNCHFILTFN pFn )
{
    // A NULL parameter unhooks the callback.
    if ( NULL == pFn )
    {
        if ( NULL != g_pfnOrigFilt )
        {
            // Restore the original unhandled exception filter.
            SetUnhandledExceptionFilter ( g_pfnOrigFilt ) ;
            g_pfnOrigFilt = NULL ;
            if ( NULL != g_ahMod )
            {
                free ( g_ahMod ) ;
                g_ahMod = NULL ;
            }
            g_pfnCallBack = NULL ;
        }
    }
    else
    {
        ASSERT ( FALSE == IsBadCodePtr ( (FARPROC)pFn ) ) ;
        if ( TRUE == IsBadCodePtr ( (FARPROC)pFn ) )
        {
            return ( FALSE ) ;
        }
        g_pfnCallBack = pFn ;

        // If a custom crash handler isn't already in use, enable
        // CrashHandlerExceptionFilter and save the original unhandled
        // exception filter.
        if ( NULL == g_pfnOrigFilt )
        {
            g_pfnOrigFilt =
               SetUnhandledExceptionFilter(CrashHandlerExceptionFilter);
        }
    }
    return ( TRUE ) ;
}

BOOL __stdcall AddCrashHandlerLimitModule ( HMODULE hMod )
{
    // Check the obvious cases.
    ASSERT ( NULL != hMod ) ;
    if ( NULL == hMod )
    {
        return ( FALSE ) ;
    }

    // Allocate a temporary array. This array must be allocated from
    // memory that's guaranteed to be around even if the process is
    // toasting. If the process is toasting, the RTL heap probably isn't
    // safe, so I allocate the temporary array from the process heap.
    HMODULE * phTemp = (HMODULE*)
                    HeapAlloc ( GetProcessHeap ( )                 ,
                                HEAP_ZERO_MEMORY |
                                   HEAP_GENERATE_EXCEPTIONS        ,
                                (sizeof(HMODULE)*(g_uiModCount+1))  ) ;
    ASSERT ( NULL != phTemp ) ;
    if ( NULL == phTemp )
    {
        TRACE0 ( "Serious trouble in the house! - "
                 "HeapAlloc failed!!!\n"       );
        return ( FALSE ) ;
    }

    if ( NULL == g_ahMod )
    {
        g_ahMod = phTemp ;
        g_ahMod[ 0 ] = hMod ;
        g_uiModCount++ ;
    }
    else
    {
        // Copy the old values.
        CopyMemory ( phTemp     ,
                     g_ahMod    ,
                     sizeof ( HMODULE ) * g_uiModCount ) ;
        // Free the old memory.
        VERIFY ( HeapFree ( GetProcessHeap ( ) , 0 , g_ahMod ) ) ;
        g_ahMod = phTemp ;
        g_ahMod[ g_uiModCount ] = hMod ;
        g_uiModCount++ ;
    }
    return ( TRUE ) ;
}

UINT __stdcall GetLimitModuleCount ( void )
{
    return ( g_uiModCount ) ;
}

int __stdcall GetLimitModulesArray ( HMODULE * pahMod , UINT uiSize )
{
    int iRet ;

    __try
    {
        ASSERT ( FALSE == IsBadWritePtr ( pahMod ,
                                          uiSize * sizeof ( HMODULE ) ) ) ;
        if ( TRUE == IsBadWritePtr ( pahMod ,
                                     uiSize * sizeof ( HMODULE ) ) )
        {
            iRet = GLMA_BADPARAM ;
            __leave ;
        }

        if ( uiSize < g_uiModCount )
        {
            iRet = GLMA_BUFFTOOSMALL ;
            __leave ;
        }

        CopyMemory ( pahMod     ,
                     g_ahMod    ,
                     sizeof ( HMODULE ) * g_uiModCount ) ;

        iRet = GLMA_SUCCESS ;
    }
    __except ( EXCEPTION_EXECUTE_HANDLER )
    {
        iRet = GLMA_FAILURE ;
    }
    return ( iRet ) ;
}

LONG __stdcall CrashHandlerExceptionFilter (EXCEPTION_POINTERS* pExPtrs)
{
    LONG lRet = EXCEPTION_CONTINUE_SEARCH ;

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
    if ( EXCEPTION_STACK_OVERFLOW ==
                              pExPtrs->ExceptionRecord->ExceptionCode )
    {
        OutputDebugString ( "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" ) ;
        OutputDebugString ( "EXCEPTION_STACK_OVERFLOW occurred\n" ) ;
        OutputDebugString ( "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" ) ;
    }

    __try
    {

        if ( NULL != g_pfnCallBack )
        {

            // The symbol engine has to be initialized here so that
            // I can look up the base module information for the
            // crash address as well as get the symbol engine
            // ready.
            InitSymEng ( ) ;

            // Check the g_ahMod list.
            BOOL bCallIt = FALSE ;
            if ( 0 == g_uiModCount )
            {
                bCallIt = TRUE ;
            }
            else
            {
                HINSTANCE hBaseAddr = (HINSTANCE)
                      SymGetModuleBase((HANDLE)GetCurrentProcessId ( ) ,
                                       (DWORD)pExPtrs->
                                            ExceptionRecord->
                                                      ExceptionAddress);
                if ( NULL != hBaseAddr )
                {
                    for ( UINT i = 0 ; i < g_uiModCount ; i ++ )
                    {
                        if ( hBaseAddr == g_ahMod[ i ] )
                        {
                            bCallIt = TRUE ;
                            break ;
                        }
                    }
                }
            }
            if ( TRUE == bCallIt )
            {
                // Check that the crash handler still exists in memory
                // before I call it. The user might have forgotten to
                // unregister, and the crash handler is invalid because
                // it got unloaded. If some other function loaded
                // back into the same address, however, there isn't much
                // I can do.
                ASSERT ( FALSE == IsBadCodePtr((FARPROC)g_pfnCallBack));
                if ( FALSE == IsBadCodePtr ( (FARPROC)g_pfnCallBack ) )
                {
                    lRet = g_pfnCallBack ( pExPtrs ) ;
                }
            }
            else
            {
                // Call the previous filter but only after it checks
                // out. I'm just being a little paranoid.
                ASSERT ( FALSE == IsBadCodePtr((FARPROC)g_pfnOrigFilt));
                if ( FALSE == IsBadCodePtr ( (FARPROC)g_pfnOrigFilt ) )
                {
                    lRet = g_pfnOrigFilt ( pExPtrs ) ;
                }
            }
            CleanupSymEng ( ) ;
        }
    }
    __except ( EXCEPTION_EXECUTE_HANDLER )
    {
        lRet = EXCEPTION_CONTINUE_SEARCH ;
    }
    return ( lRet ) ;
}

/*//////////////////////////////////////////////////////////////////////
         EXCEPTION_POINTER Translation Functions Implementation
//////////////////////////////////////////////////////////////////////*/

LPCTSTR __stdcall GetFaultReason ( EXCEPTION_POINTERS * pExPtrs )
{
    ASSERT ( FALSE == IsBadReadPtr ( pExPtrs ,
                                     sizeof ( EXCEPTION_POINTERS ) ) ) ;
    if ( TRUE == IsBadReadPtr ( pExPtrs ,
                                sizeof ( EXCEPTION_POINTERS ) ) )
    {
        TRACE0 ( "Bad parameter to GetFaultReasonA\n" ) ;
        return ( NULL ) ;
    }

    // The variable that holds the return value
    LPCTSTR szRet ;

    __try
    {

        // Initialize the symbol engine in case it isn't initialized.
        InitSymEng ( ) ;

        // The current position in the buffer
        int iCurr = 0 ;
        // A temporary value holder. This holder keeps the stack usage to a
        // minimum.
        DWORD dwTemp ;

        iCurr += BSUGetModuleBaseName ( GetCurrentProcess ( ) ,
                                        NULL                  ,
                                        g_szBuff              ,
                                        BUFF_SIZE              ) ;

        iCurr += wsprintf ( g_szBuff + iCurr , _T ( " caused an " ) ) ;

        dwTemp = (DWORD)
            ConvertSimpleException(pExPtrs->ExceptionRecord->
                                                         ExceptionCode);

        if ( NULL != dwTemp )
        {
            iCurr += wsprintf ( g_szBuff + iCurr ,
                                _T ( "%s" )      ,
                                dwTemp            ) ;
        }
        else
        {
            iCurr += (FormatMessage( FORMAT_MESSAGE_IGNORE_INSERTS |
                                            FORMAT_MESSAGE_FROM_HMODULE,
                                     GetModuleHandle (_T("NTDLL.DLL")) ,
                                     pExPtrs->ExceptionRecord->
                                                          ExceptionCode,
                                     0                                 ,
                                     g_szBuff + iCurr                  ,
                                     BUFF_SIZE ,
                                     0                                 )
                      * sizeof ( TCHAR ) ) ;
        }

        ASSERT ( iCurr < ( BUFF_SIZE - MAX_PATH ) ) ;

        iCurr += wsprintf ( g_szBuff + iCurr , _T ( " in module " ) ) ;

        dwTemp =
            SymGetModuleBase ( (HANDLE)GetCurrentProcessId ( ) ,
                               (DWORD)pExPtrs->ExceptionRecord->
                                                    ExceptionAddress ) ;
        ASSERT ( NULL != dwTemp ) ;

        if ( NULL == dwTemp )
        {
            iCurr += wsprintf ( g_szBuff + iCurr , _T ( "<UNKNOWN>" ) );
        }
        else
        {
            iCurr += BSUGetModuleBaseName ( GetCurrentProcess ( ) ,
                                            (HINSTANCE)dwTemp     ,
                                            g_szBuff + iCurr      ,
                                            BUFF_SIZE - iCurr      ) ;
        }

    #ifdef _WIN64
        iCurr += wsprintf ( g_szBuff + iCurr    ,
                            _T ( " at %016X" )   ,
                            pExPtrs->ExceptionRecord->ExceptionAddress);
    #else
        iCurr += wsprintf ( g_szBuff + iCurr                ,
                            _T ( " at %04X:%08X" )          ,
                            pExPtrs->ContextRecord->SegCs   ,
                            pExPtrs->ExceptionRecord->ExceptionAddress);
    #endif

        ASSERT ( iCurr < ( BUFF_SIZE - 200 ) ) ;

        // Start looking up the exception address.
        PIMAGEHLP_SYMBOL pSym = (PIMAGEHLP_SYMBOL)&g_stSymbol ;
        FillMemory ( pSym , NULL , SYM_BUFF_SIZE ) ;
        pSym->SizeOfStruct = sizeof ( IMAGEHLP_SYMBOL ) ;
        pSym->MaxNameLength = SYM_BUFF_SIZE - sizeof ( IMAGEHLP_SYMBOL);

        DWORD dwDisp ;
        if ( TRUE ==
              SymGetSymFromAddr ( (HANDLE)GetCurrentProcessId ( )     ,
                                  (DWORD)pExPtrs->ExceptionRecord->
                                                     ExceptionAddress ,
                                  &dwDisp                             ,
                                  pSym                                ))
        {
            iCurr += wsprintf ( g_szBuff + iCurr , _T ( ", " ) ) ;

            // Copy no more of the symbol information than there's
            // room for.
            dwTemp = lstrlen ( pSym->Name ) ;
            // Make sure there's enough room for the longest symbol
            // and the displacement.
            if ( (int)dwTemp > ( ( BUFF_SIZE - iCurr) -
                                 ( MAX_SYM_SIZE + 50 )  ) )
            {
                lstrcpyn ( g_szBuff + iCurr      ,
                           pSym->Name            ,
                           BUFF_SIZE - iCurr - 1  ) ;
                // Gotta leave now
                szRet = g_szBuff ;
                __leave ;
            }
            else
            {
                if ( dwDisp > 0 )
                {
                    iCurr += wsprintf ( g_szBuff + iCurr          ,
                                        _T ( "%s()+%04d byte(s)" ),
                                        pSym->Name                ,
                                        dwDisp                     ) ;
                }
                else
                {
                    iCurr += wsprintf ( g_szBuff + iCurr ,
                                        _T ( "%s " )     ,
                                        pSym->Name        ) ;
                }
            }
        }
        else
        {
            // If the symbol wasn't found, the source and line won't
            // be found either, so leave now.
            szRet = g_szBuff ;
            __leave ;
        }

        ASSERT ( iCurr < ( BUFF_SIZE - 200 ) ) ;

        // Look up the source file and line number.
        ZeroMemory ( &g_stLine , sizeof ( IMAGEHLP_LINE ) ) ;
        g_stLine.SizeOfStruct = sizeof ( IMAGEHLP_LINE ) ;

        if ( TRUE ==
              InternalSymGetLineFromAddr ((HANDLE)
                                            GetCurrentProcessId ( )    ,
                                          (DWORD)pExPtrs->
                                                    ExceptionRecord->
                                                      ExceptionAddress ,
                                          &dwDisp                      ,
                                          &g_stLine                   ))
        {
            iCurr += wsprintf ( g_szBuff + iCurr , _T ( ", " ) ) ;

            // Copy no more of the source file and line number
            // information than there's room for.
            dwTemp = lstrlen ( g_stLine.FileName ) ;
            if ( (int)dwTemp > ( BUFF_SIZE - iCurr -
                                 MAX_PATH - 50       ) )
            {
                lstrcpyn ( g_szBuff + iCurr      ,
                           g_stLine.FileName     ,
                           BUFF_SIZE - iCurr - 1  ) ;
                // Gotta leave now
                szRet = g_szBuff ;
                __leave ;
            }
            else
            {
                if ( dwDisp > 0 )
                {
                    iCurr += wsprintf ( g_szBuff + iCurr              ,
                                      _T("%s, line %04d+%04d byte(s)"),
                                        g_stLine.FileName             ,
                                        g_stLine.LineNumber           ,
                                        dwDisp                       );
                }
                else
                {
                    iCurr += wsprintf ( g_szBuff + iCurr     ,
                                        _T ( "%s, line %04d"),
                                        g_stLine.FileName    ,
                                        g_stLine.LineNumber   ) ;
                }
            }
        }
        szRet = g_szBuff ;
    }
    __except ( EXCEPTION_EXECUTE_HANDLER )
    {
        ASSERT ( !"Crashed in GetFaultReason" ) ;
        szRet = NULL ;
    }
    return ( szRet ) ;
}

BOOL __stdcall GetFaultReasonVB ( EXCEPTION_POINTERS * pExPtrs ,
                                  LPTSTR               szBuff  ,
                                  UINT                 uiSize   )
{
    ASSERT ( FALSE == IsBadWritePtr ( szBuff , uiSize ) ) ;
    if ( TRUE == IsBadWritePtr ( szBuff , uiSize ) )
    {
        return ( FALSE ) ;
    }

    LPCTSTR szRet ;

    __try
    {

        szRet = GetFaultReason ( pExPtrs ) ;

        ASSERT ( NULL != szRet ) ;
        if ( NULL == szRet )
        {
            __leave ;
        }
        lstrcpyn ( szBuff   ,
                   szRet    ,
                   min ( (UINT)lstrlen ( szRet ) + 1, uiSize ) ) ;
    }
    __except ( EXCEPTION_EXECUTE_HANDLER )
    {
        szRet = NULL ;
    }
    return ( NULL != szRet ) ;
}


LPCTSTR BUGSUTIL_DLLINTERFACE __stdcall
             GetFirstStackTraceString ( DWORD                dwOpts  ,
                                        EXCEPTION_POINTERS * pExPtrs  )
{
    // All the error checking is in the InternalGetStackTraceString
    // function.

    // Initialize the STACKFRAME structure.
    ZeroMemory ( &g_stFrame , sizeof ( STACKFRAME ) ) ;

    #ifdef _X86_
    g_stFrame.AddrPC.Offset       = pExPtrs->ContextRecord->Eip ;
    g_stFrame.AddrPC.Mode         = AddrModeFlat                ;
    g_stFrame.AddrStack.Offset    = pExPtrs->ContextRecord->Esp ;
    g_stFrame.AddrStack.Mode      = AddrModeFlat                ;
    g_stFrame.AddrFrame.Offset    = pExPtrs->ContextRecord->Ebp ;
    g_stFrame.AddrFrame.Mode      = AddrModeFlat                ;
    #else
    g_stFrame.AddrPC.Offset       = (DWORD)pExPtrs->ContextRecord->Fir ;
    g_stFrame.AddrPC.Mode         = AddrModeFlat ;
    g_stFrame.AddrReturn.Offset   =
                                   (DWORD)pExPtrs->ContextRecord->IntRa;
    g_stFrame.AddrReturn.Mode     = AddrModeFlat ;
    g_stFrame.AddrStack.Offset    =
                                   (DWORD)pExPtrs->ContextRecord->IntSp;
    g_stFrame.AddrStack.Mode      = AddrModeFlat ;
    g_stFrame.AddrFrame.Offset    =
                                   (DWORD)pExPtrs->ContextRecord->IntFp;
    g_stFrame.AddrFrame.Mode      = AddrModeFlat ;
    #endif

    return ( InternalGetStackTraceString ( dwOpts , pExPtrs ) ) ;
}

LPCTSTR BUGSUTIL_DLLINTERFACE __stdcall
             GetNextStackTraceString ( DWORD                dwOpts  ,
                                       EXCEPTION_POINTERS * pExPtrs  )
{
    // All error checking is in InternalGetStackTraceString.
    // Assume that GetFirstStackTraceString has already initialized the
    // stack frame information.
    return ( InternalGetStackTraceString ( dwOpts , pExPtrs ) ) ;
}

BOOL __stdcall CH_ReadProcessMemory ( HANDLE                      ,
                                      LPCVOID lpBaseAddress       ,
                                      LPVOID  lpBuffer            ,
                                      DWORD   nSize               ,
                                      LPDWORD lpNumberOfBytesRead  )
{
    return ( ReadProcessMemory ( GetCurrentProcess ( ) ,
                                 lpBaseAddress         ,
                                 lpBuffer              ,
                                 nSize                 ,
                                 lpNumberOfBytesRead    ) ) ;
}

// The internal function that does all the stack walking
LPCTSTR __stdcall
          InternalGetStackTraceString ( DWORD                dwOpts  ,
                                        EXCEPTION_POINTERS * pExPtrs  )
{

    ASSERT ( FALSE == IsBadReadPtr ( pExPtrs                      ,
                                     sizeof ( EXCEPTION_POINTERS ) ) ) ;
    if ( TRUE == IsBadReadPtr ( pExPtrs                      ,
                                sizeof ( EXCEPTION_POINTERS ) ) )
    {
        TRACE0 ( "GetStackTraceString - invalid pExPtrs!\n" ) ;
        return ( NULL ) ;
    }

    // The value that is returned
    LPCTSTR szRet ;
    // A temporary variable for all to use. This variable saves
    // stack space.
    DWORD dwTemp ;
    // The module base address. I look this up right after the stack
    // walk to ensure that the module is valid.
    DWORD dwModBase ;

    __try
    {
        // Initialize the symbol engine in case it isn't initialized.
        InitSymEng ( ) ;

#ifdef _WIN64
#define CH_MACHINE IMAGE_FILE_MACHINE_IA64
#else
#define CH_MACHINE IMAGE_FILE_MACHINE_I386
#endif
        // Note:  If the source file and line number functions are used,
        //        StackWalk can cause an access violation.
        BOOL bSWRet = StackWalk ( CH_MACHINE                        ,
                                  (HANDLE)GetCurrentProcessId ( )   ,
                                  GetCurrentThread ( )              ,
                                  &g_stFrame                        ,
                                  pExPtrs->ContextRecord            ,
                                  (PREAD_PROCESS_MEMORY_ROUTINE)
                                               CH_ReadProcessMemory ,
                                  SymFunctionTableAccess            ,
                                  SymGetModuleBase                  ,
                                  NULL                               ) ;
        if ( ( FALSE == bSWRet ) || ( 0 == g_stFrame.AddrFrame.Offset ))
        {
            szRet = NULL ;
            __leave ;
        }

        // Before I get too carried away and start calculating
        // everything, I need to double-check that the address returned
        // by StackWalk really exists. I've seen cases in which
        // StackWalk returns TRUE but the address doesn't belong to
        // a module in the process.
        dwModBase = SymGetModuleBase ( (HANDLE)GetCurrentProcessId ( ),
                                        g_stFrame.AddrPC.Offset       );
        // Commented out this as it seems to prevent the callstack for the gta process being printed
        /*
        if ( 0 == dwModBase )
        {
            szRet = NULL ;
            __leave ;
        }
        */

        int iCurr = 0 ;

        // At a minimum, put in the address.
#ifdef _WIN64
        iCurr += wsprintf ( g_szBuff + iCurr        ,
                            _T ( "0x%016X" )         ,
                            g_stFrame.AddrPC.Offset  ) ;
#else
        iCurr += wsprintf ( g_szBuff + iCurr              ,
                            _T ( "%04X:%08X" )            ,
                            pExPtrs->ContextRecord->SegCs ,
                            g_stFrame.AddrPC.Offset        ) ;
#endif

        // Output the parameters?
        if ( GSTSO_PARAMS == ( dwOpts & GSTSO_PARAMS ) )
        {
            iCurr += wsprintf ( g_szBuff + iCurr          ,
                                _T ( " (0x%08X 0x%08X "\
                                      "0x%08X 0x%08X)"  ) ,
                                g_stFrame.Params[ 0 ]     ,
                                g_stFrame.Params[ 1 ]     ,
                                g_stFrame.Params[ 2 ]     ,
                                g_stFrame.Params[ 3 ]      ) ;
        }
        // Output the module name.
        if ( GSTSO_MODULE == ( dwOpts & GSTSO_MODULE ) )
        {
            iCurr += wsprintf ( g_szBuff + iCurr  , _T ( " " ) ) ;

            ASSERT ( iCurr < ( BUFF_SIZE - MAX_PATH ) ) ;
            iCurr += BSUGetModuleBaseName ( GetCurrentProcess ( ) ,
                                            (HINSTANCE)dwModBase  ,
                                            g_szBuff + iCurr      ,
                                            BUFF_SIZE - iCurr      ) ;
        }

        ASSERT ( iCurr < ( BUFF_SIZE - MAX_PATH ) ) ;
        DWORD dwDisp ;

        // Output the symbol name?
        if ( GSTSO_SYMBOL == ( dwOpts & GSTSO_SYMBOL ) )
        {

            // Start looking up the exception address.
            PIMAGEHLP_SYMBOL pSym = (PIMAGEHLP_SYMBOL)&g_stSymbol ;
            ZeroMemory ( pSym , SYM_BUFF_SIZE ) ;
            pSym->SizeOfStruct = sizeof ( IMAGEHLP_SYMBOL ) ;
            pSym->MaxNameLength = SYM_BUFF_SIZE -
                                  sizeof ( IMAGEHLP_SYMBOL ) ;

            if ( TRUE ==
                  SymGetSymFromAddr ( (HANDLE)GetCurrentProcessId ( ) ,
                                      g_stFrame.AddrPC.Offset         ,
                                      &dwDisp                         ,
                                      pSym                            ))
            {
                iCurr += wsprintf ( g_szBuff + iCurr , _T ( ", " ) ) ;

                // Copy no more symbol information than there's room for.
                dwTemp = lstrlen ( pSym->Name ) ;
                if ( dwTemp > (DWORD)( BUFF_SIZE - iCurr -
                                     ( MAX_SYM_SIZE + 50 ) ) )
                {
                    lstrcpyn ( g_szBuff + iCurr      ,
                               pSym->Name            ,
                               BUFF_SIZE - iCurr - 1  ) ;
                    // Gotta leave now
                    szRet = g_szBuff ;
                    __leave ;
                }
                else
                {
                    if ( dwDisp > 0 )
                    {
                        iCurr += wsprintf ( g_szBuff + iCurr         ,
                                            _T( "%s()+%04d byte(s)") ,
                                            pSym->Name               ,
                                            dwDisp                   );
                    }
                    else
                    {
                        iCurr += wsprintf ( g_szBuff + iCurr ,
                                            _T ( "%s" )      ,
                                            pSym->Name        ) ;
                    }
                }
            }
            else
            {
                // If the symbol wasn't found, the source file and line
                // number won't be found either, so leave now.
                szRet = g_szBuff ;
                __leave ;
            }

        }

        ASSERT ( iCurr < ( BUFF_SIZE - MAX_PATH ) ) ;

        // Output the source file and line number information?
        if ( GSTSO_SRCLINE == ( dwOpts & GSTSO_SRCLINE ) )
        {
            ZeroMemory ( &g_stLine , sizeof ( IMAGEHLP_LINE ) ) ;
            g_stLine.SizeOfStruct = sizeof ( IMAGEHLP_LINE ) ;

            if ( TRUE ==
                   InternalSymGetLineFromAddr ( (HANDLE)
                                                  GetCurrentProcessId(),
                                                g_stFrame.AddrPC.Offset,
                                                &dwDisp                ,
                                                &g_stLine             ))
            {
                iCurr += wsprintf ( g_szBuff + iCurr , _T ( ", " ) ) ;

                // Copy no more of the source file and line number
                // information than there's room for.
                dwTemp = lstrlen ( g_stLine.FileName ) ;
                if ( dwTemp > (DWORD)( BUFF_SIZE - iCurr -
                                       ( MAX_PATH + 50     ) ) )
                {
                    lstrcpyn ( g_szBuff + iCurr      ,
                               g_stLine.FileName     ,
                               BUFF_SIZE - iCurr - 1  ) ;
                    // Gotta leave now
                    szRet = g_szBuff ;
                    __leave ;
                }
                else
                {
                    if ( dwDisp > 0 )
                    {
                        iCurr += wsprintf(g_szBuff + iCurr             ,
                                       _T("%s, line %04d+%04d byte(s)"),
                                          g_stLine.FileName            ,
                                          g_stLine.LineNumber          ,
                                          dwDisp                     );
                    }
                    else
                    {
                        iCurr += wsprintf ( g_szBuff + iCurr       ,
                                            _T ( "%s, line %04d" ) ,
                                            g_stLine.FileName      ,
                                            g_stLine.LineNumber     ) ;
                    }
                }
            }
        }

        szRet = g_szBuff ;
    }
    __except ( EXCEPTION_EXECUTE_HANDLER )
    {
        ASSERT ( !"Crashed in InternalGetStackTraceString" ) ;
        szRet = NULL ;
    }
    return ( szRet ) ;
}

BOOL __stdcall
           GetFirstStackTraceStringVB ( DWORD                dwOpts  ,
                                        EXCEPTION_POINTERS * pExPtrs ,
                                        LPTSTR               szBuff  ,
                                        UINT                 uiSize   )
{
    ASSERT ( FALSE == IsBadWritePtr ( szBuff , uiSize ) ) ;
    if ( TRUE == IsBadWritePtr ( szBuff , uiSize ) )
    {
        return ( FALSE ) ;
    }

    LPCTSTR szRet ;

    __try
    {
        szRet = GetFirstStackTraceString ( dwOpts , pExPtrs ) ;
        if ( NULL == szRet )
        {
            __leave ;
        }
        lstrcpyn ( szBuff   ,
                   szRet    ,
                   min ( (UINT)lstrlen ( szRet ) + 1 , uiSize ) ) ;
    }
    __except ( EXCEPTION_EXECUTE_HANDLER )
    {
        szRet = NULL ;
    }
    return ( NULL != szRet ) ;
}

BOOL __stdcall
           GetNextStackTraceStringVB ( DWORD                dwOpts  ,
                                       EXCEPTION_POINTERS * pExPtrs ,
                                       LPTSTR               szBuff  ,
                                       UINT                 uiSize   )
{
    ASSERT ( FALSE == IsBadWritePtr ( szBuff , uiSize ) ) ;
    if ( TRUE == IsBadWritePtr ( szBuff , uiSize ) )
    {
        return ( FALSE ) ;
    }

    LPCTSTR szRet ;

    __try
    {
        szRet = GetNextStackTraceString ( dwOpts , pExPtrs ) ;
        if ( NULL == szRet )
        {
            __leave ;
        }
        lstrcpyn ( szBuff   ,
                   szRet    ,
                   min ( (UINT)lstrlen ( szRet ) + 1 , uiSize ) ) ;
    }
    __except ( EXCEPTION_EXECUTE_HANDLER )
    {
        szRet = NULL ;
    }
    return ( NULL != szRet ) ;
}

LPCTSTR __stdcall GetRegisterString ( EXCEPTION_POINTERS * pExPtrs )
{
    // Check the parameter.
    ASSERT ( FALSE == IsBadReadPtr ( pExPtrs                      ,
                                     sizeof ( EXCEPTION_POINTERS ) ) ) ;
    if ( TRUE == IsBadReadPtr ( pExPtrs                      ,
                                sizeof ( EXCEPTION_POINTERS ) ) )
    {
        TRACE0 ( "GetRegisterString - invalid pExPtrs!\n" ) ;
        return ( NULL ) ;
    }

#ifdef _WIN64
    ASSERT ( !"IA64 is not supported (YET!) " ) ;
#else
    // This call puts 48 bytes on the stack, which could be a problem when
    // the stack is blown.
    wsprintf ( g_szBuff ,
               _T ("EAX=%08X  EBX=%08X  ECX=%08X  EDX=%08X  ESI=%08X\n"\
                   "EDI=%08X  EBP=%08X  ESP=%08X  EIP=%08X  FLG=%08X\n"\
                   "CS=%04X   DS=%04X  SS=%04X  ES=%04X   "\
                   "FS=%04X  GS=%04X" ) ,
                   pExPtrs->ContextRecord->Eax      ,
                   pExPtrs->ContextRecord->Ebx      ,
                   pExPtrs->ContextRecord->Ecx      ,
                   pExPtrs->ContextRecord->Edx      ,
                   pExPtrs->ContextRecord->Esi      ,
                   pExPtrs->ContextRecord->Edi      ,
                   pExPtrs->ContextRecord->Ebp      ,
                   pExPtrs->ContextRecord->Esp      ,
                   pExPtrs->ContextRecord->Eip      ,
                   pExPtrs->ContextRecord->EFlags   ,
                   pExPtrs->ContextRecord->SegCs    ,
                   pExPtrs->ContextRecord->SegDs    ,
                   pExPtrs->ContextRecord->SegSs    ,
                   pExPtrs->ContextRecord->SegEs    ,
                   pExPtrs->ContextRecord->SegFs    ,
                   pExPtrs->ContextRecord->SegGs     ) ;

#endif
    return ( g_szBuff ) ;

}

BOOL __stdcall GetRegisterStringVB ( EXCEPTION_POINTERS * pExPtrs ,
                                     LPTSTR               szBuff  ,
                                     UINT                 uiSize   )
{
    ASSERT ( FALSE == IsBadWritePtr ( szBuff , uiSize ) ) ;
    if ( TRUE == IsBadWritePtr ( szBuff , uiSize ) )
    {
        return ( FALSE ) ;
    }

    LPCTSTR szRet ;

    __try
    {
        szRet = GetRegisterString ( pExPtrs ) ;
        if ( NULL == szRet )
        {
            __leave ;
        }
        lstrcpyn ( szBuff   ,
                   szRet    ,
                   min ( (UINT)lstrlen ( szRet ) + 1 , uiSize ) ) ;
    }
    __except ( EXCEPTION_EXECUTE_HANDLER )
    {
        szRet = NULL ;
    }
    return ( NULL != szRet ) ;

}

LPCTSTR ConvertSimpleException ( DWORD dwExcept )
{
    switch ( dwExcept )
    {
        case EXCEPTION_ACCESS_VIOLATION         :
            return ( _T ( "EXCEPTION_ACCESS_VIOLATION" ) ) ;
        break ;

        case EXCEPTION_DATATYPE_MISALIGNMENT    :
            return ( _T ( "EXCEPTION_DATATYPE_MISALIGNMENT" ) ) ;
        break ;

        case EXCEPTION_BREAKPOINT               :
            return ( _T ( "EXCEPTION_BREAKPOINT" ) ) ;
        break ;

        case EXCEPTION_SINGLE_STEP              :
            return ( _T ( "EXCEPTION_SINGLE_STEP" ) ) ;
        break ;

        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED    :
            return ( _T ( "EXCEPTION_ARRAY_BOUNDS_EXCEEDED" ) ) ;
        break ;

        case EXCEPTION_FLT_DENORMAL_OPERAND     :
            return ( _T ( "EXCEPTION_FLT_DENORMAL_OPERAND" ) ) ;
        break ;

        case EXCEPTION_FLT_DIVIDE_BY_ZERO       :
            return ( _T ( "EXCEPTION_FLT_DIVIDE_BY_ZERO" ) ) ;
        break ;

        case EXCEPTION_FLT_INEXACT_RESULT       :
            return ( _T ( "EXCEPTION_FLT_INEXACT_RESULT" ) ) ;
        break ;

        case EXCEPTION_FLT_INVALID_OPERATION    :
            return ( _T ( "EXCEPTION_FLT_INVALID_OPERATION" ) ) ;
        break ;

        case EXCEPTION_FLT_OVERFLOW             :
            return ( _T ( "EXCEPTION_FLT_OVERFLOW" ) ) ;
        break ;

        case EXCEPTION_FLT_STACK_CHECK          :
            return ( _T ( "EXCEPTION_FLT_STACK_CHECK" ) ) ;
        break ;

        case EXCEPTION_FLT_UNDERFLOW            :
            return ( _T ( "EXCEPTION_FLT_UNDERFLOW" ) ) ;
        break ;

        case EXCEPTION_INT_DIVIDE_BY_ZERO       :
            return ( _T ( "EXCEPTION_INT_DIVIDE_BY_ZERO" ) ) ;
        break ;

        case EXCEPTION_INT_OVERFLOW             :
            return ( _T ( "EXCEPTION_INT_OVERFLOW" ) ) ;
        break ;

        case EXCEPTION_PRIV_INSTRUCTION         :
            return ( _T ( "EXCEPTION_PRIV_INSTRUCTION" ) ) ;
        break ;

        case EXCEPTION_IN_PAGE_ERROR            :
            return ( _T ( "EXCEPTION_IN_PAGE_ERROR" ) ) ;
        break ;

        case EXCEPTION_ILLEGAL_INSTRUCTION      :
            return ( _T ( "EXCEPTION_ILLEGAL_INSTRUCTION" ) ) ;
        break ;

        case EXCEPTION_NONCONTINUABLE_EXCEPTION :
            return ( _T ( "EXCEPTION_NONCONTINUABLE_EXCEPTION" ) ) ;
        break ;

        case EXCEPTION_STACK_OVERFLOW           :
            return ( _T ( "EXCEPTION_STACK_OVERFLOW" ) ) ;
        break ;

        case EXCEPTION_INVALID_DISPOSITION      :
            return ( _T ( "EXCEPTION_INVALID_DISPOSITION" ) ) ;
        break ;

        case EXCEPTION_GUARD_PAGE               :
            return ( _T ( "EXCEPTION_GUARD_PAGE" ) ) ;
        break ;

        case EXCEPTION_INVALID_HANDLE           :
            return ( _T ( "EXCEPTION_INVALID_HANDLE" ) ) ;
        break ;

        default :
            return ( NULL ) ;
        break ;
    }
}

BOOL InternalSymGetLineFromAddr ( IN  HANDLE          hProcess        ,
                                  IN  DWORD           dwAddr          ,
                                  OUT PDWORD          pdwDisplacement ,
                                  OUT PIMAGEHLP_LINE  Line            )
{
#ifdef WORK_AROUND_SRCLINE_BUG

    // The problem is that the symbol engine finds only those source
    // line addresses (after the first lookup) that fall exactly on
    // a zero displacement. I'll walk backward 100 bytes to
    // find the line and return the proper displacement.
    DWORD dwTempDis = 0 ;
    while ( FALSE == SymGetLineFromAddr ( hProcess        ,
                                          dwAddr -
                                           dwTempDis      ,
                                          pdwDisplacement ,
                                          Line             ) )
    {
        dwTempDis += 1 ;
        if ( 100 == dwTempDis )
        {
            return ( FALSE ) ;
        }
    }

    // I found the line, and the source line information is correct, so
    // change the displacement if I had to search backward to find
    // the source line.
    if ( 0 != dwTempDis )
    {
        *pdwDisplacement = dwTempDis ;
    }
    return ( TRUE ) ;

#else  // WORK_AROUND_SRCLINE_BUG
    return ( SymGetLineFromAddr ( hProcess         ,
                                  dwAddr           ,
                                  pdwDisplacement  ,
                                  Line              ) ) ;
#endif
}

// Initializes the symbol engine if needed
void InitSymEng ( void )
{
    if ( FALSE == g_bSymEngInit )
    {
        // Set up the symbol engine.
        DWORD dwOpts = SymGetOptions ( ) ;

        // Turn on line loading and deferred loading.
        SymSetOptions ( dwOpts                |
                        SYMOPT_DEFERRED_LOADS |
                        SYMOPT_LOAD_LINES      ) ;

        // Force the invade process flag no matter what operating system
        // I'm on.
        HANDLE hPID = (HANDLE)GetCurrentProcessId ( ) ;
        VERIFY ( BSUSymInitialize ( (DWORD)hPID ,
                                    hPID        ,
                                    NULL        ,
                                    TRUE         ) ) ;
        g_bSymEngInit = TRUE ;
    }
}

// Cleans up the symbol engine if needed
void CleanupSymEng ( void )
{
    if ( TRUE == g_bSymEngInit )
    {
        VERIFY ( SymCleanup ( (HANDLE)GetCurrentProcessId ( ) ) ) ;
        g_bSymEngInit = FALSE ;
    }
}




/*//////////////////////////////////////////////////////////////////////
                           File Scope Globals
//////////////////////////////////////////////////////////////////////*/
// Indicates that the version information is valid.
static BOOL g_bHasVersion = FALSE ;
// Indicates NT or 95/98.
static BOOL g_bIsNT = TRUE ;

BOOL __stdcall IsNT ( void )
{
    if ( TRUE == g_bHasVersion )
    {
        return ( TRUE == g_bIsNT ) ;
    }

    OSVERSIONINFO stOSVI ;

    memset ( &stOSVI , NULL , sizeof ( OSVERSIONINFO ) ) ;
    stOSVI.dwOSVersionInfoSize = sizeof ( OSVERSIONINFO ) ;

    BOOL bRet = GetVersionEx ( &stOSVI ) ;
    ASSERT ( TRUE == bRet ) ;
    if ( FALSE == bRet )
    {
        TRACE0 ( "GetVersionEx failed!\n" ) ;
        return ( FALSE ) ;
    }

    // Check the version and call the appropriate thing.
    if ( VER_PLATFORM_WIN32_NT == stOSVI.dwPlatformId )
    {
        g_bIsNT = TRUE ;
    }
    else
    {
        g_bIsNT = FALSE ;
    }
    g_bHasVersion = TRUE ;
    return ( TRUE == g_bIsNT ) ;
}




/*//////////////////////////////////////////////////////////////////////
                        Function Implementation
//////////////////////////////////////////////////////////////////////*/

DWORD __stdcall BSUGetModuleBaseName ( HANDLE  hProcess   ,
                                       HMODULE hModule    ,
                                       LPTSTR  lpBaseName ,
                                       DWORD   nSize       )
{
    if ( TRUE == IsNT ( ) )
    {
        // Call the NT version.  It is in NT4ProcessInfo because that is
        // where all the PSAPI wrappers are kept.
        return ( NTGetModuleBaseName ( hProcess     ,
                                       hModule      ,
                                       lpBaseName   ,
                                       nSize         ) ) ;
    }
    return ( Win95GetModuleBaseName ( hProcess     ,
                                      hModule      ,
                                      lpBaseName   ,
                                      nSize         ) ) ;

}

static DWORD __stdcall
            Win95GetModuleBaseName ( HANDLE  /*hProcess*/  ,
                                     HMODULE hModule       ,
                                     LPSTR   lpBaseName    ,
                                     DWORD   nSize          )
{
    ASSERT ( FALSE == IsBadWritePtr ( lpBaseName , nSize ) ) ;
    if ( TRUE == IsBadWritePtr ( lpBaseName , nSize ) )
    {
        TRACE0 ( "Win95GetModuleBaseName Invalid string buffer\n" ) ;
        SetLastError ( ERROR_INVALID_PARAMETER ) ;
        return ( 0 ) ;
    }

    // This could blow the stack...
    char szBuff[ MAX_PATH + 1 ] ;
    DWORD dwRet = GetModuleFileName ( hModule , szBuff , MAX_PATH ) ;
    ASSERT ( 0 != dwRet ) ;
    if ( 0 == dwRet )
    {
        return ( 0 ) ;
    }

    // Find the last '\' mark.
    char * pStart = strrchr ( szBuff , '\\' ) ;
    int iMin ;
    if ( NULL != pStart )
    {
        // Move up one character.
        pStart++ ;
        //lint -e666
        iMin = min ( (int)nSize , (lstrlen ( pStart ) + 1) ) ;
        //lint +e666
        lstrcpyn ( lpBaseName , pStart , iMin ) ;
    }
    else
    {
        // Copy the szBuff buffer in.
        //lint -e666
        iMin = min ( (int)nSize , (lstrlen ( szBuff ) + 1) ) ;
        //lint +e666
        lstrcpyn ( lpBaseName , szBuff , iMin ) ;
    }
    // Always NULL terminate.
    lpBaseName[ iMin ] = '\0' ;
    return ( (DWORD)(iMin - 1) ) ;
}

DWORD BUGSUTIL_DLLINTERFACE __stdcall
        BSUSymInitialize ( DWORD  dwPID          ,
                           HANDLE hProcess       ,
                           PSTR   UserSearchPath ,
                           BOOL   fInvadeProcess  )
{
    // If this is any flavor of NT or fInvadeProcess is FALSE, just call
    // SymInitialize itself
    if ( ( TRUE == IsNT ( ) ) || ( FALSE == fInvadeProcess ) )
    {
        return ( ::SymInitialize ( hProcess       ,
                                   UserSearchPath ,
                                   fInvadeProcess  ) ) ;
    }
    else
    {
        // This is Win9x and the user wants to invade!

        // The first step is to initialize the symbol engine.  If it
        // fails, there is not much I can do.
        BOOL bSymInit = ::SymInitialize ( hProcess       ,
                                          UserSearchPath ,
                                          fInvadeProcess  ) ;
        ASSERT ( FALSE != bSymInit ) ;
        if ( FALSE == bSymInit )
        {
            return ( FALSE ) ;
        }

        DWORD dwCount ;
        // Find out how many modules there are.  This is a BSU function.
        if ( FALSE == GetLoadedModules ( dwPID    ,
                                         0        ,
                                         NULL     ,
                                         &dwCount  ) )
        {
            ASSERT ( !"GetLoadedModules failed" ) ;
            // Clean up the symbol engine and leave.
            VERIFY ( ::SymCleanup ( hProcess ) ) ;
            return ( FALSE ) ;
        }
        // Allocate something big enough to hold the list.
        HMODULE * paMods = new HMODULE[ dwCount ] ;

        // Get the list for real.
        if ( FALSE == GetLoadedModules ( dwPID    ,
                                         dwCount  ,
                                         paMods   ,
                                         &dwCount  ) )
        {
            ASSERT ( !"GetLoadedModules failed" ) ;
            // Clean up the symbol engine and leave.
            VERIFY ( ::SymCleanup ( hProcess ) ) ;
            // Free the memory that I allocated earlier.
            delete [] paMods ;
            return ( FALSE ) ;
        }
        // The module filename.
        TCHAR szModName [ MAX_PATH ] ;
        for ( UINT uiCurr = 0 ; uiCurr < dwCount ; uiCurr++ )
        {
            // Get the module's filename.
            if ( FALSE == GetModuleFileName ( paMods[ uiCurr ]     ,
                                              szModName            ,
                                              sizeof ( szModName )  ) )
            {
                ASSERT ( !"GetModuleFileName failed!" ) ;
                // Clean up the symbol engine and leave.
                VERIFY ( ::SymCleanup ( hProcess ) ) ;
                // Free the memory that I allocated earlier.
                delete [] paMods ;
                return ( FALSE ) ;
            }

            // In order to get the symbol engine to work outside a
            // debugger, it needs a handle to the image.  Yes, this
            // will leak but the OS will close it down when the process
            // ends.
            HANDLE hFile = CreateFile ( szModName       ,
                                        GENERIC_READ    ,
                                        FILE_SHARE_READ ,
                                        NULL            ,
                                        OPEN_EXISTING   ,
                                        0               ,
                                        0                ) ;

            // For whatever reason, SymLoadModule can return zero, but
            // it still loads the modules.  Sheez.
            if ( FALSE == SymLoadModule ( hProcess               ,
                                          hFile                  ,
                                          szModName              ,
                                          NULL                   ,
                                         (DWORD)paMods[ uiCurr ] ,
                                          0                       ) )
            {
                // Check the last error value.  If it is zero, then all
                // I can assume is that it worked.
                DWORD dwLastErr = GetLastError ( ) ;
                ASSERT ( ERROR_SUCCESS == dwLastErr ) ;
                if ( ERROR_SUCCESS != dwLastErr )
                {
                    // Clean up the symbol engine and leave.
                    VERIFY ( ::SymCleanup ( hProcess ) ) ;
                    // Free the memory that I allocated earlier.
                    delete [] paMods ;
                    return ( FALSE ) ;
                }
            }
        }
        delete [] paMods ;
    }
    return ( TRUE ) ;
}

DWORD __stdcall BSUGetModuleFileNameEx ( DWORD     dwPID        ,
                                         HANDLE    hProcess     ,
                                         HMODULE   hModule      ,
                                         LPTSTR    szFilename   ,
                                         DWORD     nSize         )
{
    if ( TRUE == IsNT ( ) )
    {
        return ( NTGetModuleFileNameEx ( dwPID      ,
                                         hProcess   ,
                                         hModule    ,
                                         szFilename ,
                                         nSize       ) ) ;
    }
    return ( TLHELPGetModuleFileNameEx ( dwPID      ,
                                         hProcess   ,
                                         hModule    ,
                                         szFilename ,
                                         nSize       ) ) ;
}





/*//////////////////////////////////////////////////////////////////////
                                Typedefs
//////////////////////////////////////////////////////////////////////*/
// The typedefs for the PSAPI.DLL functions used by this module.
typedef BOOL (WINAPI *ENUMPROCESSMODULES) ( HANDLE    hProcess   ,
                                            HMODULE * lphModule  ,
                                            DWORD     cb         ,
                                            LPDWORD   lpcbNeeded  ) ;

typedef DWORD (WINAPI *GETMODULEBASENAME) ( HANDLE  hProcess   ,
                                            HMODULE hModule    ,
                                            LPTSTR  lpBaseName ,
                                            DWORD   nSize       ) ;

typedef DWORD (WINAPI *GETMODULEFILENAMEEX) ( HANDLE  hProcess   ,
                                              HMODULE hModule    ,
                                              LPTSTR  lpFilename ,
                                              DWORD   nSize       ) ;


/*//////////////////////////////////////////////////////////////////////
                            File Static Data
//////////////////////////////////////////////////////////////////////*/
// Has the function stuff here been initialized?  This is only to be
// used by the InitPSAPI function and nothing else.
static BOOL g_bInitialized = FALSE ;
// The pointer to EnumProcessModules.
static ENUMPROCESSMODULES g_pEnumProcessModules = NULL ;
// The pointer to GetModuleBaseName.
static GETMODULEBASENAME g_pGetModuleBaseName = NULL ;
// The pointer to GetModuleFileNameEx.
static GETMODULEFILENAMEEX g_pGetModuleFileNameEx = NULL ;

/*----------------------------------------------------------------------
FUNCTION        :   InitPSAPI
DISCUSSION      :
    Loads PSAPI.DLL and initializes all the pointers needed by this
file.  If BugslayerUtil.DLL statically linked to PSAPI.DLL, it would not
work on Windows9x.
    Note that I conciously chose to allow the resource leak on loading
PSAPI.DLL.
PARAMETERS      :
    None.
RETURNS         :
    TRUE  - Everything initialized properly.
    FALSE - There was a problem.
----------------------------------------------------------------------*/
static BOOL InitPSAPI ( void )
{
    if ( TRUE == g_bInitialized )
    {
        return ( TRUE ) ;
    }

    // Load up PSAPI.DLL.
    HINSTANCE hInst = LoadLibraryA ( "PSAPI.DLL" ) ;
    ASSERT ( NULL != hInst ) ;
    if ( NULL == hInst )
    {
        TRACE0 ( "Unable to load PSAPI.DLL!\n" ) ;
        return ( FALSE ) ;
    }

    // Now do the GetProcAddress stuff.
    g_pEnumProcessModules =
           (ENUMPROCESSMODULES)GetProcAddress ( hInst ,
                                                "EnumProcessModules" ) ;
    ASSERT ( NULL != g_pEnumProcessModules ) ;
    if ( NULL == g_pEnumProcessModules )
    {
        TRACE0 ( "GetProcAddress failed on EnumProcessModules!\n" ) ;
        return ( FALSE ) ;
    }

    g_pGetModuleBaseName =
        (GETMODULEBASENAME)GetProcAddress ( hInst ,
                                            "GetModuleBaseNameA" ) ;
    ASSERT ( NULL != g_pGetModuleBaseName ) ;
    if ( NULL == g_pGetModuleBaseName )
    {
        TRACE0 ( "GetProcAddress failed on GetModuleBaseNameA!\n" ) ;
        return ( FALSE ) ;
    }

    g_pGetModuleFileNameEx =
        (GETMODULEFILENAMEEX)GetProcAddress ( hInst ,
                                              "GetModuleFileNameExA" ) ;
    ASSERT ( NULL != g_pGetModuleFileNameEx ) ;
    if ( NULL == g_pGetModuleFileNameEx )
    {
        TRACE0 ( "GetProcAddress failed on GetModuleFileNameExA\n" ) ;
        return ( FALSE ) ;
    }

    // All OK, Jumpmaster!
    g_bInitialized = TRUE ;
    return ( TRUE ) ;
}

/*----------------------------------------------------------------------
FUNCTION        :   NT4GetLoadedModules
DISCUSSION      :
    The NT4 specific version of GetLoadedModules.  This function assumes
that GetLoadedModules does the work to validate the parameters.
PARAMETERS      :
    dwPID        - The process ID to look into.
    uiCount      - The number of slots in the paModArray buffer.  If
                   this value is 0, then the return value will be TRUE
                   and pdwRealCount will hold the number of items
                   needed.
    paModArray   - The array to place the HMODULES into.  If this buffer
                   is too small to hold the result and uiCount is not
                   zero, then FALSE is returned, but pdwRealCount will
                   be the real number of items needed.
    pdwRealCount - The count of items needed in paModArray, if uiCount
                   is zero, or the real number of items in paModArray.
RETURNS         :
    FALSE - There was a problem, check GetLastError.
    TRUE  - The function succeeded.  See the parameter discussion for
            the output parameters.
----------------------------------------------------------------------*/
BOOL __stdcall NT4GetLoadedModules ( DWORD     dwPID        ,
                                     UINT      uiCount      ,
                                     HMODULE * paModArray   ,
                                     LPDWORD   pdwRealCount   )
{

    // Initialize PSAPI.DLL, if needed.
    if ( FALSE == InitPSAPI ( ) )
    {
        ASSERT ( !"InitPSAPI failed!" ) ;
        SetLastErrorEx ( ERROR_DLL_INIT_FAILED , SLE_ERROR ) ;
        return ( FALSE ) ;
    }

    // Convert the process ID into a process handle.
    HANDLE hProc = OpenProcess ( PROCESS_QUERY_INFORMATION |
                                    PROCESS_VM_READ         ,
                                 FALSE                      ,
                                 dwPID                       ) ;
    ASSERT ( NULL != hProc ) ;
    if ( NULL == hProc )
    {
        TRACE1 ( "Unable to OpenProcess on %08X\n" , dwPID ) ;
        return ( FALSE ) ;
    }

    // Now get the modules for the specified process.
    ASSERT ( NULL != g_pEnumProcessModules ) ;
    // Because of possible DLL unload order differences, make sure that
    // PSAPI.DLL is still loaded in case this function is called during
    // shutdown.
    ASSERT ( FALSE == IsBadCodePtr ( (FARPROC)g_pEnumProcessModules ) );
    if ( TRUE == IsBadCodePtr ( (FARPROC)g_pEnumProcessModules ) )
    {
        TRACE0 ( "PSAPI.DLL has been unloaded on us!\n" ) ;

        // Close the process handle used.
        VERIFY ( CloseHandle ( hProc ) ) ;

        SetLastErrorEx ( ERROR_INVALID_DLL , SLE_ERROR ) ;

        return ( FALSE ) ;
    }

    DWORD dwTotal = 0 ;
    BOOL bRet = g_pEnumProcessModules ( hProc                        ,
                                        paModArray                   ,
                                        uiCount * sizeof ( HMODULE ) ,
                                        &dwTotal                      );

    // Close the process handle used.
    VERIFY ( CloseHandle ( hProc ) ) ;

    // Convert the count from bytes to HMODULE values.
    *pdwRealCount = dwTotal / sizeof ( HMODULE ) ;

    // If bRet was FALSE, and the user was not just asking for the
    // total, there was a problem.
    if ( ( ( FALSE == bRet ) && ( uiCount > 0 ) ) || ( 0 == dwTotal ) )
    {
        ASSERT ( !"EnumProcessModules failed!\n" ) ;
        return ( FALSE ) ;
    }

    // If the total returned in pdwRealCount is larger than the value in
    // uiCount, then return an error.  If uiCount is zero, then it is
    // not an error.
    if ( ( *pdwRealCount > uiCount ) && ( uiCount > 0 ) )
    {
        ASSERT ( !"Buffer is too small in NT4GetLoadedModules!\n" ) ;
        SetLastErrorEx ( ERROR_INSUFFICIENT_BUFFER , SLE_ERROR ) ;
        return ( FALSE ) ;
    }

    // All OK, Jumpmaster!
    SetLastError ( ERROR_SUCCESS ) ;
    return ( TRUE ) ;
}

DWORD __stdcall NTGetModuleBaseName ( HANDLE  hProcess   ,
                                      HMODULE hModule    ,
                                      LPTSTR  lpBaseName ,
                                      DWORD   nSize       )
{
    // Initialize PSAPI.DLL, if needed.
    if ( FALSE == InitPSAPI ( ) )
    {
        ASSERT ( !"InitiPSAPI failed!" ) ;
        SetLastErrorEx ( ERROR_DLL_INIT_FAILED , SLE_ERROR ) ;
        return ( FALSE ) ;
    }
    return ( g_pGetModuleBaseName ( hProcess    ,
                                    hModule     ,
                                    lpBaseName  ,
                                    nSize        ) ) ;
}

DWORD __stdcall NTGetModuleFileNameEx ( DWORD     /*dwPID*/    ,
                                        HANDLE    hProcess     ,
                                        HMODULE   hModule      ,
                                        LPTSTR    szFilename   ,
                                        DWORD     nSize         )
{
    // Initialize PSAPI.DLL, if needed.
    if ( FALSE == InitPSAPI ( ) )
    {
        ASSERT ( !"InitPSAPI failed" ) ;
        SetLastErrorEx ( ERROR_DLL_INIT_FAILED , SLE_ERROR ) ;
        return ( FALSE ) ;
    }
    return ( g_pGetModuleFileNameEx ( hProcess   ,
                                      hModule    ,
                                      szFilename ,
                                      nSize       ) ) ;
}






// The documentation for this function is in BugslayerUtil.h.
BOOL BUGSUTIL_DLLINTERFACE __stdcall
                           GetLoadedModules ( DWORD     dwPID        ,
                                              UINT      uiCount      ,
                                              HMODULE * paModArray   ,
                                              LPDWORD   pdwRealCount   )
{
    // Do the debug checking.
    ASSERT ( NULL != pdwRealCount ) ;
    ASSERT ( FALSE == IsBadWritePtr ( pdwRealCount , sizeof ( UINT ) ));
#ifdef _DEBUG
    if ( 0 != uiCount )
    {
        ASSERT ( NULL != paModArray ) ;
        ASSERT ( FALSE == IsBadWritePtr ( paModArray                   ,
                                          uiCount *
                                                sizeof ( HMODULE )   ));
    }
#endif

    // Do the parameter checking for real.  Note that I only check the
    // memory in paModArray if uiCount is > 0.  The user can pass zero
    // in uiCount if they are just interested in the total to be
    // returned so they could dynamically allocate a buffer.
    if ( ( TRUE == IsBadWritePtr ( pdwRealCount , sizeof(UINT) ) )    ||
         ( ( uiCount > 0 ) &&
           ( TRUE == IsBadWritePtr ( paModArray ,
                                     uiCount * sizeof(HMODULE) ) ) )   )
    {
        SetLastErrorEx ( ERROR_INVALID_PARAMETER , SLE_ERROR ) ;
        return ( FALSE ) ;
    }

    // Figure out which OS we are on.
    OSVERSIONINFO stOSVI ;

    memset ( &stOSVI , NULL , sizeof ( OSVERSIONINFO ) ) ;
    stOSVI.dwOSVersionInfoSize = sizeof ( OSVERSIONINFO ) ;

    BOOL bRet = GetVersionEx ( &stOSVI ) ;
    ASSERT ( TRUE == bRet ) ;
    if ( FALSE == bRet )
    {
        TRACE0 ( "GetVersionEx failed!\n" ) ;
        return ( FALSE ) ;
    }

    // Check the version and call the appropriate thing.
    if ( ( VER_PLATFORM_WIN32_NT == stOSVI.dwPlatformId ) &&
         ( 4 == stOSVI.dwMajorVersion                   )    )
    {
        // This is NT 4 so call its specific version in PSAPI.DLL
        return ( NT4GetLoadedModules ( dwPID        ,
                                       uiCount      ,
                                       paModArray   ,
                                       pdwRealCount  ) );
    }
    else
    {
        // Win9x and Win2K go through tool help.
        return ( TLHELPGetLoadedModules ( dwPID         ,
                                          uiCount       ,
                                          paModArray    ,
                                          pdwRealCount   ) ) ;
    }
}





/*//////////////////////////////////////////////////////////////////////
                                Typedefs
//////////////////////////////////////////////////////////////////////*/
// The typedefs for the TOOLHELP32.DLL functions used by this module.
// Type definitions for pointers to call tool help functions.
typedef BOOL (WINAPI *MODULEWALK) ( HANDLE          hSnapshot ,
                                    LPMODULEENTRY32 lpme       ) ;
typedef BOOL (WINAPI *THREADWALK) ( HANDLE          hSnapshot ,
                                    LPTHREADENTRY32 lpte       ) ;
typedef BOOL (WINAPI *PROCESSWALK) ( HANDLE           hSnapshot ,
                                     LPPROCESSENTRY32 lppe       ) ;
typedef HANDLE (WINAPI *CREATESNAPSHOT) ( DWORD dwFlags       ,
                                          DWORD th32ProcessID  ) ;


/*//////////////////////////////////////////////////////////////////////
                            File Static Data
//////////////////////////////////////////////////////////////////////*/
// Has the function stuff here been initialized?  This is only to be
// used by the InitPSAPI function and nothing else.
static CREATESNAPSHOT g_pCreateToolhelp32Snapshot = NULL  ;
static MODULEWALK     g_pModule32First            = NULL  ;
static MODULEWALK     g_pModule32Next             = NULL  ;
static PROCESSWALK    g_pProcess32First           = NULL  ;
static PROCESSWALK    g_pProcess32Next            = NULL  ;
static THREADWALK     g_pThread32First            = NULL  ;
static THREADWALK     g_pThread32Next             = NULL  ;

/*----------------------------------------------------------------------
FUNCTION        :   InitTOOLHELP32
DISCUSSION      :
    Retrieves the function pointers needed to access the tool help
routines.  Since this code is supposed to work on NT4, it cannot link
to the non-existant addresses in KERNEL32.DLL.
    This is pretty much lifter right from the MSDN.
PARAMETERS      :
    None.
RETURNS         :
    TRUE  - Everything initialized properly.
    FALSE - There was a problem.
----------------------------------------------------------------------*/
static BOOL InitTOOLHELP32 ( void )
{
    if ( TRUE == g_bInitialized )
    {
        return ( TRUE ) ;
    }

    BOOL      bRet    = FALSE ;
    HINSTANCE hKernel = NULL  ;

    // Obtain the module handle of the kernel to retrieve addresses of
    // the tool helper functions.
    hKernel = GetModuleHandleA ( "KERNEL32.DLL" ) ;
    ASSERT ( NULL != hKernel ) ;

    if ( NULL != hKernel )
    {
        g_pCreateToolhelp32Snapshot =
           (CREATESNAPSHOT)GetProcAddress ( hKernel ,
                                            "CreateToolhelp32Snapshot");
        ASSERT ( NULL != g_pCreateToolhelp32Snapshot ) ;

        g_pModule32First = (MODULEWALK)GetProcAddress (hKernel ,
                                                       "Module32First");
        ASSERT ( NULL != g_pModule32First ) ;

        g_pModule32Next = (MODULEWALK)GetProcAddress (hKernel        ,
                                                      "Module32Next"  );
        ASSERT ( NULL != g_pModule32Next ) ;

        g_pProcess32First =
                (PROCESSWALK)GetProcAddress ( hKernel          ,
                                              "Process32First"  ) ;
        ASSERT ( NULL != g_pProcess32First ) ;

        g_pProcess32Next =
                (PROCESSWALK)GetProcAddress ( hKernel         ,
                                              "Process32Next" ) ;
        ASSERT ( NULL != g_pProcess32Next ) ;

        g_pThread32First =
                (THREADWALK)GetProcAddress ( hKernel         ,
                                             "Thread32First"  ) ;
        ASSERT ( NULL != g_pThread32First ) ;

        g_pThread32Next =
                (THREADWALK)GetProcAddress ( hKernel        ,
                                             "Thread32Next"  ) ;
        ASSERT ( NULL != g_pThread32Next ) ;

        // All addresses must be non-NULL to be successful.  If one of
        // these addresses is NULL, one of the needed lists cannot be
        // walked.

        bRet =  g_pModule32First            &&
                g_pModule32Next             &&
                g_pProcess32First           &&
                g_pProcess32Next            &&
                g_pThread32First            &&
                g_pThread32Next             &&
                g_pCreateToolhelp32Snapshot    ;
    }
    else
    {
        // Could not get the module handle of kernel.
        SetLastErrorEx ( ERROR_DLL_INIT_FAILED , SLE_ERROR ) ;
        bRet = FALSE ;
    }

    ASSERT ( TRUE == bRet ) ;

    if ( TRUE == bRet )
    {
        // All OK, Jumpmaster!
        g_bInitialized = TRUE ;
    }
    return ( bRet ) ;
}

/*----------------------------------------------------------------------
FUNCTION        :   TLHELPGetLoadedModules
DISCUSSION      :
    The TOOLHELP32 specific version of GetLoadedModules.  This function
assumes that GetLoadedModules does the work to validate the parameters.
PARAMETERS      :
    dwPID        - The process ID to look into.
    uiCount      - The number of slots in the paModArray buffer.  If
                   this value is 0, then the return value will be TRUE
                   and pdwRealCount will hold the number of items
                   needed.
    paModArray   - The array to place the HMODULES into.  If this buffer
                   is too small to hold the result and uiCount is not
                   zero, then FALSE is returned, but pdwRealCount will
                   be the real number of items needed.
    pdwRealCount - The count of items needed in paModArray, if uiCount
                   is zero, or the real number of items in paModArray.
RETURNS         :
    FALSE - There was a problem, check GetLastError.
    TRUE  - The function succeeded.  See the parameter discussion for
            the output parameters.
----------------------------------------------------------------------*/
BOOL __stdcall TLHELPGetLoadedModules ( DWORD     dwPID        ,
                                        UINT      uiCount      ,
                                        HMODULE * paModArray   ,
                                        LPDWORD   pdwRealCount   )
{

    // Always set pdwRealCount to a know value before anything else.
    *pdwRealCount = 0 ;

    if ( FALSE == InitTOOLHELP32 ( ) )
    {
        ASSERT ( !"InitTOOLHELP32 failed" ) ;
        SetLastErrorEx ( ERROR_DLL_INIT_FAILED , SLE_ERROR ) ;
        return ( FALSE ) ;
    }

    // The snapshot handle.
    HANDLE        hModSnap     = NULL ;
    // The module structure.
    MODULEENTRY32 stME32              ;
    // A flag kept to report if the buffer was too small.
    BOOL          bBuffToSmall = FALSE ;


    // Get the snapshot for the specified process.
    hModSnap = g_pCreateToolhelp32Snapshot ( TH32CS_SNAPMODULE ,
                                             dwPID              ) ;
    ASSERT ( INVALID_HANDLE_VALUE != hModSnap ) ;
    if ( INVALID_HANDLE_VALUE == hModSnap )
    {
        TRACE1 ( "Unable to get module snapshot for %08X\n" , dwPID ) ;
        return ( FALSE ) ;
    }

    memset ( &stME32 , NULL , sizeof ( MODULEENTRY32 ) ) ;
    stME32.dwSize = sizeof ( MODULEENTRY32 ) ;

    // Start getting the module values.
    if ( TRUE == g_pModule32First ( hModSnap , &stME32 ) )
    {
        do
        {
            // If uiCount is not zero, copy values.
            if ( 0 != uiCount )
            {
                // If the passed in buffer is to small, set the flag.
                // This is so we match the functionality of the NT4
                // version of this function which will return the
                // correct total needed.
                if ( ( TRUE == bBuffToSmall     ) ||
                     ( *pdwRealCount == uiCount )   )
                {
                    bBuffToSmall = TRUE ;
                    break ;
                }
                else
                {
                    // Copy this value in.
                    paModArray[ *pdwRealCount ] =
                                         (HINSTANCE)stME32.modBaseAddr ;
                }
            }
            // Bump up the real total count.
            *pdwRealCount += 1 ;
        }
        while ( ( TRUE == g_pModule32Next ( hModSnap , &stME32 ) ) ) ;
    }
    else
    {
        DWORD dwLastErr = GetLastError ( ) ;
        ASSERT ( !"Failed to get first module!\n" ) ;
        SetLastErrorEx ( dwLastErr , SLE_ERROR ) ;
        return ( FALSE ) ;
    }

    // Close the snapshot handle.
    VERIFY ( CloseHandle ( hModSnap ) ) ;

    // Check if the buffer was too small.
    if ( TRUE == bBuffToSmall )
    {
        ASSERT ( !"Buffer to small in TLHELPGetLoadedModules\n" ) ;
        SetLastErrorEx ( ERROR_INSUFFICIENT_BUFFER , SLE_ERROR ) ;
        return ( FALSE ) ;
    }

    // All OK, Jumpmaster!
    SetLastError ( ERROR_SUCCESS ) ;
    return ( TRUE ) ;
}

DWORD __stdcall TLHELPGetModuleFileNameEx ( DWORD     dwPID        ,
                                            HANDLE    /*hProcess*/ ,
                                            HMODULE   hModule      ,
                                            LPTSTR    szFilename   ,
                                            DWORD     nSize         )
{
    if ( FALSE == InitTOOLHELP32 ( ) )
    {
        ASSERT ( !"InitTOOLHELP32 failed" ) ;
        SetLastErrorEx ( ERROR_DLL_INIT_FAILED , SLE_ERROR ) ;
        return ( 0 ) ;
    }

    // The snapshot handle.
    HANDLE        hModSnap     = NULL ;
    // The module structure.
    MODULEENTRY32 stME32              ;

    // Get the snapshot for the specified process.
    hModSnap = g_pCreateToolhelp32Snapshot ( TH32CS_SNAPMODULE ,
                                             dwPID              ) ;
    ASSERT ( INVALID_HANDLE_VALUE != hModSnap ) ;
    if ( INVALID_HANDLE_VALUE == hModSnap )
    {
        TRACE1 ( "Unable to get module snapshot for %08X\n" , dwPID ) ;
        return ( 0 ) ;
    }

    memset ( &stME32 , NULL , sizeof ( MODULEENTRY32 ) ) ;
    stME32.dwSize = sizeof ( MODULEENTRY32 ) ;

    DWORD dwRet = 0 ;

    // Start getting the module values.
    if ( TRUE == g_pModule32First ( hModSnap , &stME32 ) )
    {
        do
        {
            // Is this the one?
            if ( hModule == stME32.hModule )
            {
                DWORD dwLen = _tcslen ( stME32.szExePath ) ;
                if ( dwLen > nSize )
                {
                    _tcsncpy ( szFilename       ,
                               stME32.szExePath ,
                               nSize - 1         ) ;
                    szFilename[ nSize ] = _T ( '\0' ) ;
                    dwRet = nSize ;
                }
                else
                {
                    _tcscpy ( szFilename , stME32.szExePath ) ;
                    dwRet = dwLen ;
                }
                break ;
            }
        }
        while ( ( TRUE == g_pModule32Next ( hModSnap , &stME32 ) ) ) ;
    }
    else
    {
        DWORD dwLastErr = GetLastError ( ) ;
        ASSERT ( !"Failed to get first module!\n" ) ;
        SetLastErrorEx ( dwLastErr , SLE_ERROR ) ;
        return ( 0 ) ;
    }

    // Close the snapshot handle.
    VERIFY ( CloseHandle ( hModSnap ) ) ;

    // All OK, Jumpmaster!
    SetLastError ( ERROR_SUCCESS ) ;
    return ( dwRet ) ;

}

