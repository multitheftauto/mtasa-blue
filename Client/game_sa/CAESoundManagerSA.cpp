/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAESoundManagerSA.cpp
 *  PURPOSE:     Audio engine sound manager
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CAESoundManagerSA.h"

#define FUNC_CAESoundManager__CancelSoundsInBankSlot 0x4EFC60

CAESoundManagerSA* g_pAESoundManagerSA = nullptr;

CAESoundManagerSA::CAESoundManagerSA(CAESoundManagerSAInterface* pInterface) : m_pInterface{pInterface}
{
    g_pAESoundManagerSA = this;
}

void CAESoundManagerSA::CancelSoundsInBankSlot(uint uiGroup, uint uiIndex)
{
    using CAESoundManager__CancelSoundsInBankSlot = CAESound*(__thiscall*)(CAESoundManagerSAInterface*, uint, uint);
    static auto pCancelSoundsInBankSlot = reinterpret_cast<CAESoundManager__CancelSoundsInBankSlot>(FUNC_CAESoundManager__CancelSoundsInBankSlot);
    pCancelSoundsInBankSlot(m_pInterface, uiGroup, uiIndex);
}
