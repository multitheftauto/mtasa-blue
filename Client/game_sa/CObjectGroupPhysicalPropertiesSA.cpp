/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CObjectGroupPhysicalPropertiesSA.cpp
 *  PURPOSE:     Objects dynamic physical properties handler class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CObjectGroupPhysicalPropertiesSA.h"

CObjectGroupPhysicalPropertiesSAInterface* pObjectInfo = (CObjectGroupPhysicalPropertiesSAInterface*)ARRAY_ObjectGroupsDynamicInfo;
std::unordered_map<unsigned char, CObjectGroupPhysicalPropertiesSAInterface*> CObjectGroupPhysicalPropertiesSA::ms_OriginalGroupProperties;

CObjectGroupPhysicalPropertiesSA::CObjectGroupPhysicalPropertiesSA()
{
    m_pInterface = nullptr;
}

CObjectGroupPhysicalPropertiesSA::CObjectGroupPhysicalPropertiesSA(unsigned char ucObjectGroup) : m_ucObjectGroup(ucObjectGroup)
{
    m_pInterface = &pObjectInfo[ucObjectGroup];
    m_bModified = MapFind(ms_OriginalGroupProperties, ucObjectGroup);    
}

CObjectGroupPhysicalPropertiesSAInterface* CObjectGroupPhysicalPropertiesSA::GetInterface()
{
    return m_pInterface;
}

void CObjectGroupPhysicalPropertiesSA::SetGroup(unsigned char ucObjectGroup)
{
    m_pInterface = &pObjectInfo[ucObjectGroup];
    m_ucObjectGroup = ucObjectGroup;
    m_bModified = MapFind(ms_OriginalGroupProperties, ucObjectGroup);
}

unsigned char CObjectGroupPhysicalPropertiesSA::GetGroup()
{
    return m_ucObjectGroup;
}

bool CObjectGroupPhysicalPropertiesSA::IsValid()
{
    return m_pInterface;
}

