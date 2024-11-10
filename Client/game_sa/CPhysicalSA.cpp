/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPhysicalSA.cpp
 *  PURPOSE:     Physical object entity
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <CRect.h>
#include "CColModelSA.h"
#include "CGameSA.h"
#include "CPhysicalSA.h"
#include "CPoolsSA.h"

extern CGameSA* pGame;

CRect* CPhysicalSAInterface::GetBoundRect_(CRect* pRect)
{
    CVector boundCentre;
    CEntitySAInterface::GetBoundCentre(&boundCentre);
    float fRadius = CModelInfoSAInterface::GetModelInfo(m_nModelIndex)->pColModel->m_sphere.m_radius;
    *pRect = CRect(boundCentre.fX - fRadius, boundCentre.fY - fRadius, boundCentre.fX + fRadius, boundCentre.fY + fRadius);
    pRect->FixIncorrectTopLeft();            // Fix #1613: custom map collision crashes in CPhysical class (infinite loop)
    return pRect;
}

void CPhysicalSAInterface::StaticSetHooks()
{
    HookInstall(0x5449B0, &CPhysicalSAInterface::GetBoundRect_);
}

void CPhysicalSA::RestoreLastGoodPhysicsState()
{
    CEntitySA::RestoreLastGoodPhysicsState();

    CVector vecDefault;
    SetTurnSpeed(&vecDefault);
    SetMoveSpeed(vecDefault);

    CPhysicalSAInterface* pInterface = (CPhysicalSAInterface*)GetInterface();
    pInterface->m_pad4d = 0;
    pInterface->m_fDamageImpulseMagnitude = 0;
    pInterface->m_vecCollisionImpactVelocity = CVector();
    pInterface->m_vecCollisionPosition = CVector();
}

CVector* CPhysicalSA::GetMoveSpeed(CVector* vecMoveSpeed)
{
    GetMoveSpeedInternal(vecMoveSpeed);
    if (!IsValidPosition(*vecMoveSpeed))
    {
        RestoreLastGoodPhysicsState();
        GetMoveSpeedInternal(vecMoveSpeed);
    }
    return vecMoveSpeed;
}

CVector* CPhysicalSA::GetTurnSpeed(CVector* vecTurnSpeed)
{
    GetTurnSpeedInternal(vecTurnSpeed);
    if (!IsValidPosition(*vecTurnSpeed))
    {
        RestoreLastGoodPhysicsState();
        GetTurnSpeedInternal(vecTurnSpeed);
    }
    return vecTurnSpeed;
}

CVector* CPhysicalSA::GetMoveSpeedInternal(CVector* vecMoveSpeed)
{
    DWORD dwFunc = FUNC_GetMoveSpeed;
    DWORD dwThis = (DWORD)((CPhysicalSAInterface*)GetInterface());
    DWORD dwReturn = 0;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     dwReturn, eax
    }
    MemCpyFast(vecMoveSpeed, (void*)dwReturn, sizeof(CVector));
    return vecMoveSpeed;
}

CVector* CPhysicalSA::GetTurnSpeedInternal(CVector* vecTurnSpeed)
{
    DWORD dwFunc = FUNC_GetTurnSpeed;
    DWORD dwThis = (DWORD)((CPhysicalSAInterface*)GetInterface());
    DWORD dwReturn = 0;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     dwReturn, eax
    }
    MemCpyFast(vecTurnSpeed, (void*)dwReturn, sizeof(CVector));
    return vecTurnSpeed;
}

void CPhysicalSA::SetMoveSpeed(const CVector& vecMoveSpeed) noexcept
{
    try
    {
        DWORD dwFunc = FUNC_GetMoveSpeed;
        DWORD dwThis = (DWORD)((CPhysicalSAInterface*)GetInterface());
        DWORD dwReturn = 0;

        __asm
        {
            mov     ecx, dwThis
            call    dwFunc
            mov     dwReturn, eax
        }
        MemCpyFast((void*)dwReturn, &vecMoveSpeed, sizeof(CVector));

        if (GetInterface()->nType == ENTITY_TYPE_OBJECT)
        {
            AddToMovingList();
            SetStatic(false);
        }
    }
    catch (...)
    {
    }
}

