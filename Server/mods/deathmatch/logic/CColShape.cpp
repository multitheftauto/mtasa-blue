/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CColShape.cpp
 *  PURPOSE:     Base shaped collision entity class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CColShape.h"
#include "CColManager.h"
#include "CColCallback.h"
#include "CStaticFunctionDefinitions.h"

CColShape::CColShape(CColManager* pManager, CElement* pParent, bool bIsPartnered) : CElement(pParent)
{
    // Init
    m_bIsEnabled = true;
    m_bAutoCallEvent = true;
    m_pCallback = NULL;
    m_iType = COLSHAPE;
    m_bPartnered = bIsPartnered;

    SetTypeName("colshape");

    // Add it to our manager's list
    m_pManager = pManager;
    pManager->AddToList(this);
}

CColShape::~CColShape()
{
    if (m_pCallback)
        m_pCallback->Callback_OnCollisionDestroy(this);

    RemoveAllColliders();
    // Unlink us from our manager
    Unlink();
}

void CColShape::Unlink()
{
    // Remove us from manager's list
    m_pManager->RemoveFromList(this);
}

const CVector& CColShape::GetPosition()
{
    if (m_pAttachedTo)
        GetAttachedPosition(m_vecPosition);
    return m_vecPosition;
}

void CColShape::SetPosition(const CVector& vecPosition)
{
    m_vecPosition = vecPosition;
    UpdateSpatialData();
    CStaticFunctionDefinitions::RefreshColShapeColliders(this);
}

void CColShape::AttachTo(CElement* pElement)
{
    CElement::AttachTo(pElement);
    if (pElement && m_pAttachedTo)
    {
        CVector pVector = pElement->GetPosition();
        SetPosition(pVector + m_vecAttachedPosition);
    }
}

void CColShape::CallHitCallback(CElement& Element)
{
    // Call the callback with us as the shape if it exists
    if (m_pCallback)
    {
        m_pCallback->Callback_OnCollision(*this, Element);
    }
}

void CColShape::CallLeaveCallback(CElement& Element)
{
    // Call the callback with us as the shape if it exists
    if (m_pCallback)
    {
        m_pCallback->Callback_OnLeave(*this, Element);
    }
}

bool CColShape::ColliderExists(CElement* pElement)
{
    std::list<CElement*>::iterator iter = m_Colliders.begin();
    for (; iter != m_Colliders.end(); iter++)
    {
        if (*iter == pElement)
        {
            return true;
        }
    }
    return false;
}

void CColShape::RemoveAllColliders()
{
    std::list<CElement*>::iterator iter = m_Colliders.begin();
    for (; iter != m_Colliders.end(); iter++)
    {
        (*iter)->RemoveCollision(this);
    }
    m_Colliders.clear();
}

void CColShape::SizeChanged()
{
    UpdateSpatialData();
    // Maybe queue RefreshColliders for v1.1
}
