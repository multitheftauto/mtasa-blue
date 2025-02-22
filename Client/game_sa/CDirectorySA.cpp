/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CDirectorySA.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CDirectorySA.h"

bool CDirectorySAInterface::AddEntry(DirectoryInfoSA& entry)
{
    if (m_nNumEntries >= m_nCapacity)
        return false;

    if (GetModelEntry(entry.m_szName))
        return false;

    entry.m_nOffset = 0;

    if (m_nNumEntries > 0)
    {
        DirectoryInfoSA* lastEntry = m_pEntries + m_nNumEntries - 1;
        entry.m_nOffset = lastEntry->m_nOffset + lastEntry->m_nStreamingSize;

        if (entry.m_nOffset % 2048)
            entry.m_nOffset += 2048 - (entry.m_nOffset % 2048);
    }

    m_pEntries[m_nNumEntries] = entry;
    m_nNumEntries++;

    return true;
}

bool CDirectorySAInterface::RemoveEntry(const char* pFileName)
{
    if (m_nNumEntries <= 0)
        return false;

    DirectoryInfoSA* entry = GetModelEntry(pFileName);

    if (!entry)
        return false;

    std::uint32_t index = entry - m_pEntries;

    if (index < m_nNumEntries - 1)
    {
        DirectoryInfoSA* lastEntry = m_pEntries + m_nNumEntries - 1;
        entry->m_nOffset = lastEntry->m_nOffset + lastEntry->m_nSizeInArchive;
    }

    m_nNumEntries--;

    if (index < m_nNumEntries)
        memmove(entry, entry + 1, (m_nNumEntries - index) * sizeof(DirectoryInfoSA));

    return true;
}

DirectoryInfoSA* CDirectorySAInterface::GetModelEntry(const char* pFileName)
{
    if (m_nNumEntries <= 0)
        return nullptr;

    for (DirectoryInfoSA* it = m_pEntries; it != m_pEntries + m_nNumEntries; it++)
    {
        if (strcmp(it->m_szName, pFileName) == 0)
            return it;
    }

    return nullptr;
}

DirectoryInfoSA* CDirectorySAInterface::GetModelEntry(std::uint16_t modelId)
{
    if (m_nNumEntries <= 0)
        return nullptr;

    DirectoryInfoSA* entry = m_pEntries + modelId;

    if (!entry)
        return nullptr;

    return entry;
}

bool CDirectorySAInterface::SetModelStreamingSize(std::uint16_t modelId, std::uint16_t size)
{
    DirectoryInfoSA* entry = GetModelEntry(modelId);

    if (!entry)
        return false;

    if (entry->m_nStreamingSize == size)
        return false;

    entry->m_nStreamingSize = size;
    return true;
}


std::uint16_t CDirectorySAInterface::GetModelStreamingSize(std::uint16_t modelId)
{
    DirectoryInfoSA* entry = GetModelEntry(modelId);

    if (!entry)
        return 0;

    return entry->m_nStreamingSize;
}
