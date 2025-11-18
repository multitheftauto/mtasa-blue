/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientObject.cpp
 *  PURPOSE:     Physical object entity class
 *
 *****************************************************************************/

#include <StdInc.h>

#define MTA_BUILDINGS
#define CCLIENTOBJECT_MAX 250

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

CClientObject::CClientObject(CClientManager* pManager, ElementID ID, unsigned short usModel, bool bLowLod)
    : ClassInit(this), CClientStreamElement(bLowLod ? pManager->GetObjectLodStreamer() : pManager->GetObjectStreamer(), ID), m_bIsLowLod(bLowLod)
{
    // Init
    m_pManager = pManager;
    m_pObjectManager = pManager->GetObjectManager();
    m_pModelRequester = pManager->GetModelRequestManager();

    m_pObject = NULL;
    m_usModel = usModel;

    SetTypeName("object");

    m_usModel = usModel;
    m_bIsVisible = true;
    m_bIsFrozen = false;
    m_bUsesCollision = true;
    m_ucAlpha = 255;
    m_vecScale = CVector(1.0f, 1.0f, 1.0f);
    m_fHealth = 1000.0f;
    m_bBreakingDisabled = false;
    m_bRespawnEnabled = true;
    m_fMass = -1.0f;
    m_fTurnMass = -1.0f;
    m_fAirResistance = -1.0f;
    m_fElasticity = -1.0f;
    m_fBuoyancyConstant = -1.0f;
    m_vecCenterOfMass = CVector(0.0f, 0.0f, 0.0f);

    m_pModelInfo = g_pGame->GetModelInfo(usModel);

    // Add this object to the list
    m_pObjectManager->AddToList(this);

    if (m_bIsLowLod)
        m_pManager->OnLowLODElementCreated();
    m_clientModel = pManager->GetModelManager()->FindModelByID(usModel);
}

CClientObject::~CClientObject()
{
    // Unrequest whatever we've requested or we'll crash in unlucky situations
    m_pModelRequester->Cancel(this, false);

    // Detach us from anything
    AttachTo(NULL);

    // Destroy the object
    Destroy();

    // Remove us from the list
    Unlink();

    if (m_bIsLowLod)
        m_pManager->OnLowLODElementDestroyed();
    m_clientModel = nullptr;
}

void CClientObject::Unlink()
{
    m_pObjectManager->RemoveFromLists(this);
    g_pClientGame->GetObjectRespawner()->Unreference(this);

    // Remove LowLod refs in others
    SetLowLodObject(NULL);
    while (!m_HighLodObjectList.empty())
        m_HighLodObjectList[0]->SetLowLodObject(NULL);

    // Remove from ifp list if needed
    if (m_animationBlockNameHash > 0)
    {
        auto ifp = g_pClientGame->GetIFPPointerFromMap(m_animationBlockNameHash);
        if (ifp)
            ifp->RemoveEntityUsingThisIFP(this);
    }
}

void CClientObject::GetPosition(CVector& vecPosition) const
{
    if (m_pObject)
    {
        vecPosition = *m_pObject->GetPosition();
    }
    else if (m_pAttachedToEntity)
    {
        m_pAttachedToEntity->GetPosition(vecPosition);
        vecPosition += m_vecAttachedPosition;
    }
    else
    {
        vecPosition = m_vecPosition;
    }
}

void CClientObject::SetPosition(const CVector& vecPosition)
{
    // Move the object
    if (m_pObject)
    {
        CVector vecRot;
        GetRotationRadians(vecRot);
        m_pObject->Teleport(vecPosition.fX, vecPosition.fY, vecPosition.fZ);
#ifndef MTA_BUILDINGS
        m_pObject->ProcessCollision();
#endif
        m_pObject->SetupLighting();
        SetRotationRadians(vecRot);
    }

    if (m_vecPosition != vecPosition)
    {
        // Store the position in our datastruct
        m_vecPosition = vecPosition;

        // Update our streaming position
        UpdateStreamPosition(vecPosition);
    }
}

void CClientObject::GetRotationDegrees(CVector& vecRotation) const
{
    GetRotationRadians(vecRotation);
    ConvertRadiansToDegrees(vecRotation);
}

