/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPedWastedPacket.cpp
*  PURPOSE:     Ped wasted state packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Cazomino05 <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CPedWastedPacket::CPedWastedPacket ( void )
{
    m_PedID = INVALID_ELEMENT_ID;
    m_Killer = INVALID_ELEMENT_ID;
    m_ucKillerWeapon = 0xFF;
    m_usAmmo = 0;
    m_bStealth = false;
}


CPedWastedPacket::CPedWastedPacket ( CPed * pPed, CElement * pKiller, unsigned char ucKillerWeapon, unsigned char ucBodyPart, bool bStealth, AssocGroupId animGroup, AnimationId animID )
{
    m_PedID = pPed->GetID ();
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


bool CPedWastedPacket::Read ( NetBitStreamInterface& BitStream )
{
    SWeaponTypeSync weapon;
    SBodypartSync bodyPart;
    SPositionSync pos ( false );

    if ( BitStream.ReadCompressed ( m_AnimGroup ) &&
         BitStream.ReadCompressed ( m_AnimID ) &&
         BitStream.Read ( m_Killer ) &&
         BitStream.Read ( &weapon ) &&
         BitStream.Read ( &bodyPart ) &&
         BitStream.Read ( &pos ) &&
         BitStream.Read ( m_PedID ) )
    {
        m_ucKillerWeapon = weapon.data.ucWeaponType;
        m_ucBodyPart = bodyPart.data.uiBodypart;
        m_vecPosition = pos.data.vecPosition;

        SWeaponAmmoSync ammo ( m_ucKillerWeapon, true, false );
        if ( BitStream.Read ( &ammo ) )
        {
            m_usAmmo = ammo.data.usTotalAmmo;
            return true;
        }
    }

    return false;
}


bool CPedWastedPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    SWeaponTypeSync weapon;
    SBodypartSync bodyPart;

    BitStream.Write ( m_PedID );
    BitStream.Write ( m_Killer );

    weapon.data.ucWeaponType = m_ucKillerWeapon;
    BitStream.Write ( &weapon );

    bodyPart.data.uiBodypart = m_ucBodyPart;
    BitStream.Write ( &bodyPart );

    BitStream.WriteBit ( m_bStealth );    
    BitStream.Write ( m_ucTimeContext );
    BitStream.WriteCompressed ( m_AnimGroup );
    BitStream.WriteCompressed ( m_AnimID );

    return true;
}
