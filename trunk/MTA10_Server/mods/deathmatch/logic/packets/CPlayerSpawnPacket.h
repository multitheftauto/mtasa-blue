/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPlayerSpawnPacket.h
*  PURPOSE:     Player spawn packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_CPLAYERSPAWNPACKET_H
#define __PACKETS_CPLAYERSPAWNPACKET_H

#include "../CCommon.h"
#include "CPacket.h"

class CPlayerSpawnPacket : public CPacket
{
public:
                            CPlayerSpawnPacket          ( void );
                            CPlayerSpawnPacket          ( ElementID PlayerID,
                                                          const CVector& vecSpawnPosition,
                                                          float fSpawnRotation,
                                                          unsigned short usPlayerSkin,
                                                          unsigned char ucInterior,
                                                          unsigned short usDimension,
                                                          ElementID Team,
                                                          unsigned char ucTimeContext );

    inline ePacketID        GetPacketID                 ( void ) const                  { return PACKET_ID_PLAYER_SPAWN; };
    inline unsigned long    GetFlags                    ( void ) const                  { return PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool                    Write                       ( NetBitStreamInterface& BitStream ) const;

    inline ElementID        GetPlayer                   ( void )                        { return m_PlayerID; };
    inline CVector&         GetSpawnPosition            ( void )                        { return m_vecSpawnPosition; };
    inline float            GetSpawnRotation            ( void )                        { return m_fSpawnRotation; };
    inline unsigned short   GetPlayerSkin               ( void )                        { return m_usPlayerSkin; };
    inline ElementID        GetTeam                     ( void )                        { return m_Team; }
    inline unsigned char    GetInterior                 ( void )                        { return m_ucInterior; }
    inline unsigned short   GetDimension                ( void )                        { return m_usDimension; }

    inline void             SetPlayer                   ( ElementID PlayerID )          { m_PlayerID = PlayerID; };
    inline void             SetSpawnPosition            ( CVector& vecPosition )        { m_vecSpawnPosition = vecPosition; };
    inline void             SetSpawnRotation            ( float fRotation )             { m_fSpawnRotation = fRotation; };
    inline void             SetPlayerSkin               ( unsigned short usPlayerSkin ) { m_usPlayerSkin = usPlayerSkin; };
    inline void             SetTeam                     ( ElementID TeamID )            { m_Team = TeamID; }
    inline void             SetInterior                 ( unsigned char ucInterior )    { m_ucInterior = ucInterior; }
    inline void             SetDimension                ( unsigned short usDimension )  { m_usDimension = usDimension; }

private:
    ElementID               m_PlayerID;
    CVector                 m_vecSpawnPosition;
    float                   m_fSpawnRotation;
    unsigned short          m_usPlayerSkin;
    ElementID               m_Team;
    unsigned char           m_ucInterior;
    unsigned short          m_usDimension;
    unsigned char           m_ucTimeContext;
};

#endif
