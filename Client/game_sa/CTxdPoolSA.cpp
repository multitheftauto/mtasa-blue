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
    if (!m_ppTxdPoolInterface || !(*m_ppTxdPoolInterface))
        return false;

    return (*m_ppTxdPoolInterface)->IsEmpty(uiTxdId);
}

std::uint32_t CTxdPoolSA::GetFreeTextureDictonarySlot()
{
    if (!m_ppTxdPoolInterface || !(*m_ppTxdPoolInterface))
        return static_cast<std::uint32_t>(-1);

    return (*m_ppTxdPoolInterface)->GetFreeSlot();
}

int CTxdPoolSA::GetPoolSize() const noexcept
{
    if (!m_ppTxdPoolInterface || !(*m_ppTxdPoolInterface))
        return 0;

    return (*m_ppTxdPoolInterface)->m_nSize;
}

int CTxdPoolSA::GetUsedSlotCount() const noexcept
{
    if (!m_ppTxdPoolInterface || !(*m_ppTxdPoolInterface))
        return -1;

    CPoolSAInterface<CTextureDictonarySAInterface>* pPool = *m_ppTxdPoolInterface;
    if (!pPool->m_byteMap || pPool->m_nSize <= 0)
        return -1;

    int iUsedCount = 0;
    for (int i = 0; i < pPool->m_nSize; ++i)
    {
        if (!pPool->m_byteMap[i].bEmpty)
            ++iUsedCount;
    }

    return iUsedCount;
}

CTextureDictonarySAInterface* CTxdPoolSA::GetTextureDictonarySlot(std::uint32_t uiTxdId) noexcept
{
    if (!m_ppTxdPoolInterface || !(*m_ppTxdPoolInterface))
        return nullptr;

    if (!(*m_ppTxdPoolInterface)->IsContains(uiTxdId))
        return nullptr;

    return (*m_ppTxdPoolInterface)->GetObject(uiTxdId);
}

bool CTxdPoolSA::SetTextureDictonarySlot(std::uint32_t uiTxdId, RwTexDictionary* pTxd, std::uint16_t usParentIndex) noexcept
{
    if (!m_ppTxdPoolInterface || !(*m_ppTxdPoolInterface))
        return false;

    CTextureDictonarySAInterface* pSlot = GetTextureDictonarySlot(uiTxdId);
    if (!pSlot)
        return false;

    pSlot->rwTexDictonary = pTxd;
    pSlot->usParentIndex = usParentIndex;
    return true;
}
