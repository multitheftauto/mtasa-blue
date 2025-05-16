/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CDirectInputHookManager.h
 *  PURPOSE:     Header file for DirectInput hook manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CDirectInputHook8.h"

class CDirectInputHookManager
{
public:
    CDirectInputHookManager();
    ~CDirectInputHookManager();

    void ApplyHook();
    void RemoveHook();

private:
    CDirectInputHook8* m_pDirectInputHook8;
};
