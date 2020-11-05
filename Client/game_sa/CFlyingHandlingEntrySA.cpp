/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CFlyingHandlingEntrySA.cpp
 *  PURPOSE:     Vehicle flying handling data entry
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CFlyingHandlingEntrySA::CFlyingHandlingEntrySA()
{
    // Create a new interface and zero it
    m_pFlyingHandlingSA = std::make_shared<tFlyingHandlingDataSA>();
    memset(m_pFlyingHandlingSA.get(), 0, sizeof(tFlyingHandlingDataSA));
}

CFlyingHandlingEntrySA::CFlyingHandlingEntrySA(tFlyingHandlingDataSA* pOriginal)
{
    // Store gta's pointer
    m_pFlyingHandlingSA = std::make_shared<tFlyingHandlingDataSA>();
    memcpy(m_pFlyingHandlingSA.get(), pOriginal, sizeof(tFlyingHandlingDataSA));
}

// Apply the handlingdata from another data
void CFlyingHandlingEntrySA::Assign(const CFlyingHandlingEntry* pData)
{
    // Copy the data
    const CFlyingHandlingEntrySA* pEntrySA = static_cast<const CFlyingHandlingEntrySA*>(pData);
    m_pFlyingHandlingSA = pEntrySA->m_pFlyingHandlingSA;
}
