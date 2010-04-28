/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CSetCursorPosHook.cpp
*  PURPOSE:     Cursor position setting hook
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "detours/include/detours.h"

template<> CSetCursorPosHook * CSingleton< CSetCursorPosHook >::m_pSingleton = NULL;

CSetCursorPosHook::CSetCursorPosHook ( void ) 
{
    WriteDebugEvent ( "CSetCursorPosHook::CSetCursorPosHook" );

    m_bCanCall          = true;
    m_pfnSetCursorPos   = NULL;
}


CSetCursorPosHook::~CSetCursorPosHook ( void ) 
{
    WriteDebugEvent ( "CSetCursorPosHook::~CSetCursorPosHook" );

    if ( m_pfnSetCursorPos != NULL )
    {
        RemoveHook ( );
    }
}

void CSetCursorPosHook::ApplyHook ( void ) 
{
    // Hook SetCursorPos
    m_pfnSetCursorPos = reinterpret_cast < pSetCursorPos > ( DetourFunction ( DetourFindFunction ( "User32.dll", "SetCursorPos" ),
                                                                              reinterpret_cast <PBYTE> ( API_SetCursorPos ) ) );
}


void CSetCursorPosHook::RemoveHook ( void ) 
{
    // Remove hook
    if ( m_pfnSetCursorPos )
    {
        DetourRemove ( reinterpret_cast < PBYTE > ( m_pfnSetCursorPos ), 
                       reinterpret_cast < PBYTE > ( API_SetCursorPos  ) );
    }

    // Reset variables
    m_pfnSetCursorPos = NULL;
    m_bCanCall = true;
}


void CSetCursorPosHook::DisableSetCursorPos ( void ) 
{
    m_bCanCall = false;
}


void CSetCursorPosHook::EnableSetCursorPos ( void ) 
{
    m_bCanCall = true;
}


bool CSetCursorPosHook::IsSetCursorPosEnabled ( void ) 
{
    return m_bCanCall;
}


BOOL CSetCursorPosHook::CallSetCursorPos ( int X, int Y ) 
{
    if ( m_pfnSetCursorPos == NULL )
    {
        // We should never get here, but if we do, attempt to call 
        // an imported SetCursorPos.
        return SetCursorPos ( X, Y );
    }
    else
    {
        return m_pfnSetCursorPos ( X, Y );
    }
}

BOOL WINAPI CSetCursorPosHook::API_SetCursorPos ( int X, int Y ) 
{
    CSetCursorPosHook * pThis;

    // Get self-pointer.
    pThis = CSetCursorPosHook::GetSingletonPtr ( );

    // Check to see if this function should be called properly.
    if ( (pThis->m_bCanCall) && (pThis->m_pfnSetCursorPos != NULL) )
    {
        return pThis->m_pfnSetCursorPos ( X, Y );
    }
    
    return FALSE;
}
