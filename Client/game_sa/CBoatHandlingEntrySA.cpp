/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CBoatHandlingEntrySA.cpp
 *  PURPOSE:     Vehicle flying handling data entry
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CBoatHandlingEntrySA::CBoatHandlingEntrySA()
{
    // Create a new interface and zero it
    m_pBoatHandlingSA = std::make_shared<tBoatHandlingDataSA>();
    memset(m_pBoatHandlingSA.get(), 0, sizeof(tBoatHandlingDataSA));
}

CBoatHandlingEntrySA::CBoatHandlingEntrySA(tBoatHandlingDataSA* pOriginal)
{
    // Store gta's pointer
    m_pBoatHandlingSA = std::make_shared<tBoatHandlingDataSA>();
    memcpy(m_pBoatHandlingSA.get(), pOriginal, sizeof(tBoatHandlingDataSA));
}

// Apply the handlingdata from another data
void CBoatHandlingEntrySA::Assign(const CBoatHandlingEntry* pData)
{
    // Copy the data
    const CBoatHandlingEntrySA* pEntrySA = static_cast<const CBoatHandlingEntrySA*>(pData);
    m_pBoatHandlingSA = pEntrySA->m_pBoatHandlingSA;
}
