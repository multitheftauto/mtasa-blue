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
    bool bHasOrigin;
    if ( !BitStream.ReadBit ( bHasOrigin ) )
        return false;

    m_OriginID = INVALID_ELEMENT_ID;
    if ( bHasOrigin && !BitStream.ReadCompressed ( m_OriginID ) )
        return false;

    SPositionSync origin ( false );
    if ( !BitStream.Read ( &origin ) )
        return false;
    m_vecOrigin = origin.data.vecPosition;

    SWeaponTypeSync weaponType;
    if ( !BitStream.Read ( &weaponType ) )
        return false;
    m_ucWeaponType = weaponType.data.ucWeaponType;

    switch ( m_ucWeaponType )
    {
        case 16: // WEAPONTYPE_GRENADE
        case 17: // WEAPONTYPE_TEARGAS            
        case 18: // WEAPONTYPE_MOLOTOV
        case 39: // WEAPONTYPE_REMOTE_SATCHEL_CHARGE
        {
            SFloatSync < 7, 17 > projectileForce;
            if ( !BitStream.Read ( &projectileForce ) )
                return false;
            m_fForce = projectileForce.data.fValue;

            SVelocitySync velocity;
            if ( !BitStream.Read ( &velocity ) )
                return false;
            m_vecMoveSpeed = velocity.data.vecVelocity;

            break;
        }
        case 19: // WEAPONTYPE_ROCKET
        case 20: // WEAPONTYPE_ROCKET_HS
        {
            bool bHasTarget;
            if ( !BitStream.ReadBit ( bHasTarget ) )
                return false;

            m_TargetID = INVALID_ELEMENT_ID;
            if ( bHasTarget && !BitStream.ReadCompressed ( m_TargetID ) )
                return false;

            SVelocitySync velocity;
            if ( !BitStream.Read ( &velocity ) )
                return false;
            m_vecMoveSpeed = velocity.data.vecVelocity;

            SRotationRadiansSync rotation ( true );
            if ( !BitStream.Read ( &rotation ) )
                return false;
            m_vecRotation = rotation.data.vecRotation;

            break;
        }
        case 58: // WEAPONTYPE_FLARE
        case 21: // WEAPONTYPE_FREEFALL_BOMB
            break;

        default:
            return false;
    }
    return true;
}


bool CProjectileSyncPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    // Write the source player and latency if any. Otherwize 0
    if ( m_pSourceElement )
    {
        BitStream.WriteBit ( true );
        BitStream.WriteCompressed ( m_pSourceElement->GetID () );

        unsigned short usLatency = static_cast < CPlayer * > ( m_pSourceElement )->GetPing ();
        BitStream.WriteCompressed ( usLatency );
    }
    else
        BitStream.WriteBit ( false );


    if ( m_OriginID != INVALID_ELEMENT_ID )
    {
        BitStream.WriteBit ( true );
        BitStream.WriteCompressed ( m_OriginID );
    }
    else
        BitStream.WriteBit ( false );

    SPositionSync position ( false );
    position.data.vecPosition = m_vecOrigin;
    BitStream.Write ( &position );

    SWeaponTypeSync weaponType;
    weaponType.data.ucWeaponType = m_ucWeaponType;
    BitStream.Write ( &weaponType );

    switch ( m_ucWeaponType )
    {
        case 16: // WEAPONTYPE_GRENADE
        case 17: // WEAPONTYPE_TEARGAS
        case 18: // WEAPONTYPE_MOLOTOV
        case 39: // WEAPONTYPE_REMOTE_SATCHEL_CHARGE
        {
            SFloatSync < 7, 17 > projectileForce;
            projectileForce.data.fValue = m_fForce;
            BitStream.Write ( &projectileForce );

            SVelocitySync velocity;
            velocity.data.vecVelocity = m_vecMoveSpeed;
            BitStream.Write ( &velocity );

            break;
        }
        case 19: // WEAPONTYPE_ROCKET
        case 20: // WEAPONTYPE_ROCKET_HS
        {
            if ( m_TargetID != INVALID_ELEMENT_ID )
            {
                BitStream.WriteBit ( true );
                BitStream.WriteCompressed ( m_TargetID );
            }
            else
                BitStream.WriteBit ( false );

            SVelocitySync velocity;
            velocity.data.vecVelocity = m_vecMoveSpeed;
            BitStream.Write ( &velocity );

            SRotationRadiansSync rotation ( true );
            rotation.data.vecRotation = m_vecRotation;
            BitStream.Write ( &rotation );

            break;
        }
        case 58: // WEAPONTYPE_FLARE
            break;
    }

    return true;
}
