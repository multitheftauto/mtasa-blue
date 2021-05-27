/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CObjectGroupPhysicalPropertiesSA.cpp
 *  PURPOSE:     Objects dynamic physical properties handler class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CObjectGroupPhysicalPropertiesSA.h"
extern CGameSA* pGame;

CObjectGroupPhysicalPropertiesSAInterface* pObjectInfo = *(CObjectGroupPhysicalPropertiesSAInterface**)(0x59F857 + 6);
std::unordered_map<unsigned char, std::unique_ptr<CObjectGroupPhysicalPropertiesSAInterface>> CObjectGroupPhysicalPropertiesSA::ms_OriginalGroupProperties;

CObjectGroupPhysicalPropertiesSA::CObjectGroupPhysicalPropertiesSA() : m_pInterface(nullptr)
{}

CObjectGroupPhysicalPropertiesSA::CObjectGroupPhysicalPropertiesSA(unsigned char ucObjectGroup) : m_ucObjectGroup(ucObjectGroup)
{
    m_pInterface = &pObjectInfo[ucObjectGroup];
    m_bModified = MapFind(ms_OriginalGroupProperties, ucObjectGroup);
}

CObjectGroupPhysicalPropertiesSAInterface* CObjectGroupPhysicalPropertiesSA::GetInterface() const
{
    return m_pInterface;
}

void CObjectGroupPhysicalPropertiesSA::SetGroup(unsigned char ucObjectGroup)
{
    m_pInterface = &pObjectInfo[ucObjectGroup];
    m_ucObjectGroup = ucObjectGroup;
    m_bModified = MapFind(ms_OriginalGroupProperties, ucObjectGroup);
}

unsigned char CObjectGroupPhysicalPropertiesSA::GetGroup() const
{
    return m_ucObjectGroup;
}

bool CObjectGroupPhysicalPropertiesSA::IsValid() const
{
    return m_pInterface != nullptr;
}

void CObjectGroupPhysicalPropertiesSA::ChangeSafeguard()
{
    if (m_bModified)
        return;

    m_bModified = true;
    // Make copy of original
    if (!MapFind(ms_OriginalGroupProperties, m_ucObjectGroup))
    {
        auto pOriginalCopy = std::make_unique<CObjectGroupPhysicalPropertiesSAInterface>();
        memcpy(pOriginalCopy.get(), m_pInterface, sizeof(CObjectGroupPhysicalPropertiesSAInterface));
        ms_OriginalGroupProperties[m_ucObjectGroup] = std::move(pOriginalCopy);
    }
}

void CObjectGroupPhysicalPropertiesSA::RestoreDefault()
{
    if (!m_bModified)
        return;

    auto ppOriginalCopy = MapFind(ms_OriginalGroupProperties, m_ucObjectGroup);
    dassert(ppOriginalCopy);
    if (!ppOriginalCopy)
        return;

    dassert(ppOriginalCopy->get());
    if (!ppOriginalCopy->get())
        return;

    memcpy(m_pInterface, ppOriginalCopy->get(), sizeof(CObjectGroupPhysicalPropertiesSAInterface));
    m_bModified = false;
}

void CObjectGroupPhysicalPropertiesSA::RestoreDefaultValues()
{
    for (auto& entry : ms_OriginalGroupProperties)
    {
        pGame->GetObjectGroupPhysicalProperties(entry.first)->RestoreDefault();
        entry.second.reset();
    }

    ms_OriginalGroupProperties.clear();
}

void CObjectGroupPhysicalPropertiesSA::SetMass(float fMass)
{
    if (fabs(m_pInterface->fMass - fMass) < FLOAT_EPSILON)
        return;

    ChangeSafeguard();
    m_pInterface->fMass = fMass;
}

float CObjectGroupPhysicalPropertiesSA::GetMass() const
{
    return m_pInterface->fMass;
}

void CObjectGroupPhysicalPropertiesSA::SetTurnMass(float fTurnMass)
{
    if (fabs(m_pInterface->fTurnMass - fTurnMass) < FLOAT_EPSILON)
        return;

    ChangeSafeguard();
    m_pInterface->fTurnMass = fTurnMass;
}

float CObjectGroupPhysicalPropertiesSA::GetTurnMass() const
{
    return m_pInterface->fTurnMass;
}

