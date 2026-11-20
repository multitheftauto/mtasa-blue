/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CConsole.cpp
 *  PURPOSE:     Console handler class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CConsole.h"
#include "Utils.h"
#include "CLogger.h"
#include "CAccessControlListManager.h"
#include "CRegisteredCommands.h"
#include "CPlayerManager.h"
#include "CConsoleClient.h"

CConsole::CConsole(CBlipManager* pBlipManager, CMapManager* pMapManager, CPlayerManager* pPlayerManager, CRegisteredCommands* pRegisteredCommands,
                   CVehicleManager* pVehicleManager, CBanManager* pBanManager, CAccessControlListManager* pACLManager)
{
    // Init
    m_pBlipManager = pBlipManager;
    m_pMapManager = pMapManager;
    m_pPlayerManager = pPlayerManager;
    m_pRegisteredCommands = pRegisteredCommands;
    m_pVehicleManager = pVehicleManager;
    m_pBanManager = pBanManager;
    m_pACLManager = pACLManager;
}

CConsole::~CConsole()
{
    // Delete all our commands
    DeleteAllCommands();
}

bool CConsole::HandleInput(const char* szCommand, CClient* pClient, CClient* pEchoClient)
{
    if (!szCommand || !pClient)
        return false;

    std::string commandBuffer = szCommand;
    if (commandBuffer.empty())
        return false;

    stripControlCodes(&commandBuffer[0]);

    // Split into key and arguments safely without thread-unsafe strtok or fixed buffer truncation
    std::string  key;
    const char*  szArguments = nullptr;
    const size_t spacePos = commandBuffer.find(' ');
    if (spacePos != std::string::npos)
    {
        key = commandBuffer.substr(0, spacePos);
        if (spacePos + 1 < commandBuffer.length())
            szArguments = commandBuffer.c_str() + spacePos + 1;
    }
    else
    {
        key = commandBuffer;
    }

    // Does the key exist?
    if (!key.empty())
    {
        const char* szKey = key.c_str();

        if (pClient->GetClientType() == CClient::CLIENT_PLAYER)
        {
            CPlayer* pPlayer = static_cast<CPlayer*>(pClient);

            CLuaArguments Arguments;
            Arguments.PushString(szKey);

            if (!pPlayer->CallEvent("onPlayerCommand", Arguments))
                return false;
        }

        CConsoleCommand* pCommand = GetCommand(szKey);
        if (pCommand)
        {
            // Can this user use this command?
            if (m_pACLManager->CanObjectUseRight(pClient->GetAccount()->GetName().c_str(), CAccessControlListGroupObject::OBJECT_TYPE_USER, szKey,
                                                 CAccessControlListRight::RIGHT_TYPE_COMMAND, !pCommand->IsRestricted()))
            {
                return (*pCommand)(this, szArguments, pClient, pEchoClient);
            }

            // Not enough access, tell the console
            CLogger::LogPrintf("ACL: Denied '%s' access to command '%s'\n", pClient->GetNick(), szKey);

            // Tell the client
            char szBuffer[128];
            snprintf(szBuffer, sizeof(szBuffer), "ACL: Access denied for '%s'", szKey);
            szBuffer[sizeof(szBuffer) - 1] = '\0';

            pClient->SendEcho(szBuffer);
            return false;
        }

        // Let the script handle it
        int iClientType = pClient->GetClientType();

        switch (iClientType)
        {
            case CClient::CLIENT_PLAYER:
            {
                // See if any registered command can process it
                CPlayer* pPlayer = static_cast<CPlayer*>(pClient);
                m_pRegisteredCommands->ProcessCommand(szKey, szArguments, pClient);

                // HACK: if the client gets destroyed before here, dont continue
                if (m_pPlayerManager->Exists(pPlayer))
                {
                    // Call the console event
                    CLuaArguments Arguments;
                    Arguments.PushString(szCommand);
                    pPlayer->CallEvent("onConsole", Arguments);
                }
                break;
            }
            case CClient::CLIENT_CONSOLE:
            {
                // See if any registered command can process it
                CConsoleClient* pConsole = static_cast<CConsoleClient*>(pClient);
                m_pRegisteredCommands->ProcessCommand(szKey, szArguments, pClient);

                // Call the console event
                CLuaArguments Arguments;
                Arguments.PushString(szCommand);
                pConsole->CallEvent("onConsole", Arguments);
                break;
            }
            default:
                break;
        }
    }

    // Doesn't exist
    return false;
}

void CConsole::AddCommand(FCommandHandler* pHandler, const char* szCommand, bool bRestricted, const char* szConsoleHelpText)
{
    if (!szCommand)
        return;

    // Make a command class and add it to the list and map
    CConsoleCommand* pCommand = new CConsoleCommand(pHandler, szCommand, bRestricted, szConsoleHelpText);
    m_Commands.push_back(pCommand);
    m_commandMap[szCommand] = pCommand;
}

void CConsole::DeleteCommand(const char* szCommand)
{
    if (!szCommand)
        return;

    m_commandMap.erase(szCommand);

    // Find the command and delete it
    for (auto iter = m_Commands.begin(); iter != m_Commands.end(); ++iter)
    {
        // Names match?
        if (strcmp(szCommand, (*iter)->GetCommand()) == 0)
        {
            delete *iter;
            m_Commands.erase(iter);
            return;
        }
    }
}

void CConsole::DeleteAllCommands()
{
    // Delete all the command classes
    for (auto* pCommand : m_Commands)
    {
        delete pCommand;
    }

    // Clear the commandlist and map
    m_Commands.clear();
    m_commandMap.clear();
}

CConsoleCommand* CConsole::GetCommand(const char* szKey)
{
    if (!szKey)
        return nullptr;

    const auto iter = m_commandMap.find(szKey);
    if (iter != m_commandMap.end())
        return iter->second;

    return nullptr;
}
