/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CBulletsyncPacket.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CBulletsyncPacket.h"
#include "net/SyncStructures.h"
#include "CPlayer.h"
#include "CWeaponStatManager.h"

CBulletsyncPacket::CBulletsyncPacket(CPlayer* pPlayer)
{
    m_pSourceElement = pPlayer;
    m_WeaponType = WEAPONTYPE_UNARMED;
    m_ucOrderCounter = 0;
    m_fDamage = 0;
    m_ucHitZone = 0;
    m_DamagedPlayerID = INVALID_ELEMENT_ID;
}

bool CBulletsyncPacket::Read(NetBitStreamInterface& BitStream)
{
    // Got a player?
    if (m_pSourceElement)
    {
        char cWeaponType = 0;
        if (!BitStream.Read(cWeaponType) || !CWeaponStatManager::HasWeaponBulletSync(cWeaponType))
            return false;
        m_WeaponType = (eWeaponType)cWeaponType;

        if (!BitStream.Read((char*)&m_vecStart, sizeof(CVector)) || !BitStream.Read((char*)&m_vecEnd, sizeof(CVector)))
            return false;

        if (!m_vecStart.IsValid() || !m_vecEnd.IsValid())
            return false;

        // Duplicate packet protection
        if (!BitStream.Read(m_ucOrderCounter))
            return false;

        if (BitStream.ReadBit())
        {
            BitStream.Read(m_fDamage);
            BitStream.Read(m_ucHitZone);
            BitStream.Read(m_DamagedPlayerID);
        }
        return true;
    }

    return false;
}

// Note: Relays a previous Read()
bool CBulletsyncPacket::Write(NetBitStreamInterface& BitStream) const
{
    // Got a player to write?
    if (m_pSourceElement)
    {
        CPlayer* pSourcePlayer = static_cast<CPlayer*>(m_pSourceElement);

        // Write the source player id
        ElementID PlayerID = pSourcePlayer->GetID();
        BitStream.Write(PlayerID);

        // Write the bulletsync data
        BitStream.Write((char)m_WeaponType);
        BitStream.Write((const char*)&m_vecStart, sizeof(CVector));
        BitStream.Write((const char*)&m_vecEnd, sizeof(CVector));

        // Duplicate packet protection
        BitStream.Write(m_ucOrderCounter);

        if (m_fDamage > 0 && m_DamagedPlayerID != INVALID_ELEMENT_ID)
        {
            BitStream.WriteBit(true);
            BitStream.Write(m_fDamage);
            BitStream.Write(m_ucHitZone);
            BitStream.Write(m_DamagedPlayerID);
        }
        else
        {
            BitStream.WriteBit(false);
        }

        return true;
    }

    return false;
}
