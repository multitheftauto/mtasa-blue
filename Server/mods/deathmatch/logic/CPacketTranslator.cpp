/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CPacketTranslator.cpp
 *  PURPOSE:     Network packet translator class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPacketTranslator.h"
#include "packets/CPlayerJoinPacket.h"
#include "packets/CPlayerJoinDataPacket.h"
#include "packets/CPedWastedPacket.h"
#include "packets/CPlayerWastedPacket.h"
#include "packets/CPlayerQuitPacket.h"
#include "packets/CPlayerTimeoutPacket.h"
#include "packets/CPlayerPuresyncPacket.h"
#include "packets/CVehiclePuresyncPacket.h"
#include "packets/CKeysyncPacket.h"
#include "packets/CBulletsyncPacket.h"
#include "packets/CPedTaskPacket.h"
#include "packets/CCustomWeaponBulletSyncPacket.h"
#include "packets/CDetonateSatchelsPacket.h"
#include "packets/CDestroySatchelsPacket.h"
#include "packets/CCommandPacket.h"
#include "packets/CExplosionSyncPacket.h"
#include "packets/CProjectileSyncPacket.h"
#include "packets/CVehicleInOutPacket.h"
#include "packets/CVehicleDamageSyncPacket.h"
#include "packets/CVehicleTrailerPacket.h"
#include "packets/CVoiceDataPacket.h"
#include "packets/CVoiceEndPacket.h"
#include "packets/CUnoccupiedVehicleSyncPacket.h"
#include "packets/CPedSyncPacket.h"
#include "packets/CLuaEventPacket.h"
#include "packets/CCustomDataPacket.h"
#include "packets/CHealthPacket.h"
#include "packets/CCameraSyncPacket.h"
#include "packets/CObjectSyncPacket.h"
#include "packets/CPlayerTransgressionPacket.h"
#include "packets/CPlayerDiagnosticPacket.h"
#include "packets/CPlayerModInfoPacket.h"
#include "packets/CPlayerACInfoPacket.h"
#include "packets/CPlayerScreenShotPacket.h"
#include "packets/CUnoccupiedVehiclePushPacket.h"
#include "packets/CPlayerNoSocketPacket.h"
#include "packets/CPlayerNetworkStatusPacket.h"
#include "packets/CPlayerResourceStartPacket.h"

CPacketTranslator::CPacketTranslator(CPlayerManager* pPlayerManager)
{
    m_pPlayerManager = pPlayerManager;
}

CPacketTranslator::~CPacketTranslator()
{
}

