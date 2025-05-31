/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CBuilding.h
 *  PURPOSE:     Object entity class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include "CElement.h"
#include "CEvents.h"

#include "CEasingCurve.h"
#include "TInterpolation.h"
#include "CPositionRotationAnimation.h"

class CBuildingManager;

#define SERVERSIDE_BUILDING_MIN_CLIENT_VERSION "1.6.0-9.22824"

class CBuilding final : public CElement
{
    friend class CPlayer;

public:
    explicit CBuilding(CElement* pParent, class CBuildingManager* pObjectManager);
    explicit CBuilding(const CBuilding& Copy);
    ~CBuilding();
    CElement* Clone(bool* bAddEntity, CResource* pResource) override;

    bool IsEntity() override { return true; }

    void Unlink() override;

    const CVector& GetPosition() override;
    void           SetPosition(const CVector& vecPosition) override;

    void GetRotation(CVector& vecRotation) override;
    void SetRotation(const CVector& vecRotation);

    void GetMatrix(CMatrix& matrix) override;
    void SetMatrix(const CMatrix& matrix) override;

    std::uint16_t GetModel() const noexcept { return m_model; }
    void          SetModel(std::uint16_t model) noexcept { m_model = model; }

    bool GetCollisionEnabled() const noexcept { return m_bCollisionsEnabled; }
    void SetCollisionEnabled(bool bCollisionEnabled) noexcept { m_bCollisionsEnabled = bCollisionEnabled; }

    bool       SetLowLodBuilding(CBuilding* pLowLodBuilding) noexcept;
    CBuilding* GetLowLodElement() const noexcept { return m_pLowLodBuilding; }

    CBuilding* GetHighLodBuilding() const { return m_pHighLodBuilding; }
    void       SetHighLodObject(CBuilding* pHighLodObject) { m_pHighLodBuilding = pHighLodObject; }

protected:
    bool ReadSpecialData(const int iLine) override;

private:
    CBuildingManager* m_pBuildingManager;
    CVector           m_vecRotation;
    std::uint16_t     m_model;

protected:
    bool m_bCollisionsEnabled;

    CBuilding* m_pLowLodBuilding;
    CBuilding* m_pHighLodBuilding;
};
