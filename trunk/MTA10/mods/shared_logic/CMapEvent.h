/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CMapEvent.h
*  PURPOSE:     Map event class header
*  DEVELOPERS:  Jax <>
*               Oliver Brown <>
*               Kevin Whiteside <kevuwk@gmail.com>
*
*****************************************************************************/

#ifndef __CMAPEVENT_H
#define __CMAPEVENT_H

#define MAPEVENT_MAX_LENGTH_NAME 100

#include <string>

class CMapEvent
{
    friend class CMapEventManager;

public:
    inline class CLuaMain*  GetVM               ( void )                                { return m_pMain; };
    inline const SString&   GetName             ( void )                                { return m_strName; };
    inline CLuaFunctionRef  GetLuaFunction      ( void )                                { return m_iLuaFunction; };
    inline bool             IsBeingDestroyed    ( void )                                { return m_bBeingDestroyed; }
    inline bool             IsPropagated        ( void )                                { return m_bPropagated; }
    bool                    ShouldAllowAspectRatioAdjustment    ( void )                { return m_bAllowAspectRatioAdjustment; }
    bool                    ShouldForceAspectRatioAdjustment    ( void )                { return m_bForceAspectRatioAdjustment; }

    void                    Call                ( const class CLuaArguments& Arguments );
    bool                    IsHigherPriorityThan ( const CMapEvent* pOther );

private:
                            CMapEvent           ( class CLuaMain* pMain, const char* szName, const CLuaFunctionRef& iLuaFunction, bool bPropagated, EEventPriorityType eventPriority, float fPriorityMod );
                            ~CMapEvent          ( void );

    inline void             SetBeingDestroyed   ( bool bBeingDestroyed )                { m_bBeingDestroyed = bBeingDestroyed; }

    class CLuaMain*         m_pMain;
    CLuaFunctionRef         m_iLuaFunction;
    SString                 m_strName;
    bool                    m_bDestroyFunction;
    bool                    m_bPropagated;
    bool                    m_bBeingDestroyed;
    EEventPriorityType      m_eventPriority;
    float                   m_fPriorityMod;
    bool                    m_bAllowAspectRatioAdjustment;
    bool                    m_bForceAspectRatioAdjustment;
};

#endif
