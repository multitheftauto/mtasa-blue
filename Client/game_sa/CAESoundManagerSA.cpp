/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CAESoundManagerSA.cpp
 *  PURPOSE:     Audio engine sound manager
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CAESoundManagerSA.h"

CAESoundManagerSA* g_pAESoundManagerSA = nullptr;

CAESoundManagerSA::CAESoundManagerSA(CAESoundManagerSAInterface* pInterface) : m_pInterface{pInterface}
{
    g_pAESoundManagerSA = this;
}

void CAESoundManagerSA::CancelSoundsInBankSlot(uint uiGroup, uint uiIndex)
{
    (reinterpret_cast<CAESound*(__thiscall*)(CAESoundManagerSAInterface*, uint, uint)>(0x4EFC60))(m_pInterface, uiGroup, uiIndex);
}
