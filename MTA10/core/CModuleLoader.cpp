/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CModuleLoader.cpp
*  PURPOSE:     Dynamic module loading
*  DEVELOPERS:  Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using std::string;

CModuleLoader::CModuleLoader ( string ModuleName )
{
    m_hLoadedModule = 0;
    LoadModule ( ModuleName );
}

CModuleLoader::CModuleLoader ( )
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
        return true;
    }
    else
    {
        return false;
    }
}

void CModuleLoader::UnloadModule ( )
{
    FreeLibrary ( m_hLoadedModule );
}

PVOID CModuleLoader::GetFunctionPointer ( string FunctionName )
{
    FARPROC fpProcAddr;

    fpProcAddr = GetProcAddress ( m_hLoadedModule, FunctionName.c_str() );

    return static_cast < PVOID > ( fpProcAddr );
}
