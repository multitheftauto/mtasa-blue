/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CHandlingEntrySA.cpp
 *  PURPOSE:     Vehicle handling data entry
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CBikeHandlingEntrySA.h"

CBikeHandlingEntrySA::CBikeHandlingEntrySA()
{
    memset(&m_pBikeHandlingSA, 0, sizeof(tBikeHandlingDataSA));
}

CBikeHandlingEntrySA::CBikeHandlingEntrySA(tBikeHandlingDataSA* pOriginal)
{
    memset(&m_pBikeHandlingSA, 0, sizeof(tBikeHandlingDataSA));
    m_BikeHandling = *pOriginal;
}

void CBikeHandlingEntrySA::Assign(const CBikeHandlingEntry* pData)
{
    const CBikeHandlingEntrySA* pEntrySA = static_cast<const CBikeHandlingEntrySA*>(pData);
    m_BikeHandling = pEntrySA->m_BikeHandling;
}

void CBikeHandlingEntrySA::Recalculate()
{
    auto HandlingDataMgr_ConvertBikeDataToGameUnits = (void(_stdcall*)(tBikeHandlingDataSA*))0x6F5290;
    m_pBikeHandlingSA = m_BikeHandling;
    HandlingDataMgr_ConvertBikeDataToGameUnits(&m_pBikeHandlingSA);
}
