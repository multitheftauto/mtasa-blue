/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CRPCFunctions.cpp
 *  PURPOSE:     Remote procedure call functionality class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CRPCFunctions.h"
#include "CGame.h"
#include "CMainConfig.h"
#include "Utils.h"
#include "CElementIDs.h"
#include "CWeaponNames.h"
#include "CPerfStatManager.h"
#include "CKeyBinds.h"
#include "CStaticFunctionDefinitions.h"
#include "net/SyncStructures.h"

CRPCFunctions* g_pRPCFunctions = NULL;
extern CGame*  g_pGame;

CPlayer* CRPCFunctions::m_pSourcePlayer;

static CPlayerManager* m_pPlayerManager;

CRPCFunctions::CRPCFunctions()
{
    g_pRPCFunctions = this;

    m_pPlayerManager = g_pGame->GetPlayerManager();
    AddHandlers();
}

CRPCFunctions::~CRPCFunctions()
{
    std::vector<SRPCHandler*>::iterator iter = m_RPCHandlers.begin();
    for (; iter != m_RPCHandlers.end(); iter++)
    {
        delete *iter;
    }
    m_RPCHandlers.clear();

    g_pRPCFunctions = NULL;
}

void CRPCFunctions::AddHandlers()
{
    AddHandler(PLAYER_INGAME_NOTICE, PlayerInGameNotice);
    AddHandler(INITIAL_DATA_STREAM, InitialDataStream);
    AddHandler(PLAYER_TARGET, PlayerTarget);
    AddHandler(PLAYER_WEAPON, PlayerWeapon);
    AddHandler(KEY_BIND, KeyBind);
    AddHandler(CURSOR_EVENT, CursorEvent);
    AddHandler(REQUEST_STEALTH_KILL, RequestStealthKill);
}

void CRPCFunctions::AddHandler(unsigned char ucID, pfnRPCHandler Callback)
{
    SRPCHandler* pHandler = new SRPCHandler;
    pHandler->ID = ucID;
    pHandler->Callback = Callback;
    g_pRPCFunctions->m_RPCHandlers.push_back(pHandler);
}

void CRPCFunctions::ProcessPacket(const NetServerPlayerID& Socket, NetBitStreamInterface& bitStream)
{
    m_pSourcePlayer = m_pPlayerManager->Get(Socket);
    if (m_pSourcePlayer && !m_pSourcePlayer->IsBeingDeleted())
    {
        unsigned char ucFunctionID = 255;
        bitStream.Read(ucFunctionID);

        CPerfStatRPCPacketUsage::GetSingleton()->UpdatePacketUsageIn(ucFunctionID, bitStream.GetNumberOfBytesUsed());

        SRPCHandler*                   pHandler;
        vector<SRPCHandler*>::iterator iter = m_RPCHandlers.begin();
        for (; iter != m_RPCHandlers.end(); iter++)
        {
            pHandler = *iter;
            if (pHandler->ID == ucFunctionID)
            {
                (pHandler->Callback)(bitStream);
                break;
            }
        }
    }
}

void CRPCFunctions::PlayerInGameNotice(NetBitStreamInterface& bitStream)
{
    CLOCK("NetServerPulse::RPC", "PlayerInGameNotice");
    // Already ingame? Protocol error
    if (m_pSourcePlayer->IsJoined())
    {
        DisconnectPlayer(g_pGame, *m_pSourcePlayer, "Protocol error: Already ingame");
    }
    else
    {
        // Join him to the game
        g_pGame->JoinPlayer(*m_pSourcePlayer);
    }
    UNCLOCK("NetServerPulse::RPC", "PlayerInGameNotice");
}

void CRPCFunctions::InitialDataStream(NetBitStreamInterface& bitStream)
{
    CLOCK("NetServerPulse::RPC", "InitialDataStream");
    // Already sent initial stuff? Protocol error
    if (m_pSourcePlayer->IsJoined())
    {
        DisconnectPlayer(g_pGame, *m_pSourcePlayer, "Protocol error: Already joined");
    }
    else
    {
        // Send him the initial stuff
        g_pGame->InitialDataStream(*m_pSourcePlayer);
    }
    UNCLOCK("NetServerPulse::RPC", "InitialDataStream");
}

