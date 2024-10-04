/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientMarker.cpp
 *  PURPOSE:     Marker entity class
 *
 *****************************************************************************/

#include "StdInc.h"

extern CClientGame* g_pClientGame;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

unsigned int CClientMarker::m_uiStreamedInMarkers = 0;

CClientMarker::CClientMarker(CClientManager* pManager, ElementID ID, int iMarkerType) : ClassInit(this), CClientStreamElement(pManager->GetMarkerStreamer(), ID)
{
    // Init
    m_pManager = pManager;
    m_pMarkerManager = pManager->GetMarkerManager();
    m_pCollision = NULL;
    m_pMarker = NULL;

    // Typename
    SetTypeName("marker");

    // Create the marker
    CreateOfType(iMarkerType);

    // Add us to marker manager list
    m_pMarkerManager->AddToList(this);
    UpdateSpatialData();
}

CClientMarker::~CClientMarker()
{
    AttachTo(nullptr);

    // Unlink
    Unlink();

    // Make sure nothing is still referencing us
    m_pManager->UnreferenceEntity(this);

    SAFE_DELETE(m_pCollision)

    // Stream out first so the element counter is correct
    StreamOut();

    if (m_pMarker)
    {
        // Destroy the marker class
        delete m_pMarker;
        m_pMarker = NULL;
    }
}

void CClientMarker::Unlink()
{
    // Remove us from marker manager list
    m_pMarkerManager->RemoveFromList(this);
}

void CClientMarker::GetPosition(CVector& vecPosition) const
{
    if (m_pMarker)
    {
        m_pMarker->GetPosition(vecPosition);
    }
    else
    {
        vecPosition = m_vecPosition;
    }
}

void CClientMarker::SetPosition(const CVector& vecPosition)
{
    if (m_vecPosition != vecPosition)
    {
        m_vecPosition = vecPosition;
        if (m_pMarker)
            m_pMarker->SetPosition(vecPosition);
        if (m_pCollision)
            m_pCollision->SetPosition(vecPosition);

        // Update our streaming position
        UpdateStreamPosition(vecPosition);
    }
}

void CClientMarker::AttachTo(CClientEntity* pEntity)
{
    CClientEntity::AttachTo(pEntity);
    if (m_pAttachedToEntity)
    {
        DoAttaching();
        UpdateStreamPosition(m_vecPosition);
    }
}

void CClientMarker::SetAttachedOffsets(CVector& vecPosition, CVector& vecRotation)
{
    CClientEntity::SetAttachedOffsets(vecPosition, vecRotation);
    if (m_pCollision)
    {
        m_pCollision->SetAttachedOffsets(vecPosition, vecRotation);
    }
}

bool CClientMarker::SetMatrix(const CMatrix& matrix)
{
    if (m_pMarker)
        m_pMarker->SetMatrix(const_cast<CMatrix&>(matrix));
    if (m_pCollision)
        m_pCollision->SetPosition(matrix.vPos);

    // Update our streaming position
    UpdateStreamPosition(matrix.vPos);

    return true;
}

void CClientMarker::DoPulse()
{
    // Update our position/rotation if we're attached
    DoAttaching();

    // Pulse the element we contain
    if (m_pMarker)
        m_pMarker->DoPulse();
}

CClientMarker::eMarkerType CClientMarker::GetMarkerType() const
{
    // Grab the marker class type
    unsigned int uiMarkerType = m_pMarker->GetMarkerType();
    switch (uiMarkerType)
    {
        // If it's a checkpoint, it can be either a checkpoint or a ring
        case CClientMarkerCommon::CLASS_CHECKPOINT:
        {
            unsigned long ulCheckpointType = static_cast<CClientCheckpoint*>(m_pMarker)->GetCheckpointType();
            if (ulCheckpointType == CClientCheckpoint::TYPE_NORMAL)
                return MARKER_CHECKPOINT;
            else
                return MARKER_RING;
        }

        // If it's a 3d marker it can either be a cylinder or an arrow
        case CClientMarkerCommon::CLASS_3DMARKER:
        {
            unsigned long ul3DType = static_cast<CClient3DMarker*>(m_pMarker)->Get3DMarkerType();
            if (ul3DType == CClient3DMarker::TYPE_CYLINDER)
                return MARKER_CYLINDER;
            else
                return MARKER_ARROW;
        }

        // Corona
        case CClientMarkerCommon::CLASS_CORONA:
            return MARKER_CORONA;
    }

    return MARKER_INVALID;
}

