/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CClient.cpp
 *  PURPOSE:     Connected client class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CClient.h"
#include "CGame.h"
#include "CAccountManager.h"

extern CGame* g_pGame;

CClient::CClient(bool bAddGuestAccount)
{
    if (bAddGuestAccount)
    {
        m_pAccount = g_pGame->GetAccountManager()->AddGuestAccount(GUEST_ACCOUNT_NAME);
    }
}

CClient::~CClient()
{
    if (m_pAccount)
    {
        m_pAccount->SetClient(NULL);

        // If its a guest account, delete it
        if (!m_pAccount->IsRegistered())
            delete m_pAccount;
    }
}

bool CClient::IsRegistered()
{
    return m_pAccount->IsRegistered();
}

CElement* CClient::GetElement()
{
    CElement* pElement = NULL;
    switch (GetClientType())
    {
        case CLIENT_CONSOLE:
        {
            CConsoleClient* pClient = static_cast<CConsoleClient*>(this);
            pElement = static_cast<CElement*>(pClient);
            break;
        }
        case CLIENT_PLAYER:
        {
            CPlayer* pClient = static_cast<CPlayer*>(this);
            pElement = static_cast<CElement*>(pClient);
            break;
        }
    }
    return pElement;
}
