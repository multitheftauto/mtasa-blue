/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CDirect3DHookManager.cpp
*  PURPOSE:     Direct3D version independant hook manager
*  DEVELOPERS:  Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CDirect3DHookManager::CDirect3DHookManager ( )
{
    WriteDebugEvent ( "CDirect3DHookManager::CDirect3DHookManager" );

    // Set our defaults.
    m_pDirect3DHook9     = NULL;
}

CDirect3DHookManager::~CDirect3DHookManager ( )
{
    WriteDebugEvent ( "CDirect3DHookManager::~CDirect3DHookManager" );

    if ( m_pDirect3DHook9 )
    {
        m_pDirect3DHook9->RemoveHook ( );
        delete m_pDirect3DHook9;
    }
}
    
void CDirect3DHookManager::ApplyHook ( )
{
    if ( m_pDirect3DHook9 )
    {
        WriteDebugEvent ( "CDirect3DHookManager::ApplyHook - Ignoring second call" );
        return;
    }

    WriteDebugEvent ( "CDirect3DHookManager::ApplyHook" );

    PBYTE           pbDirect3D9;

    // First load the module(s).
    pbDirect3D9 = reinterpret_cast < PBYTE > ( LoadLibrary ( "D3D9.DLL" ) );

    if ( pbDirect3D9 != NULL )
    {
        m_pDirect3DHook9 = new CDirect3DHook9 ( );
        m_pDirect3DHook9->ApplyHook ( );
        // Self-maintaining code.  Will delete on shutdown...
    }
}

void CDirect3DHookManager::RemoveHook ( )
{
    // Make sure we have a valid pointer.
    if ( m_pDirect3DHook9 != NULL )
    {
        // Remove the hook.
        m_pDirect3DHook9->RemoveHook ( );
    }
}