void CPhysicalSA::SetTurnSpeed(CVector* vecTurnSpeed)
{
    ((CPhysicalSAInterface*)GetInterface())->m_vecAngularVelocity = *vecTurnSpeed;

    if (GetInterface()->nType == ENTITY_TYPE_OBJECT)
    {
        AddToMovingList();
        SetStatic(false);
    }
}

float CPhysicalSA::GetMass()
{
    return ((CPhysicalSAInterface*)GetInterface())->m_fMass;
}

void CPhysicalSA::SetMass(float fMass)
{
    ((CPhysicalSAInterface*)GetInterface())->m_fMass = fMass;
}

float CPhysicalSA::GetTurnMass()
{
    return ((CPhysicalSAInterface*)GetInterface())->m_fTurnMass;
}

void CPhysicalSA::SetTurnMass(float fTurnMass)
{
    ((CPhysicalSAInterface*)GetInterface())->m_fTurnMass = fTurnMass;
}

float CPhysicalSA::GetAirResistance()
{
    return ((CPhysicalSAInterface*)GetInterface())->m_fAirResistance;
}

void CPhysicalSA::SetAirResistance(float fAirResistance)
{
    ((CPhysicalSAInterface*)GetInterface())->m_fAirResistance = fAirResistance;
}

float CPhysicalSA::GetElasticity()
{
    return ((CPhysicalSAInterface*)GetInterface())->m_fElasticity;
}

void CPhysicalSA::SetElasticity(float fElasticity)
{
    ((CPhysicalSAInterface*)GetInterface())->m_fElasticity = fElasticity;
}

float CPhysicalSA::GetBuoyancyConstant()
{
    return ((CPhysicalSAInterface*)GetInterface())->m_fBuoyancyConstant;
}

void CPhysicalSA::SetBuoyancyConstant(float fBuoyancyConstant)
{
    ((CPhysicalSAInterface*)GetInterface())->m_fBuoyancyConstant = fBuoyancyConstant;
}

void CPhysicalSA::GetCenterOfMass(CVector& vecCenterOfMass)
{
    vecCenterOfMass = ((CPhysicalSAInterface*)GetInterface())->m_vecCenterOfMass;
}

void CPhysicalSA::SetCenterOfMass(CVector& vecCenterOfMass)
{
    ((CPhysicalSAInterface*)GetInterface())->m_vecCenterOfMass = vecCenterOfMass;
}

void CPhysicalSA::ProcessCollision()
{
    DWORD dwFunc = FUNC_ProcessCollision;
    DWORD dwThis = (DWORD)GetInterface();

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}

void CPhysicalSA::AddToMovingList()
{
    DWORD dwFunc = FUNC_CPhysical_AddToMovingList;
    DWORD dwThis = (DWORD)GetInterface();

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}

float CPhysicalSA::GetDamageImpulseMagnitude()
{
    return ((CPhysicalSAInterface*)GetInterface())->m_fDamageImpulseMagnitude;
}

void CPhysicalSA::SetDamageImpulseMagnitude(float fMagnitude)
{
    ((CPhysicalSAInterface*)GetInterface())->m_fDamageImpulseMagnitude = fMagnitude;
}

CEntity* CPhysicalSA::GetDamageEntity()
{
    CEntitySAInterface* pInterface = ((CPhysicalSAInterface*)GetInterface())->m_pCollidedEntity;
    if (pInterface)
    {
        CPools* pPools = pGame->GetPools();
        return pPools->GetEntity((DWORD*)pInterface);
    }
    return nullptr;
}

void CPhysicalSA::SetDamageEntity(CEntity* pEntity)
{
    CEntitySA* pEntitySA = dynamic_cast<CEntitySA*>(pEntity);
    if (pEntitySA)
        ((CPhysicalSAInterface*)GetInterface())->m_pCollidedEntity = pEntitySA->GetInterface();
}

