/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CBoatHandlingEntrySA.cpp
 *  PURPOSE:     Vehicle flying handling data entry
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CBoatHandlingEntrySA.h"

CBoatHandlingEntrySA::CBoatHandlingEntrySA()
{
    memset(&m_pBoatHandlingSA, 0, sizeof(tBoatHandlingDataSA));
}

CBoatHandlingEntrySA::CBoatHandlingEntrySA(tBoatHandlingDataSA* pOriginal)
{
    m_pBoatHandlingSA = *pOriginal;
}

void CBoatHandlingEntrySA::Assign(const CBoatHandlingEntry* pData)
{
    const CBoatHandlingEntrySA* pEntrySA = static_cast<const CBoatHandlingEntrySA*>(pData);
    m_pBoatHandlingSA = pEntrySA->m_pBoatHandlingSA;
}
