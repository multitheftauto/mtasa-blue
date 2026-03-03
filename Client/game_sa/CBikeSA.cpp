/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CBikeSA.cpp
 *  PURPOSE:     Bike vehicle entity
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CBikeSA.h"

CBikeSA::CBikeSA(CBikeSAInterface* pInterface)
{
    SetInterface(pInterface);
    Init();
}

CBikeHandlingEntry* CBikeSA::GetBikeHandlingData()
{
    return m_pBikeHandlingData;
}

void CBikeSA::SetBikeHandlingData(CBikeHandlingEntry* pBikeHandling)
{
    if (!pBikeHandling)
        return;
    m_pBikeHandlingData = static_cast<CBikeHandlingEntrySA*>(pBikeHandling);
    GetBikeInterface()->m_pBikeHandlingData = m_pBikeHandlingData->GetInterface();
    RecalculateBikeHandling();
}

void CBikeSA::RecalculateBikeHandling()
{
    if (m_pBikeHandlingData)
        m_pBikeHandlingData->Recalculate();
}

bool CBikeSA::IsAnyWheelTouchingGround() const
{
    CBikeSAInterface* bikeInterface = GetBikeInterface();
    if (!bikeInterface)
        return false;

    return bikeInterface->m_wheelRatios[0] < 1.0f || bikeInterface->m_wheelRatios[1] < 1.0f || bikeInterface->m_wheelRatios[2] < 1.0f || bikeInterface->m_wheelRatios[3] < 1.0f;
}
