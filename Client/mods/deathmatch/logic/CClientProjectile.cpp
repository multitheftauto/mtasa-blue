/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientProjectile.cpp
 *  PURPOSE:     Projectile entity class
 *
 *****************************************************************************/

#include <StdInc.h>
#include <game/CProjectileInfo.h>

#define AIRBOMB_MAX_LIFETIME 60000

/* An instance of this class is created when GTA creates a projectile, it automatically
   destroys itself when GTA is finished with the projectile, this could/should eventually be
   used as a server created element and streamed.
*/
CClientProjectile::CClientProjectile(class CClientManager* pManager, CProjectile* pProjectile, CProjectileInfo* pProjectileInfo, CClientEntity* pCreator,
                                     CClientEntity* pTarget, eWeaponType weaponType, CVector* pvecOrigin, CVector* pvecTarget, float fForce, bool bLocal)
    : ClassInit(this), CClientEntity(INVALID_ELEMENT_ID)
{
    CClientEntityRefManager::AddEntityRefs(ENTITY_REF_DEBUG(this, "CClientProjectile"), &m_pCreator, &m_pTarget, NULL);

    m_pManager = pManager;
    m_pProjectileManager = pManager->GetProjectileManager();
    m_pProjectile = pProjectile;
    m_pProjectileInfo = pProjectileInfo;

    SetTypeName("projectile");

    m_pCreator = pCreator;
    m_pTarget = pTarget;
    m_weaponType = weaponType;
    if (pvecOrigin)
        m_pvecOrigin = new CVector(*pvecOrigin);
    else
        m_pvecOrigin = NULL;
    if (pvecTarget)
        m_pvecTarget = new CVector(*pvecTarget);
    else
        m_pvecTarget = NULL;
    m_fForce = fForce;
    m_bLocal = bLocal;
    m_llCreationTime = GetTickCount64_();

    m_pInitiateData = NULL;
    m_bInitiate = true;

    m_pProjectileManager->AddToList(this);
    m_bLinked = true;

    if (pCreator)
    {
        switch (pCreator->GetType())
        {
            case CCLIENTPLAYER:
            case CCLIENTPED:
                static_cast<CClientPed*>(pCreator)->AddProjectile(this);
                break;
            case CCLIENTVEHICLE:
                static_cast<CClientVehicle*>(pCreator)->AddProjectile(this);
                break;
            default:
                break;
        }
    }
}

CClientProjectile::~CClientProjectile()
{
    // If our creator is getting destroyed, this should be null
    if (m_pCreator)
    {
        switch (m_pCreator->GetType())
        {
            case CCLIENTPLAYER:
            case CCLIENTPED:
                static_cast<CClientPed*>((CClientEntity*)m_pCreator)->RemoveProjectile(this);
                break;
            case CCLIENTVEHICLE:
                static_cast<CClientVehicle*>((CClientEntity*)m_pCreator)->RemoveProjectile(this);
                break;
            default:
                break;
        }
    }

    if (m_pvecOrigin)
        delete m_pvecOrigin;
    if (m_pvecTarget)
        delete m_pvecTarget;

    if (m_pInitiateData)
        delete m_pInitiateData;

    Unlink();

    if (m_pProjectile)
    {
        // Make sure we're destroyed

        //m_pProjectile->Destroy(true);
        m_pProjectile = NULL;
    }

    CClientEntityRefManager::RemoveEntityRefs(0, &m_pCreator, &m_pTarget, NULL);
}

void CClientProjectile::Unlink()
{
    // Are we still linked? (this bool will be set to false when our manager is being destroyed)
    if (m_bLinked)
    {
        m_pProjectileManager->RemoveFromList(this);
        m_bLinked = false;
        if (m_pProjectile)
        {
            // Make sure we're destroyed
            //m_pProjectile->Destroy(true);

            m_pProjectile = NULL;
        }
    }
}

void CClientProjectile::DoPulse()
{
    // Update our position/rotation if we're attached
    DoAttaching();

    if (m_bCorrected == false && m_pProjectile != NULL && GetWeaponType() == eWeaponType::WEAPONTYPE_REMOTE_SATCHEL_CHARGE)
        m_bCorrected = m_pProjectile->CorrectPhysics();
}

void CClientProjectile::Initiate(CVector& vecPosition, CVector& vecRotation, CVector& vecVelocity, unsigned short usModel)
{
    SetPosition(vecPosition);

    if (vecRotation != CVector(0, 0, 0))
        SetRotationRadians(vecRotation);

    if (vecVelocity != CVector(0, 0, 0))
        SetVelocity(vecVelocity);

    SetModel(usModel);

    g_pClientGame->ProjectileInitiateHandler(this);
}

void CClientProjectile::Destroy(bool bBlow)
{
    if (m_pProjectile)
    {
        m_pProjectile->Destroy(bBlow);
        m_pProjectile = nullptr;
    }
}

