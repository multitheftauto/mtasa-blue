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
#include "CBuildingManager.h"

#include "CEasingCurve.h"
#include "TInterpolation.h"
#include "CPositionRotationAnimation.h"

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

    void GetMatrix(CMatrix& matrix);
    void SetMatrix(const CMatrix& matrix);

    unsigned char GetAlpha() { return m_ucAlpha; }
    void          SetAlpha(unsigned char ucAlpha) { m_ucAlpha = ucAlpha; }

    unsigned short GetModel() { return m_usModel; }
    void           SetModel(unsigned short usModel) { m_usModel = usModel; }

    bool GetCollisionEnabled() { return m_bCollisionsEnabled; }
    void SetCollisionEnabled(bool bCollisionEnabled) { m_bCollisionsEnabled = bCollisionEnabled; }

    bool     SetLowLodObject(CBuilding* pLowLodObject);
    CBuilding* GetLowLodElement() const { return m_pLowLodObject; };

    CBuilding* GetHighLodObject() const { return m_HighLodObject; };
    void SetHighLodObject(CBuilding* pHighLodObject) { m_HighLodObject = pHighLodObject; };

protected:
    bool ReadSpecialData(const int iLine) override;

private:
    CBuildingManager* m_pObjectManager;
    CVector         m_vecRotation;
    unsigned char   m_ucAlpha;
    unsigned short  m_usModel;

protected:
    bool m_bCollisionsEnabled;

    CBuilding* m_pLowLodObject;
    CBuilding* m_HighLodObject;
};