CPacket* CPacketTranslator::Translate(const NetServerPlayerID& Socket, ePacketID PacketID, NetBitStreamInterface& BitStream, SNetExtraInfo* pNetExtraInfo)
{
    // Create the packet class
    CPacket* pTemp = NULL;
    switch ((int)PacketID)
    {
        case PACKET_ID_PLAYER_JOIN:
            pTemp = new CPlayerJoinPacket;
            break;

        case PACKET_ID_PLAYER_JOINDATA:
            pTemp = new CPlayerJoinDataPacket;
            break;

        case PACKET_ID_PED_WASTED:
            pTemp = new CPedWastedPacket;
            break;

        case PACKET_ID_PLAYER_WASTED:
            pTemp = new CPlayerWastedPacket;
            break;

        case PACKET_ID_PLAYER_QUIT:
            pTemp = new CPlayerQuitPacket;
            break;

        case PACKET_ID_PLAYER_TIMEOUT:
            pTemp = new CPlayerTimeoutPacket;
            break;

        case PACKET_ID_PLAYER_PURESYNC:
            pTemp = new CPlayerPuresyncPacket;
            break;

        case PACKET_ID_PLAYER_VEHICLE_PURESYNC:
            pTemp = new CVehiclePuresyncPacket;
            break;

        case PACKET_ID_PLAYER_KEYSYNC:
            pTemp = new CKeysyncPacket;
            break;

        case PACKET_ID_PLAYER_BULLETSYNC:
            pTemp = new CBulletsyncPacket;
            break;

        case PACKET_ID_PED_TASK:
            pTemp = new CPedTaskPacket;
            break;

        case PACKET_ID_WEAPON_BULLETSYNC:
            pTemp = new CCustomWeaponBulletSyncPacket;
            break;

        case PACKET_ID_DETONATE_SATCHELS:
            pTemp = new CDetonateSatchelsPacket;
            break;

        case PACKET_ID_DESTROY_SATCHELS:
            pTemp = new CDestroySatchelsPacket;
            break;

        case PACKET_ID_COMMAND:
            pTemp = new CCommandPacket;
            break;

        case PACKET_ID_EXPLOSION:
            pTemp = new CExplosionSyncPacket;
            break;

        case PACKET_ID_PROJECTILE:
            pTemp = new CProjectileSyncPacket;
            break;

        case PACKET_ID_VEHICLE_INOUT:
            pTemp = new CVehicleInOutPacket;
            break;

        case PACKET_ID_VEHICLE_DAMAGE_SYNC:
            pTemp = new CVehicleDamageSyncPacket;
            break;

        case PACKET_ID_VEHICLE_TRAILER:
            pTemp = new CVehicleTrailerPacket;
            break;

        case PACKET_ID_VOICE_DATA:
            pTemp = new CVoiceDataPacket;
            break;

        case PACKET_ID_VOICE_END:
            pTemp = new CVoiceEndPacket;
            break;

        case PACKET_ID_UNOCCUPIED_VEHICLE_SYNC:
            pTemp = new CUnoccupiedVehicleSyncPacket;
            break;

        case PACKET_ID_PED_SYNC:
            pTemp = new CPedSyncPacket;
            break;

        case PACKET_ID_LUA_EVENT:
            pTemp = new CLuaEventPacket;
            break;

        case PACKET_ID_CUSTOM_DATA:
            pTemp = new CCustomDataPacket;
            break;

        case PACKET_ID_HEALTH_UPDATE:
            pTemp = new CHealthPacket;
            break;

        case PACKET_ID_CAMERA_SYNC:
            pTemp = new CCameraSyncPacket;
            break;

        case PACKET_ID_OBJECT_SYNC:
            pTemp = new CObjectSyncPacket;
            break;

        case PACKET_ID_PLAYER_TRANSGRESSION:
            pTemp = new CPlayerTransgressionPacket;
            break;

        case PACKET_ID_PLAYER_DIAGNOSTIC:
            pTemp = new CPlayerDiagnosticPacket;
            break;

        case PACKET_ID_PLAYER_MODINFO:
            pTemp = new CPlayerModInfoPacket;
            break;

        case PACKET_ID_PLAYER_ACINFO:
            pTemp = new CPlayerACInfoPacket;
            break;

        case PACKET_ID_PLAYER_SCREENSHOT:
            pTemp = new CPlayerScreenShotPacket;
            break;

        case PACKET_ID_VEHICLE_PUSH_SYNC:
            pTemp = new CUnoccupiedVehiclePushPacket;
            break;

        case PACKET_ID_PLAYER_NO_SOCKET:
            pTemp = new CPlayerNoSocketPacket;
            break;

        case PACKET_ID_PLAYER_NETWORK_STATUS:
            pTemp = new CPlayerNetworkStatusPacket;
            break;

        case PACKET_ID_PLAYER_RESOURCE_START:
            pTemp = new CPlayerResourceStartPacket;
            break;

        default:
            break;
    }

    // Could we create the packet?
    if (pTemp)
    {
        // Set the source socket and player
        pTemp->SetSourceSocket(Socket);

        // Make sure players that have just disconnected don't get their packet processed
        CPlayer* pSourcePlayer = m_pPlayerManager->Get(Socket);
        if (pSourcePlayer && pSourcePlayer->IsBeingDeleted())
            pSourcePlayer = NULL;

        if (pSourcePlayer && pNetExtraInfo)
        {
            if (pNetExtraInfo->m_bHasPing)
                pSourcePlayer->SetPing(pNetExtraInfo->m_uiPing);
        }

        // Set the source player
        pTemp->SetSourceElement(pSourcePlayer);

        // Check we have a source player if the packet type needs it
        if (!pSourcePlayer && pTemp->RequiresSourcePlayer())
        {
            delete pTemp;
            pTemp = NULL;
        }
        else
            // Attempt to read the content, if we fail, delete the packet again
            if (!pTemp->Read(BitStream))
        {
            delete pTemp;
            pTemp = NULL;
        }
    }

    // Return the class
    return pTemp;
}
