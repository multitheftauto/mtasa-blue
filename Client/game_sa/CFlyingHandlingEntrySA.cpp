/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CFlyingHandlingEntrySA.cpp
 *  PURPOSE:     Vehicle flying handling data entry
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CFlyingHandlingEntrySA.h"

CFlyingHandlingEntrySA::CFlyingHandlingEntrySA()
{
    memset(&m_pFlyingHandlingSA, 0, sizeof(tFlyingHandlingDataSA));
}

CFlyingHandlingEntrySA::CFlyingHandlingEntrySA(tFlyingHandlingDataSA* pOriginal)
{
    m_pFlyingHandlingSA = *pOriginal;
}

void CFlyingHandlingEntrySA::Assign(const CFlyingHandlingEntry* pData)
{
    if (!pData)
        return;
    const CFlyingHandlingEntrySA* pEntrySA = static_cast<const CFlyingHandlingEntrySA*>(pData);
    m_pFlyingHandlingSA = pEntrySA->m_pFlyingHandlingSA;
}
