/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CConsole.h
*  PURPOSE:     Console handler class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Oliver Brown <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCONSOLE_H
#define __CCONSOLE_H

#include "CConsoleCommand.h"
#include <list>
#include "CWhoWas.h"

using namespace std;

class CConsole
{
public:
                                        CConsole                    ( class CBlipManager* pBlipManager, class CMapManager* pMapManager, class CPlayerManager* pPlayerManager, class CRegisteredCommands* pRegisteredCommands, class CVehicleManager* pVehicleManager, class CLuaManager* pLuaManager, CWhoWas* pWhoWas, class CMapFiles* pMapFiles, class CBanManager* pBanManager, class CAccessControlListManager* pACLManager );
                                        ~CConsole                   ( void );

    bool                                HandleInput                 ( const char* szCommand, CClient* pClient, CClient* pEchoClient );

    void                                AddCommand                  ( FCommandHandler* pHandler, const char* szCommand, bool bRestricted );
    void                                DeleteCommand               ( const char* szCommand );
    void                                DeleteAllCommands           ( void );
    CConsoleCommand*                    GetCommand                  ( char* szKey );

    inline list < CConsoleCommand* > ::const_iterator CommandsBegin ( void )                { return m_Commands.begin (); };
    inline list < CConsoleCommand* > ::const_iterator CommandsEnd   ( void )                { return m_Commands.end (); };

    inline class CBlipManager*          GetBlipManager              ( void )                { return m_pBlipManager; };
    inline class CLuaManager*           GetLuaManager               ( void )                { return m_pLuaManager; };
    inline class CMapFiles*             GetMapFiles                 ( void )                { return m_pMapFiles; };
    inline class CMapManager*           GetMapManager               ( void )                { return m_pMapManager; };
    inline class CPlayerManager*        GetPlayerManager            ( void )                { return m_pPlayerManager; };
    inline class CVehicleManager*       GetVehicleManager           ( void )                { return m_pVehicleManager; };
    inline class CBanManager*           GetBanManager               ( void )                { return m_pBanManager; };
    inline class CWhoWas*               GetWhoWas                   ( void )                { return m_pWhoWas; };

private:
    class CBlipManager*                 m_pBlipManager;
    class CMapFiles*                    m_pMapFiles;
    class CMapManager*                  m_pMapManager;
    class CPlayerManager*               m_pPlayerManager;
    class CRegisteredCommands*          m_pRegisteredCommands;
    class CVehicleManager*              m_pVehicleManager;
    class CLuaManager*                  m_pLuaManager;
    class CBanManager*                  m_pBanManager;
    class CWhoWas*                      m_pWhoWas;
    class CAccessControlListManager*    m_pACLManager;

    list < CConsoleCommand* >           m_Commands;
};

#endif
