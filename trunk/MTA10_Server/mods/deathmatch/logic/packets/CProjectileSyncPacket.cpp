/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CProjectileSyncPacket.cpp
*  PURPOSE:     Projectile synchronization packet class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CProjectileSyncPacket::CProjectileSyncPacket ( void )
{
}


bool CProjectileSyncPacket::Read ( NetBitStreamInterface& BitStream )
{
    if ( BitStream.Read ( m_OriginID ) &&
         BitStream.Read ( m_vecOrigin.fX ) &&
         BitStream.Read ( m_vecOrigin.fY ) &&
         BitStream.Read ( m_vecOrigin.fZ ) &&
         BitStream.Read ( m_ucWeaponType ) )
    {
        switch ( m_ucWeaponType )
        {
            case 16: // WEAPONTYPE_GRENADE
            case 17: // WEAPONTYPE_TEARGAS            
            case 18: // WEAPONTYPE_MOLOTOV
            case 39: // WEAPONTYPE_REMOTE_SATCHEL_CHARGE
            {
                return ( BitStream.Read ( m_fForce ) &&
                         BitStream.Read ( m_vecMoveSpeed.fX ) &&
                         BitStream.Read ( m_vecMoveSpeed.fY ) &&
                         BitStream.Read ( m_vecMoveSpeed.fZ ) );
                break;
            }
            case 19: // WEAPONTYPE_ROCKET
            case 20: // WEAPONTYPE_ROCKET_HS
            {
                return ( BitStream.Read ( m_TargetID ) &&                    
                         BitStream.Read ( m_vecRotation.fX ) &&
                         BitStream.Read ( m_vecRotation.fY ) &&
                         BitStream.Read ( m_vecRotation.fZ ) &&
                         BitStream.Read ( m_vecMoveSpeed.fX ) &&
                         BitStream.Read ( m_vecMoveSpeed.fY ) &&
                         BitStream.Read ( m_vecMoveSpeed.fZ ) );
                break;
            }
            case 58: // WEAPONTYPE_FLARE
            case 21: // WEAPONTYPE_FREEFALL_BOMB
                return true;
                break;
        }
    }
    return false;
}


bool CProjectileSyncPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    // Write the source player and latency if any. Otherwize 0
    if ( m_pSourceElement )
    {
        ElementID ID = m_pSourceElement->GetID ();
        BitStream.Write ( ID );

        unsigned short usLatency = static_cast < CPlayer * > ( m_pSourceElement )->GetPing ();
        BitStream.WriteCompressed ( usLatency );
    }
    else
    {
        BitStream.Write ( static_cast < ElementID > ( INVALID_ELEMENT_ID ) );
        BitStream.Write ( static_cast < unsigned short > ( 0 ) );
    }


    BitStream.Write ( m_OriginID );
    BitStream.Write ( m_vecOrigin.fX );
    BitStream.Write ( m_vecOrigin.fY );
    BitStream.Write ( m_vecOrigin.fZ );
    BitStream.Write ( m_ucWeaponType );

    switch ( m_ucWeaponType )
    {
        case 16: // WEAPONTYPE_GRENADE
        case 17: // WEAPONTYPE_TEARGAS
        case 18: // WEAPONTYPE_MOLOTOV
        case 39: // WEAPONTYPE_REMOTE_SATCHEL_CHARGE
        {
            BitStream.Write ( m_fForce );
            BitStream.Write ( m_vecMoveSpeed.fX );
            BitStream.Write ( m_vecMoveSpeed.fY );
            BitStream.Write ( m_vecMoveSpeed.fZ );
            break;
        }
        case 19: // WEAPONTYPE_ROCKET
        case 20: // WEAPONTYPE_ROCKET_HS
        {            
            BitStream.Write ( m_TargetID );                   
            BitStream.Write ( m_vecRotation.fX );
            BitStream.Write ( m_vecRotation.fY );
            BitStream.Write ( m_vecRotation.fZ );
            BitStream.Write ( m_vecMoveSpeed.fX );
            BitStream.Write ( m_vecMoveSpeed.fY );
            BitStream.Write ( m_vecMoveSpeed.fZ );
            break;
        }
        case 58: // WEAPONTYPE_FLARE
            break;
    }

    return true;
}
