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
    unsigned char ucType;
};

class CPlayerClothesPacket final : public CPacket
{
public:
    ~CPlayerClothesPacket() noexcept;

    ePacketID     GetPacketID() const noexcept { return PACKET_ID_PLAYER_CLOTHES; }
    std::uint32_t GetFlags() const noexcept { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; }

    bool Write(NetBitStreamInterface& BitStream) const noexcept;

    void        Add(const char* szTexture, const char* szModel, std::uint8_t ucType) noexcept;
    void        Add(CPlayerClothes* pClothes) noexcept;
    std::size_t Count() const noexcept { return m_List.size(); }

private:
    std::vector<SPlayerClothes*> m_List;
};
