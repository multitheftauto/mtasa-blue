/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CMapEvent.h
*  PURPOSE:     Map event class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Chris McArthur <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/
#pragma once

#define MAPEVENT_MAX_LENGTH_NAME 100

#include <cstdio>
#include <string>

class CMapEvent
{
public:
    CMapEvent(class CLuaMain* pMain, std::string strName, const CLuaFunctionRef& iLuaFunction, bool bPropagated, EEventPriorityType eventPriority, float fPriorityMod);

    inline class CLuaMain* GetVM() const { return m_pMain; }
    inline const SString& GetName() const { return m_strName; }
    const CLuaFunctionRef& GetLuaFunction() const { return m_iLuaFunction; }
    inline EEventPriorityType GetPriority() const { return m_eventPriority; }
    inline float GetPriorityMod() const { return m_fPriorityMod; }
    inline bool IsPropagated() const { return m_bPropagated; }
    inline bool IsBeingDestroyed() const { return m_bBeingDestroyed; }
    inline void SetBeingDestroyed(bool bBeingDestroyed) { m_bBeingDestroyed = bBeingDestroyed; }
#ifdef MTA_CLIENT
    bool ShouldAllowAspectRatioAdjustment()
    {
        return m_bAllowAspectRatioAdjustment;
    }
    bool ShouldForceAspectRatioAdjustment() { return m_bForceAspectRatioAdjustment; }
#endif

    void Call(const class CLuaArguments& Arguments);
    bool IsHigherPriorityThan(const CMapEvent* pOther);

private:
    class CLuaMain* m_pMain;
    CLuaFunctionRef m_iLuaFunction;
    SString m_strName;
    bool m_bPropagated;
    bool m_bDestroyFunction;
    bool m_bBeingDestroyed;
    EEventPriorityType m_eventPriority;
    float m_fPriorityMod;
#ifdef MTA_CLIENT
    bool m_bAllowAspectRatioAdjustment;
    bool m_bForceAspectRatioAdjustment;
#endif
};
