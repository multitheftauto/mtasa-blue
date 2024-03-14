/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CBuildingRemoval.h
 *  PURPOSE:     Header file for building removal.
 *
 *****************************************************************************/

#pragma once

class CBuildingRemoval
{
public:
    CBuildingRemoval(std::uint16_t usModel, float fRadius, const CVector& vecPos, char cInterior);
    ~CBuildingRemoval();

    float          GetRadius() { return m_fRadius; }
    const CVector& GetPosition() { return m_vecPos; }
    std::uint16_t GetModel() { return m_usModel; }
    char           GetInterior() { return m_cInterior; }

private:
    std::uint16_t m_usModel;
    CVector        m_vecPos;
    float          m_fRadius;
    char           m_cInterior;
};
