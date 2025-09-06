/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/core/CSetCursorPosHookInterface.h
 *  PURPOSE:     Cursor hooking interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CSetCursorPosHookInterface
{
public:
    virtual void ApplyHook() = 0;
    virtual void RemoveHook() = 0;

    virtual void DisableSetCursorPos() = 0;
    virtual void EnableSetCursorPos() = 0;
    virtual bool IsSetCursorPosEnabled() = 0;

    virtual BOOL CallSetCursorPos(int X, int Y) = 0;
};