void CRPCFunctions::PlayerTarget(NetBitStreamInterface& bitStream)
{
    CLOCK("NetServerPulse::RPC", "PlayerTarget");
    if (m_pSourcePlayer->IsJoined())
    {
        ElementID TargetID;
        bitStream.Read(TargetID);

        CElement* pTarget = NULL;
        if (TargetID != INVALID_ELEMENT_ID)
            pTarget = CElementIDs::GetElement(TargetID);
        m_pSourcePlayer->SetTargetedElement(pTarget);

        // Call our script event
        CLuaArguments Arguments;
        if (pTarget)
            Arguments.PushElement(pTarget);
        else
            Arguments.PushBoolean(false);

        m_pSourcePlayer->CallEvent("onPlayerTarget", Arguments);
    }
    UNCLOCK("NetServerPulse::RPC", "PlayerTarget");
}

void CRPCFunctions::PlayerWeapon(NetBitStreamInterface& bitStream)
{
    CLOCK("NetServerPulse::RPC", "PlayerWeapon");
    if (m_pSourcePlayer->IsJoined() && m_pSourcePlayer->IsSpawned())
    {
        unsigned char ucPrevSlot = m_pSourcePlayer->GetWeaponSlot();

        // We don't get the puresync packet containing totalAmmo = 0 for slot 8 (THROWN), slot 7 (HEAVY) and slot 9 (SPECIAL)
        if ((bitStream.Version() >= 0x44 && ucPrevSlot == WEAPONSLOT_TYPE_THROWN) || bitStream.Version() >= 0x4D)
        {
            if (bitStream.ReadBit() && (ucPrevSlot == WEAPONSLOT_TYPE_THROWN ||
                                        (bitStream.Version() >= 0x5A && (ucPrevSlot == WEAPONSLOT_TYPE_HEAVY || ucPrevSlot == WEAPONSLOT_TYPE_SPECIAL))))
            {
                CWeapon* pPrevWeapon = m_pSourcePlayer->GetWeapon(ucPrevSlot);
                pPrevWeapon->usAmmo = 0;
                pPrevWeapon->usAmmoInClip = 0;
            }
        }

        SWeaponSlotSync slot;
        bitStream.Read(&slot);
        unsigned int uiSlot = slot.data.uiSlot;

        if (uiSlot != ucPrevSlot)
        {
            CLuaArguments Arguments;
            Arguments.PushNumber(m_pSourcePlayer->GetWeaponType(ucPrevSlot));
            Arguments.PushNumber(m_pSourcePlayer->GetWeaponType(uiSlot));

            m_pSourcePlayer->CallEvent("onPlayerWeaponSwitch", Arguments);
        }

        m_pSourcePlayer->SetWeaponSlot(uiSlot);
        CWeapon* pWeapon = m_pSourcePlayer->GetWeapon(uiSlot);

        if (CWeaponNames::DoesSlotHaveAmmo(uiSlot))
        {
            if (pWeapon)
            {
                SWeaponAmmoSync ammo(pWeapon->ucType, true, true);
                bitStream.Read(&ammo);

                pWeapon->usAmmo = ammo.data.usTotalAmmo;
                pWeapon->usAmmoInClip = ammo.data.usAmmoInClip;
            }
        }
        else if (pWeapon)
        {
            pWeapon->usAmmo = 1;
            pWeapon->usAmmoInClip = 1;
            // Keep the server synced with the client (GTASA gives the client a detonator when they shoot so if they changed to slot 12 they obviously have one)
            if (uiSlot == 12)
                // Give them the detonator
                CStaticFunctionDefinitions::GiveWeapon(m_pSourcePlayer, 40, 1, true);
        }
    }
    UNCLOCK("NetServerPulse::RPC", "PlayerWeapon");
}

void CRPCFunctions::KeyBind(NetBitStreamInterface& bitStream)
{
    CLOCK("NetServerPulse::RPC", "KeyBind");

    unsigned char ucType;
    bool          bHitState = false;
    if (bitStream.ReadBit() == true)
        ucType = 1;
    else
        ucType = 0;
    bitStream.ReadBit(bHitState);

    unsigned char ucKeyLength = bitStream.GetNumberOfUnreadBits() >> 3;

    char szKey[256];
    bitStream.Read(szKey, ucKeyLength);
    szKey[ucKeyLength] = 0;

    m_pSourcePlayer->GetKeyBinds()->ProcessKey(szKey, bHitState, (eKeyBindType)ucType);

    UNCLOCK("NetServerPulse::RPC", "KeyBind");
}