void CClientObject::GetRotationRadians(CVector& vecRotation) const
{
    if (m_pObject && m_pAttachedToEntity)            // Temp fix for static objects->
    {
        // We've been returning the rotation that got set last so far (::m_vecRotation)..
        //   but we need to get the real rotation for when the game moves the objects..
        CMatrix matTemp;
        m_pObject->GetMatrix(&matTemp);
        vecRotation = matTemp.GetRotation();
    }
    else
    {
        vecRotation = m_vecRotation;
    }
}

void CClientObject::SetRotationDegrees(const CVector& vecRotation)
{
    // Convert from degrees to radians
    CVector vecTemp;
    vecTemp.fX = vecRotation.fX * 3.1415926535897932384626433832795f / 180.0f;
    vecTemp.fY = vecRotation.fY * 3.1415926535897932384626433832795f / 180.0f;
    vecTemp.fZ = vecRotation.fZ * 3.1415926535897932384626433832795f / 180.0f;

    SetRotationRadians(vecTemp);
}

void CClientObject::SetRotationRadians(const CVector& vecRotation)
{
    if (m_pObject)
    {
        m_pObject->SetOrientation(vecRotation.fX, vecRotation.fY, vecRotation.fZ);
#ifndef MTA_BUILDINGS
        m_pObject->ProcessCollision();
#endif
        m_pObject->SetupLighting();
    }

    // Store it in our datastruct
    m_vecRotation = vecRotation;
}

void CClientObject::AttachTo(CClientEntity* pEntity)
{
    CClientEntity::AttachTo(pEntity);

    if (m_pAttachedToEntity)
    {
        DoAttaching();
        UpdateStreamPosition(m_vecPosition);
    }
}

void CClientObject::GetOrientation(CVector& vecPosition, CVector& vecRotationRadians)
{
    GetPosition(vecPosition);
    GetRotationRadians(vecRotationRadians);
}

void CClientObject::SetOrientation(const CVector& vecPosition, const CVector& vecRotationRadians)
{
    if (m_vecPosition != vecPosition)
    {
        // Store the position in our datastruct
        m_vecPosition = vecPosition;

        // Update our streaming position
        UpdateStreamPosition(vecPosition);
    }

    // Update our internal data
    m_vecPosition = vecPosition;
    m_vecRotation = vecRotationRadians;

    // Eventually move the object
    if (m_pObject)
    {
        m_pObject->Teleport(vecPosition.fX, vecPosition.fY, vecPosition.fZ);
        m_pObject->SetOrientation(vecRotationRadians.fX, vecRotationRadians.fY, vecRotationRadians.fZ);
#ifndef MTA_BUILDINGS
        m_pObject->ProcessCollision();
#endif
        m_pObject->SetupLighting();
    }
}

void CClientObject::ModelRequestCallback(CModelInfo* pModelInfo)
{
    // The model loading may take a while and there's a chance of object being moved to other dimension.
    if (!IsVisibleInAllDimensions() && GetDimension() != m_pStreamer->GetDimension())
    {
        NotifyUnableToCreate();
        return;
    }

    // Create our object
    Create();
}

float CClientObject::GetDistanceFromCentreOfMassToBaseOfModel()
{
    if (m_pObject)
    {
        return m_pObject->GetDistanceFromCentreOfMassToBaseOfModel();
    }
    else
    {
        return 0;
    }
}

void CClientObject::SetVisible(bool bVisible)
{
    m_bIsVisible = bVisible;
    UpdateVisibility();
}

// Call this when m_bIsVisible, m_IsHiddenLowLod or m_pObject is changed
void CClientObject::UpdateVisibility()
{
    if (m_pObject)
    {
        m_pObject->SetVisible(m_bIsVisible && !m_IsHiddenLowLod);
    }
}

void CClientObject::SetModel(unsigned short usModel)
{
    // Valid model ID?
    if (CClientObjectManager::IsValidModel(usModel))
    {
        // Destroy current model
        Destroy();

        // Set the new model ID and recreate the model
        m_usModel = usModel;
        if (m_clientModel && m_clientModel->GetModelID() != m_usModel)
            m_clientModel = nullptr;
        m_pModelInfo = g_pGame->GetModelInfo(usModel);
        UpdateSpatialData();

        // Create the object if we're streamed in
        if (IsStreamedIn())
        {
            // Request the new model so we can recreate when it's done
            if (m_pModelRequester->Request(usModel, this))
            {
                Create();
            }
        }
    }
}

