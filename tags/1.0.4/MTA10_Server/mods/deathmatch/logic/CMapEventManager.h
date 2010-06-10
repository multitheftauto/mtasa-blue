/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CMapEventManager.h
*  PURPOSE:     Map event manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Kevin Whiteside <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
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

    bool                    Call                            ( const char* szName, const CLuaArguments& Arguments, class CElement* pSource, class CElement* pThis, class CPlayer* pCaller = NULL );

private:
    void                        TakeOutTheTrash                 ( void );

    std::list < CMapEvent* >    m_Events;
    std::list < CMapEvent* >    m_TrashCan;
    bool                        m_bIteratingList;
};

#endif
