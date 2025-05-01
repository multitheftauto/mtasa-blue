/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CConsole.h
 *  PURPOSE:     Console handler class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CConsoleCommand.h"
#include <list>

class CConsole
{
public:
    CConsole(class CBlipManager* pBlipManager, class CMapManager* pMapManager, class CPlayerManager* pPlayerManager,
             class CRegisteredCommands* pRegisteredCommands, class CVehicleManager* pVehicleManager, class CBanManager* pBanManager,
             class CAccessControlListManager* pACLManager);
    ~CConsole();

    bool HandleInput(const char* szCommand, CClient* pClient, CClient* pEchoClient);

    void             AddCommand(FCommandHandler* pHandler, const char* szCommand, bool bRestricted, const char* szConsoleHelpText);
    void             DeleteCommand(const char* szCommand);
    void             DeleteAllCommands();
    CConsoleCommand* GetCommand(const char* szKey);

    std::list<CConsoleCommand*>::const_iterator CommandsBegin() { return m_Commands.begin(); };
    std::list<CConsoleCommand*>::const_iterator CommandsEnd() { return m_Commands.end(); };
    const auto&                                 CommandsList() { return m_Commands; }

    class CBlipManager*    GetBlipManager() { return m_pBlipManager; };
    class CMapManager*     GetMapManager() { return m_pMapManager; };
    class CPlayerManager*  GetPlayerManager() { return m_pPlayerManager; };
    class CVehicleManager* GetVehicleManager() { return m_pVehicleManager; };
    class CBanManager*     GetBanManager() { return m_pBanManager; };

private:
    class CBlipManager*              m_pBlipManager;
    class CMapManager*               m_pMapManager;
    class CPlayerManager*            m_pPlayerManager;
    class CRegisteredCommands*       m_pRegisteredCommands;
    class CVehicleManager*           m_pVehicleManager;
    class CBanManager*               m_pBanManager;
    class CAccessControlListManager* m_pACLManager;

    std::list<CConsoleCommand*> m_Commands;
};
