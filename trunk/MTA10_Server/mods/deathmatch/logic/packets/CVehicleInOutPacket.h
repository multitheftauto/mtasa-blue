/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CVehicleInOutPacket.h
*  PURPOSE:     Vehicle enter/exit synchronization packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_CVEHICLEINOUTPACKET_H
#define __PACKETS_CVEHICLEINOUTPACKET_H

#include "CPacket.h"
#include <CVector.h>

class CVehicleInOutPacket : public CPacket
{
public:
                            CVehicleInOutPacket         ( void );
                            CVehicleInOutPacket         ( ElementID ID,
                                                          unsigned char ucSeat,
                                                          unsigned char ucAction );
                            CVehicleInOutPacket         ( ElementID ID,
                                                          unsigned char ucSeat,
                                                          unsigned char ucAction,
                                                          ElementID PlayerIn,
                                                          ElementID PlayerOut );
    virtual                 ~CVehicleInOutPacket        ( void );

    inline ePacketID        GetPacketID                 ( void ) const                  { return PACKET_ID_VEHICLE_INOUT; };
    inline unsigned long    GetFlags                    ( void ) const                  { return PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool                    Read                        ( NetServerBitStreamInterface& BitStream );
    bool                    Write                       ( NetServerBitStreamInterface& BitStream ) const;

    inline ElementID        GetID                ( void )                        { return m_ID; };
    inline unsigned char    GetSeat                     ( void )                        { return m_ucSeat; };
    inline unsigned char    GetAction                   ( void )                        { return m_ucAction; };
    inline ElementID        GetPlayerIn                 ( void )                        { return m_PlayerIn; };
    inline ElementID        GetPlayerOut                ( void )                        { return m_PlayerOut; };
    inline unsigned char    GetStartedJacking           ( void )                        { return m_ucStartedJacking; };

    inline void             SetID                ( ElementID ID )         { m_ID = ID; };
    inline void             SetSeat                     ( unsigned char ucSeat )        { m_ucSeat = ucSeat; };
    inline void             SetAction                   ( unsigned char ucAction )      { m_ucAction = ucAction; };
    inline void             SetPlayerIn                 ( ElementID PlayerIn )          { m_PlayerIn = PlayerIn; };
    inline void             SetPlayerOut                ( ElementID PlayerOut )         { m_PlayerOut = PlayerOut; };
    inline void             SetFailReason               ( unsigned char ucReason )      { m_ucFailReason = ucReason; }
    inline void             SetCorrectVector            ( const CVector & vector )            { m_pCorrectVector = new CVector ( vector.fX, vector.fY, vector.fZ ); }

private:
    ElementID               m_ID;
    unsigned char           m_ucSeat;
    unsigned char           m_ucAction;
    ElementID               m_PlayerIn;
    ElementID               m_PlayerOut;
    unsigned char           m_ucStartedJacking;
    unsigned char           m_ucFailReason;
    CVector *               m_pCorrectVector;
};

#endif
