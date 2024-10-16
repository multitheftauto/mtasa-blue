/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CPacketHandler.h
 *  PURPOSE:     Header for packet handler class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <filesystem>
#include <CClientCommon.h>

class CClientEntity;
class CCustomData;

class CPacketHandler
{
    enum ePlayerDisconnectType
    {
        NO_REASON,
        INVALID_PASSWORD,
        INVALID_NICKNAME,
        BANNED_SERIAL,
        BANNED_IP,
        BANNED_ACCOUNT,
        VERSION_MISMATCH,
        JOIN_FLOOD,
        INCORRECT_PASSWORD,
        DIFFERENT_BRANCH,
        BAD_VERSION,
        SERVER_NEWER,
        SERVER_OLDER,
        NICK_CLASH,
        ELEMENT_FAILURE,
        GENERAL_REFUSED,
        SERIAL_VERIFICATION,
        CONNECTION_DESYNC,
        BAN,
        KICK,
        CUSTOM,
        SHUTDOWN
    };

    struct SEntityDependantStuff
    {
        CClientEntity* pEntity;
        ElementID      Parent;
        ElementID      AttachedToID;
        ElementID      LowLodObjectID;
    };

public:
    bool ProcessPacket(unsigned char ucPacketID, NetBitStreamInterface& bitStream);

    void Packet_ServerConnected(NetBitStreamInterface& bitStream);
    void Packet_ServerJoined(NetBitStreamInterface& bitStream);
    void Packet_ServerDisconnected(NetBitStreamInterface& bitStream);
    void Packet_PlayerList(NetBitStreamInterface& bitStream);
    void Packet_PlayerQuit(NetBitStreamInterface& bitStream);
    void Packet_PlayerSpawn(NetBitStreamInterface& bitStream);
    void Packet_PlayerWasted(NetBitStreamInterface& bitStream);
    void Packet_PlayerChangeNick(NetBitStreamInterface& bitStream);
    void Packet_ChatEcho(NetBitStreamInterface& bitStream);
    void Packet_ConsoleEcho(NetBitStreamInterface& bitStream);
    void Packet_DebugEcho(NetBitStreamInterface& bitStream);
    void Packet_VehicleSpawn(NetBitStreamInterface& bitStream);
    void Packet_VehicleDamageSync(NetBitStreamInterface& bitStream);
    void Packet_Vehicle_InOut(NetBitStreamInterface& bitStream);
    void Packet_VehicleTrailer(NetBitStreamInterface& bitStream);
    void Packet_MapInfo(NetBitStreamInterface& bitStream);
    void Packet_PartialPacketInfo(NetBitStreamInterface& bitStream);
    void Packet_PlayerNetworkStatus(NetBitStreamInterface& bitStream);
    void Packet_EntityAdd(NetBitStreamInterface& bitStream);
    void Packet_EntityRemove(NetBitStreamInterface& bitStream);
    void Packet_PickupHideShow(NetBitStreamInterface& bitStream);
    void Packet_PickupHitConfirm(NetBitStreamInterface& bitStream);
    void Packet_Lua(unsigned char ucPacketID, NetBitStreamInterface& bitStream);
    void Packet_TextItem(NetBitStreamInterface& bitStream);
    void Packet_MarkerHit(NetBitStreamInterface& bitStream);
    void Packet_ExplosionSync(NetBitStreamInterface& bitStream);
    void Packet_FireSync(NetBitStreamInterface& bitStream);
    void Packet_ProjectileSync(NetBitStreamInterface& bitStream);
    void Packet_PlayerStats(NetBitStreamInterface& bitStream);
    void Packet_PlayerClothes(NetBitStreamInterface& bitStream);
    void Packet_LuaEvent(NetBitStreamInterface& bitStream);
    void Packet_ScriptControl(NetBitStreamInterface& bitStream);
    void Packet_ResourceStart(NetBitStreamInterface& bitStream);
    void Packet_ResourceStop(NetBitStreamInterface& bitStream);
    void Packet_ResourceClientScripts(NetBitStreamInterface& bitStream);
    void Packet_DetonateSatchels(NetBitStreamInterface& bitStream);
    void Packet_DestroySatchels(NetBitStreamInterface& bitStream);
    void Packet_VoiceData(NetBitStreamInterface& bitStream);
    void Packet_UpdateInfo(NetBitStreamInterface& bitStream);
    void Packet_LatentTransfer(NetBitStreamInterface& bitStream);
    void Packet_SyncSettings(NetBitStreamInterface& bitStream);
    void Packet_PedTask(NetBitStreamInterface& bitStream);
    void Packet_ChatClear(NetBitStreamInterface& bitStream);
    void Packet_ServerInfoSync(NetBitStreamInterface& bitStream);
    void Packet_PolygonHeight(NetBitStreamInterface& bitStream);

    // For debugging protocol errors during ENTITY_ADD packet
    void    EntityAddDebugBegin(uint uiNumEntities, NetBitStreamInterface* pBitStream);
    void    EntityAddDebugNext(uint uiEntityIndex, int iReadOffset);
    void    RaiseEntityAddError(uint uiCode);
    SString EntityAddDebugRead(NetBitStreamInterface& bitStream);

    std::vector<int>       m_EntityAddReadOffsetStore;
    NetBitStreamInterface* m_pEntityAddBitStream;
    uint                   m_uiEntityAddNumEntities;
};
