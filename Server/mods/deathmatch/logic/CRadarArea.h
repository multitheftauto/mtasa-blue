/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CRadarArea.h
 *  PURPOSE:     Radar area entity class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPerPlayerEntity.h"
#include <CVector2D.h>

class CRadarArea : public CPerPlayerEntity
{
    friend class CRadarAreaManager;

public:
    ~CRadarArea();
    CElement* Clone(bool* bAddEntity, CResource* pResource) override;

    void Unlink();

    const CVector2D& GetSize() { return m_vecSize; };
    SColor           GetColor() const { return m_Color; };
    bool             IsFlashing() { return m_bIsFlashing; };

    void SetPosition(const CVector& vecPosition);
    void SetSize(const CVector2D& vecSize);
    void SetColor(const SColor color);
    void SetFlashing(bool bFlashing);

protected:
    bool ReadSpecialData(const int iLine) override;

private:
    CRadarArea(class CRadarAreaManager* pRadarAreaManager, CElement* pParent);

    class CRadarAreaManager* m_pRadarAreaManager;

    CVector2D m_vecSize;
    SColor    m_Color;
    bool      m_bIsFlashing;
};
