/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CCustomWeaponBulletSyncPacket.h
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "CCustomWeapon.h"

class CCustomWeaponBulletSyncPacket : public CPacket
{
public:
    inline                                  CCustomWeaponBulletSyncPacket           ( void )                        {};
                                            CCustomWeaponBulletSyncPacket           ( class CPlayer* pPlayer );

    inline ePacketID                        GetPacketID                 ( void ) const                  { return PACKET_ID_WEAPON_BULLETSYNC; };
    unsigned long                           GetFlags                    ( void ) const                  { return PACKET_MEDIUM_PRIORITY | PACKET_RELIABLE; };

    bool                                    Read                        ( NetBitStreamInterface& BitStream );
    bool                                    Write                       ( NetBitStreamInterface& BitStream ) const;

    CPlayer *                               GetWeaponOwner              ( void )                        { return m_pWeapon != NULL ? m_pWeapon->GetOwner ( ) : NULL; };
    CCustomWeapon *                         GetWeapon                   ( void )                        { return m_pWeapon; };
    CVector                                 GetStart                    ( void )                        { return m_vecStart; };
    CVector                                 GetEnd                      ( void )                        { return m_vecEnd; };

    CCustomWeapon *         m_pWeapon;
    CVector                 m_vecStart;
    CVector                 m_vecEnd;
    uchar                   m_ucOrderCounter;
};

