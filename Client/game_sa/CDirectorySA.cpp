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
