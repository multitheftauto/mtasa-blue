/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CSetCursorPosHook.cpp
 *  PURPOSE:     Header file for cursor position hook class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <windows.h>
#include "CSingleton.h"
#include <core/CSetCursorPosHookInterface.h>

typedef BOOL(WINAPI* pSetCursorPos)(int X, int Y);

class CSetCursorPosHook : public CSetCursorPosHookInterface, public CSingleton<CSetCursorPosHook>
{
public:
    CSetCursorPosHook();
    ~CSetCursorPosHook();

    void ApplyHook();
    void RemoveHook();

    void DisableSetCursorPos();
    void EnableSetCursorPos();
    bool IsSetCursorPosEnabled();

    BOOL CallSetCursorPos(int X, int Y);

    static BOOL WINAPI API_SetCursorPos(int X, int Y);

private:
    bool          m_bCanCall;
    pSetCursorPos m_pfnSetCursorPos;
};
