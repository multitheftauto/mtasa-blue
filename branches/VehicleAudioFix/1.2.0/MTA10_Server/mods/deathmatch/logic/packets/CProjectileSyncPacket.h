/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CProjectileSyncPacket.h
*  PURPOSE:     Projectile synchronization packet class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CProjectileSYNCPACKET_H
#define __CProjectileSYNCPACKET_H

#include "CPacket.h"
#include <CVector.h>

class CProjectileSyncPacket : public CPacket
{
public:
                            CProjectileSyncPacket       ( void );
                            
    inline ePacketID                GetPacketID         ( void ) const                  { return PACKET_ID_PROJECTILE; };
    inline unsigned long            GetFlags            ( void ) const                  { return PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool                    Read                        ( NetBitStreamInterface& BitStream );
    bool                    Write                       ( NetBitStreamInterface& BitStream ) const;

    unsigned char           m_ucWeaponType;
    ElementID               m_OriginID;
    CVector                 m_vecOrigin;
    float                   m_fForce;
    unsigned char           m_ucHasTarget;
    ElementID               m_TargetID;
    CVector                 m_vecTarget;
    CVector                 m_vecRotation;
    CVector                 m_vecMoveSpeed;
};

#endif
