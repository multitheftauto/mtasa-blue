/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CDirectInputHookManager.cpp
*  PURPOSE:     DirectInput hook manager
*  DEVELOPERS:  Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CDirectInputHookManager::CDirectInputHookManager ( )
{
    WriteDebugEvent ( "CDirectInputHookManager::CDirectInputHookManager" );

    // Initialize our member variables.
    m_pDirectInputHook8      = NULL;
}

CDirectInputHookManager::~CDirectInputHookManager ( )
{
    WriteDebugEvent ( "CDirectInputHookManager::~CDirectInputHookManager" );

    // Delete our hook object.
    SAFE_DELETE ( m_pDirectInputHook8 );
}

void CDirectInputHookManager::ApplyHook ( )
{
    PBYTE pbDirectInput8;

    // First load the module(s).
    pbDirectInput8 = reinterpret_cast < PBYTE > ( LoadLibrary ( "DINPUT8.DLL" ) );

    // If we have a valid pointer, we can hook the respective library.
    if ( pbDirectInput8 != NULL )
    {
        m_pDirectInputHook8 = new CDirectInputHook8 ( );
        m_pDirectInputHook8->ApplyHook ( );
        // Self-maintaining code.  Will delete on shutdown...
    }
}

void CDirectInputHookManager::RemoveHook ( )
{
    // Make sure we have a valid pointer.
    if ( m_pDirectInputHook8 != NULL )
    {
        // Remove the hook.
        m_pDirectInputHook8->RemoveHook ( );
    }
}