void CPhysicalSA::ResetLastDamage()
{
    ((CPhysicalSAInterface*)GetInterface())->m_fDamageImpulseMagnitude = 0.0f;
    ((CPhysicalSAInterface*)GetInterface())->m_pCollidedEntity = NULL;
}

CEntity* CPhysicalSA::GetAttachedEntity()
{
    CEntitySAInterface* pInterface = ((CPhysicalSAInterface*)GetInterface())->m_pAttachedEntity;
    if (pInterface)
    {
        CPools* pPools = pGame->GetPools();
        return pPools->GetEntity((DWORD*)pInterface);
    }
    return nullptr;
}

void CPhysicalSA::AttachEntityToEntity(CPhysical& Entity, const CVector& vecPosition, const CVector& vecRotation)
{
    CPhysicalSA& EntitySA = dynamic_cast<CPhysicalSA&>(Entity);
    DWORD        dwEntityInterface = (DWORD)EntitySA.GetInterface();

    InternalAttachEntityToEntity(dwEntityInterface, &vecPosition, &vecRotation);
}

void CPhysicalSA::DetachEntityFromEntity(float fUnkX, float fUnkY, float fUnkZ, bool bUnk)
{
    DWORD dwFunc = FUNC_DetatchEntityFromEntity;
    DWORD dwThis = (DWORD)GetInterface();

    // DetachEntityFromEntity appears to crash when there's no entity attached (0x544403, bug 2350)
    // So do a NULL check here
    if (((CPhysicalSAInterface*)GetInterface())->m_pAttachedEntity == NULL)
        return;

    _asm
    {
        push    bUnk
        push    fUnkZ
        push    fUnkY
        push    fUnkX
        mov     ecx, dwThis
        call    dwFunc
    }
}

bool CPhysicalSA::InternalAttachEntityToEntity(DWORD dwEntityInterface, const CVector* vecPosition, const CVector* vecRotation)
{
    DWORD dwFunc = FUNC_AttachEntityToEntity;
    DWORD dwThis = (DWORD)GetInterface();
    DWORD dwReturn = 0;
    _asm
    {
        mov     ecx, vecRotation
        push    [ecx+8]
        push    [ecx+4]
        push    [ecx]
        mov     ecx, vecPosition
        push    [ecx+8]
        push    [ecx+4]
        push    [ecx]
        push    dwEntityInterface
        mov     ecx, dwThis
        call    dwFunc
        mov     dwReturn, eax
    }
    return (dwReturn != NULL);
}

void CPhysicalSA::GetAttachedOffsets(CVector& vecPosition, CVector& vecRotation)
{
    CPhysicalSAInterface* pInterface = (CPhysicalSAInterface*)GetInterface();
    if (pInterface->m_pAttachedEntity)
    {
        vecPosition = pInterface->m_vecAttachedOffset;
        vecRotation = pInterface->m_vecAttachedRotation;
    }
}

void CPhysicalSA::SetAttachedOffsets(CVector& vecPosition, CVector& vecRotation)
{
    CPhysicalSAInterface* pInterface = (CPhysicalSAInterface*)GetInterface();
    if (pInterface->m_pAttachedEntity)
    {
        pInterface->m_vecAttachedOffset = vecPosition;
        pInterface->m_vecAttachedRotation = vecRotation;
    }
}

float CPhysicalSA::GetLighting()
{
    CPhysicalSAInterface* pInterface = (CPhysicalSAInterface*)GetInterface();
    return pInterface->m_fLighting;
}

void CPhysicalSA::SetLighting(float fLighting)
{
    CPhysicalSAInterface* pInterface = (CPhysicalSAInterface*)GetInterface();
    pInterface->m_fLighting = fLighting;
}

void CPhysicalSA::SetFrozen(bool bFrozen)
{
    CPhysicalSAInterface* pInterface = (CPhysicalSAInterface*)GetInterface();

    pInterface->bDontApplySpeed = bFrozen;
    // Don't enable friction for static objects
    pInterface->bDisableFriction = (bFrozen || pInterface->m_fMass >= PHYSICAL_MAXMASS);
}
