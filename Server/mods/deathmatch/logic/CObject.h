/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CObject.h
 *  PURPOSE:     Object entity class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

// Kayl: There is now too many includes here, try to make it work with StdInc.h if possible
#include "CElement.h"
#include "CEvents.h"
#include "CObjectManager.h"

#include "CEasingCurve.h"
#include "TInterpolation.h"
#include "CPositionRotationAnimation.h"

class CObject : public CElement
{
    friend class CPlayer;

public:
    explicit CObject(CElement* pParent, class CObjectManager* pObjectManager, bool bIsLowLod);
    explicit CObject(const CObject& Copy);
    ~CObject();
    CElement* Clone(bool* bAddEntity, CResource* pResource) override;

    bool IsEntity() { return true; }

    void Unlink();

    const CVector& GetPosition();
    void           SetPosition(const CVector& vecPosition);

    void GetRotation(CVector& vecRotation);
    void SetRotation(const CVector& vecRotation);

    void GetMatrix(CMatrix& matrix);
    void SetMatrix(const CMatrix& matrix);

    bool                              IsMoving();
    void                              Move(const CPositionRotationAnimation& a_rMoveAnimation);
    void                              StopMoving();
    const CPositionRotationAnimation* GetMoveAnimation();

    unsigned char GetAlpha() { return m_ucAlpha; }
    void          SetAlpha(unsigned char ucAlpha) { m_ucAlpha = ucAlpha; }

    unsigned short GetModel() { return m_usModel; }
    void           SetModel(unsigned short usModel) { m_usModel = usModel; }

    const CVector& GetScale() { return m_vecScale; }
    void           SetScale(const CVector& vecScale) { m_vecScale = vecScale; }

    bool GetCollisionEnabled() { return m_bCollisionsEnabled; }
    void SetCollisionEnabled(bool bCollisionEnabled) { m_bCollisionsEnabled = bCollisionEnabled; }

    bool IsFrozen() { return m_bIsFrozen; }
    void SetFrozen(bool bFrozen) { m_bIsFrozen = bFrozen; }

    float GetHealth() { return m_fHealth; }
    void  SetHealth(float fHealth) { m_fHealth = fHealth; }

    bool IsSyncable() { return m_bSyncable; }
    void SetSyncable(bool bSyncable) { m_bSyncable = bSyncable; }

    CPlayer* GetSyncer() { return m_pSyncer; }
    void     SetSyncer(CPlayer* pPlayer);

    bool     IsLowLod();
    bool     SetLowLodObject(CObject* pLowLodObject);
    CObject* GetLowLodObject();

    bool IsVisibleInAllDimensions() { return m_bVisibleInAllDimensions; };
    void SetVisibleInAllDimensions(bool bVisible) { m_bVisibleInAllDimensions = bVisible; };

    bool IsBreakable() { return m_bBreakable; }
    void SetBreakable(bool bBreakable) { m_bBreakable = bBreakable; }

    bool IsRespawnEnabled() const noexcept { return m_bRespawnable; }
    void SetRespawnEnabled(bool bRespawn) noexcept { m_bRespawnable = bRespawn; }

protected:
    bool ReadSpecialData(const int iLine) override;

private:
    CObjectManager* m_pObjectManager;
    CVector         m_vecRotation;
    unsigned char   m_ucAlpha;
    unsigned short  m_usModel;
    CVector         m_vecScale;
    bool            m_bIsFrozen;
    float           m_fHealth;
    bool            m_bBreakable;
    bool            m_bSyncable;
    CPlayer*        m_pSyncer;
    bool            m_bVisibleInAllDimensions = false;
    bool            m_bRespawnable;

protected:
    bool m_bCollisionsEnabled;

    const bool            m_bIsLowLod;                    // true if this object is low LOD
    CObject*              m_pLowLodObject;                // Pointer to low LOD version of this object
    std::vector<CObject*> m_HighLodObjectList;            // List of objects that use this object as a low LOD version

public:
    CPositionRotationAnimation* m_pMoveAnimation;
};
