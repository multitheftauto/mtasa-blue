/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CDynamicLibrary.cpp
*  PURPOSE:     Dynamic library handling class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CDynamicLibrary.h"
#include "MTAPlatform.h"

#ifdef WIN32
#include <stdio.h>
#endif

CDynamicLibrary::CDynamicLibrary ( void )
{
    // Init
    m_hModule = 0;
}


CDynamicLibrary::~CDynamicLibrary ( void )
{
    // Make sure we unload
    Unload ();
}


bool CDynamicLibrary::Load ( const char* szFilename )
{
    // Unload the previous library
    Unload ();

#ifdef WIN32

    // Copy the filename
    char szFilenameBuffer [MAX_PATH];
    strncpy ( szFilenameBuffer, szFilename, MAX_PATH );

    // Replace forward slashes with backward slashes or the windows function will fail.
    size_t sizeFilename = strlen ( szFilenameBuffer );
    for ( size_t i = 0; i < sizeFilename; i++ )
    {
        if ( szFilenameBuffer [i] == '/' )
            szFilenameBuffer [i] = '\\';
    }

    // Use this pointer now
    szFilename = szFilenameBuffer;

#endif


    // Load the new library
    #ifdef WIN32
        m_hModule = LoadLibraryEx ( szFilename, NULL, LOAD_WITH_ALTERED_SEARCH_PATH );

        // Failed?
        if ( !m_hModule )
        {
            // Grab the error message
            LPVOID lpMsgBuf;
            DWORD dw = GetLastError(); 

            FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                dw,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR) &lpMsgBuf,
                0, NULL );

            // Display the error message and exit the process
            printf ( "Loading library %s failed; %s\n", szFilename, lpMsgBuf );

            // Free the error message buffer
            LocalFree ( lpMsgBuf );
        }
    #else
        m_hModule = dlopen ( szFilename, RTLD_NOW );

        // Output error if needed
        if ( !m_hModule )
        {
            const char* szError = dlerror();
            if ( szError )
                Print ( "%s\n", szError );
            else
                printf ( "Loading %s failed\n", szFilename );
        }
    #endif

    // Return whether we succeeded or not
    return m_hModule != 0;
}


void CDynamicLibrary::Unload ( void )
{
    // Got a module?
    if ( m_hModule != 0 )
    {
        #ifdef WIN32
            FreeLibrary ( m_hModule );
        #else
            dlclose ( m_hModule );
        #endif

        // Zero out our library as it's no longer valid
        m_hModule = 0;
    }
}


bool CDynamicLibrary::IsLoaded ( void )
{
    return m_hModule != 0;
}


FuncPtr_t CDynamicLibrary::GetProcedureAddress ( const char* szProcName )
{
    // Got a module?
    if ( m_hModule != 0 )
    {
        #ifdef WIN32
            return (FuncPtr_t)GetProcAddress ( m_hModule, szProcName );
        #else
            char* szError = NULL;
        dlerror ();

            FuncPtr_t pFunc = (FuncPtr_t)dlsym ( m_hModule, szProcName );
            if (  ( szError = dlerror () ) != NULL )
            {
                return NULL;
            }

            return pFunc;
        #endif
    }

    return NULL;
}
