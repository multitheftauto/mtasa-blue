/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CConsoleClient.cpp
 *  PURPOSE:     Console connected client class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CConsoleClient.h"
#include "CConsole.h"
#include "CMapManager.h"
#include "CAccountManager.h"
#include "CGame.h"

CConsoleClient::CConsoleClient(CConsole* pConsole) : CElement(pConsole->GetMapManager()->GetRootElement()), CClient(false)
{
    m_pAccount = g_pGame->GetAccountManager()->AddConsoleAccount(CONSOLE_ACCOUNT_NAME);
    m_iType = CElement::CONSOLE;
    SetTypeName("console");
    m_strNick = "Console";
    m_pConsole = pConsole;
}
