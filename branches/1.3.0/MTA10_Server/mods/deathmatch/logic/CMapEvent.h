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

#ifndef __CMAPEVENT_H
#define __CMAPEVENT_H

#define MAPEVENT_MAX_LENGTH_NAME 100

#include <stdio.h>
#include <string>

class CMapEvent
{
    friend class CMapEventManager;

public:
    inline class CLuaMain*  GetVM               ( void )                                { return m_pMain; };
    inline const char*      GetName             ( void )                                { return m_strName; };
    const CLuaFunctionRef&  GetLuaFunction      ( void )                                { return m_iLuaFunction; };
    inline bool             IsPropagated        ( void )                                { return m_bPropagated; }
    inline bool             IsBeingDestroyed    ( void )                                { return m_bBeingDestroyed; }

    void                    Call                ( const class CLuaArguments& Arguments );
    bool                    IsHigherPriorityThan ( const CMapEvent* pOther );

private:
                            CMapEvent           ( class CLuaMain* pMain, const char* szName, const CLuaFunctionRef& iLuaFunction, bool bPropagated, EEventPriorityType eventPriority, float fPriorityMod );
                            ~CMapEvent          ( void );

    inline void             SetBeingDestroyed   ( bool bBeingDestroyed )                { m_bBeingDestroyed = bBeingDestroyed; }

    class CLuaMain*         m_pMain;
    CLuaFunctionRef         m_iLuaFunction;
    SString                 m_strName;
    bool                    m_bPropagated;
    bool                    m_bDestroyFunction;
    bool                    m_bBeingDestroyed;
    EEventPriorityType      m_eventPriority;
    float                   m_fPriorityMod;
};

#endif
