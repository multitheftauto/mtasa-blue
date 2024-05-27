/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerClothesPacket.cpp
 *  PURPOSE:     Player clothes packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPlayerClothesPacket.h"
#include "CElement.h"

CPlayerClothesPacket::~CPlayerClothesPacket() noexcept
{
    for (const auto& pClothes : m_List)
    {
        delete[] pClothes->szTexture;
        delete[] pClothes->szModel;
        delete pClothes;
    }
    m_List.clear();
}

bool CPlayerClothesPacket::Write(NetBitStreamInterface& BitStream) const noexcept
{
    // Write the source player.
    if (!m_pSourceElement)
        return false;

    BitStream.Write(m_pSourceElement->GetID());

    // Write the clothes
    auto usNumClothes = static_cast<std::uint16_t>(m_List.size());
    BitStream.Write(usNumClothes);

    for (const auto& pClothes : m_List)
    {
        char*         szTexture = pClothes->szTexture;
        char*         szModel = pClothes->szModel;
        auto ucTextureLength = static_cast<std::uint8_t>(strlen(szTexture));
        auto ucModelLength = static_cast<std::uint8_t>(strlen(szModel));
        BitStream.Write(ucTextureLength);
        BitStream.Write(szTexture, ucTextureLength);
        BitStream.Write(ucModelLength);
        BitStream.Write(szModel, ucModelLength);
        BitStream.Write(pClothes->ucType);
    }

    return true;
}

void CPlayerClothesPacket::Add(const char* szTexture, const char* szModel, std::uint8_t ucType) noexcept
{
    SPlayerClothes* Clothes = new SPlayerClothes;
    Clothes->szTexture = new char[strlen(szTexture) + 1];
    strcpy(Clothes->szTexture, szTexture);
    Clothes->szModel = new char[strlen(szModel) + 1];
    strcpy(Clothes->szModel, szModel);
    Clothes->ucType = ucType;
    m_List.push_back(Clothes);
}

void CPlayerClothesPacket::Add(CPlayerClothes* pClothes) noexcept
{
    for (auto ucType = 0; ucType < PLAYER_CLOTHING_SLOTS; ucType++)
    {
        const SPlayerClothing* pClothing = pClothes->GetClothing(ucType);
        if (!pClothing)
            continue;

        Add(pClothing->szTexture, pClothing->szModel, ucType);
    }
}
