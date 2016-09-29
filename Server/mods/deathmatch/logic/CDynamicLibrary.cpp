/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CDynamicLibrary.cpp
*  PURPOSE:     Dynamic libraries loader
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include "CDynamicLibrary.h"

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

    // Load the new library
    #ifdef WIN32
        // Search at the same path for dependencies (path must be abslolute)
        m_hModule = LoadLibraryEx ( szFilename, NULL, LOAD_WITH_ALTERED_SEARCH_PATH );

        if ( !m_hModule )
        {
            DWORD dwError = GetLastError ();
            char szError [ 2048 ] = { 0 };
            FormatMessage ( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dwError, LANG_NEUTRAL, szError, sizeof ( szError ), NULL );
            CLogger::ErrorPrintf ( "Could not load %s - %s", szFilename, szError );
        }

    #else
        m_hModule = dlopen ( szFilename, RTLD_NOW );
    
        // Output error if needed
        if ( !m_hModule )
        {
            CLogger::ErrorPrintf ( "Could not load %s - %s", szFilename, dlerror( ) );
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


void* CDynamicLibrary::GetProcedureAddress ( const char* szProcName )
{
    // Got a module?
    if ( m_hModule != 0 )
    {
        #ifdef WIN32
            return GetProcAddress ( m_hModule, szProcName );
        #else
            char* szError = NULL;
        dlerror ();

            void* pFunc = dlsym ( m_hModule, szProcName );
            if (  ( szError = dlerror () ) != NULL )
            {
                return NULL;
            }

            return pFunc;
        #endif
    }

    return NULL;
}