void CObjectGroupPhysicalPropertiesSA::SetAirResistance(float fAirResistance)
{
    if (fabs(m_pInterface->fAirResistance - fAirResistance) < FLOAT_EPSILON)
        return;

    ChangeSafeguard();
    m_pInterface->fAirResistance = fAirResistance;
}

float CObjectGroupPhysicalPropertiesSA::GetAirResistance() const
{
    return m_pInterface->fAirResistance;
}

void CObjectGroupPhysicalPropertiesSA::SetElasticity(float fElasticity)
{
    if (fabs(m_pInterface->fElasticity - fElasticity) < FLOAT_EPSILON)
        return;

    ChangeSafeguard();
    m_pInterface->fElasticity = fElasticity;
}

float CObjectGroupPhysicalPropertiesSA::GetElasticity() const
{
    return m_pInterface->fElasticity;
}

void CObjectGroupPhysicalPropertiesSA::SetBuoyancy(float fBuoyancy)
{
    if (fabs(m_pInterface->fBuoyancy - fBuoyancy) < FLOAT_EPSILON)
        return;

    ChangeSafeguard();
    m_pInterface->fBuoyancy = fBuoyancy;
}

float CObjectGroupPhysicalPropertiesSA::GetBuoyancy() const
{
    return m_pInterface->fBuoyancy;
}

void CObjectGroupPhysicalPropertiesSA::SetUprootLimit(float fUprootLimit)
{
    if (fabs(m_pInterface->fUprootLimit - fUprootLimit) < FLOAT_EPSILON)
        return;

    ChangeSafeguard();
    m_pInterface->fUprootLimit = fUprootLimit;
}

float CObjectGroupPhysicalPropertiesSA::GetUprootLimit() const
{
    return m_pInterface->fUprootLimit;
}

void CObjectGroupPhysicalPropertiesSA::SetCollisionDamageMultiplier(float fColMult)
{
    if (fabs(m_pInterface->fColDamageMultiplier - fColMult) < FLOAT_EPSILON)
        return;

    ChangeSafeguard();
    m_pInterface->fColDamageMultiplier = fColMult;
}

float CObjectGroupPhysicalPropertiesSA::GetCollisionDamageMultiplier() const
{
    return m_pInterface->fColDamageMultiplier;
}

void CObjectGroupPhysicalPropertiesSA::SetCollisionDamageEffect(eObjectGroup::DamageEffect eDamageEffect)
{
    if (static_cast<eObjectGroup::DamageEffect>(m_pInterface->eColDamageEffect) == eDamageEffect)
        return;

    ChangeSafeguard();
    m_pInterface->eColDamageEffect = eDamageEffect;
}

eObjectGroup::DamageEffect CObjectGroupPhysicalPropertiesSA::GetCollisionDamageEffect() const
{
    return static_cast<eObjectGroup::DamageEffect>(m_pInterface->eColDamageEffect);
}

void CObjectGroupPhysicalPropertiesSA::SetCollisionSpecialResponseCase(eObjectGroup::CollisionResponse eResponseCase)
{
    if (static_cast<eObjectGroup::CollisionResponse>(m_pInterface->eSpecialColResponse) == eResponseCase)
        return;

    ChangeSafeguard();
    m_pInterface->eSpecialColResponse = eResponseCase;
}

eObjectGroup::CollisionResponse CObjectGroupPhysicalPropertiesSA::GetCollisionSpecialResponseCase() const
{
    return static_cast<eObjectGroup::CollisionResponse>(m_pInterface->eSpecialColResponse);
}

void CObjectGroupPhysicalPropertiesSA::SetCameraAvoidObject(bool bAvoid)
{
    if (m_pInterface->bCameraAvoidObject == bAvoid)
        return;

    ChangeSafeguard();
    m_pInterface->bCameraAvoidObject = bAvoid;
}

bool CObjectGroupPhysicalPropertiesSA::GetCameraAvoidObject() const
{
    return m_pInterface->bCameraAvoidObject;
}

void CObjectGroupPhysicalPropertiesSA::SetCausesExplosion(bool bExplodes)
{
    if (m_pInterface->bCausesExplosion == bExplodes)
        return;

    ChangeSafeguard();
    m_pInterface->bCausesExplosion = bExplodes;
}

