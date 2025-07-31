/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CColShape.h
 *  PURPOSE:     Base shaped collision entity class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CElement.h"

enum eColShapeType
{
    COLSHAPE_CIRCLE,
    COLSHAPE_CUBOID,
    COLSHAPE_SPHERE,
    COLSHAPE_RECTANGLE,
    COLSHAPE_POLYGON,
    COLSHAPE_TUBE,
};

class CColShape : public CElement
{
public:
    CColShape(class CColManager* pManager, CElement* pParent, bool bIsPartnered = false);
    virtual ~CColShape();

    virtual eColShapeType GetShapeType() = 0;

    void Unlink();

    virtual bool DoHitDetection(const CVector& vecNowPosition) = 0;

    bool IsEnabled() { return m_bIsEnabled; };
    void SetEnabled(bool bEnabled) { m_bIsEnabled = bEnabled; };

    const CVector& GetPosition();
    virtual void   SetPosition(const CVector& vecPosition);

    void AttachTo(CElement* pElement);

    void                CallHitCallback(CElement& Element);
    void                CallLeaveCallback(CElement& Element);
    class CColCallback* SetCallback(class CColCallback* pCallback) { return (m_pCallback = pCallback); };

    bool GetAutoCallEvent() { return m_bAutoCallEvent; };
    void SetAutoCallEvent(bool bAutoCallEvent) { m_bAutoCallEvent = bAutoCallEvent; };

    void AddCollider(CElement* pElement) { m_Colliders.push_back(pElement); }
    void RemoveCollider(CElement* pElement) { m_Colliders.remove(pElement); }
    bool ColliderExists(CElement* pElement);
    void RemoveAllColliders();

    std::list<CElement*>::iterator CollidersBegin() { return m_Colliders.begin(); }
    std::list<CElement*>::iterator CollidersEnd() { return m_Colliders.end(); }

    bool IsPartnered() { return m_bPartnered; }

    void SizeChanged();

protected:
    CVector            m_vecPosition;
    class CColManager* m_pManager;

private:
    bool                m_bIsEnabled;
    class CColCallback* m_pCallback;
    bool                m_bAutoCallEvent;

    std::list<CElement*> m_Colliders;

    bool m_bPartnered;
};
