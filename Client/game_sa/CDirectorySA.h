/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CDirectorySA.h
 *
 *****************************************************************************/

#pragma once

struct DirectoryInfoSA
{
    uint32 m_nOffset;
    uint16 m_nStreamingSize;
    uint16 m_nSizeInArchive;
    char   m_szName[24];
};

class CDirectorySAInterface
{
    public:
        DirectoryInfoSA* GetModelEntry(std::uint16_t modelId);
        bool             SetModelStreamingSize(std::uint16_t modelId, uint16 size);
        uint16           GetModelStreamingSize(std::uint16_t modelId);

    private:
        DirectoryInfoSA* m_pEntries{};
        uint32         m_nCapacity{};
        uint32         m_nNumEntries{};
        bool           m_bOwnsEntries{};
};
