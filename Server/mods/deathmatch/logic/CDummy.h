/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CDummy.h
 *  PURPOSE:     Dummy entity class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CElement.h"

class CDummy final : public CElement
{
public:
    CDummy(class CGroups* pGroups, CElement* pParent);
    ~CDummy();
    CElement* Clone(bool* bAddEntity, CResource* pResource) override;

    bool IsEntity() { return true; }

    void Unlink();

    const CVector& GetPosition() { return m_vecPosition; };
    void           GetPosition(CVector& vecPosition) const { vecPosition = m_vecPosition; };
    const CVector& GetRotation() { return m_vecRotation; };
    void           GetRotation(CVector& vecRotation) const { vecRotation = m_vecRotation; };
    void           SetPosition(const CVector& vecPosition) { m_vecPosition = vecPosition; };
    void           SetRotation(const CVector& vecRotation) { m_vecRotation = vecRotation; };

protected:
    bool ReadSpecialData(const int iLine) override;

private:
    class CGroups* m_pGroups;
    CVector        m_vecPosition;
    CVector        m_vecRotation;
};
