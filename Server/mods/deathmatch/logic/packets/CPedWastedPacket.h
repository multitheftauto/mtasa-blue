/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPedWastedPacket.h
 *  PURPOSE:     Ped wasted state packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include <CVector.h>

class CPed;
typedef unsigned long AssocGroupId;
typedef unsigned long AnimationId;

class CPedWastedPacket final : public CPacket
{
public:
    CPedWastedPacket();
    CPedWastedPacket(CPed* pPed, CElement* pKiller, unsigned char ucKillerWeapon, unsigned char ucBodyPart, bool bStealth, AssocGroupId animGroup = 0,
                     AnimationId animID = 15);

    ePacketID     GetPacketID() const { return PACKET_ID_PED_WASTED; };
    unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;

    ElementID      m_PedID;
    ElementID      m_Killer;
    unsigned char  m_ucKillerWeapon;
    unsigned char  m_ucBodyPart;
    CVector        m_vecPosition;
    unsigned short m_usAmmo;
    bool           m_bStealth;
    unsigned char  m_ucTimeContext;
    unsigned long  m_AnimGroup, m_AnimID;
};
