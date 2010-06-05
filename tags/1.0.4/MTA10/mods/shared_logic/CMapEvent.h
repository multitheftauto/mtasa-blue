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
    inline const char*      GetName             ( void )                                { return m_szName; };
    inline int              GetLuaFunction      ( void )                                { return m_iLuaFunction; };
    inline bool             IsBeingDestroyed    ( void )                                { return m_bBeingDestroyed; }
    inline bool             IsPropagated        ( void )                                { return m_bPropagated; }

    void                    Call                ( const class CLuaArguments& Arguments );

private:
                            CMapEvent           ( class CLuaMain* pMain, const char* szName, int iLuaFunction, bool bPropagated );
                            ~CMapEvent          ( void );

    inline void             SetBeingDestroyed   ( bool bBeingDestroyed )                { m_bBeingDestroyed = bBeingDestroyed; }

    class CLuaMain*         m_pMain;
    int                     m_iLuaFunction;
    char                    m_szName [MAPEVENT_MAX_LENGTH_NAME + 1];
    bool                    m_bDestroyFunction;
    bool                    m_bPropagated;
    bool                    m_bBeingDestroyed;
};

#endif