bool CClientProjectile::IsActive()
{
    // Ensure airbomb is cleaned up
    if (m_weaponType == WEAPONTYPE_FREEFALL_BOMB && GetTickCount64_() - m_llCreationTime > AIRBOMB_MAX_LIFETIME)
        return false;
    return (m_pProjectile && m_pProjectileInfo->IsActive());
}

bool CClientProjectile::GetMatrix(CMatrix& matrix) const
{
    if (m_pProjectile)
    {
        if (m_pProjectile->GetMatrix(&matrix))
        {
            // Jax: If the creator is a ped, we need to invert X and Y on Direction and Was for CMultiplayer::ConvertMatrixToEulerAngles
            if (m_pCreator && IS_PED(m_pCreator))
            {
                matrix.vFront.fX = 0.0f - matrix.vFront.fX;
                matrix.vFront.fY = 0.0f - matrix.vFront.fY;
                matrix.vUp.fX = 0.0f - matrix.vUp.fX;
                matrix.vUp.fY = 0.0f - matrix.vUp.fY;
            }
            return true;
        }
    }
    return false;
}

bool CClientProjectile::SetMatrix(const CMatrix& matrix_)
{
    CMatrix matrix(matrix_);

    // Jax: If the creator is a ped, we need to invert X and Y on Direction and Was for CMultiplayer::ConvertEulerAnglesToMatrix
    if (m_pCreator && IS_PED(m_pCreator))
    {
        matrix.vFront.fX = 0.0f - matrix.vFront.fX;
        matrix.vFront.fY = 0.0f - matrix.vFront.fY;
        matrix.vUp.fX = 0.0f - matrix.vUp.fX;
        matrix.vUp.fY = 0.0f - matrix.vUp.fY;
    }

    if (m_pProjectile)
        m_pProjectile->SetMatrix(&matrix);
    return true;
}

void CClientProjectile::GetPosition(CVector& vecPosition) const
{
    if (m_pProjectile)
        vecPosition = *m_pProjectile->GetPosition();
    else
        vecPosition = CVector();
}

void CClientProjectile::SetPosition(const CVector& vecPosition)
{
    if (m_pProjectile)
        m_pProjectile->SetPosition(const_cast<CVector*>(&vecPosition));
}

void CClientProjectile::GetRotationRadians(CVector& vecRotation) const
{
    CMatrix matrix;
    GetMatrix(matrix);
    g_pMultiplayer->ConvertMatrixToEulerAngles(matrix, vecRotation.fX, vecRotation.fY, vecRotation.fZ);
}

void CClientProjectile::GetRotationDegrees(CVector& vecRotation) const
{
    GetRotationRadians(vecRotation);
    ConvertRadiansToDegrees(vecRotation);
}

void CClientProjectile::SetRotationRadians(const CVector& vecRotation)
{
    CMatrix matrix;
    GetPosition(matrix.vPos);
    g_pMultiplayer->ConvertEulerAnglesToMatrix(matrix, vecRotation.fX, vecRotation.fY, vecRotation.fZ);
    SetMatrix(matrix);
}

void CClientProjectile::SetRotationDegrees(const CVector& vecRotation)
{
    CVector vecTemp = vecRotation;
    ConvertDegreesToRadians(vecTemp);
    SetRotationRadians(vecTemp);
}

void CClientProjectile::GetVelocity(CVector& vecVelocity)
{
    if (m_pProjectile)
        m_pProjectile->GetMoveSpeed(&vecVelocity);
    else
        vecVelocity = CVector();
}

void CClientProjectile::SetVelocity(CVector& vecVelocity)
{
    if (m_pProjectile)
        m_pProjectile->SetMoveSpeed(vecVelocity);
}

unsigned short CClientProjectile::GetModel()
{
    if (m_pProjectile)
        return m_pProjectile->GetModelIndex();
    return 0;
}

void CClientProjectile::SetModel(unsigned short usModel)
{
    if (m_pProjectile)
        m_pProjectile->SetModelIndex(usModel);
}

void CClientProjectile::SetCounter(DWORD dwCounter)
{
    if (m_pProjectile)
        m_pProjectileInfo->SetCounter(dwCounter);
}

DWORD CClientProjectile::GetCounter()
{
    if (m_pProjectile)
        return m_pProjectileInfo->GetCounter();
    return 0;
}

CClientEntity* CClientProjectile::GetSatchelAttachedTo()
{
    if (!m_pProjectile)
        return NULL;

    CEntity* pAttachedToSA = m_pProjectile->GetAttachedEntity();

    if (!pAttachedToSA)
        return NULL;

    CPools* pPools = g_pGame->GetPools();
    return pPools->GetClientEntity((DWORD*)pAttachedToSA->GetInterface());
}
