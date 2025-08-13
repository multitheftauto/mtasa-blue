/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CMapEvent.h
 *  PURPOSE:     Map event class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#define MAPEVENT_MAX_LENGTH_NAME 100

#include "Enums.h"
#include "lua/CLuaFunctionRef.h"
#include <stdio.h>
#include <string>

class CMapEvent
{
    friend class CMapEventManager;

public:
    class CLuaMain*        GetVM() { return m_pMain; };
    const SString&         GetName() { return m_strName; };
    const CLuaFunctionRef& GetLuaFunction() { return m_iLuaFunction; };
    bool                   IsPropagated() { return m_bPropagated; }
    bool                   IsBeingDestroyed() { return m_bBeingDestroyed; }

    void Call(const class CLuaArguments& Arguments);
    bool IsHigherPriorityThan(const CMapEvent* pOther);

private:
    CMapEvent(class CLuaMain* pMain, const char* szName, const CLuaFunctionRef& iLuaFunction, bool bPropagated, EEventPriorityType eventPriority,
              float fPriorityMod);
    ~CMapEvent();

    void SetBeingDestroyed(bool bBeingDestroyed) { m_bBeingDestroyed = bBeingDestroyed; }

    class CLuaMain*    m_pMain;
    CLuaFunctionRef    m_iLuaFunction;
    SString            m_strName;
    bool               m_bPropagated;
    bool               m_bDestroyFunction;
    bool               m_bBeingDestroyed;
    EEventPriorityType m_eventPriority;
    float              m_fPriorityMod;
};
