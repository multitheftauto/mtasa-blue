/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientColShape.cpp
 *  PURPOSE:     Shaped collision entity base class
 *
 *****************************************************************************/

#include "StdInc.h"

using std::list;

CClientColShape::CClientColShape(CClientManager* pManager, ElementID ID) : ClassInit(this), CClientEntity(ID)
{
    CClientEntityRefManager::AddEntityRefs(ENTITY_REF_DEBUG(this, "CClientColShape"), &m_pOwningMarker, &m_pOwningPickup, NULL);

    // Init
    m_pManager = pManager;
    m_bIsEnabled = true;
    m_bAutoCallEvent = true;
    m_pCallback = NULL;
    m_pOwningMarker = NULL;
    m_pOwningPickup = NULL;

    SetTypeName("colshape");

    // Add it to our manager's list
    m_pColManager = pManager->GetColManager();
    m_pColManager->AddToList(this);
}

CClientColShape::~CClientColShape()
{
    if (m_pOwningMarker && m_pOwningMarker->m_pCollision == this)
        m_pOwningMarker->m_pCollision = NULL;
    else if (m_pOwningPickup && m_pOwningPickup->m_pCollision == this)
        m_pOwningPickup->m_pCollision = NULL;

    RemoveAllColliders();
    Unlink();
    CClientEntityRefManager::RemoveEntityRefs(0, &m_pOwningMarker, &m_pOwningPickup, NULL);
}

void CClientColShape::Unlink()
{
    m_pColManager->RemoveFromList(this);
}

void CClientColShape::SizeChanged()
{
    UpdateSpatialData();
    // Maybe queue RefreshColliders for v1.1
}

void CClientColShape::DoPulse()
{
    // Update our position/rotation if we're attached
    DoAttaching();
}

bool CClientColShape::IsAttachable()
{
    return (!m_pOwningPickup && !m_pOwningMarker);
}

void CClientColShape::SetPosition(const CVector& vecPosition)
{
    if (vecPosition != m_vecPosition)
    {
        m_vecPosition = vecPosition;
        UpdateSpatialData();
        CStaticFunctionDefinitions::RefreshColShapeColliders(this);
    }
}

void CClientColShape::AttachTo(CClientEntity* pEntity)
{
    CClientEntity::AttachTo(pEntity);
    if (pEntity && m_pAttachedToEntity)
    {
        CVector pVector;

        pEntity->GetPosition(pVector);
        SetPosition(pVector + m_vecAttachedPosition);
    }
}

void CClientColShape::CallHitCallback(CClientEntity& Entity)
{
    // Call the callback with us as the shape if it exists
    if (m_pCallback)
    {
        m_pCallback->Callback_OnCollision(*this, Entity);
    }
}

void CClientColShape::CallLeaveCallback(CClientEntity& Entity)
{
    // Call the callback with us as the shape if it exists
    if (m_pCallback)
    {
        m_pCallback->Callback_OnLeave(*this, Entity);
    }
}

bool CClientColShape::ColliderExists(CClientEntity* pEntity)
{
    return m_Colliders.contains(pEntity);
}

void CClientColShape::RemoveAllColliders()
{
    CFastList<CClientEntity*>::iterator iter = m_Colliders.begin();
    for (; iter != m_Colliders.end(); iter++)
    {
        (*iter)->RemoveCollision(this);
    }
    m_Colliders.clear();
}