void CObjectGroupPhysicalPropertiesSA::ChangeSafeguard()
{
    if (m_bModified)
        return;

    m_bModified = true;
    // Make copy of original
    if (!MapFind(ms_OriginalGroupProperties, m_ucObjectGroup))
    {
        auto pOriginalCopy = new CObjectGroupPhysicalPropertiesSAInterface;
        memcpy(pOriginalCopy, m_pInterface, sizeof(CObjectGroupPhysicalPropertiesSAInterface));
        MapSet(ms_OriginalGroupProperties, m_ucObjectGroup, pOriginalCopy);
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

    auto pOriginal = *ppOriginalCopy;
    dassert(pOriginal);
    if (!pOriginal)
        return;
    
    memcpy(m_pInterface, pOriginal, sizeof(CObjectGroupPhysicalPropertiesSAInterface));
    m_bModified = false;
}

void CObjectGroupPhysicalPropertiesSA::RestoreDefaultValues()
{
    for (auto& entry : ms_OriginalGroupProperties)
    {
        memcpy(&pObjectInfo[entry.first], entry.second, sizeof(CObjectGroupPhysicalPropertiesSAInterface));
    }
}

void CObjectGroupPhysicalPropertiesSA::SetMass(float fMass)
{
    ChangeSafeguard();
    m_pInterface->fMass = fMass;
}

float CObjectGroupPhysicalPropertiesSA::GetMass()
{
    return m_pInterface->fMass;
}

void CObjectGroupPhysicalPropertiesSA::SetTurnMass(float fTurnMass)
{
    ChangeSafeguard();
    m_pInterface->fTurnMass = fTurnMass;
}

float CObjectGroupPhysicalPropertiesSA::GetTurnMass()
{
    return m_pInterface->fTurnMass;
}

void CObjectGroupPhysicalPropertiesSA::SetAirResistance(float fAirResistance)
{
    ChangeSafeguard();
    m_pInterface->fAirResistance = fAirResistance;
}

float CObjectGroupPhysicalPropertiesSA::GetAirResistance()
{
    ChangeSafeguard();
    return m_pInterface->fAirResistance;
}

void CObjectGroupPhysicalPropertiesSA::SetElasticity(float fElasticity)
{
    ChangeSafeguard();
    m_pInterface->fElasticity = fElasticity;
}

float CObjectGroupPhysicalPropertiesSA::GetElasticity()
{
    return m_pInterface->fElasticity;
}

void CObjectGroupPhysicalPropertiesSA::SetBuoyancy(float fBuoyancy)
{
    ChangeSafeguard();
    m_pInterface->fBuoyancy = fBuoyancy;
}

float CObjectGroupPhysicalPropertiesSA::GetBuoyancy()
{
    return m_pInterface->fBuoyancy;
}

void CObjectGroupPhysicalPropertiesSA::SetUprootLimit(float fUprootLimit)
{
    ChangeSafeguard();
    m_pInterface->fUprootLimit = fUprootLimit;
}

float CObjectGroupPhysicalPropertiesSA::GetUprootLimit()
{
    return m_pInterface->fUprootLimit;
}

void CObjectGroupPhysicalPropertiesSA::SetCollisionDamageMultiplier(float fColMult)
{
    ChangeSafeguard();
    m_pInterface->fColDamageMultiplier = fColMult;
}

float CObjectGroupPhysicalPropertiesSA::GetCollisionDamageMultiplier()
{
    return m_pInterface->fColDamageMultiplier;
}

void CObjectGroupPhysicalPropertiesSA::SetCollisionDamageEffect(eObjectGroup::DamageEffect eDamageEffect)
{
    ChangeSafeguard();
    m_pInterface->eColDamageEffect = eDamageEffect;
}

eObjectGroup::DamageEffect CObjectGroupPhysicalPropertiesSA::GetCollisionDamageEffect()
{
    return static_cast<eObjectGroup::DamageEffect>(m_pInterface->eColDamageEffect);
}

void CObjectGroupPhysicalPropertiesSA::SetCollisionSpecialResponseCase(eObjectGroup::CollisionResponse eResponseCase)
{
    ChangeSafeguard();
    m_pInterface->eSpecialColResponse = eResponseCase;
}

eObjectGroup::CollisionResponse CObjectGroupPhysicalPropertiesSA::GetCollisionSpecialResponseCase()
{
    return static_cast<eObjectGroup::CollisionResponse>(m_pInterface->eSpecialColResponse);
}

void CObjectGroupPhysicalPropertiesSA::SetCameraAvoidObject(bool bAvoid)
{
    ChangeSafeguard();
    m_pInterface->bCameraAvoidObject = bAvoid;
}

bool CObjectGroupPhysicalPropertiesSA::GetCameraAvoidObject()
{
    return m_pInterface->bCameraAvoidObject;
}

void CObjectGroupPhysicalPropertiesSA::SetCausesExplosion(bool bExplodes)
{
    ChangeSafeguard();
    m_pInterface->bCausesExplosion = bExplodes;
}

bool CObjectGroupPhysicalPropertiesSA::GetCausesExplosion()
{
    return m_pInterface->bCausesExplosion;
}

void CObjectGroupPhysicalPropertiesSA::SetFxType(eObjectGroup::FxType eFxType)
{
    ChangeSafeguard();
    m_pInterface->eFxType = eFxType;
}

eObjectGroup::FxType CObjectGroupPhysicalPropertiesSA::GetFxType()
{
    return static_cast<eObjectGroup::FxType>(m_pInterface->eFxType);
}

void CObjectGroupPhysicalPropertiesSA::SetFxOffset(CVector vecOffset)
{
    ChangeSafeguard();
    m_pInterface->vecFxOffset = vecOffset;
}

CVector CObjectGroupPhysicalPropertiesSA::GetFxOffset()
{
    return m_pInterface->vecFxOffset;
}

void CObjectGroupPhysicalPropertiesSA::SetSmashMultiplier(float fMult)
{
    ChangeSafeguard();
    m_pInterface->fSmashMultiplier = fMult;
}

float CObjectGroupPhysicalPropertiesSA::GetSmashMultiplier()
{
    return m_pInterface->fSmashMultiplier;
}

void CObjectGroupPhysicalPropertiesSA::SetBreakVelocity(CVector vecVelocity)
{
    ChangeSafeguard();
    m_pInterface->vecBreakVelocity = vecVelocity;
}

CVector CObjectGroupPhysicalPropertiesSA::GetBreakVelocity()
{
    return m_pInterface->vecBreakVelocity;
}

void CObjectGroupPhysicalPropertiesSA::SetBreakVelocityRandomness(float fRand)
{
    ChangeSafeguard();
    m_pInterface->fBreakVelocityRand = fRand;
}

float CObjectGroupPhysicalPropertiesSA::GetBreakVelocityRandomness()
{
    return m_pInterface->fBreakVelocityRand;
}

void CObjectGroupPhysicalPropertiesSA::SetBreakMode(eObjectGroup::BreakMode eBreakMode)
{
    ChangeSafeguard();
    m_pInterface->eBreakMode = eBreakMode;
}

eObjectGroup::BreakMode CObjectGroupPhysicalPropertiesSA::GetBreakMode()
{
    return static_cast<eObjectGroup::BreakMode>(m_pInterface->eBreakMode);
}

void CObjectGroupPhysicalPropertiesSA::SetSparksOnImpact(bool bSparks)
{
    ChangeSafeguard();
    m_pInterface->dwSparksOnImpact = static_cast<bool>(bSparks);
}

bool CObjectGroupPhysicalPropertiesSA::GetSparksOnImpact()
{
    return static_cast<bool>(m_pInterface->dwSparksOnImpact);
}
