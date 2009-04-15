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

CModuleLoader::CModuleLoader ( string ModuleName )
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

bool CModuleLoader::LoadModule ( string ModuleName )
{
    m_hLoadedModule = LoadLibrary ( ModuleName.c_str() );

    if ( m_hLoadedModule != NULL )
    {
        m_bStatus = true;
    }
    else
    {
        m_bStatus = false;
    }

    return m_bStatus;
}

void CModuleLoader::UnloadModule ( )
{
    FreeLibrary ( m_hLoadedModule );
    m_hLoadedModule = 0;
    m_bStatus = false;
}

PVOID CModuleLoader::GetFunctionPointer ( string FunctionName )
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