bool CClientObject::IsLowLod()
{
    return m_bIsLowLod;
}

bool CClientObject::SetLowLodObject(CClientObject* pNewLowLodObject)
{
    // This object has to be high lod
    if (m_bIsLowLod)
        return false;

    // Set or clear?
    if (!pNewLowLodObject)
    {
        // Check if already clear
        if (!m_pLowLodObject)
            return false;

        // Verify link
        assert(ListContains(m_pLowLodObject->m_HighLodObjectList, this));

        // Clear there and here
        ListRemove(m_pLowLodObject->m_HighLodObjectList, this);
        m_pLowLodObject = NULL;
        return true;
    }
    else
    {
        // new object has to be low lod
        if (!pNewLowLodObject->m_bIsLowLod)
            return false;

        // Remove any previous link
        SetLowLodObject(NULL);

        // Make new link
        m_pLowLodObject = pNewLowLodObject;
        pNewLowLodObject->m_HighLodObjectList.push_back(this);
        return true;
    }
}

CClientObject* CClientObject::GetLowLodObject()
{
    if (m_bIsLowLod)
        return NULL;
    return m_pLowLodObject;
}

void CClientObject::Render()
{
    if (m_pObject)
    {
        m_pObject->Render();
    }
}

void CClientObject::SetFrozen(bool bFrozen)
{
    m_bIsFrozen = bFrozen;

    if (m_pObject)
    {
        m_pObject->SetFrozen(bFrozen);
    }

    // Reset speed if we frozing object
    if (bFrozen)
    {
        // Reset speed only if object is actually moving
        CVector vecZero;
        CVector vecSpeed;

        GetMoveSpeed(vecSpeed);
        if (vecZero != vecSpeed)
        {
            SetMoveSpeed(vecZero);
        }

        GetTurnSpeed(vecSpeed);
        if (vecZero != vecSpeed)
        {
            SetTurnSpeed(vecZero);
        }
    }
}

void CClientObject::SetAlpha(unsigned char ucAlpha)
{
    if (m_pObject)
    {
        m_pObject->SetAlpha(ucAlpha);
    }
    m_ucAlpha = ucAlpha;
}

void CClientObject::GetScale(CVector& vecScale) const
{
    if (m_pObject)
    {
        vecScale = *m_pObject->GetScale();
    }
    else
    {
        vecScale = m_vecScale;
    }
}

void CClientObject::SetScale(const CVector& vecScale)
{
    if (m_pObject)
    {
        m_pObject->SetScale(vecScale.fX, vecScale.fY, vecScale.fZ);
    }
    m_vecScale = vecScale;
}

void CClientObject::SetCollisionEnabled(bool bCollisionEnabled)
{
    if (m_pObject)
        m_pObject->SetUsesCollision(bCollisionEnabled);

    // Remove all contacts
    for (const auto& ped : m_Contacts)
        RemoveContact(ped);

    m_bUsesCollision = bCollisionEnabled;
}

float CClientObject::GetHealth()
{
    if (m_pObject)
    {
        return m_pObject->GetHealth();
    }

    return m_fHealth;
}

void CClientObject::SetHealth(float fHealth)
{
    if (m_pObject)
    {
        m_pObject->SetHealth(fHealth);
    }

    m_fHealth = fHealth;
}

void CClientObject::SetAnimation(CAnimBlendHierarchySAInterface* animation, unsigned int blockNameHash)
{
    m_animation = animation;
    m_animationBlockNameHash = blockNameHash;

    RunAnimation();
}

bool CClientObject::SetAnimationSpeed(float speed)
{
    m_animationSpeed = speed;
    m_animationSpeedUpdated = false;

    if (m_pObject && m_pObject->GetRpClump())
    {
        m_animationSpeedUpdated = true;
        return m_pObject->SetAnimationSpeed(speed);
    }

    return true;
}

