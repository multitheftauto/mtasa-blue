/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientColShape.h
 *  PURPOSE:     Shaped collision entity base class
 *
 *****************************************************************************/

#pragma once

#include "CClientEntity.h"

class CClientColCallback;
class CClientMarker;
class CClientPickup;

enum eColShapeType
{
    COLSHAPE_CIRCLE,
    COLSHAPE_CUBOID,
    COLSHAPE_SPHERE,
    COLSHAPE_RECTANGLE,
    COLSHAPE_POLYGON,
    COLSHAPE_TUBE,
};

class CClientColShape : public CClientEntity
{
    DECLARE_CLASS(CClientColShape, CClientEntity)
    friend class CClientMarker;
    friend class CClientPickup;

public:
    CClientColShape(class CClientManager* pManager, ElementID ID);
    ~CClientColShape();

    void Unlink();

    virtual eColShapeType GetShapeType() = 0;

    void DoPulse();
    bool IsAttachable();

    eClientEntityType GetType() const { return CCLIENTCOLSHAPE; }

    virtual void GetPosition(CVector& vecPosition) const { vecPosition = m_vecPosition; };
    virtual void SetPosition(const CVector& vecPosition);

    void AttachTo(CClientEntity* pEntity) override;

    virtual bool DoHitDetection(const CVector& vecNowPosition, float fRadius) = 0;

    bool IsEnabled() { return m_bIsEnabled; };
    void SetEnabled(bool bEnabled) { m_bIsEnabled = bEnabled; };

    void                CallHitCallback(CClientEntity& Entity);
    void                CallLeaveCallback(CClientEntity& Entity);
    CClientColCallback* SetHitCallback(CClientColCallback* pCallback) { return (m_pCallback = pCallback); };

    bool GetAutoCallEvent() { return m_bAutoCallEvent; };
    void SetAutoCallEvent(bool bAutoCallEvent) { m_bAutoCallEvent = bAutoCallEvent; };

    void                                AddCollider(CClientEntity* pEntity) { m_Colliders.push_back(pEntity); }
    void                                RemoveCollider(CClientEntity* pEntity) { m_Colliders.remove(pEntity); }
    bool                                ColliderExists(CClientEntity* pEntity);
    void                                RemoveAllColliders();
    CFastList<CClientEntity*>::iterator CollidersBegin() { return m_Colliders.begin(); }
    CFastList<CClientEntity*>::iterator CollidersEnd() { return m_Colliders.end(); }

    void SizeChanged();

protected:
    CVector          m_vecPosition;
    CClientMarkerPtr m_pOwningMarker;
    CClientPickupPtr m_pOwningPickup;

private:
    bool                      m_bIsEnabled;
    class CClientColManager*  m_pColManager;
    CClientColCallback*       m_pCallback;
    bool                      m_bAutoCallEvent;
    CFastList<CClientEntity*> m_Colliders;
};
