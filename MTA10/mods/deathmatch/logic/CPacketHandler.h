/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPacketHandler.h
*  PURPOSE:     Header for packet handler class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Cecill Etheredge <ijsf@gmx.net>
*               Alberto Alonso <rydencillo@gmail.com>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CPACKETHANDLER_H
#define __CPACKETHANDLER_H

#include <CClientCommon.h>

class CClientEntity;
class CCustomData;

class CPacketHandler
{
    struct SEntityDependantStuff
    {
        CClientEntity*      pEntity;
        ElementID           Parent;
        ElementID           AttachedToID;
    };

public:
    bool                ProcessPacket                   ( unsigned char ucPacketID, NetBitStreamInterface& bitStream );

    void                Packet_ServerConnected          ( NetBitStreamInterface& bitStream );
    void                Packet_ServerJoined             ( NetBitStreamInterface& bitStream );
    void                Packet_ServerDisconnected       ( NetBitStreamInterface& bitStream );
    void                Packet_PlayerList               ( NetBitStreamInterface& bitStream );
    void                Packet_PlayerQuit               ( NetBitStreamInterface& bitStream );
    void                Packet_PlayerSpawn              ( NetBitStreamInterface& bitStream );
    void                Packet_PlayerWasted             ( NetBitStreamInterface& bitStream );
    void                Packet_PlayerChangeNick         ( NetBitStreamInterface& bitStream );
    void                Packet_PlayerPings              ( NetBitStreamInterface& bitStream );
    void                Packet_ChatEcho                 ( NetBitStreamInterface& bitStream );
    void                Packet_ConsoleEcho              ( NetBitStreamInterface& bitStream );
    void                Packet_DebugEcho                ( NetBitStreamInterface& bitStream );
    void                Packet_VehicleSpawn             ( NetBitStreamInterface& bitStream );
    void                Packet_VehicleDamageSync        ( NetBitStreamInterface& bitStream );
    void                Packet_Vehicle_InOut            ( NetBitStreamInterface& bitStream );
    void                Packet_VehicleTrailer           ( NetBitStreamInterface& bitStream );
    void                Packet_MapInfo                  ( NetBitStreamInterface& bitStream );
    void                Packet_PartialPacketInfo        ( NetBitStreamInterface& bitStream );
    void                Packet_EntityAdd                ( NetBitStreamInterface& bitStream );
    void                Packet_EntityRemove             ( NetBitStreamInterface& bitStream );
    void                Packet_PickupHideShow           ( NetBitStreamInterface& bitStream );
    void                Packet_PickupHitConfirm         ( NetBitStreamInterface& bitStream );
    void                Packet_Lua                      ( NetBitStreamInterface& bitStream );
    void                Packet_TextItem                 ( NetBitStreamInterface& bitStream );
    void                Packet_MarkerHit                ( NetBitStreamInterface& bitStream );
    void                Packet_ExplosionSync            ( NetBitStreamInterface& bitStream );
	void				Packet_FireSync					( NetBitStreamInterface& bitStream );
	void                Packet_ProjectileSync           ( NetBitStreamInterface& bitStream );
    void			    Packet_PlayerStats				( NetBitStreamInterface& bitStream );
    void                Packet_PlayerClothes            ( NetBitStreamInterface& bitStream );
    void                Packet_LuaEvent                 ( NetBitStreamInterface& bitStream );
    void                Packet_ScriptControl            ( NetBitStreamInterface& bitStream );
    void                Packet_ResourceStart            ( NetBitStreamInterface& bitStream );
    void                Packet_ResourceStop             ( NetBitStreamInterface& bitStream );
    void                Packet_DetonateSatchels         ( NetBitStreamInterface& bitStream );
    void                Packet_VoiceData                ( NetBitStreamInterface& bitStream );
};

#endif
