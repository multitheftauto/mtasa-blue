/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CHandlingEntrySA.cpp
 *  PURPOSE:     Vehicle handling data entry
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CGameSA.h"
#include "CHandlingEntrySA.h"
#include "CHandlingManagerSA.h"

extern CGameSA* pGame;

CHandlingEntrySA::CHandlingEntrySA()
{
    // Create a new interface and zero it
    m_pHandlingSA = new tHandlingDataSA;
    memset(m_pHandlingSA, 0, sizeof(tHandlingDataSA));
    m_bDeleteInterface = true;
}

CHandlingEntrySA::CHandlingEntrySA(tHandlingDataSA* pOriginal)
{
    // Store gta's pointer
    m_pHandlingSA = nullptr;
    m_bDeleteInterface = false;
    memcpy(&m_Handling, pOriginal, sizeof(tHandlingDataSA));
}

CHandlingEntrySA::~CHandlingEntrySA()
{
    if (m_bDeleteInterface)
    {
        SAFE_DELETE(m_pHandlingSA);
    }
}

// Apply the handlingdata from another data
void CHandlingEntrySA::Assign(const CHandlingEntry* pEntry)
{
    if (!pEntry)
        return;

    // Copy the data
    const CHandlingEntrySA* pEntrySA = static_cast<const CHandlingEntrySA*>(pEntry);
    m_Handling = pEntrySA->m_Handling;
}

void CHandlingEntrySA::Recalculate()
{
    // Real GTA class?
    if (!m_pHandlingSA)
        return;

    // Copy our stored field to GTA's
    memcpy(m_pHandlingSA, &m_Handling, sizeof(m_Handling));
    ((void(_stdcall*)(tHandlingDataSA*))FUNC_HandlingDataMgr_ConvertDataToGameUnits)(m_pHandlingSA);
}

void CHandlingEntrySA::SetSuspensionForceLevel(float fForce) noexcept
{
    CheckSuspensionChanges();
    m_Handling.fSuspensionForceLevel = fForce;
}

void CHandlingEntrySA::SetSuspensionDamping(float fDamping) noexcept
{
    CheckSuspensionChanges();
    m_Handling.fSuspensionDamping = fDamping;
}

void CHandlingEntrySA::SetSuspensionHighSpeedDamping(float fDamping) noexcept
{
    CheckSuspensionChanges();
    m_Handling.fSuspensionHighSpdDamping = fDamping;
}

void CHandlingEntrySA::SetSuspensionUpperLimit(float fUpperLimit) noexcept
{
    CheckSuspensionChanges();
    m_Handling.fSuspensionUpperLimit = fUpperLimit;
}

void CHandlingEntrySA::SetSuspensionLowerLimit(float fLowerLimit) noexcept
{
    CheckSuspensionChanges();
    m_Handling.fSuspensionLowerLimit = fLowerLimit;
}

void CHandlingEntrySA::SetSuspensionFrontRearBias(float fBias) noexcept
{
    CheckSuspensionChanges();
    m_Handling.fSuspensionFrontRearBias = fBias;
}

void CHandlingEntrySA::SetSuspensionAntiDiveMultiplier(float fAntidive) noexcept
{
    CheckSuspensionChanges();
    m_Handling.fSuspensionAntiDiveMultiplier = fAntidive;
}

void CHandlingEntrySA::CheckSuspensionChanges() noexcept
{
    pGame->GetHandlingManager()->CheckSuspensionChanges(this);
}