void CClientObject::StreamIn(bool bInstantly)
{
    // Don't stream the object in, if respawn is disabled and the object is broken
    if (!m_bRespawnEnabled && m_fHealth == 0.0f)
        return;

    // We need to load now?
    if (bInstantly)
    {
        // Request the model blocking
        if (m_pModelRequester->RequestBlocking(m_usModel, "CClientObject::StreamIn - bInstantly"))
        {
            // Create us
            Create();
        }
        else
            NotifyUnableToCreate();
    }
    else
    {
        // Request the model async
        if (m_pModelRequester->Request(m_usModel, this))
        {
            // Create us now if we already had it loaded
            Create();
        }
        else
            NotifyUnableToCreate();
    }
}

void CClientObject::StreamOut()
{
    // Save the health
    if (m_pObject)
    {
        // If respawn is enabled, reset the health
        if (m_bRespawnEnabled && m_fHealth == 0.0f)
            m_fHealth = 1000.0f;
        else
            m_fHealth = m_pObject->GetHealth();
    }

    // Destroy the object.
    Destroy();

    // Cancel anything we might've requested.
    m_pModelRequester->Cancel(this, true);
}

// Don't call this function directly by lua functions
void CClientObject::ReCreate()
{
    m_fHealth = 1000.0f;

    if (m_pObject)
        Destroy();

    Create();
}

void CClientObject::Create()
{
    // Not already created an object?
    if (!m_pObject)
    {
        // Check again that the limit isn't reached. We are required to do so because
        // we load async. The streamer isn't always aware of our limits.
        if (IsLowLod() ? !CClientObjectManager::StaticIsLowLodObjectLimitReached() : !CClientObjectManager::StaticIsObjectLimitReached())
        {
            // Add a reference to the object
            m_pModelInfo->ModelAddRef(BLOCKING, "CClientObject::Create");

            // If the new object is not breakable, allow it into the vertical line test
            g_pMultiplayer->AllowCreatedObjectsInVerticalLineTest(!CClientObjectManager::IsBreakableModel(m_usModel));

            // Create the object
            m_pObject = g_pGame->GetPools()->AddObject(this, m_usModel, m_bIsLowLod, m_bBreakingDisabled);

            // Restore default behaviour
            g_pMultiplayer->AllowCreatedObjectsInVerticalLineTest(false);

            if (m_pObject)
            {
                // Put our pointer in its stored pointer
                m_pObject->SetStoredPointer(this);

                // Apply our data to the object
                m_pObject->Teleport(m_vecPosition.fX, m_vecPosition.fY, m_vecPosition.fZ);
                m_pObject->SetOrientation(m_vecRotation.fX, m_vecRotation.fY, m_vecRotation.fZ);
                #ifndef MTA_BUILDINGS
                m_pObject->ProcessCollision();
                #endif
                m_pObject->SetupLighting();
                m_pObject->SetFrozen(m_bIsFrozen);

                UpdateVisibility();
                if (!m_bUsesCollision)
                    SetCollisionEnabled(false);
                if (m_vecScale.fX != 1.0f || m_vecScale.fY != 1.0f || m_vecScale.fZ != 1.0f)
                    SetScale(m_vecScale);
                m_pObject->SetAreaCode(m_ucInterior);
                SetAlpha(m_ucAlpha);
                m_pObject->SetHealth(m_fHealth);

                // Set object properties
                if (m_fMass != -1.0f)
                    m_pObject->SetMass(m_fMass);
                if (m_fTurnMass != -1.0f)
                    m_pObject->SetTurnMass(m_fTurnMass);
                if (m_fAirResistance != -1.0f)
                    m_pObject->SetAirResistance(m_fAirResistance);
                if (m_fElasticity != -1.0f)
                    m_pObject->SetElasticity(m_fElasticity);
                if (m_fBuoyancyConstant != -1.0f)
                    m_pObject->SetBuoyancyConstant(m_fBuoyancyConstant);
                if (m_vecCenterOfMass.fX != 0.0f || m_vecCenterOfMass.fY != 0.0f || m_vecCenterOfMass.fZ != 0.0f)
                    m_pObject->SetCenterOfMass(m_vecCenterOfMass);

                m_animationPlaying = false;
                m_animationSpeedUpdated = false;

                // Reattach to an entity + any entities attached to this
                ReattachEntities();

                // Validate this entity again
                m_pManager->RestoreEntity(this);

                // Tell the streamer we've created this object
                NotifyCreate();

                // Done
                return;
            }
            else
            {
                // Remove our reference to the object again
                m_pModelInfo->RemoveRef();
            }
        }

        // Tell the streamer we could not create it
        NotifyUnableToCreate();
    }
}