void CRPCFunctions::CursorEvent(NetBitStreamInterface& bitStream)
{
    CLOCK("NetServerPulse::RPC", "CursorEvent");

    SMouseButtonSync button;
    unsigned short   usX;
    unsigned short   usY;
    SPositionSync    position(false);
    bool             bHasCollisionElement;
    ElementID        elementID;

    if (bitStream.Read(&button) && bitStream.ReadCompressed(usX) && bitStream.ReadCompressed(usY) && bitStream.Read(&position) &&
        bitStream.ReadBit(bHasCollisionElement) && (!bHasCollisionElement || bitStream.Read(elementID)))
    {
        unsigned char ucButton = button.data.ucButton;
        CVector2D     vecCursorPosition(static_cast<float>(usX), static_cast<float>(usY));
        CVector       vecPosition = position.data.vecPosition;
        if (!bHasCollisionElement)
            elementID = INVALID_ELEMENT_ID;

        if (m_pSourcePlayer->IsJoined())
        {
            // Get the button and state
            const char* szButton = NULL;
            const char* szState = NULL;
            switch (ucButton)
            {
                case 0:
                    szButton = "left";
                    szState = "down";
                    break;
                case 1:
                    szButton = "left";
                    szState = "up";
                    break;
                case 2:
                    szButton = "middle";
                    szState = "down";
                    break;
                case 3:
                    szButton = "middle";
                    szState = "up";
                    break;
                case 4:
                    szButton = "right";
                    szState = "down";
                    break;
                case 5:
                    szButton = "right";
                    szState = "up";
                    break;
            }
            if (szButton && szState)
            {
                CElement* pElement = CElementIDs::GetElement(elementID);
                if (pElement)
                {
                    // Call the onElementClicked event
                    CLuaArguments Arguments;
                    Arguments.PushString(szButton);
                    Arguments.PushString(szState);
                    Arguments.PushElement(m_pSourcePlayer);
                    Arguments.PushNumber(vecPosition.fX);
                    Arguments.PushNumber(vecPosition.fY);
                    Arguments.PushNumber(vecPosition.fZ);
                    pElement->CallEvent("onElementClicked", Arguments);
                }
                // Call the onPlayerClick event
                CLuaArguments Arguments;
                Arguments.PushString(szButton);
                Arguments.PushString(szState);
                if (pElement)
                    Arguments.PushElement(pElement);
                else
                    Arguments.PushNil();
                Arguments.PushNumber(vecPosition.fX);
                Arguments.PushNumber(vecPosition.fY);
                Arguments.PushNumber(vecPosition.fZ);
                Arguments.PushNumber(vecCursorPosition.fX);
                Arguments.PushNumber(vecCursorPosition.fY);
                m_pSourcePlayer->CallEvent("onPlayerClick", Arguments);

                // TODO: iterate server-side element managers for the click events, eg: colshapes
            }
        }
    }
    UNCLOCK("NetServerPulse::RPC", "CursorEvent");
}

void CRPCFunctions::RequestStealthKill(NetBitStreamInterface& bitStream)
{
    CLOCK("NetServerPulse::RPC", "RequestStealthKill");
    ElementID ID;
    bitStream.Read(ID);
    CElement* pElement = CElementIDs::GetElement(ID);
    if (pElement)
    {
        int elementType = pElement->GetType();
        if (elementType == CElement::PLAYER || elementType == CElement::PED)
        {
            CPed* pTarget = static_cast<CPed*>(pElement);

            // Are they both alive?
            if (!m_pSourcePlayer->IsDead() && !pTarget->IsDead())
            {
                // Do we have any record of the killer currently having a knife?
                if (m_pSourcePlayer->GetWeaponType(1) == 4)
                {
                    // Are they close enough?
                    if (DistanceBetweenPoints3D(m_pSourcePlayer->GetPosition(), pTarget->GetPosition()) <= STEALTH_KILL_RANGE)
                    {
                        CLuaArguments Arguments;
                        Arguments.PushElement(pTarget);
                        if (m_pSourcePlayer->CallEvent("onPlayerStealthKill", Arguments))
                        {
                            // Start the stealth kill
                            CStaticFunctionDefinitions::KillPed(pTarget, m_pSourcePlayer, 4 /*WEAPONTYPE_KNIFE*/, 9 /*BODYPART_HEAD*/, true);
                        }
                    }
                }
                else
                {
                    // You shouldn't be able to get here without cheating to get a knife.
                    if (!g_pGame->GetConfig()->IsDisableAC("2"))
                    {
                        // Kick disabled as sometimes causing false positives due weapon slot sync problems
                        #if 0
                        CStaticFunctionDefinitions::KickPlayer ( m_pSourcePlayer, NULL, "AC #2: You were kicked from the game" );
                        #endif
                    }
                }
            }
        }
    }
    UNCLOCK("NetServerPulse::RPC", "RequestStealthKill");
}
