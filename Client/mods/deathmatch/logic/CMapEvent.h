/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CMapEvent.h
 *  PURPOSE:     Map event class header
 *
 *****************************************************************************/

#pragma once

#define MAPEVENT_MAX_LENGTH_NAME 100

#include <string>

class CMapEvent
{
    friend class CMapEventManager;

public:
    class CLuaMain* GetVM() { return m_pMain; };
    const SString&  GetName() { return m_strName; };
    CLuaFunctionRef GetLuaFunction() { return m_iLuaFunction; };
    bool            IsBeingDestroyed() { return m_bBeingDestroyed; }
    bool            IsPropagated() { return m_bPropagated; }
    bool            ShouldAllowAspectRatioAdjustment() { return m_bAllowAspectRatioAdjustment; }
    bool            ShouldForceAspectRatioAdjustment() { return m_bForceAspectRatioAdjustment; }

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
    bool               m_bDestroyFunction;
    bool               m_bPropagated;
    bool               m_bBeingDestroyed;
    EEventPriorityType m_eventPriority;
    float              m_fPriorityMod;
    bool               m_bAllowAspectRatioAdjustment;
    bool               m_bForceAspectRatioAdjustment;
};
