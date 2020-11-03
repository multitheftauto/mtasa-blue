/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CHandlingEntrySA.cpp
 *  PURPOSE:     Vehicle handling data entry
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CBikeHandlingEntrySA::CBikeHandlingEntrySA()
{
    // Create a new interface and zero it
    m_pBikeHandlingSA = new tBikeHandlingDataSA;
    memset(m_pBikeHandlingSA, 0, sizeof(tBikeHandlingDataSA));
    m_bDeleteInterface = true;
}

CBikeHandlingEntrySA::CBikeHandlingEntrySA(tBikeHandlingDataSA* pOriginal)
{
    // Store gta's pointer
    m_pBikeHandlingSA = NULL;
    m_bDeleteInterface = false;
    memcpy(&m_BikeHandling, pOriginal, sizeof(tBikeHandlingDataSA));
}

CBikeHandlingEntrySA::~CBikeHandlingEntrySA()
{
    if (m_bDeleteInterface)
    {
        delete m_pBikeHandlingSA;
    }
}

// Apply the handlingdata from another data
void CBikeHandlingEntrySA::Assign(const CBikeHandlingEntry* pData)
{
    // Copy the data
    const CBikeHandlingEntrySA* pEntrySA = static_cast<const CBikeHandlingEntrySA*>(pData);
    m_BikeHandling = pEntrySA->m_BikeHandling;
}

void CBikeHandlingEntrySA::Recalculate()
{
    // Real GTA class?
    if (m_pBikeHandlingSA)
    {
        // Copy our stored field to GTA's
        memcpy(m_pBikeHandlingSA, &m_BikeHandling, sizeof(m_BikeHandling));

       ((void(_stdcall*)(tBikeHandlingDataSA*))FUNC_HandlingDataMgr_ConvertBikeDataToGameUnits)(m_pBikeHandlingSA);
    }
}