void CClientObject::Destroy()
{
    // If the object exists
    if (m_pObject)
    {
        // Invalidate
        m_pManager->InvalidateEntity(this);

        // Destroy the object
        g_pGame->GetPools()->RemoveObject(m_pObject);
        m_pObject = NULL;

        // Remove our reference to its model
        m_pModelInfo->RemoveRef();

        NotifyDestroy();
    }
}

void CClientObject::NotifyCreate()
{
    m_pObjectManager->OnCreation(this);
    CClientStreamElement::NotifyCreate();
}

void CClientObject::NotifyDestroy()
{
    m_pObjectManager->OnDestruction(this);
}

void CClientObject::StreamedInPulse()
{
    // Some things to do if low LOD object
    if (m_bIsLowLod)
    {
        // Manually update attaching in case other object is streamed out
        DoAttaching();

        // Be hidden if all HighLodObjects are fully visible
        m_IsHiddenLowLod = true;
        if (m_HighLodObjectList.empty())
            m_IsHiddenLowLod = false;
        for (std::vector<CClientObject*>::iterator iter = m_HighLodObjectList.begin(); iter != m_HighLodObjectList.end(); ++iter)
        {
            CObject* pObject = (*iter)->m_pObject;
            if (!pObject || !pObject->IsFullyVisible())
            {
                m_IsHiddenLowLod = false;
                break;
            }
        }

        UpdateVisibility();
    }
    else
    {
        // Fixed attachment bug #9339 where [object1] -> [object2] -> [vehicle] causes positional lag for [object1]
        if (m_pAttachedToEntity && m_pAttachedToEntity->GetAttachedTo())
        {
            DoAttaching();
        }
    }

    // Are we not frozen
    if (!m_bIsFrozen)
    {
        // Grab our actual position (as GTA moves it too)
        CVector vecPosition = *m_pObject->GetPosition();

        // Has it moved without MTA knowing?
        if (vecPosition != m_vecPosition)
        {
            m_vecPosition = vecPosition;

            // Update our streaming position
            UpdateStreamPosition(m_vecPosition);
        }
    }

    if (m_animation && !m_animationPlaying)
        RunAnimation();

    if (!m_animationSpeedUpdated)
    {
        if (m_pObject && m_pObject->GetRpClump())
        {
            m_pObject->SetAnimationSpeed(m_animationSpeed);
            m_animationSpeedUpdated = true;
        }
    }
}

void CClientObject::RunAnimation()
{
    if (!m_pObject)
        return;

    m_pObject->SetAnimation(m_animation);
    m_pObject->SetAnimationSpeed(m_animationSpeed);

    m_animationPlaying = m_animation != nullptr && m_pObject->GetRpClump();
}

void CClientObject::GetMoveSpeed(CVector& vecMoveSpeed) const
{
    if (m_pObject)
    {
        m_pObject->GetMoveSpeed(&vecMoveSpeed);
    }
    else
    {
        vecMoveSpeed = m_vecMoveSpeed;
    }
}

void CClientObject::SetMoveSpeed(const CVector& vecMoveSpeed)
{
    if (m_pObject)
    {
        m_pObject->SetMoveSpeed(vecMoveSpeed);
    }
    m_vecMoveSpeed = vecMoveSpeed;
}

void CClientObject::GetTurnSpeed(CVector& vecTurnSpeed) const
{
    if (m_pObject)
    {
        m_pObject->GetTurnSpeed(&vecTurnSpeed);
    }
    else
    {
        vecTurnSpeed = m_vecTurnSpeed;
    }
}

void CClientObject::SetTurnSpeed(const CVector& vecTurnSpeed)
{
    if (m_pObject)
    {
        m_pObject->SetTurnSpeed(const_cast<CVector*>(&vecTurnSpeed));
    }
    m_vecTurnSpeed = vecTurnSpeed;
}

