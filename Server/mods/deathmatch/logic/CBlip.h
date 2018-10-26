/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CBlip.h
 *  PURPOSE:     Blip entity class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CBlip;

#pragma once

#include "CPerPlayerEntity.h"

class CBlip : public CPerPlayerEntity
{
public:
    CBlip(CElement* pParent, class CBlipManager* pBlipManager);
    ~CBlip(void);
    CElement* Clone(bool* bAddEntity, CResource* pResource) override;

    void Unlink(void);

    const CVector& GetPosition(void);
    void           SetPosition(const CVector& vecPosition);

    SColor GetColor(void) const { return m_Color; }
    void   SetColor(const SColor color) { m_Color = color; }

    void AttachTo(CElement* pElement);

protected:
    bool ReadSpecialData(const int iLine) override;

private:
    class CBlipManager* m_pBlipManager;
    CVector             m_vecPosition;

public:
    unsigned char  m_ucSize;
    unsigned char  m_ucIcon;
    SColor         m_Color;
    short          m_sOrdering;
    unsigned short m_usVisibleDistance;
};