bool CObjectGroupPhysicalPropertiesSA::GetCausesExplosion() const
{
    return m_pInterface->bCausesExplosion;
}

void CObjectGroupPhysicalPropertiesSA::SetFxType(eObjectGroup::FxType eFxType)
{
    if (static_cast<eObjectGroup::FxType>(m_pInterface->eFxType) == eFxType)
        return;

    ChangeSafeguard();
    m_pInterface->eFxType = eFxType;
}

eObjectGroup::FxType CObjectGroupPhysicalPropertiesSA::GetFxType() const
{
    return static_cast<eObjectGroup::FxType>(m_pInterface->eFxType);
}

void CObjectGroupPhysicalPropertiesSA::SetFxOffset(CVector vecOffset)
{
    if (m_pInterface->vecFxOffset == vecOffset)
        return;

    ChangeSafeguard();
    m_pInterface->vecFxOffset = vecOffset;
}

CVector CObjectGroupPhysicalPropertiesSA::GetFxOffset() const
{
    return m_pInterface->vecFxOffset;
}

bool CObjectGroupPhysicalPropertiesSA::SetFxParticleSystem(CFxSystemBPSAInterface* pBlueprint)
{
    if (!pBlueprint)
        return false;

    if (pBlueprint->cPlayMode != 0)
        return false;

    if (m_pInterface->pFxSystemBlueprintPtr == pBlueprint)
        return true;

    ChangeSafeguard();
    m_pInterface->pFxSystemBlueprintPtr = pBlueprint;
    return true;
}

void CObjectGroupPhysicalPropertiesSA::RemoveFxParticleSystem()
{
    if (!m_pInterface->pFxSystemBlueprintPtr)
        return;

    ChangeSafeguard();
    m_pInterface->pFxSystemBlueprintPtr = nullptr;
}

void CObjectGroupPhysicalPropertiesSA::SetSmashMultiplier(float fMult)
{
    if (fabs(m_pInterface->fSmashMultiplier - fMult) < FLOAT_EPSILON)
        return;

    ChangeSafeguard();
    m_pInterface->fSmashMultiplier = fMult;
}

float CObjectGroupPhysicalPropertiesSA::GetSmashMultiplier() const
{
    return m_pInterface->fSmashMultiplier;
}

void CObjectGroupPhysicalPropertiesSA::SetBreakVelocity(CVector vecVelocity)
{
    if (m_pInterface->vecBreakVelocity == vecVelocity)
        return;

    ChangeSafeguard();
    m_pInterface->vecBreakVelocity = vecVelocity;
}

CVector CObjectGroupPhysicalPropertiesSA::GetBreakVelocity() const
{
    return m_pInterface->vecBreakVelocity;
}

void CObjectGroupPhysicalPropertiesSA::SetBreakVelocityRandomness(float fRand)
{
    if (fabs(m_pInterface->fBreakVelocityRand - fRand) < FLOAT_EPSILON)
        return;

    ChangeSafeguard();
    m_pInterface->fBreakVelocityRand = fRand;
}

float CObjectGroupPhysicalPropertiesSA::GetBreakVelocityRandomness() const
{
    return m_pInterface->fBreakVelocityRand;
}

void CObjectGroupPhysicalPropertiesSA::SetBreakMode(eObjectGroup::BreakMode eBreakMode)
{
    if (static_cast<eObjectGroup::BreakMode>(m_pInterface->eBreakMode) == eBreakMode)
        return;

    ChangeSafeguard();
    m_pInterface->eBreakMode = eBreakMode;
}

eObjectGroup::BreakMode CObjectGroupPhysicalPropertiesSA::GetBreakMode() const
{
    return static_cast<eObjectGroup::BreakMode>(m_pInterface->eBreakMode);
}

void CObjectGroupPhysicalPropertiesSA::SetSparksOnImpact(bool bSparks)
{
    if (static_cast<bool>(m_pInterface->dwSparksOnImpact) == bSparks)
        return;

    ChangeSafeguard();
    m_pInterface->dwSparksOnImpact = static_cast<bool>(bSparks);
}

bool CObjectGroupPhysicalPropertiesSA::GetSparksOnImpact() const
{
    return static_cast<bool>(m_pInterface->dwSparksOnImpact);
}