void CClientMarker::SetMarkerType(CClientMarker::eMarkerType eType)
{
    // Different from current type?
    eMarkerType eCurrentType = GetMarkerType();
    if (eCurrentType != eType)
    {
        // Current type is a checkpoint and new type is a ring?
        if (eCurrentType == MARKER_CHECKPOINT && eType == MARKER_RING)
        {
            // Just change the type
            static_cast<CClientCheckpoint*>(m_pMarker)->SetCheckpointType(CClientCheckpoint::TYPE_RING);
            return;
        }

        // Or current type is a ring and new type is a checkpoint?
        if (eCurrentType == MARKER_RING && eType == MARKER_CHECKPOINT)
        {
            // Just change the type
            static_cast<CClientCheckpoint*>(m_pMarker)->SetCheckpointType(CClientCheckpoint::TYPE_NORMAL);
            return;
        }

        // Current type is a cylinder and new type is an arrow
        if (eCurrentType == MARKER_CYLINDER && eType == MARKER_ARROW)
        {
            // Just change the type
            static_cast<CClient3DMarker*>(m_pMarker)->Set3DMarkerType(CClient3DMarker::TYPE_ARROW);
            return;
        }

        // Current type is an arrow and new type is an cylinder
        if (eCurrentType == MARKER_ARROW && eType == MARKER_CYLINDER)
        {
            // Just change the type
            static_cast<CClient3DMarker*>(m_pMarker)->Set3DMarkerType(CClient3DMarker::TYPE_CYLINDER);
            return;
        }

        // No easy way of changing the type. Different classes. Remember position and color and recreate it.
        CVector vecPosition;
        m_pMarker->GetPosition(vecPosition);
        bool   bVisible = m_pMarker->IsVisible();
        float  fSize = m_pMarker->GetSize();
        SColor color = m_pMarker->GetColor();
        bool   bStreamedIn = IsStreamedIn();

        // Destroy the old.
        delete m_pMarker;
        m_pMarker = NULL;

        // Create a new one of the correct type
        CreateOfType(eType);

        // Reset stored position to re-apply last position (fixes issue #2194)
        m_vecPosition = {};

        // Set the properties back
        SetPosition(vecPosition);
        SetSize(fSize);
        SetColor(color);
        SetVisible(bVisible);

        // Stream it in if it was streamed in
        if (bStreamedIn)
        {
            m_pMarker->StreamIn();
        }
    }
}

CClient3DMarker* CClientMarker::Get3DMarker()
{
    if (m_pMarker->GetMarkerType() == CClientMarkerCommon::CLASS_3DMARKER)
        return static_cast<CClient3DMarker*>(m_pMarker);

    return NULL;
}

CClientCheckpoint* CClientMarker::GetCheckpoint()
{
    if (m_pMarker->GetMarkerType() == CClientMarkerCommon::CLASS_CHECKPOINT)
        return static_cast<CClientCheckpoint*>(m_pMarker);

    return NULL;
}

CClientCorona* CClientMarker::GetCorona()
{
    if (m_pMarker->GetMarkerType() == CClientMarkerCommon::CLASS_CORONA)
        return static_cast<CClientCorona*>(m_pMarker);

    return NULL;
}

bool CClientMarker::IsHit(const CVector& vecPosition) const
{
    return m_pMarker->IsHit(vecPosition);
}

bool CClientMarker::IsHit(CClientEntity* pEntity) const
{
    return m_pMarker->IsHit(pEntity);
}

bool CClientMarker::IsVisible() const
{
    return m_pMarker->IsVisible();
}

void CClientMarker::SetVisible(bool bVisible)
{
    m_pMarker->SetVisible(bVisible);
}

SColor CClientMarker::GetColor() const
{
    return m_pMarker->GetColor();
}

void CClientMarker::SetColor(const SColor color)
{
    m_pMarker->SetColor(color);
}

float CClientMarker::GetSize() const
{
    return m_pMarker->GetSize();
}

void CClientMarker::SetSize(float fSize)
{
    switch (m_pCollision->GetShapeType())
    {
        case COLSHAPE_CIRCLE:
        {
            CClientColCircle* pShape = static_cast<CClientColCircle*>(m_pCollision);
            pShape->SetRadius(fSize);
            break;
        }
        case COLSHAPE_SPHERE:
        {
            CClientColSphere* pShape = static_cast<CClientColSphere*>(m_pCollision);
            pShape->SetRadius(fSize);
            break;
        }
    }
    m_pMarker->SetSize(fSize);
}

int CClientMarker::StringToType(const char* szString)
{
    if (stricmp(szString, "default") == 0 || stricmp(szString, "checkpoint") == 0)
    {
        return MARKER_CHECKPOINT;
    }
    else if (stricmp(szString, "ring") == 0)
    {
        return MARKER_RING;
    }
    else if (stricmp(szString, "cylinder") == 0)
    {
        return MARKER_CYLINDER;
    }
    else if (stricmp(szString, "arrow") == 0)
    {
        return MARKER_ARROW;
    }
    else if (stricmp(szString, "corona") == 0)
    {
        return MARKER_CORONA;
    }
    else
    {
        return MARKER_INVALID;
    }
}

bool CClientMarker::TypeToString(unsigned int uiType, SString& strOutString)
{
    switch (uiType)
    {
        case MARKER_CHECKPOINT:
            strOutString = "checkpoint";
            return true;

        case MARKER_RING:
            strOutString = "ring";
            return true;

        case MARKER_CYLINDER:
            strOutString = "cylinder";
            return true;

        case MARKER_ARROW:
            strOutString = "arrow";
            return true;

        case MARKER_CORONA:
            strOutString = "corona";
            return true;

        default:
            strOutString = "invalid";
            return false;
    }
}

