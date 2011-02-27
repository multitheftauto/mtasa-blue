/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPlayerWastedPacket.cpp
*  PURPOSE:     Player wasted state packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Cazomino05 <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CPlayerWastedPacket::CPlayerWastedPacket ( void )
{
    m_PlayerID = INVALID_ELEMENT_ID;
    m_Killer = INVALID_ELEMENT_ID;
    m_ucKillerWeapon = 0xFF;
    m_usAmmo = 0;
    m_bStealth = false;
}


CPlayerWastedPacket::CPlayerWastedPacket ( CPed * pPed, CElement * pKiller, unsigned char ucKillerWeapon, unsigned char ucBodyPart, bool bStealth, AssocGroupId animGroup, AnimationId animID )
{
    m_PlayerID = pPed->GetID ();
    m_Killer = ( pKiller ) ? pKiller->GetID () : INVALID_ELEMENT_ID;
    m_ucKillerWeapon = ucKillerWeapon;
    m_ucBodyPart = ucBodyPart;
    m_vecPosition = pPed->GetPosition ();
    m_usAmmo = 0;
    m_bStealth = bStealth;
    m_AnimGroup = animGroup;
    m_AnimID = animID;
    // Generate a new sync-time context for the dead ped
    m_ucTimeContext = pPed->GenerateSyncTimeContext ();
}


bool CPlayerWastedPacket::Read ( NetBitStreamInterface& BitStream )
{
    SWeaponTypeSync weapon;
    SBodypartSync bodyPart;
    SPositionSync pos ( false );

    if ( BitStream.ReadCompressed ( m_AnimGroup ) &&
         BitStream.ReadCompressed ( m_AnimID ) &&
         BitStream.ReadCompressed ( m_Killer ) &&
         BitStream.Read ( &weapon ) &&
         BitStream.Read ( &bodyPart ) &&
         BitStream.Read ( &pos ) )
    {
        SWeaponAmmoSync ammo ( weapon.data.ucWeaponType, true, false );
        if ( BitStream.Read ( &ammo ) )
        {
            m_vecPosition = pos.data.vecPosition;
            m_ucBodyPart = bodyPart.data.uiBodypart;
            m_ucKillerWeapon = weapon.data.ucWeaponType;
            m_usAmmo = ammo.data.usTotalAmmo;
            return true;
        }
    }
    return false;
}


bool CPlayerWastedPacket::Write ( NetBitStreamInterface& BitStream ) const
{    
    SWeaponTypeSync weapon;
    SBodypartSync bodyPart;

    weapon.data.ucWeaponType = m_ucKillerWeapon;
    bodyPart.data.uiBodypart = m_ucBodyPart;

    BitStream.WriteCompressed ( m_PlayerID );
    BitStream.WriteCompressed ( m_Killer );
    BitStream.Write ( &weapon );
    BitStream.Write ( &bodyPart );
    BitStream.WriteBit ( m_bStealth );    
    BitStream.Write ( m_ucTimeContext );
    BitStream.WriteCompressed ( m_AnimGroup );
    BitStream.WriteCompressed ( m_AnimID );

    return true;
}
