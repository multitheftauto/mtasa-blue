/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CModuleLoader.cpp
*  PURPOSE:     Dynamic module loading
*  DEVELOPERS:  Derek Abdine <>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using std::string;

CModuleLoader::CModuleLoader ( const string& ModuleName )
: m_bStatus ( false )
{
    m_hLoadedModule = 0;
    LoadModule ( ModuleName );
}

CModuleLoader::CModuleLoader ( )
: m_bStatus ( false ) 
{
    m_hLoadedModule = 0;
}
 
CModuleLoader::~CModuleLoader ( )
{
    UnloadModule ( );
}

bool CModuleLoader::LoadModule ( const string& ModuleName )
{
    m_hLoadedModule = LoadLibrary ( ModuleName.c_str() );

    if ( m_hLoadedModule != NULL )
    {
        m_bStatus = true;
        m_strLastError = "";
    }
    else
    {
        m_bStatus = false;
        DWORD dwError = GetLastError ();
        char szError [ 2048 ] = { 0 };
        FormatMessage ( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dwError, LANG_NEUTRAL, szError, sizeof ( szError ), NULL );
        m_strLastError = szError;
    }

    return m_bStatus;
}

void CModuleLoader::UnloadModule ( )
{
    FreeLibrary ( m_hLoadedModule );
    m_hLoadedModule = 0;
    m_bStatus = false;
    m_strLastError = "";
}

PVOID CModuleLoader::GetFunctionPointer ( const string& FunctionName )
{
    if ( m_bStatus )
    {
        FARPROC fpProcAddr;

        fpProcAddr = GetProcAddress ( m_hLoadedModule, FunctionName.c_str() );

        return static_cast < PVOID > ( fpProcAddr );
    }
    else
        return NULL;
}

const SString& CModuleLoader::GetLastErrorMessage ( void ) const
{
    return m_strLastError;
}
