/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientObject.h
 *  PURPOSE:     Physical object entity class
 *
 *****************************************************************************/

class CClientObject;

#pragma once

#include <game/CObject.h>
#include "CClientStreamElement.h"
#include "CClientModel.h"

struct SLastSyncedObjectData
{
    CVector vecPosition;
    CVector vecRotation;
    CVector vecVelocity;
    CVector vecTurnVelocity;
    float   fHealth;
    bool    bIsInWater;
    CVector vecCenterOfMass;
};

class CClientObject : public CClientStreamElement
{
    DECLARE_CLASS(CClientObject, CClientStreamElement)
    friend class CClientObjectManager;
    friend class CClientPed;

public:
    CClientObject(class CClientManager* pManager, ElementID ID, unsigned short usModel, bool bLowLod);
    ~CClientObject();

    void Unlink();

    eClientEntityType GetType() const { return CCLIENTOBJECT; }

    CObject*       GetGameObject() const noexcept { return m_pObject; }
    CEntity*       GetGameEntity() noexcept { return m_pObject; }
    const CEntity* GetGameEntity() const noexcept { return m_pObject; }

    void            GetPosition(CVector& vecPosition) const;
    void            SetPosition(const CVector& vecPosition);
    virtual CSphere GetWorldBoundingSphere();

    void         GetRotationDegrees(CVector& vecRotation) const;
    void         GetRotationRadians(CVector& vecRotation) const;
    void         SetRotationDegrees(const CVector& vecRotation);
    virtual void SetRotationRadians(const CVector& vecRotation);

    void AttachTo(CClientEntity* pEntity) override;

    void GetMoveSpeed(CVector& vecMoveSpeed) const;
    void SetMoveSpeed(const CVector& vecMoveSpeed);

    void GetTurnSpeed(CVector& vecTurnSpeed) const;
    void SetTurnSpeed(const CVector& vecTurnSpeed);

    void         GetOrientation(CVector& vecPosition, CVector& vecRotationRadians);
    virtual void SetOrientation(const CVector& vecPosition, const CVector& vecRotationRadians);

    void ModelRequestCallback(CModelInfo* pModelInfo);

    float GetDistanceFromCentreOfMassToBaseOfModel();

    bool IsVisible() const { return m_bIsVisible; };
    void SetVisible(bool bVisible);

    unsigned short GetModel() const const { return m_usModel; };
    void           SetModel(unsigned short usModel);

    bool           IsLowLod() const { return m_bIsLowLod; }
    bool           SetLowLodObject(CClientObject* pLowLodObject);
    CClientObject* GetLowLodObject();

    void Render();

    bool IsFrozen() const { return m_bIsFrozen; }
    void SetFrozen(bool bFrozen);

    unsigned char GetAlpha() const { return m_ucAlpha; }
    void          SetAlpha(unsigned char ucAlpha);
    void          GetScale(CVector& vecScale) const;
    void          SetScale(const CVector& vecScale);

    bool IsCollisionEnabled() { return m_bUsesCollision; };
    void SetCollisionEnabled(bool bCollisionEnabled);

    float GetHealth();
    void  SetHealth(float fHealth);
    float GetTurnMass();
    void  SetTurnMass(float fTurnMass);
    float GetAirResistance();
    void  SetAirResistance(float fAirResistance);
    float GetElasticity();
    void  SetElasticity(float fElasticity);
    float GetBuoyancyConstant();
    void  SetBuoyancyConstant(float fBuoyancyConstant);
    void  GetCenterOfMass(CVector& vecCenterOfMass) const;
    void  SetCenterOfMass(const CVector& vecCenterOfMass);

    bool IsBreakable(bool bCheckModelList = true);
    bool SetBreakable(bool bBreakable);
    bool Break();
    bool IsRespawnEnabled() const { return m_bRespawnEnabled; };
    void SetRespawnEnabled(bool bRespawnEnabled) { m_bRespawnEnabled = bRespawnEnabled; };

    float GetMass();
    void  SetMass(float fMass);

    bool IsVisibleInAllDimensions() const { return m_bVisibleInAllDimensions; };
    void SetVisibleInAllDimensions(bool bVisible, unsigned short usNewDimension = 0);

    void ReCreate();
    void UpdateVisibility();

    bool IsBeingRespawned() const { return m_bBeingRespawned; };
    void SetBeingRespawned(bool bBeingRespawned) { m_bBeingRespawned = bBeingRespawned; };

    bool IsInWater();

    bool IsStatic() const { return m_bIsStatic; }
    void SetStatic(bool isStatic) { m_bIsStatic = isStatic; }

    ElementID GetAttackerID() const { return m_AttackerID; }
    void SetAttackerID(ElementID attackerID) { m_AttackerID = attackerID; }
protected:
    void StreamIn(bool bInstantly);
    void StreamOut();

    void Create();
    void Destroy();

    void NotifyCreate();
    void NotifyDestroy();

    void StreamedInPulse();

    class CClientObjectManager*       m_pObjectManager;
    class CClientModelRequestManager* m_pModelRequester;

    unsigned short m_usModel;

    CVector       m_vecPosition;
    CVector       m_vecRotation;
    bool          m_bIsVisible;
    bool          m_bIsFrozen;
    bool          m_bUsesCollision;
    unsigned char m_ucAlpha;
    CVector       m_vecScale;
    float         m_fHealth;
    bool          m_bBreakingDisabled;
    bool          m_bBeingRespawned;
    bool          m_bRespawnEnabled;
    float         m_fMass;
    float         m_fTurnMass;
    float         m_fAirResistance;
    float         m_fElasticity;
    float         m_fBuoyancyConstant;
    CVector       m_vecCenterOfMass;
    bool          m_bVisibleInAllDimensions = false;
    bool          m_bIsStatic;            // true if the object never moved using setElementVelocity/setElementAngularVelocity

    CVector m_vecMoveSpeed;
    CVector m_vecTurnSpeed;

    ElementID m_AttackerID = INVALID_ELEMENT_ID;

    const bool                    m_bIsLowLod;                    // true if this object is low LOD
    CClientObject*                m_pLowLodObject;                // Pointer to low LOD version of this object
    std::vector<CClientObject*>   m_HighLodObjectList;            // List of objects that use this object as a low LOD version
    bool                          m_IsHiddenLowLod;               // true if this object is low LOD and should not be drawn
    std::shared_ptr<CClientModel> m_clientModel;

public:
    CObject*              m_pObject;
    SLastSyncedObjectData m_LastSyncedData;
};
