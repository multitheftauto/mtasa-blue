/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/models/CModelAtomic.h
 *  PURPOSE:     Atomic model class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CModelAtomic;

#pragma once

class CModelAtomic : public CModelBase
{
public:
    CModelAtomic(uint32_t uiModelID, char strName[22], char strTextureName[22], uint32_t uiDrawDistance, uint32_t uiFlags);
    ~CModelAtomic();

    eModelInfoType GetType() { return eModelInfoType::ATOMIC; };
    void           Unload();

    uint32_t       GetDrawDistance() { return m_uiDrawDistance; };
    void           SetDrawDistance(uint32_t uiDistance) { m_uiDrawDistance = uiDistance; };
    void           SetFlags(uint32_t uiFlags) { m_uiDrawDistance = uiFlags; };
    uint32_t       GetFlags() { return m_uiFlags; };

private:
    char     m_strTextureName[22];
    uint32_t m_uiDrawDistance;
    uint32_t m_uiFlags;
};
