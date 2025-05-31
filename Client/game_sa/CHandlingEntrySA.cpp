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
    if (m_HandlingSA = std::make_unique<tHandlingDataSA>())
    {
        MemSetFast(m_HandlingSA.get(), 0, sizeof(tHandlingDataSA));
    }
}

CHandlingEntrySA::CHandlingEntrySA(const tHandlingDataSA* const pOriginal)
{
    // Store gta's pointer
    m_HandlingSA = nullptr;
    if (pOriginal)
    {
        MemCpyFast(&m_Handling, pOriginal, sizeof(tHandlingDataSA));
    }
}

// Apply the handlingdata from another data
void CHandlingEntrySA::Assign(const CHandlingEntry* const pEntry) noexcept
{
    if (!pEntry)
        return;

    // Copy the data
    const CHandlingEntrySA* const pEntrySA = static_cast<const CHandlingEntrySA const*>(pEntry);
    m_Handling = pEntrySA->m_Handling;
}

void CHandlingEntrySA::Recalculate() noexcept
{
    // Real GTA class?
    if (!m_HandlingSA)
        return;

     // Copy our stored field to GTA's
    MemCpyFast(m_HandlingSA.get(), &m_Handling, sizeof(m_Handling));
    ((void(_stdcall*)(tHandlingDataSA*))FUNC_HandlingDataMgr_ConvertDataToGameUnits)(m_HandlingSA.get());
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

void CHandlingEntrySA::CheckSuspensionChanges() const noexcept
{
    pGame->GetHandlingManager()->CheckSuspensionChanges(this);
}
