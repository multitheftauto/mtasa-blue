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
    if (m_numEntries >= m_capacity || GetModelEntry(entry.m_name))
        return false;

    entry.m_offset = 0;

    if (m_numEntries > 0)
    {
        DirectoryInfoSA* lastEntry = m_entries + m_numEntries - 1;
        entry.m_offset = lastEntry->m_offset + lastEntry->m_streamingSize;

        if (entry.m_offset % 2048)
            entry.m_offset += 2048 - (entry.m_offset % 2048);
    }

    m_entries[m_numEntries++] = entry;

    return true;
}

bool CDirectorySAInterface::RemoveEntry(const char* fileName)
{
    if (m_numEntries == 0)
        return false;

    DirectoryInfoSA* entry = GetModelEntry(fileName);

    if (!entry)
        return false;

    std::ptrdiff_t index = entry - m_entries;

    if (index < m_numEntries - 1)
    {
        DirectoryInfoSA* lastEntry = m_entries + m_numEntries - 1;
        entry->m_offset = lastEntry->m_offset + lastEntry->m_sizeInArchive;
    }

    m_numEntries--;

    if (index < m_numEntries)
        std::memmove(entry, entry + 1, (m_numEntries - index) * sizeof(DirectoryInfoSA));

    return true;
}

DirectoryInfoSA* CDirectorySAInterface::GetModelEntry(const char* fileName)
{
    if (m_numEntries == 0)
        return nullptr;

    for (DirectoryInfoSA* it = m_entries; it != m_entries + m_numEntries; it++)
    {
        if (std::strcmp(it->m_name, fileName) == 0)
            return it;
    }

    return nullptr;
}

DirectoryInfoSA* CDirectorySAInterface::GetModelEntry(std::uint16_t modelId)
{
    if (m_numEntries == 0)
        return nullptr;

    DirectoryInfoSA* entry = m_entries + modelId;

    if (!entry)
        return nullptr;

    return entry;
}

bool CDirectorySAInterface::SetModelStreamingSize(std::uint16_t modelId, std::uint16_t size)
{
    DirectoryInfoSA* entry = GetModelEntry(modelId);

    if (!entry || entry->m_streamingSize == size)
        return false;

    entry->m_streamingSize = size;
    return true;
}


std::uint16_t CDirectorySAInterface::GetModelStreamingSize(std::uint16_t modelId)
{
    DirectoryInfoSA* entry = GetModelEntry(modelId);

    if (!entry)
        return 0;

    return entry->m_streamingSize;
}
