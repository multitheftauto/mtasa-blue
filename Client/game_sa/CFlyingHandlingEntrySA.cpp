/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CFlyingHandlingEntrySA.cpp
 *  PURPOSE:     Vehicle flying handling data entry
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
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