bool CClientMarker::IsLimitReached()
{
    return m_uiStreamedInMarkers >= 32;
}

void CClientMarker::StreamIn(bool bInstantly)
{
    // Not already streamed in?
    if (!IsStreamedIn())
    {
        // Stream the marker in
        m_pMarker->StreamIn();

        // Increment streamed in counter
        ++m_uiStreamedInMarkers;

        // Tell the streamer we've created this object
        NotifyCreate();
    }
}

void CClientMarker::StreamOut()
{
    // Streamed in?
    if (IsStreamedIn())
    {
        // Decrement streamed in counter
        --m_uiStreamedInMarkers;

        // Stream the marker out
        m_pMarker->StreamOut();
    }
}

void CClientMarker::Callback_OnCollision(CClientColShape& Shape, CClientEntity& Entity)
{
    if (IS_PLAYER(&Entity))
    {
        // Call the marker hit event
        CLuaArguments Arguments;
        Arguments.PushElement(&Entity);                                              // player that hit it
        Arguments.PushBoolean((GetDimension() == Entity.GetDimension()));            // matching dimension?
        CallEvent("onClientMarkerHit", Arguments, true);
    }
}

void CClientMarker::Callback_OnLeave(CClientColShape& Shape, CClientEntity& Entity)
{
    if (IS_PLAYER(&Entity))
    {
        // Call the marker hit event
        CLuaArguments Arguments;
        Arguments.PushElement(&Entity);                                              // player that hit it
        Arguments.PushBoolean((GetDimension() == Entity.GetDimension()));            // matching dimension?
        CallEvent("onClientMarkerLeave", Arguments, true);
    }
}

void CClientMarker::CreateOfType(int iType)
{
    SAFE_DELETE(m_pCollision)

    CVector vecOrigin;
    GetPosition(vecOrigin);
    switch (iType)
    {
        case MARKER_CHECKPOINT:
        {
            CClientCheckpoint* pCheckpoint = new CClientCheckpoint(this);
            pCheckpoint->SetCheckpointType(CClientCheckpoint::TYPE_NORMAL);
            m_pMarker = pCheckpoint;
            m_pCollision = new CClientColCircle(g_pClientGame->GetManager(), NULL, vecOrigin, GetSize());
            m_pCollision->m_pOwningMarker = this;
            m_pCollision->SetHitCallback(this);
            break;
        }

        case MARKER_RING:
        {
            CClientCheckpoint* pCheckpoint = new CClientCheckpoint(this);
            pCheckpoint->SetCheckpointType(CClientCheckpoint::TYPE_RING);
            m_pMarker = pCheckpoint;
            m_pCollision = new CClientColSphere(g_pClientGame->GetManager(), NULL, vecOrigin, GetSize());
            m_pCollision->m_pOwningMarker = this;
            m_pCollision->SetHitCallback(this);
            break;
        }

        case MARKER_CYLINDER:
        {
            CClient3DMarker* p3DMarker = new CClient3DMarker(this);
            p3DMarker->Set3DMarkerType(CClient3DMarker::TYPE_CYLINDER);
            m_pMarker = p3DMarker;
            m_pCollision = new CClientColCircle(g_pClientGame->GetManager(), NULL, vecOrigin, GetSize());
            m_pCollision->m_pOwningMarker = this;
            m_pCollision->SetHitCallback(this);
            break;
        }

        case MARKER_ARROW:
        {
            CClient3DMarker* p3DMarker = new CClient3DMarker(this);
            p3DMarker->Set3DMarkerType(CClient3DMarker::TYPE_ARROW);
            m_pMarker = p3DMarker;
            m_pCollision = new CClientColSphere(g_pClientGame->GetManager(), NULL, vecOrigin, GetSize());
            m_pCollision->m_pOwningMarker = this;
            m_pCollision->SetHitCallback(this);
            break;
        }

        case MARKER_CORONA:
        {
            m_pMarker = new CClientCorona(this);
            m_pCollision = new CClientColSphere(g_pClientGame->GetManager(), NULL, vecOrigin, GetSize());
            m_pCollision->m_pOwningMarker = this;
            m_pCollision->SetHitCallback(this);
            break;
        }

        default:
            break;
    }
    if (m_pCollision)
        m_pCollision->SetCanBeDestroyedByScript(false);
}

CSphere CClientMarker::GetWorldBoundingSphere()
{
    CSphere sphere;
    GetPosition(sphere.vecPosition);
    // sphere.vecPosition = GetStreamPosition ();
    sphere.fRadius = GetSize();
    return sphere;
}

void CClientMarker::SetIgnoreAlphaLimits(bool ignore)
{
    m_pMarker->SetIgnoreAlphaLimits(ignore);
}
