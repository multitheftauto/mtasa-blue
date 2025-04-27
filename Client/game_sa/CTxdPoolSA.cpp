/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CTxdPoolSA.h"
#include "CGameSA.h"
#include "CKeyGenSA.h"

extern CGameSA* pGame;

CTxdPoolSA::CTxdPoolSA()
{
    m_ppTxdPoolInterface = (CPoolSAInterface<CTextureDictonarySAInterface>**)0xC8800C;
}

std::uint32_t CTxdPoolSA::AllocateTextureDictonarySlot(std::uint32_t uiSlotId, std::string& strTxdName)
{
    CTextureDictonarySAInterface* pTxd = (*m_ppTxdPoolInterface)->AllocateAt(uiSlotId);
    if (!pTxd)
        return -1;

    strTxdName.resize(24);

    pTxd->usUsagesCount = 0;
    pTxd->hash = pGame->GetKeyGen()->GetUppercaseKey(strTxdName.c_str());
    pTxd->rwTexDictonary = nullptr;
    pTxd->usParentIndex = -1;

    return (*m_ppTxdPoolInterface)->GetObjectIndex(pTxd);
}

void CTxdPoolSA::RemoveTextureDictonarySlot(std::uint32_t uiTxdId)
{
    if (!(*m_ppTxdPoolInterface)->IsContains(uiTxdId))
        return;

    typedef std::uint32_t(__cdecl * Function_TxdReleaseSlot)(std::uint32_t uiTxdId);
    ((Function_TxdReleaseSlot)(0x731E90))(uiTxdId);

    (*m_ppTxdPoolInterface)->Release(uiTxdId);
}

bool CTxdPoolSA::IsFreeTextureDictonarySlot(std::uint32_t uiTxdId)
{
    return (*m_ppTxdPoolInterface)->IsEmpty(uiTxdId);
}

std::uint16_t CTxdPoolSA::GetFreeTextureDictonarySlot()
{
    return (*m_ppTxdPoolInterface)->GetFreeSlot();
}
