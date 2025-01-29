/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CDirectorySA.cpp
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CDirectorySA.h"

DirectoryInfo* CDirectorySA::GetModelEntry(ushort modelId)
{
    if (m_nNumEntries <= 0)
        return nullptr;

    DirectoryInfo* entry = m_pEntries + modelId;

    if (!entry)
        return nullptr;

    return entry;
}

bool CDirectorySA::SetModelStreamingSize(ushort modelId, uint16 size)
{
    DirectoryInfo* entry = GetModelEntry(modelId);

    if (!entry)
        return false;

    if (entry->m_nStreamingSize == size)
        return false;

    entry->m_nStreamingSize = size;
    return true;
}


uint16 CDirectorySA::GetModelStreamingSize(ushort modelId)
{
    DirectoryInfo* entry = GetModelEntry(modelId);

    if (!entry)
        return false;

    return entry->m_nStreamingSize;
}
