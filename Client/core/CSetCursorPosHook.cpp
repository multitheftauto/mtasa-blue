/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CSetCursorPosHook.cpp
 *  PURPOSE:     Cursor position setting hook
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <SharedUtil.Detours.h>

template <>
CSetCursorPosHook* CSingleton<CSetCursorPosHook>::m_pSingleton = NULL;

CSetCursorPosHook::CSetCursorPosHook()
{
    WriteDebugEvent("CSetCursorPosHook::CSetCursorPosHook");

    m_bCanCall = true;
    m_pfnSetCursorPos = NULL;
}

CSetCursorPosHook::~CSetCursorPosHook()
{
    WriteDebugEvent("CSetCursorPosHook::~CSetCursorPosHook");

    if (m_pfnSetCursorPos != NULL)
    {
        RemoveHook();
    }
}

void CSetCursorPosHook::ApplyHook()
{
    DetourLibraryFunction("user32.dll", "SetCursorPos", m_pfnSetCursorPos, API_SetCursorPos);
}

void CSetCursorPosHook::RemoveHook()
{
    // Remove hook
    if (m_pfnSetCursorPos)
    {
        UndoFunctionDetour(m_pfnSetCursorPos, API_SetCursorPos);
    }

    // Reset variables
    m_pfnSetCursorPos = nullptr;
    m_bCanCall = true;
}

void CSetCursorPosHook::DisableSetCursorPos()
{
    m_bCanCall = false;
}

void CSetCursorPosHook::EnableSetCursorPos()
{
    m_bCanCall = true;
}

bool CSetCursorPosHook::IsSetCursorPosEnabled()
{
    return m_bCanCall;
}

BOOL CSetCursorPosHook::CallSetCursorPos(int X, int Y)
{
    if (m_pfnSetCursorPos == NULL)
    {
        // We should never get here, but if we do, attempt to call
        // an imported SetCursorPos.
        return SetCursorPos(X, Y);
    }
    else
    {
        return m_pfnSetCursorPos(X, Y);
    }
}

BOOL WINAPI CSetCursorPosHook::API_SetCursorPos(int X, int Y)
{
    CSetCursorPosHook* pThis;

    // Get self-pointer.
    pThis = CSetCursorPosHook::GetSingletonPtr();

    // Check to see if this function should be called properly.
    if ((pThis->m_bCanCall) && (pThis->m_pfnSetCursorPos != NULL))
    {
        return pThis->m_pfnSetCursorPos(X, Y);
    }

    return FALSE;
}
