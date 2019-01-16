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

CObjectDynamicInfoSA::CObjectDynamicInfoSA()
{
    m_pInterface = nullptr;
    m_pOriginalCopy = nullptr;
}

CObjectDynamicInfoSA::CObjectDynamicInfoSA(unsigned char ucObjectGroup) : m_ucObjectGroup(ucObjectGroup)
{
    m_pInterface = &pObjectInfo[ucObjectGroup];
    // Make copy of original
    m_pOriginalCopy = new CObjectDynamicInfoSAInterface;
    memcpy(m_pOriginalCopy, m_pInterface, sizeof(CObjectDynamicInfoSAInterface));
}

CObjectDynamicInfoSA::~CObjectDynamicInfoSA()
{
    if (m_pOriginalCopy != nullptr)
    {
        delete m_pOriginalCopy;
        m_pOriginalCopy = nullptr;
    }
}

CObjectDynamicInfoSAInterface* CObjectDynamicInfoSA::GetInterface()
{
    return m_pInterface;
}

void CObjectDynamicInfoSA::SetObjectGroup(unsigned char ucObjectGroup)
{
    if (m_pOriginalCopy)
        RestoreDefaultValues();

    m_pInterface = &pObjectInfo[ucObjectGroup];
    // Make copy of original
    memcpy(m_pOriginalCopy, m_pInterface, sizeof(CObjectDynamicInfoSAInterface));
}

unsigned char CObjectDynamicInfoSA::GetObjectGroup()
{
    return m_ucObjectGroup;
}

bool CObjectDynamicInfoSA::IsValid()
{
    return m_pInterface && m_pOriginalCopy;
}

void CObjectDynamicInfoSA::SetMass(float fMass)
{
    m_pInterface->fMass = fMass;
}

float CObjectDynamicInfoSA::GetMass()
{
    return m_pInterface->fMass;
}

void CObjectDynamicInfoSA::SetTurnMass(float fTurnMass)
{
    m_pInterface->fTurnMass = fTurnMass;
}

float CObjectDynamicInfoSA::GetTurnMass()
{
    return m_pInterface->fTurnMass;
}

void CObjectDynamicInfoSA::SetAirResistance(float fAirResistance)
{
    m_pInterface->fAirResistance = fAirResistance;
}

float CObjectDynamicInfoSA::GetAirResistance()
{
    return m_pInterface->fAirResistance;
}

void CObjectDynamicInfoSA::SetElasticity(float fElasticity)
{
    m_pInterface->fElasticity = fElasticity;
}

float CObjectDynamicInfoSA::GetElasticity()
{
    return m_pInterface->fElasticity;
}

void CObjectDynamicInfoSA::SetUprootLimit(float fUprootLimit)
{
    m_pInterface->fUprootLimit = fUprootLimit;
}

float CObjectDynamicInfoSA::GetUprootLimit()
{
    return m_pInterface->fUprootLimit;
}

void CObjectDynamicInfoSA::SetColissionDamageMultiplier(float fColMult)
{
    m_pInterface->fColDamageMultiplier = fColMult;
}

float CObjectDynamicInfoSA::GetColissionDamageMultiplier()
{
    return m_pInterface->fColDamageMultiplier;
}

void CObjectDynamicInfoSA::SetColissionDamageEffect(unsigned char ucDamageEffect)
{
    m_pInterface->ucColDamageEffect = ucDamageEffect;
}

unsigned char CObjectDynamicInfoSA::GetCollisionDamageEffect()
{
    return m_pInterface->ucColDamageEffect;
}

void CObjectDynamicInfoSA::SetCollisionSpecialResponseCase(unsigned char ucResponseCase)
{
    m_pInterface->ucSpecialColResponse = ucResponseCase;
}

unsigned char CObjectDynamicInfoSA::GetCollisionSpecialResponseCase()
{
    return m_pInterface->ucSpecialColResponse;
}

void CObjectDynamicInfoSA::SetCameraAvoidObject(bool bAvoid)
{
    m_pInterface->bCameraAvoidObject = bAvoid;
}

bool CObjectDynamicInfoSA::GetCameraAvoidObject()
{
    return m_pInterface->bCameraAvoidObject;
}

void CObjectDynamicInfoSA::SetCausesExplosion(bool bExplodes)
{
    m_pInterface->bCausesExplosion = bExplodes;
}

bool CObjectDynamicInfoSA::GetCausesExplosion()
{
    return m_pInterface->bCausesExplosion;
}

void CObjectDynamicInfoSA::SetFxOffset(CVector vecOffset)
{
    m_pInterface->vecFxOffset = vecOffset;
}

CVector CObjectDynamicInfoSA::GetFxOffset()
{
    return m_pInterface->vecFxOffset;
}

void CObjectDynamicInfoSA::SetSmashMultiplier(float fMult)
{
    m_pInterface->fSmashMultiplier = fMult;
}

float CObjectDynamicInfoSA::GetSmashMultiplier()
{
    return m_pInterface->fSmashMultiplier;
}

void CObjectDynamicInfoSA::SetBreakVelocity(CVector vecVelocity)
{
    m_pInterface->vecBreakVelocity = vecVelocity;
}

CVector CObjectDynamicInfoSA::GetBreakVelocity()
{
    return m_pInterface->vecBreakVelocity;
}

void CObjectDynamicInfoSA::SetBreakVelocityRandomness(float fRand)
{
    m_pInterface->fBreakVelocityRand = fRand;
}

float CObjectDynamicInfoSA::GetBreakVelocityRandomness()
{
    return m_pInterface->fBreakVelocityRand;
}

void CObjectDynamicInfoSA::SetGunBreakMode(DWORD dwBreakMode){
    m_pInterface->dwGunBreakMode = dwBreakMode;
}

DWORD CObjectDynamicInfoSA::GetGunBreakMode()
{
    return m_pInterface->dwGunBreakMode;
}

void CObjectDynamicInfoSA::SetSparksOnImpact(DWORD dwSparks)
{
    m_pInterface->dwSparksOnImpact = dwSparks;
}

DWORD CObjectDynamicInfoSA::GetSparksOnImpact()
{
    return m_pInterface->dwSparksOnImpact;
}

void CObjectDynamicInfoSA::RestoreDefaultValues()
{
    assert(m_pOriginalCopy && m_pInterface);
    memcpy(m_pInterface, m_pOriginalCopy, sizeof(CObjectDynamicInfoSAInterface));
}