CSphere CClientObject::GetWorldBoundingSphere()
{
    CSphere     sphere;
    CModelInfo* pModelInfo = g_pGame->GetModelInfo(GetModel());
    if (pModelInfo)
    {
        CBoundingBox* pBoundingBox = pModelInfo->GetBoundingBox();
        if (pBoundingBox)
        {
            sphere.vecPosition = pBoundingBox->vecBoundOffset;
            sphere.fRadius = pBoundingBox->fRadius;
        }
    }
    sphere.vecPosition += GetStreamPosition();
    return sphere;
}

bool CClientObject::IsBreakable(bool bCheckModelList)
{
    if (!bCheckModelList)
        return !m_bBreakingDisabled;

    return (CClientObjectManager::IsBreakableModel(m_usModel) && !m_bBreakingDisabled);
}

bool CClientObject::SetBreakable(bool bBreakable)
{
    bool bDisableBreaking = !bBreakable;
    // Are we breakable and have we changed
    if (CClientObjectManager::IsBreakableModel(m_usModel) && m_bBreakingDisabled != bDisableBreaking)
    {
        m_bBreakingDisabled = bDisableBreaking;
        // We can't use ReCreate directly (otherwise the game will crash)
        g_pClientGame->GetObjectRespawner()->Respawn(this);
        return true;
    }
    return false;
}

bool CClientObject::Break()
{
    // Are we breakable?
    if (m_pObject && CClientObjectManager::IsBreakableModel(m_usModel) && !m_bBreakingDisabled)
    {
        m_pObject->Break();
        return true;
    }
    return false;
}

float CClientObject::GetMass()
{
    if (m_pObject)
        return m_pObject->GetMass();

    return m_fMass;
}

void CClientObject::SetMass(float fMass)
{
    if (m_pObject)
        m_pObject->SetMass(fMass);

    m_fMass = fMass;
}

float CClientObject::GetTurnMass()
{
    if (m_pObject)
        return m_pObject->GetTurnMass();

    return m_fTurnMass;
}

void CClientObject::SetTurnMass(float fTurnMass)
{
    if (m_pObject)
        m_pObject->SetTurnMass(fTurnMass);

    m_fTurnMass = fTurnMass;
}

float CClientObject::GetAirResistance()
{
    if (m_pObject)
        return m_pObject->GetAirResistance();

    return m_fAirResistance;
}

void CClientObject::SetAirResistance(float fAirResistance)
{
    if (m_pObject)
        m_pObject->SetAirResistance(fAirResistance);

    m_fAirResistance = fAirResistance;
}

float CClientObject::GetElasticity()
{
    if (m_pObject)
        return m_pObject->GetElasticity();

    return m_fElasticity;
}

void CClientObject::SetElasticity(float fElasticity)
{
    if (m_pObject)
        m_pObject->SetElasticity(fElasticity);

    m_fElasticity = fElasticity;
}

float CClientObject::GetBuoyancyConstant()
{
    if (m_pObject)
        return m_pObject->GetBuoyancyConstant();

    return m_fBuoyancyConstant;
}

void CClientObject::SetBuoyancyConstant(float fBuoyancyConstant)
{
    if (m_pObject)
        m_pObject->SetBuoyancyConstant(fBuoyancyConstant);

    m_fBuoyancyConstant = fBuoyancyConstant;
}

void CClientObject::GetCenterOfMass(CVector& vecCenterOfMass) const
{
    if (m_pObject)
        m_pObject->GetCenterOfMass(vecCenterOfMass);
    else
        vecCenterOfMass = m_vecCenterOfMass;
}

void CClientObject::SetCenterOfMass(const CVector& vecCenterOfMass)
{
    if (m_pObject)
        m_pObject->SetCenterOfMass(const_cast<CVector&>(vecCenterOfMass));

    m_vecCenterOfMass = vecCenterOfMass;
}

void CClientObject::SetVisibleInAllDimensions(bool bVisible, unsigned short usNewDimension)
{
    m_bVisibleInAllDimensions = bVisible;

    // Stream-in/out the object as needed
    if (bVisible)
    {
        if (g_pClientGame->GetLocalPlayer())
        {
            SetDimension(g_pClientGame->GetLocalPlayer()->GetDimension());
        }
    }
    else
    {
        SetDimension(usNewDimension);
    }
}
