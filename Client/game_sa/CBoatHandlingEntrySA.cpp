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
    m_pBoatHandlingSA = new tBoatHandlingDataSA;
    memset(m_pBoatHandlingSA, 0, sizeof(tBoatHandlingDataSA));
    m_bDeleteInterface = true;
}

CBoatHandlingEntrySA::CBoatHandlingEntrySA(tBoatHandlingDataSA* pOriginal)
{
    // Store gta's pointer
    m_pBoatHandlingSA = new tBoatHandlingDataSA;
    m_bDeleteInterface = false;
    memcpy(m_pBoatHandlingSA, pOriginal, sizeof(tBoatHandlingDataSA));
}

CBoatHandlingEntrySA::~CBoatHandlingEntrySA()
{
    if (m_bDeleteInterface)
    {
        delete m_pBoatHandlingSA;
    }
}

// Apply the handlingdata from another data
void CBoatHandlingEntrySA::Assign(const CBoatHandlingEntry* pData)
{
    // Copy the data
    const CBoatHandlingEntrySA* pEntrySA = static_cast<const CBoatHandlingEntrySA*>(pData);
    m_pBoatHandlingSA = pEntrySA->m_pBoatHandlingSA;
}
