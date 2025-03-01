/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/rpc/CPlayerRPCs.cpp
 *  PURPOSE:     Player remote procedure calls
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
#include "CPlayerRPCs.h"

void CPlayerRPCs::LoadFunctions()
{
    AddHandler(SET_PLAYER_MONEY, SetPlayerMoney, "SetPlayerMoney");
    AddHandler(SHOW_PLAYER_HUD_COMPONENT, ShowPlayerHudComponent, "ShowPlayerHudComponent");
    AddHandler(FORCE_PLAYER_MAP, ForcePlayerMap, "ForcePlayerMap");
    AddHandler(SET_PLAYER_NAMETAG_TEXT, SetPlayerNametagText, "SetPlayerNametagText");
    AddHandler(REMOVE_PLAYER_NAMETAG_COLOR, RemovePlayerNametagColor, "RemovePlayerNametagColor");
    AddHandler(SET_PLAYER_NAMETAG_COLOR, SetPlayerNametagColor, "SetPlayerNametagColor");
    AddHandler(SET_PLAYER_NAMETAG_SHOWING, SetPlayerNametagShowing, "SetPlayerNametagShowing");
    AddHandler(SET_PLAYER_TEAM, SetPlayerTeam, "SetPlayerTeam");
    AddHandler(TAKE_PLAYER_SCREEN_SHOT, TakePlayerScreenShot, "TakePlayerScreenShot");
    AddHandler(SET_PLAYER_SCRIPT_DEBUG_LEVEL, SetPlayerScriptDebugLevel, "SetPlayerScriptDebugLevel");
}

void CPlayerRPCs::SetPlayerMoney(NetBitStreamInterface& bitStream)
{
    // Read out the new money amount
    long lMoney;
    bool bInstant = false;
    if (bitStream.Read(lMoney))
    {
        if (bitStream.GetNumberOfUnreadBits() > 0)
            bitStream.ReadBit(bInstant);

        // Set it
        m_pClientGame->SetMoney(lMoney, bInstant);
    }
}

void CPlayerRPCs::ShowPlayerHudComponent(NetBitStreamInterface& bitStream)
{
    unsigned char ucComponent, ucShow;
    if (bitStream.Read(ucComponent) && bitStream.Read(ucShow))
    {
        bool bShow = (ucShow != 0);
        g_pGame->GetHud()->SetComponentVisible((eHudComponent)ucComponent, bShow);

        if (ucComponent == HUD_AREA_NAME || ucComponent == HUD_ALL)
            g_pClientGame->SetHudAreaNameDisabled(!bShow);
    }
}

void CPlayerRPCs::ForcePlayerMap(NetBitStreamInterface& bitStream)
{
    unsigned char ucVisible;
    if (bitStream.Read(ucVisible))
    {
        bool bVisible = (ucVisible == 1);
        m_pClientGame->GetPlayerMap()->SetForcedState(bVisible);
    }
}

void CPlayerRPCs::SetPlayerNametagText(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    unsigned short usTextLength;
    if (bitStream.Read(usTextLength))
    {
        CClientPlayer* pPlayer = m_pPlayerManager->Get(pSource->GetID());
        if (pPlayer)
        {
            char* szText = NULL;
            if (usTextLength > 0)
            {
                szText = new char[usTextLength + 1];
                szText[usTextLength] = NULL;
                if (bitStream.Read(szText, usTextLength))
                {
                    pPlayer->SetNametagText(szText);
                }
                delete[] szText;
            }
            else
                pPlayer->SetNametagText(NULL);
        }
    }
}

void CPlayerRPCs::SetPlayerNametagColor(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    unsigned char ucR, ucG, ucB;
    if (bitStream.Read(ucR) && bitStream.Read(ucG) && bitStream.Read(ucB))
    {
        CClientPlayer* pPlayer = m_pPlayerManager->Get(pSource->GetID());
        if (pPlayer)
            pPlayer->SetNametagOverrideColor(ucR, ucG, ucB);
    }
}

void CPlayerRPCs::RemovePlayerNametagColor(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    CClientPlayer* pPlayer = m_pPlayerManager->Get(pSource->GetID());
    if (pPlayer)
        pPlayer->RemoveNametagOverrideColor();
}

void CPlayerRPCs::SetPlayerNametagShowing(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    unsigned char ucShowing;
    if (bitStream.Read(ucShowing))
    {
        CClientPlayer* pPlayer = m_pPlayerManager->Get(pSource->GetID());
        if (pPlayer)
        {
            pPlayer->SetNametagShowing((ucShowing == 1));
        }
    }
}

void CPlayerRPCs::SetPlayerTeam(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    ElementID TeamID;
    if (bitStream.Read(TeamID))
    {
        CClientTeam*   pTeam = m_pTeamManager->GetTeam(TeamID);
        CClientPlayer* pPlayer = m_pPlayerManager->Get(pSource->GetID());
        if (pPlayer)
        {
            pPlayer->SetTeam(pTeam, true);
        }
    }
}

void CPlayerRPCs::TakePlayerScreenShot(NetBitStreamInterface& bitStream)
{
    ushort     usSizeX;
    ushort     usSizeY;
    SString    strTag;
    uchar      ucQuality;
    uint       uiMaxBandwidth;
    ushort     usMaxPacketSize;
    CResource* pResource;
    uint       uiServerSentTime;

    bitStream.Read(usSizeX);
    bitStream.Read(usSizeY);
    bitStream.ReadString(strTag);
    bitStream.Read(ucQuality);
    bitStream.Read(uiMaxBandwidth);
    bitStream.Read(usMaxPacketSize);
    if (bitStream.Version() >= 0x53)
    {
        ushort usResourceNetId;
        bitStream.Read(usResourceNetId);
        pResource = g_pClientGame->GetResourceManager()->GetResourceFromNetID(usResourceNetId);
    }
    else
    {
        SString strResourceName;
        bitStream.ReadString(strResourceName);
        pResource = g_pClientGame->GetResourceManager()->GetResource(strResourceName);
    }

    if (!bitStream.Read(uiServerSentTime))
        return;

    m_pClientGame->TakePlayerScreenShot(usSizeX, usSizeY, strTag, ucQuality, uiMaxBandwidth, usMaxPacketSize, pResource, uiServerSentTime);
}

void CPlayerRPCs::SetPlayerScriptDebugLevel(NetBitStreamInterface& stream)
{
    CClientPlayer* localPlayer = g_pClientGame->GetPlayerManager()->GetLocalPlayer();

    if (!localPlayer)
        return;

    std::uint8_t scriptDebugLevel;

    if (!stream.Read(scriptDebugLevel))
        return;

    localPlayer->SetPlayerScriptDebugLevel(scriptDebugLevel);
}
