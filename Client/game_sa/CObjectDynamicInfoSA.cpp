/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CObjectDynamicInfoSA.cpp
 *  PURPOSE:     Objects dynamic physical properties handler class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CObjectDynamicInfoSA.h"

CObjectDynamicInfoSAInterface* pObjectInfo = (CObjectDynamicInfoSAInterface*)ARRAY_ObjectGroupsDynamicInfo;
std::unordered_map<unsigned char, CObjectDynamicInfoSAInterface*> CObjectDynamicInfoSA::ms_OriginalGroupProperties;

CObjectDynamicInfoSA::CObjectDynamicInfoSA()
{
    m_pInterface = nullptr;
}

CObjectDynamicInfoSA::CObjectDynamicInfoSA(unsigned char ucObjectGroup) : m_ucObjectGroup(ucObjectGroup)
{
    m_pInterface = &pObjectInfo[ucObjectGroup];
    m_bModified = MapFind(ms_OriginalGroupProperties, ucObjectGroup);    
}

CObjectDynamicInfoSAInterface* CObjectDynamicInfoSA::GetInterface()
{
    return m_pInterface;
}

void CObjectDynamicInfoSA::SetGroup(unsigned char ucObjectGroup)
{
    m_pInterface = &pObjectInfo[ucObjectGroup];
    m_ucObjectGroup = ucObjectGroup;
    m_bModified = MapFind(ms_OriginalGroupProperties, ucObjectGroup);
}

unsigned char CObjectDynamicInfoSA::GetGroup()
{
    return m_ucObjectGroup;
}

bool CObjectDynamicInfoSA::IsValid()
{
    return m_pInterface;
}

void CObjectDynamicInfoSA::ChangeSafeguard()
{
    if (m_bModified)
        return;

    m_bModified = true;
    // Make copy of original
    if (!MapFind(ms_OriginalGroupProperties, m_ucObjectGroup))
    {
        auto pOriginalCopy = new CObjectDynamicInfoSAInterface;
        memcpy(pOriginalCopy, m_pInterface, sizeof(CObjectDynamicInfoSAInterface));
        MapSet(ms_OriginalGroupProperties, m_ucObjectGroup, pOriginalCopy);
    }
}

void CObjectDynamicInfoSA::RestoreDefaultValues()
{
    for (auto& entry : ms_OriginalGroupProperties)
    {
        memcpy(&pObjectInfo[entry.first], entry.second, sizeof(CObjectDynamicInfoSAInterface));
    }
}

void CObjectDynamicInfoSA::SetMass(float fMass)
{
    ChangeSafeguard();
    m_pInterface->fMass = fMass;
}

float CObjectDynamicInfoSA::GetMass()
{
    return m_pInterface->fMass;
}

void CObjectDynamicInfoSA::SetTurnMass(float fTurnMass)
{
    ChangeSafeguard();
    m_pInterface->fTurnMass = fTurnMass;
}

float CObjectDynamicInfoSA::GetTurnMass()
{
    return m_pInterface->fTurnMass;
}

void CObjectDynamicInfoSA::SetAirResistance(float fAirResistance)
{
    ChangeSafeguard();
    m_pInterface->fAirResistance = fAirResistance;
}

float CObjectDynamicInfoSA::GetAirResistance()
{
    ChangeSafeguard();
    return m_pInterface->fAirResistance;
}

void CObjectDynamicInfoSA::SetElasticity(float fElasticity)
{
    ChangeSafeguard();
    m_pInterface->fElasticity = fElasticity;
}

float CObjectDynamicInfoSA::GetElasticity()
{
    return m_pInterface->fElasticity;
}

void CObjectDynamicInfoSA::SetUprootLimit(float fUprootLimit)
{
    ChangeSafeguard();
    m_pInterface->fUprootLimit = fUprootLimit;
}

float CObjectDynamicInfoSA::GetUprootLimit()
{
    return m_pInterface->fUprootLimit;
}

void CObjectDynamicInfoSA::SetColissionDamageMultiplier(float fColMult)
{
    ChangeSafeguard();
    m_pInterface->fColDamageMultiplier = fColMult;
}

float CObjectDynamicInfoSA::GetColissionDamageMultiplier()
{
    return m_pInterface->fColDamageMultiplier;
}

void CObjectDynamicInfoSA::SetColissionDamageEffect(unsigned char ucDamageEffect)
{
    ChangeSafeguard();
    m_pInterface->ucColDamageEffect = ucDamageEffect;
}

unsigned char CObjectDynamicInfoSA::GetCollisionDamageEffect()
{
    return m_pInterface->ucColDamageEffect;
}

void CObjectDynamicInfoSA::SetCollisionSpecialResponseCase(unsigned char ucResponseCase)
{
    ChangeSafeguard();
    m_pInterface->ucSpecialColResponse = ucResponseCase;
}

unsigned char CObjectDynamicInfoSA::GetCollisionSpecialResponseCase()
{
    return m_pInterface->ucSpecialColResponse;
}

void CObjectDynamicInfoSA::SetCameraAvoidObject(bool bAvoid)
{
    ChangeSafeguard();
    m_pInterface->bCameraAvoidObject = bAvoid;
}

bool CObjectDynamicInfoSA::GetCameraAvoidObject()
{
    return m_pInterface->bCameraAvoidObject;
}

void CObjectDynamicInfoSA::SetCausesExplosion(bool bExplodes)
{
    ChangeSafeguard();
    m_pInterface->bCausesExplosion = bExplodes;
}

bool CObjectDynamicInfoSA::GetCausesExplosion()
{
    return m_pInterface->bCausesExplosion;
}

void CObjectDynamicInfoSA::SetFxOffset(CVector vecOffset)
{
    ChangeSafeguard();
    m_pInterface->vecFxOffset = vecOffset;
}

CVector CObjectDynamicInfoSA::GetFxOffset()
{
    return m_pInterface->vecFxOffset;
}

void CObjectDynamicInfoSA::SetSmashMultiplier(float fMult)
{
    ChangeSafeguard();
    m_pInterface->fSmashMultiplier = fMult;
}

float CObjectDynamicInfoSA::GetSmashMultiplier()
{
    return m_pInterface->fSmashMultiplier;
}

void CObjectDynamicInfoSA::SetBreakVelocity(CVector vecVelocity)
{
    ChangeSafeguard();
    m_pInterface->vecBreakVelocity = vecVelocity;
}

CVector CObjectDynamicInfoSA::GetBreakVelocity()
{
    return m_pInterface->vecBreakVelocity;
}

void CObjectDynamicInfoSA::SetBreakVelocityRandomness(float fRand)
{
    ChangeSafeguard();
    m_pInterface->fBreakVelocityRand = fRand;
}

float CObjectDynamicInfoSA::GetBreakVelocityRandomness()
{
    return m_pInterface->fBreakVelocityRand;
}

void CObjectDynamicInfoSA::SetGunBreakMode(DWORD dwBreakMode)
{
    ChangeSafeguard();
    m_pInterface->dwGunBreakMode = dwBreakMode;
}

DWORD CObjectDynamicInfoSA::GetGunBreakMode()
{
    return m_pInterface->dwGunBreakMode;
}

void CObjectDynamicInfoSA::SetSparksOnImpact(DWORD dwSparks)
{
    ChangeSafeguard();
    m_pInterface->dwSparksOnImpact = dwSparks;
}

DWORD CObjectDynamicInfoSA::GetSparksOnImpact()
{
    return m_pInterface->dwSparksOnImpact;
}
