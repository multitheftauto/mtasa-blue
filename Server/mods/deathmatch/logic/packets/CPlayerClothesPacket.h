/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerClothesPacket.h
 *  PURPOSE:     Player clothes packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include "../CPlayerClothes.h"

struct SPlayerClothes
{
    char*         szTexture;
    char*         szModel;
    std::uint8_t ucType;
};

class CPlayerClothesPacket final : public CPacket
{
public:
    ~CPlayerClothesPacket();

    ePacketID     GetPacketID() const { return PACKET_ID_PLAYER_CLOTHES; };
    std::uint32_t GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Write(NetBitStreamInterface& BitStream) const;

    void         Add(const char* szTexture, const char* szModel, std::uint8_t ucType);
    void         Add(CPlayerClothes* pClothes);
    std::uint32_t Count() { return static_cast<std::uint32_t>(m_List.size()); }

private:
    std::vector<SPlayerClothes*> m_List;
};
