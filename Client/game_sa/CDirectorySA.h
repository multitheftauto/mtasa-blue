/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CDirectorySA.h
 *
 *****************************************************************************/

#pragma once

struct DirectoryInfo
{
    uint32 m_nOffset;
    uint16 m_nStreamingSize;
    uint16 m_nSizeInArchive;
    char   m_szName[24];
};

class CDirectorySA
{
    public:
        DirectoryInfo*  GetModelEntry(ushort modelId);
        bool            SetModelStreamingSize(ushort modelId, uint16 size);
        uint16          GetModelStreamingSize(ushort modelId);

    private:
        DirectoryInfo* m_pEntries{};
        uint32         m_nCapacity{};
        uint32         m_nNumEntries{};
        bool           m_bOwnsEntries{};
};
