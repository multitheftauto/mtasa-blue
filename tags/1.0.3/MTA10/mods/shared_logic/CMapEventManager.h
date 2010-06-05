/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CMapEventManager.h
*  PURPOSE:     Map event manager class header
*  DEVELOPERS:  Jax <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Cecill Etheredge <ijsf@gmx.net>
*               Chris McArthur <>
*               Christian Myhre Lundheim <>
*
*****************************************************************************/

#ifndef __CMAPEVENTMANAGER_H
#define __CMAPEVENTMANAGER_H

#include "lua/CLuaArguments.h"
#include "CMapEvent.h"
#include <list>

class CMapEventManager
{
public:
                            CMapEventManager                ( void );
                            ~CMapEventManager               ( void );

    bool                    Add                             ( CLuaMain* pLuaMain, const char* szName, int iLuaFunction, bool bPropagated );
    bool                    Delete                          ( CLuaMain* pLuaMain, const char* szName, int iLuaFunction = -1 );
    void                    Delete                          ( CMapEvent* pEvent );
    void                    Delete                          ( CLuaMain* pLuaMain );
    void                    DeleteAll                       ( void );

    inline bool             Exists                          ( const char* szName )  { return Get ( szName ) != NULL; };
    bool                    Exists                          ( CMapEvent* pEvent );
    bool                    HandleExists                    ( CLuaMain* pLuaMain, const char* szName, int iLuaFunction );
    CMapEvent*              Get                             ( const char* szName );
    CMapEvent*              GetFromXML                      ( const char* szName );

    bool                    Call                            ( const char* szName, const CLuaArguments& Arguments, class CClientEntity* pSource, class CClientEntity* pThis );

private:
    void                        TakeOutTheTrash                 ( void );

    std::list < CMapEvent* >    m_Events;
    std::list < CMapEvent* >    m_TrashCan;
    bool                        m_bIteratingList;
};

#endif
