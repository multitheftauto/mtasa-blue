/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CVehicleTrailerPacket.h
*  PURPOSE:     Vehicle trailer synchronization packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_CVEHICLETRAILERPACKET_H
#define __PACKETS_CVEHICLETRAILERPACKET_H

#include "../CVehicle.h"
#include "../CCommon.h"
#include "CPacket.h"

class CVehicleTrailerPacket : public CPacket
{
public:
                                    CVehicleTrailerPacket       ( void ) {};
                                    CVehicleTrailerPacket       ( CVehicle* pVehicle,
                                                                  CVehicle* pTrailer,
                                                                  bool bAttached );

    inline ePacketID                GetPacketID                 ( void ) const              { return PACKET_ID_VEHICLE_TRAILER; };
    inline unsigned long            GetFlags                    ( void ) const              { return PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool                            Read                        ( NetBitStreamInterface& BitStream );
    bool                            Write                       ( NetBitStreamInterface& BitStream ) const;

    inline ElementID                GetVehicle                  ( void )                    { return m_Vehicle; };
    inline ElementID                GetAttachedVehicle          ( void )                    { return m_AttachedVehicle; };
    inline bool                     GetAttached                 ( void )                    { return m_ucAttached == 1; }
    inline CVector                  GetPosition                 ( void )                    { return m_vecPosition; }
    inline CVector                  GetRotationDegrees          ( void )                    { return m_vecRotationDegrees; }
    inline CVector                  GetTurnSpeed                ( void )                    { return m_vecTurnSpeed; }

private:
    ElementID                       m_Vehicle;
    ElementID                       m_AttachedVehicle;
    unsigned char                   m_ucAttached;
    CVector                         m_vecPosition;
    CVector                         m_vecRotationDegrees;
    CVector                         m_vecTurnSpeed;
};

#endif
