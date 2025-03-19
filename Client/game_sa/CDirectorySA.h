/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CDirectorySA.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

struct DirectoryInfoSA
{
    std::uint32_t m_offset;
    std::uint16_t m_streamingSize;
    std::uint16_t m_sizeInArchive;
    char          m_name[24];
};

class CDirectorySAInterface
{
    public:
        bool             AddEntry(DirectoryInfoSA& entry);
        bool             RemoveEntry(const char* fileName);

        DirectoryInfoSA* GetModelEntry(const char* fileName);
        DirectoryInfoSA* GetModelEntry(std::uint16_t modelId);
        bool             SetModelStreamingSize(std::uint16_t modelId, std::uint16_t size);
        std::uint16_t    GetModelStreamingSize(std::uint16_t modelId);

    private:
        DirectoryInfoSA* m_entries{};
        std::uint32_t    m_capacity{};
        std::uint32_t    m_numEntries{};
        bool             m_ownsEntries{};
};
